#include <cleanMqtt/Mqtt/MqttClient.h>

#include <cleanMqtt/Mqtt/Packets/PacketUtils.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/SendConnectJob.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/SendPublishJob.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/SendPingJob.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/SendPubAckJob.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/SendSubscribeJob.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/SendUnSubscribeJob.h>
#include <cleanMqtt/Mqtt/Packets/Subscribe/SubscribeAck.h>

#include <iostream>
#include <functional>

namespace cleanMqtt
{
	namespace mqtt
	{
		using namespace cleanMqtt::mqtt::packets;

		MqttClient::MqttClient(const Config config, std::unique_ptr<interfaces::IWebSocket> socket)
			: m_config{ config }, m_socket { std::move(socket) }
		{
			assert(m_socket != nullptr);

			m_socket->setOnConnectCallback([this](bool success){handleSocketConnectEvent(success);});
			m_socket->setOnDisconnectCallback([this]() { handleSocketDisconnectEvent(); });
			m_socket->setOnPacketRecvdCallback([this](ByteBuffer&& buffer) { handleSocketPacketReceivedEvent(std::move(buffer)); });
			m_socket->setOnErrorCallback([this](std::uint16_t error) { handleSocketErrorEvent(error); });

			m_sendPubAckEvent.add([this](PacketID packetId) { pubAck(packetId, packets::PubAckReasonCode::SUCCESS, PubAckOptions{}); });
		}

		MqttClient::~MqttClient()
		{
			shutdown();

			m_socket = nullptr;
		}

		ClientError MqttClient::connect(ConnectArgs&& args, ConnectAddress&& address) noexcept
		{
			LockGuard guard{ m_mutex };

			LogDebug("MqttClient", "Starting MQTT connection proccess.");

			if (m_connectionStatus == ConnectionStatus::CONNECTED || m_connectionStatus == ConnectionStatus::CONNECTING)
			{
				LogWarning("MqttClient", "Cannot connect() again while already connected or attempting to connect.");
				return m_connectionStatus == ConnectionStatus::CONNECTED ? ClientErrorCode::Already_Connected : ClientErrorCode::Already_Connecting;
			}

			if (address.primaryAddress.hostname().empty())
			{
				LogError("MqttClient", "Cannot connect() without primary address.");
				return { ClientErrorCode::Missing_Argument, "Cannot connect() without primary address." };
			}

			if (args.clientId.empty())
			{
				LogError("MqttClient", "Cannot connect() without client ID argument.");
				return { ClientErrorCode::Missing_Argument, "Cannot connect() without client ID argument." };
			}

			if (args.will != nullptr)
			{
				if (args.will->payload != nullptr && args.will->payload->size() <= 0)
				{
					LogError("MqttClient", "Payload size must be bigger than 0! Payload is included in the will, but payload size is 0.");
					return { ClientErrorCode::Invalid_Argument, "Payload size must be bigger than 0! Payload is included in the will, but payload size is 0." };
				}

				if (args.will->correlationData != nullptr && args.will->correlationData->size() <= 0)
				{
					LogError("MqttClient", "Correlatation data size must be bigger than 0! Correlatation data is included in the will, but Correlatation data size is 0.");
					return { ClientErrorCode::Invalid_Argument, "Correlatation data size must be bigger than 0! Correlatation data is included in the will, but Correlatation data size is 0." };
				}

				if (args.will->willTopic.empty())
				{
					LogError("MqttClient", "Attempted to add a Will to the Connect packet, but Will Topic has not been set!");
					return { ClientErrorCode::Missing_Argument, "Attempted to add a Will to the Connect packet, but Will Topic has not been set!" };
				}
			}

			m_receiveQueue.clear();

			LogInfo("MqttClient",
				"Hostname: %s\n\tPort: %s\n\tUsername: %s\n\tClientId: %s\n\tVersion: %d\n\tProtocol Name: %s",
				address.primaryAddress.hostname().c_str(),
				address.primaryAddress.port().c_str(),
				args.username.c_str(),
				args.clientId.c_str(),
				static_cast<std::uint8_t>(args.version),
				args.protocolName.c_str());

			m_connectionStatus = ConnectionStatus::CONNECTING;
			m_connectionInfo.connectArgs = std::move(args);
			m_connectionInfo.connectAddress = std::move(address);
			m_connectionInfo.reconnectAddress.reset(m_connectionInfo.connectAddress);

			LogInfo("MqttClient", "Socket attempting to connect.");

			const bool sConnectResult{ m_socket->connect(
				m_connectionInfo.connectAddress.primaryAddress.hostname(),
				m_connectionInfo.connectAddress.primaryAddress.port()) };

			if (!sConnectResult)
			{
				m_connectionStatus = ConnectionStatus::DISCONNECTED;
				return ClientErrorCode::Socket_Connect_Failed;
			}

			m_connectionInfo.connectionStartTime = std::chrono::steady_clock::now();

			return ClientErrorCode::No_Error;
		}

		ClientError MqttClient::publish(const char* topic, ByteBuffer&& payload, PublishOptions&& options) noexcept
		{
			LockGuard guard{ m_mutex };

			LogTrace("MqttClient", "Started publish(): Topic: %s", topic);

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				LogError("MqttClient", "Client not connected, cannot publish()!");
				return { ClientErrorCode::Not_Connected, "Client not connected, cannot publish()!" };
			}

			if (options.topicAlias > m_connectionInfo.maxServerTopicAlias)
			{
				LogError( "MqttClient", "Topic alias exceeds `max server topic alias` received from broker.");
			}

			PacketID packetId{ 0U };
			if (options.qos >= mqtt::Qos::QOS_1)
			{
				packetId = m_packetIdPool.getId();

				ByteBuffer payloadCopy{ payload.size() };
				payloadCopy.append(payload);
				PublishMessageData msgData{ topic, std::move(payloadCopy), options };

				const auto error{ m_connectionInfo.sessionState.addMessage(packetId, std::move(msgData)) };
				if (error != ClientErrorCode::No_Error)
				{
					return { error, "Failed to add message to session state." };
				}
			}

			m_sendQueue.addToQueue(std::make_unique<SendPublishJob>(&m_connectionInfo,
				[this](const packets::BasePacket& packet) {return sendPacket(packet); },
				&m_packetIdPool,
				packetId,
				topic,
				std::move(payload),
				std::move(options)));

			LogInfo("MqttClient", "Queued publish message for sending, Topic: %s, Packet ID: %d, QOS: %d", topic, packetId, static_cast<std::uint8_t>(options.qos));

			return ClientErrorCode::No_Error;
		}

		ClientError MqttClient::subscribe(const std::vector<Topic>& topics, SubscribeOptions&& options) noexcept
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				LogError("MqttClient", "Client not connected, cannot subscribe()!");
				return { ClientErrorCode::Not_Connected, "Client not connected, cannot subscribe()!" };
			}

			if (topics.empty())
			{
				LogError("MqttClient", "Cannot subscribe to an empty list of topics!");
				return { ClientErrorCode::Invalid_Argument, "Cannot subscribe to an empty list of topics!" };
			}

			auto id{ m_packetIdPool.getId() };
			m_connectionInfo.pendingSubscriptions.push_back( PendingSubscription{ id, topics });

			m_sendQueue.addToQueue(std::make_unique<SendSubscribeJob>(&m_connectionInfo,
				[this](const packets::BasePacket& packet) {return sendPacket(packet); },
				&m_packetIdPool,
				id,
				topics,
				std::move(options)));

			LogTrace("MqttClient", "Started subscribe: Subscribing to; %s", allTopicsToStr(topics).c_str());

			return ClientErrorCode::No_Error;
		}

		ClientError MqttClient::unSubscribe(const std::vector<Topic>& topics, UnSubscribeOptions&& options) noexcept
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				LogWarning("MqttClient", "Client not connected, cannot unSubscribe()!");
				return { ClientErrorCode::Not_Connected, "Client not connected, cannot unSubscribe()!" };
			}

			if (topics.empty())
			{
				LogError("MqttClient", "Cannot unsubscribe with an empty list of topics!");
				return { ClientErrorCode::Invalid_Argument, "Cannot unsubscribe with an empty list of topics!" };
			}

			auto id{ m_packetIdPool.getId() };
			m_connectionInfo.pendingUnSubscriptions.push_back(PendingUnSubscription{ id, topics });

			m_sendQueue.addToQueue(std::make_unique<SendUnSubscribeJob>(&m_connectionInfo,
				[this](const packets::BasePacket& packet) {return sendPacket(packet); },
				&m_packetIdPool,
				id,
				topics,
				std::move(options)));

			LogTrace("MqttClient", "Started unSubscribe: Unsubscribing from %s", allTopicsToStr(topics).c_str());

			return ClientErrorCode::No_Error;
		}

		ClientError MqttClient::disconnect(DisconnectArgs&& args) noexcept
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus == ConnectionStatus::DISCONNECTED)
			{
				LogWarning("MqttClient", "Client already disconnected, cannot disconnect()!");
				return { ClientErrorCode::Not_Connected, "Client not connected, cannot disconnect()!" };
			}

			handleInternalDisconnect(
				args.willPublish ? packets::DisconnectReasonCode::DISCONNECT_WITH_WILL_MESSAGE : packets::DisconnectReasonCode::NORMAL_DISCONNECTION,
				args);

			return ClientErrorCode::No_Error;
		}

		ClientError MqttClient::shutdown() noexcept
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus == ConnectionStatus::DISCONNECTED)
			{
				return { ClientErrorCode::MQTT_Not_Active };
			}

			assert(m_socket != nullptr);

			LogInfo("MqttClient", "Client shutdown.");

			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			m_disconnectEvent.invoke({});

			m_eventDeferrer.clear();
			m_errorEvent.removeAll();
			m_connectEvent.removeAll();
			m_disconnectEvent.removeAll();
			m_publishEvent.removeAll();
			m_pubAckEvent.removeAll();
			m_subAckEvent.removeAll();

			m_socket->close();
			m_sendQueue.clearQueue();
			m_receiveQueue.clear();

			clearState();

			return ClientErrorCode::No_Error;
		}

		void MqttClient::tick(float /*deltaTime*/) noexcept
		{
			assert(m_socket != nullptr);

			LockGuard guard{ m_mutex };

			tickCheckTimeOut();

			if (m_connectionStatus != ConnectionStatus::DISCONNECTED)
			{
				if (m_socket->isConnected())
				{
					tickSendPackets();
					tickReceivePackets();
					tickCheckKeepAlive();
					tickPendingPublishMessageRetries();
				}

				m_socket->tick();
			}

			m_eventDeferrer.invokeEvents();
		}

		ConnectionStatus MqttClient::getConnectionStatus() const noexcept
		{
			return m_connectionStatus;
		}

		const MqttConnectionInfo& MqttClient::getConnectionInfo() const noexcept
		{
			return m_connectionInfo;
		}

		void MqttClient::handleReceivedConnectAcknowledge(ConnectAck&& packet)
		{
			//Connection counts as succeeded for any ConnectReasonCode < 128U so report back as connection success, else start failure proccess.
			if (packet.getVariableHeader().reasonCode < packets::ConnectReasonCode::UNSPECIFIED_ERROR)
			{
				m_connectionStatus = ConnectionStatus::CONNECTED;
				m_connectionInfo.hasBeenConnected = true;

				m_connectionInfo.reconnectAddress.reset(m_connectionInfo.connectAddress);

				//Handle properties received from the server.
				const std::uint16_t* serverKeepAlive{ nullptr };
				if (packet.getVariableHeader().properties.tryGetProperty<std::uint16_t>(PropertyType::SERVER_KEEP_ALIVE, serverKeepAlive))
				{
					m_connectionInfo.serverKeepAlive = *serverKeepAlive;
				}
				else
				{
					m_connectionInfo.serverKeepAlive = m_connectionInfo.connectArgs.keepAliveInSec * 1000;
				}

				//Set-up ping interval
				m_connectionInfo.pingInterval = Milliseconds{ m_connectionInfo.serverKeepAlive };
				if (m_config.pingAlways && m_connectionInfo.serverKeepAlive <= 0)
				{
					m_connectionInfo.pingInterval = Milliseconds{ m_config.defaultPingInterval };
				}

				//Set-up max server topic alias
				const std::uint16_t* serverTopicAliasMax{ nullptr };
				if (packet.getVariableHeader().properties.tryGetProperty<std::uint16_t>(PropertyType::TOPIC_ALIAS_MAXIMUM, serverTopicAliasMax))
				{
					m_connectionInfo.maxServerTopicAlias = *serverTopicAliasMax;
				}

				//Check is retain available
				const std::uint8_t* retainAvailable{ 0 };
				if (packet.getVariableHeader().properties.tryGetProperty<std::uint8_t>(PropertyType::RETAIN_AVAILABLE, retainAvailable))
				{
					m_connectionInfo.isRetainAvailable = *retainAvailable == 1;
				}

				//Set-up max server packet size
				const std::uint32_t* maxPacketSize{ nullptr };
				if (packet.getVariableHeader().properties.tryGetProperty<std::uint32_t>(PropertyType::MAXIMUM_PACKET_SIZE, maxPacketSize))
				{
					m_connectionInfo.maxServerPacketSize = static_cast<std::size_t>(*maxPacketSize);
				}
				else
				{
					m_connectionInfo.maxServerPacketSize = static_cast<std::size_t>(MAX_PACKET_SIZE);
				}

				m_connectionInfo.sessionState = SessionState{ m_connectionInfo.connectArgs.clientId.c_str(),
					m_connectionInfo.connectArgs.sessionExpiryInterval,
					m_config.retryPublishIntervalMS,
					nullptr };

				//TODO read mqtt5 spec for rest of properties of Connect Ack (relating to publishing, subscribing and reconnection).

				if (m_connectionStatus == ConnectionStatus::RECONNECTING)
				{
					LogDebug("MqttClient", "Reconnection complete to broker: %s Port: %s",
						m_connectionInfo.reconnectAddress.primaryAddress.hostname().c_str(),
						m_connectionInfo.reconnectAddress.primaryAddress.port().c_str());

					m_eventDeferrer.defer([&, p = std::move(packet)]() {m_reconnectEvent({ ReconnectionStatus::SUCCEEDED, true, 0, ClientErrorCode::No_Error }, p); });
				}
				else
				{
					LogDebug("MqttClient", "Client connected to broker: %s Port: %s",
						m_connectionInfo.connectAddress.primaryAddress.hostname().c_str(),
						m_connectionInfo.reconnectAddress.primaryAddress.port().c_str());

					m_eventDeferrer.defer([&, p = std::move(packet)]() {m_connectEvent({ true, true, ClientErrorCode::No_Error }, p); });
				}
			}
			else
			{
				assert(m_connectionStatus == ConnectionStatus::RECONNECTING || m_connectionStatus == ConnectionStatus::CONNECTING);

				if (m_connectionStatus == ConnectionStatus::RECONNECTING)
				{
					handleFailedReconnect(std::move(packet));
				}
				else
				{
					handleFailedConnect(std::move(packet));
				}

				//TODO read mqtt5 spec for properties of Connect Ack (relating to publishing, subscribing and reconnection).
			}
		}

		void MqttClient::handleReceivedDisconnect(Disconnect&& packet)
		{
			handleExternalDisconnect(packet);
		}

		void MqttClient::handleReceivedPublish(Publish&& packet)
		{
			if (packet.getVariableHeader().qos == Qos::QOS_0)
			{
				firePublishReceivedEvent(std::move(packet));
			}
			else
			{
				//If QoS is 1 or 2, we need to send a PUBACK or PUBREC packet back to the server + Store to Session State.
				//const auto packetId{ packet.getVariableHeader().packetIdentifier};

				//PublishMessageData data{packet.getVariableHeader().}

			//	m_connectionInfo.sessionState.addMessage(packetId, )

			/*	if (packet.getVariableHeader().qos == Qos::QOS_1)
				{
					m_sendQueue.addToQueue(std::make_unique<SendPublishReceivedJob>(&m_connectionInfo,
						[this](const packets::BasePacket& packet) {return sendPacket(packet); },
						packetId,
						m_config.enforceMaxPacketSizeOnSend,
						m_config.maxAllowedPacketSize));
				}
				else if (packet.getVariableHeader().qos == Qos::QOS_2)
				{
					m_sendQueue.addToQueue(std::make_unique<SendPublishReceivedJob>(&m_connectionInfo,
						[this](const packets::BasePacket& packet) {return sendPacket(packet); },
						packetId,
						m_config.enforceMaxPacketSizeOnSend,
						m_config.maxAllowedPacketSize));
				}*/
			}
		}

		void MqttClient::handleReceivedPublishAck(packets::PublishAck&& packet)
		{
			const auto packetId{ packet.getVariableHeader().packetId };

			m_connectionInfo.sessionState.removeMessage(packetId);
			m_packetIdPool.releaseId(packetId);

			m_eventDeferrer.defer([&, id = packetId, p = std::move(packet)]() {m_pubAckEvent({ id }, p); });
		}

		void MqttClient::handleReceivedSubscribeAcknowledge(packets::SubscribeAck&& packet)
		{
			const auto packetId{ packet.getVariableHeader().packetId };
			SubAckResults results;

			bool foundPacketId{ false };

			for (auto iter = m_connectionInfo.pendingSubscriptions.begin(); iter != m_connectionInfo.pendingSubscriptions.end(); ++iter)
			{
				if (iter->packetId == packetId)
				{
					foundPacketId = true;
					results = std::move(iter->subscriptionResults);
					m_connectionInfo.pendingSubscriptions.erase(iter);
					break;
				}
			}

			if (!foundPacketId)
			{
				m_packetIdPool.releaseId(packetId);
				LogWarning("MqttClient", "Received SUBACK packet with Packet ID %d, but no pending subscription found for it.", packetId);
				return;
			}

			results.setTopicReasons(packet.getPayloadHeader().reasonCodes);
			m_packetIdPool.releaseId(packetId);

			m_eventDeferrer.defer([&, id = packetId, res = std::move(results), p = std::move(packet)]() {m_subAckEvent({ id, std::move(res) }, p); });
		}

		void MqttClient::handleReceivedUnSubscribeAcknowledge(packets::UnSubscribeAck&& packet)
		{
			const auto packetId{ packet.getVariableHeader().packetId };
			UnSubAckResults results;

			bool foundPacketId{ false };

			for (auto iter = m_connectionInfo.pendingUnSubscriptions.begin(); iter != m_connectionInfo.pendingUnSubscriptions.end(); ++iter)
			{
				if (iter->packetId == packetId)
				{
					foundPacketId = true;
					results = std::move(iter->unSubscriptionResults);
					m_connectionInfo.pendingUnSubscriptions.erase(iter);
					break;
				}
			}

			if (!foundPacketId)
			{
				m_packetIdPool.releaseId(packetId);
				LogWarning("MqttClient", "Received UNSUBACK packet with Packet ID %d, but no pending un-subscription found for it.", packetId);
				return;
			}

			results.setTopicReasons(packet.getPayloadHeader().reasonCodes);
			m_packetIdPool.releaseId(packetId);

			m_eventDeferrer.defer([&, id = packetId, res = std::move(results), p = std::move(packet)]() {m_unSubAckEvent({ id, std::move(res) }, p); });
		}

		void MqttClient::handleReceivedPingResponse(PingResp&& packet)
		{
			(void)packet;

			m_connectionInfo.awaitingPingResponse = false;
		}

		void MqttClient::firePublishReceivedEvent(packets::Publish&& packet) noexcept
		{
			std::string topicName{ packet.getVariableHeader().topicName.getString() };

			const auto* properties = &packet.getVariableHeader().properties;
			const std::uint16_t* topicAlias{ nullptr };

			if (properties->tryGetProperty<std::uint16_t>(PropertyType::TOPIC_ALIAS, topicAlias))
			{
				if (m_connectionInfo.connectArgs.maximumTopicAliases == 0)
				{
					LogError("MqttClient", "Client does not allow topic aliases but server has sent us a topic alias of value %d.", *topicAlias);

					DisconnectArgs args;
					args.disconnectReasonText = "Client does not allow topic aliases but server has sent us a topic alias";

					m_eventDeferrer.defer([&, reasonText = args.disconnectReasonText.c_str()]() {m_errorEvent({ ClientErrorCode::Recv_Topic_Alias_Disallowed, reasonText }, {}); });
					handleInternalDisconnect(DisconnectReasonCode::PROTOCOL_ERROR, std::move(args));
					return;
				}

				if (*topicAlias <= 0 || *topicAlias > m_connectionInfo.connectArgs.maximumTopicAliases)
				{
					LogError("MqttClient",
						"Publish packet TOPIC ALIAS is invalid: %d\nTOPIC ALIAS - Min allowed value: %d. Max allowed value: %d",
						*topicAlias,
						1,
						m_connectionInfo.connectArgs.maximumTopicAliases);

					DisconnectArgs args;
					args.disconnectReasonText = "Publish packet TOPIC ALIAS value is invalid.";

					m_eventDeferrer.defer([&, reasonText = args.disconnectReasonText.c_str()]() {m_errorEvent({ ClientErrorCode::Recv_Invalid_Topic_Alias, reasonText }, {}); });
					handleInternalDisconnect(DisconnectReasonCode::TOPIC_ALIAS_INVALID, std::move(args));
					return;
				}

				if (topicName.empty())
				{
					const char* topicNameFound{ nullptr };
					if (m_connectionInfo.topicAliases.tryFindTopicName(*topicAlias, topicNameFound))
					{
						topicName = topicNameFound;
					}
				}
				else
				{
					m_connectionInfo.topicAliases.tryAddTopicAlias(topicName.c_str(), *topicAlias);
				}
			}
			else if (topicName.empty())
			{
				LogError("MqttClient", "Publish packet TOPIC ALIAS is invalid and TOPIC NAME is empty.");

				DisconnectArgs args;
				args.disconnectReasonText = "Publish packet TOPIC ALIAS is invalid and TOPIC NAME is empty.";

				m_eventDeferrer.defer([&, reasonText = args.disconnectReasonText.c_str()]() {m_errorEvent({ ClientErrorCode::Recv_Empty_Topic_Name_And_Invalid_Alias, reasonText }, {}); });
				handleInternalDisconnect(DisconnectReasonCode::PROTOCOL_ERROR, std::move(args));
				return;
			}

			const auto qos{ packet.getVariableHeader().qos };
			const auto id{ packet.getVariableHeader().packetIdentifier };

			m_eventDeferrer.defer([&, tName = std::move(topicName), pload = &packet.getPayloadHeader().payload, p = std::move(packet)]() {m_publishEvent({ std::move(tName), pload }, p); });

			if (qos == Qos::QOS_1)
			{
				//After packet is sent to application layer (previous deferEvent), send PUBACK packet back to the server.
				m_eventDeferrer.defer([&]() {m_sendPubAckEvent(id); });
			}
		}

		void MqttClient::tickCheckTimeOut()
		{
			if (m_connectionStatus == ConnectionStatus::CONNECTING || m_connectionStatus == ConnectionStatus::RECONNECTING)
			{
				const auto elapsed{ std::chrono::duration_cast<Milliseconds>(std::chrono::steady_clock::now() - m_connectionInfo.connectionStartTime).count() };

				if (elapsed >= m_config.connectTimeOutMS)
				{
					if (m_connectionStatus == ConnectionStatus::RECONNECTING)
					{
						handleTimeOutReconnect();
					}
					else
					{
						handleTimeOutConnect();
					}
				}
			}
		}

		void MqttClient::tickCheckKeepAlive()
		{
			if (m_connectionStatus == ConnectionStatus::CONNECTED)
			{
				if (m_connectionInfo.serverKeepAlive == 0 && !m_config.pingAlways)
				{
					return;
				}

				if (m_connectionInfo.awaitingPingResponse)
				{
					const auto elapsedTimeSincePingReq{ std::chrono::duration_cast<Milliseconds>(std::chrono::steady_clock::now() - m_connectionInfo.lastPingReqSentTime).count() };

					if (elapsedTimeSincePingReq > m_config.pingTimeOutMS)
					{
						DisconnectArgs args;
						args.clearQueue = true;
						args.disconnectReasonText = "Failed ping: Did not receive PingResp packet.";

						handleInternalDisconnect(DisconnectReasonCode::UNSPECIFIED_ERROR, args);
					}

					return;
				}

				Milliseconds elapsedTimeSinceControlPacket;

				if (m_config.pingAlways)
				{
					elapsedTimeSinceControlPacket = std::chrono::duration_cast<Milliseconds>(std::chrono::steady_clock::now() - m_connectionInfo.lastPingReqSentTime);
				}
				else
				{
					elapsedTimeSinceControlPacket = std::chrono::duration_cast<Milliseconds>(std::chrono::steady_clock::now() - m_connectionInfo.lastControlPacketTime);
				}

				if (elapsedTimeSinceControlPacket >= m_connectionInfo.pingInterval)
				{
					m_sendQueue.addToQueue(std::make_unique<SendPingJob>(&m_connectionInfo, [this](const packets::BasePacket& packet) {return sendPacket(packet); }));
				}
			}
		}

		void MqttClient::tickSendPackets()
		{
			//Send queued encoded packets.
			m_sendQueue.sendNextBatch(m_batchResultData);

			if (m_batchResultData.packetsSent > 0)
			{
				m_connectionInfo.lastControlPacketTime = std::chrono::steady_clock::now();

				LogInfo("MqttClient", "Packets Sent: %d/%d", m_batchResultData.packetsSent, m_batchResultData.packetsAttemptedToSend);
			}

			//Check for potential issues
			if (m_batchResultData.packetsSent != m_batchResultData.packetsAttemptedToSend)
			{
				LogWarning("MqttClient",
					"Failed to send all packets from the previous batch of packets queued for sending.\n\tPackets Sent: %d\n\tPackets Expected: %d",
					m_batchResultData.packetsSent,
					m_batchResultData.packetsAttemptedToSend);
			}

			//If result from sending action was specified as unrecoverable, then log error and start disconnect.
			if (!m_batchResultData.isRecoverable)
			{
				DisconnectArgs args;
				args.disconnectReasonText = "Hit unrecoverable error in sending packet: " + m_batchResultData.unrecoverableReasonStr + 
					", Encode result: " + m_batchResultData.lastSendResult.encodeResult.reason;
				args.clearQueue = true;

				m_eventDeferrer.defer([&, reasonText = args.disconnectReasonText.c_str(), res = m_batchResultData.lastSendResult]() {m_errorEvent({ ClientErrorCode::Failed_Sending_Packet, reasonText }, std::move(res)); });
				handleInternalDisconnect(packets::DisconnectReasonCode::UNSPECIFIED_ERROR, args);
			}
		}

		void MqttClient::tickReceivePackets()
		{
			//Check, decode, and handle received packets.
			const DecodeResult result{ m_receiveQueue.receiveNextBatch() };
			if (!result.isSuccess())
			{
				handleDecodeError(result);
			}
		}

		void MqttClient::tickPendingPublishMessageRetries()
		{
			const auto& msgs = m_connectionInfo.sessionState.messages();

			for(const auto& msg : msgs)
			{
				if (msg.nextRetryTime < std::chrono::steady_clock::now())
				{
					packets::PacketType type{ msg.getRetryPacketType() };

					LogTrace("MqttClient", "Retrying message: Type: %s, Packet ID: %d, Topic: %s",
						packets::packetTypeToString(type),
						msg.data.packetID,
						msg.data.publishMsgData.topic);

					if (type == packets::PacketType::PUBLISH)
					{
						PublishOptions options{ msg.data.publishMsgData.options };
						ByteBuffer payloadCopy{ msg.data.publishMsgData.payload.size()};
						payloadCopy.append(msg.data.publishMsgData.payload);

						m_sendQueue.addToQueue(std::make_unique<SendPublishJob>(&m_connectionInfo,
							[this](const packets::BasePacket& packet) {return sendPacket(packet); },
							&m_packetIdPool,
							msg.data.packetID,
							msg.data.publishMsgData.topic,
							std::move(payloadCopy),
							std::move(options)));
					}
					else if (type == packets::PacketType::PUBLISH_RECEIVED)
					{
						//TODO handle retrying PUBLISH_RECEIVED packet.
					}
					else if(type == packets::PacketType::PUBLISH_RELEASED)
					{
						//TODO handle retrying PUBLISH_RELEASED packet.
					}
				}
				else
				{
					break; //Messages are sorted in a way that next retry time will be higher anyway, so if we hit one that is not ready to be retried, we can stop checking the rest.
				}
			}
		}

		void MqttClient::handleFailedReconnect(packets::ConnectAck&& packet)
		{
			LogDebug("MqttClient", "Client could not reconnect. ConnectReasonCode: %d", static_cast<std::uint8_t>(packet.getVariableHeader().reasonCode));
			m_socket->close();
			m_receiveQueue.clear();

			if (m_connectionInfo.reconnectAddress.tryCycleToNextPrimaryAddress())
			{
				reconnect();
			}
			else
			{
				m_sendQueue.clearQueue();
				m_connectionStatus = ConnectionStatus::DISCONNECTED;
				m_eventDeferrer.defer([&, p = std::move(packet)]() {m_reconnectEvent({ ReconnectionStatus::FAILED, false, 0, ClientErrorCode::No_Error }, p); });

				if (m_connectionInfo.hasBeenConnected)
				{
					m_eventDeferrer.defer([&]() {m_disconnectEvent({ DisconnectReasonCode::NORMAL_DISCONNECTION, false, false, ClientErrorCode::No_Error }); });
				}
			}
		}

		void MqttClient::handleFailedConnect(packets::ConnectAck&& packet)
		{
			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			LogDebug("MqttClient", "Client could not connect to broker. ConnectReasonCode: %d", static_cast<std::uint8_t>(packet.getVariableHeader().reasonCode));

			m_socket->close();
			m_receiveQueue.clear();

			const bool brokerRedirectionStarted = tryStartBrokerRedirection(
				static_cast<std::uint8_t>(packet.getVariableHeader().reasonCode),
				packet.getVariableHeader().properties);

			if (brokerRedirectionStarted)
			{
				return;
			}

			m_sendQueue.clearQueue();
			m_connectionInfo.hasBeenConnected = false;

			m_eventDeferrer.defer([&, p = std::move(packet)]() {m_connectEvent({ false, true, ClientErrorCode::No_Error }, p); });
		}

		void MqttClient::handleTimeOutConnect()
		{
			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			LogDebug("MqttClient", "Client could not connect to broker. Connection timed out");

			m_socket->setOnDisconnectCallback(nullptr);
			m_socket->close();
			m_receiveQueue.clear();
			m_sendQueue.clearQueue();
			m_connectionInfo.hasBeenConnected = false;

			m_eventDeferrer.defer([&]() {m_connectEvent({ false, false, ClientErrorCode::TimeOut }, {}); });
		}

		void MqttClient::handleTimeOutReconnect()
		{
			LogDebug("MqttClient", "Client could not reconnect. Connection timed out.");
			m_socket->setOnDisconnectCallback(nullptr);
			m_socket->close();
			m_receiveQueue.clear();

			if (m_connectionInfo.reconnectAddress.tryCycleToNextPrimaryAddress())
			{
				reconnect();
			}
			else
			{
				m_sendQueue.clearQueue();
				m_connectionStatus = ConnectionStatus::DISCONNECTED;
				m_eventDeferrer.defer([&]() {m_reconnectEvent({ ReconnectionStatus::FAILED, false, 0, ClientErrorCode::No_Error }, {}); });

				if (m_connectionInfo.hasBeenConnected)
				{
					m_eventDeferrer.defer([&]() {m_disconnectEvent({ DisconnectReasonCode::NORMAL_DISCONNECTION, false, false, ClientErrorCode::TimeOut }); });
				}
			}
		}

		void MqttClient::handleDecodeError(const DecodeResult& result) noexcept
		{
			DisconnectArgs args{ false, true };
			args.disconnectReasonText = result.reason;

			LogInfo("MqttClient", "Failed to decode received packet. Packet Type: %d, Reason: %s", static_cast<std::uint8_t>(result.packetType), result.reason.c_str());

			if (m_connectionStatus == ConnectionStatus::CONNECTING)
			{
				const bool hasAck{ result.packetType == packets::PacketType::CONNECT_ACKNOWLEDGE };

				m_eventDeferrer.defer([&, hA = hasAck, p = std::move(packets::ConnectAck{})]() {m_connectEvent({ false, hA, ClientErrorCode::Failed_Decoding_Packet }, p); });
			}

			m_eventDeferrer.defer([&]() {m_errorEvent({ ClientErrorCode::Failed_Decoding_Packet, args.disconnectReasonText.c_str() }, {}); });
			handleInternalDisconnect(result.getDisconnectReason(), args);
		}

		void MqttClient::pubAck(PacketID packetId, packets::PubAckReasonCode code, PubAckOptions&& options) noexcept
		{
			m_sendQueue.addToQueue(std::make_unique<SendPubAckJob>(&m_connectionInfo,
				[this](const packets::BasePacket& packet) {return sendPacket(packet); },
				packetId,
				code, 
				std::move(options)));
		}

		bool MqttClient::tryStartBrokerRedirection(std::uint8_t failedConnectionReasonCode, const packets::Properties& properties) noexcept
		{
			static_assert(static_cast<std::uint8_t>(packets::ConnectReasonCode::SERVER_MOVED) == static_cast<std::uint8_t>(packets::DisconnectReasonCode::SERVER_MOVED),
				"ConnectReasonCode::SERVER_MOVED value no longer matches DisconnectReasonCode::SERVER_MOVED value: Required `tryBrokerRedirection()` logic to be amended.");
			static_assert(static_cast<std::uint8_t>(packets::ConnectReasonCode::USE_ANOTHER_SERVER) == static_cast<std::uint8_t>(packets::DisconnectReasonCode::USE_ANOTHER_SERVER),
				"ConnectReasonCode::USE_ANOTHER_SERVER value no longer matches DisconnectReasonCode::USE_ANOTHER_SERVER value: Required `tryBrokerRedirection()` logic to be amended.");

			static constexpr std::uint8_t SERVER_MOVED_VAL = static_cast<std::uint8_t>(packets::DisconnectReasonCode::SERVER_MOVED);
			static constexpr std::uint8_t USE_ANOTHER_SERVER_VAL = static_cast<std::uint8_t>(packets::DisconnectReasonCode::USE_ANOTHER_SERVER);

			if (failedConnectionReasonCode == SERVER_MOVED_VAL || failedConnectionReasonCode == USE_ANOTHER_SERVER_VAL)
			{
				const UTF8String* serverRef;
				const bool hasServerReference{ properties.tryGetProperty(PropertyType::SERVER_REFERENCE, serverRef) };

				if (hasServerReference)
				{
					if (serverRef->stringSize() != 0)
					{
						std::vector<Address> addresses = Address::toAddress(serverRef->getString().c_str());

						if (failedConnectionReasonCode == SERVER_MOVED_VAL)
						{
							LogInfo("MqttClient", "Disconnect packet contains a SERVER_REFERENCE for new temporary server: ", serverRef->getString().c_str());

							m_connectionInfo.reconnectAddress.addAddresses(addresses);
							m_connectionInfo.reconnectAddress.tryCycleToNextPrimaryAddress();
						}
						else
						{
							LogInfo("MqttClient", "Disconnect packet contains a SERVER_REFERENCE for new permanent server: ", serverRef->getString().c_str());
							m_connectionInfo.connectAddress.primaryAddress = addresses[0];
							m_connectionInfo.reconnectAddress.primaryAddress = addresses[0];
							addresses.erase(addresses.begin());

							m_connectionInfo.reconnectAddress.addAddresses(addresses);
						}

						reconnect();
						return true;
					}
					else
					{
						LogWarning("MqttClient", "Disconnect packet contains an empty SERVER_REFERENCE property!");
					}
				}

				if (m_connectionInfo.reconnectAddress.tryCycleToNextPrimaryAddress())
				{
					LogInfo("MqttClient",
						"Disconnect reason is %s but no SERVER_REFERENCE, will try reconnect using additionally provided addresses. ",
						failedConnectionReasonCode == SERVER_MOVED_VAL ? "SERVER_MOVED" : "USE_ANOTHER_SERVER");

					reconnect();
					return true;
				}
			}

			return false;
		}

		void MqttClient::reconnect()
		{
			m_eventDeferrer.defer([&]() {m_reconnectEvent({ ReconnectionStatus::RECONNECTING, false, 0, ClientErrorCode::No_Error }, packets::ConnectAck{}); });

			m_connectionStatus = ConnectionStatus::RECONNECTING;
			m_receiveQueue.clear();

			LogDebug("MqttClient", "Starting MQTT re-connection proccess.");
			LogInfo("MqttClient",
				"Hostname: %s\n\tPort: %s\n\tUsername: %s\n\tClientId: %s\n\tVersion: %d\n\tProtocol Name: %s",
				m_connectionInfo.reconnectAddress.primaryAddress.hostname().c_str(),
				m_connectionInfo.reconnectAddress.primaryAddress.port().c_str(),
				m_connectionInfo.connectArgs.username.c_str(),
				m_connectionInfo.connectArgs.clientId.c_str(),
				static_cast<std::uint8_t>(m_connectionInfo.connectArgs.version),
				m_connectionInfo.connectArgs.protocolName.c_str());

			LogInfo("MqttClient", "Socket attempting to connect.");
			m_socket->connect(m_connectionInfo.reconnectAddress.primaryAddress.hostname(), m_connectionInfo.reconnectAddress.primaryAddress.port());

			m_connectionInfo.connectionStartTime = std::chrono::steady_clock::now();
		}

		void MqttClient::handleInternalDisconnect(packets::DisconnectReasonCode reason, const DisconnectArgs& args) noexcept
		{
			LogInfo("MqttClient", "Starting internal disconnect: %s", args.disconnectReasonText.c_str());

			//Bypass send queue and send packet ASAP to not wait for next tick before performing disconnect.
			Disconnect packet{ createDisconnectPacket(m_connectionInfo, args, reason) };
			auto encodeResult{ packet.encode() };

			if (encodeResult.isSuccess())
			{
				if (sendPacket(packet) != 0)
				{
					LogWarning("MqttClient", "Failed to send disconnect packet during internal disconnect. Skipping sending to broker.");
				}
			}
			else
			{
				LogWarning("MqttClient", "Failed to encode disconnect packet during internal disconnect. Skipping sending to broker.");
			}

			//TODO add graceful disconnect handling, e.g. wait for all packets to be sent before closing socket. Means we cant just clear the queue here.
			//Maybe new function.
			if (args.clearQueue)
			{
				m_sendQueue.clearQueue();
			}

			m_connectionStatus = ConnectionStatus::DISCONNECTED;

			m_socket->setOnDisconnectCallback(nullptr);

			if (!m_socket->close())
			{
				LogError("MqttClient", "Error closing socket properly during internal disconnect: %d", m_socket->getLastError());

				//TODO maybe self force termination? Add terminate event?
			}

			clearState();

			m_eventDeferrer.defer([&, reasonCode = reason]() {m_disconnectEvent({ reasonCode, false, false, ClientErrorCode::No_Error }); });
		}

		void MqttClient::handleExternalDisconnect(const packets::Disconnect& packet)
		{
			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				return;
			}

			const packets::DisconnectReasonCode code{ packet.getVariableHeader().reasonCode };

			LogInfo("MqttClient",
				"Starting disconnect from received Disconnect packet with reason code: %d",
				static_cast<std::uint8_t>(packet.getVariableHeader().reasonCode));

			const UTF8String* reasonText;
			if (packet.getVariableHeader().properties.tryGetProperty(PropertyType::REASON_STRING, reasonText))
			{
				LogDebug("MqttClient", "Disconnect Reason String: %s", reasonText->getString().c_str());
			}

			m_socket->setOnDisconnectCallback(nullptr);
			m_socket->close();
			m_receiveQueue.clear();

			const bool brokerRedirectionStarted = tryStartBrokerRedirection(static_cast<std::uint8_t>(code), packet.getVariableHeader().properties);
			if (brokerRedirectionStarted)
			{
				return;
			}

			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			m_sendQueue.clearQueue();
			clearState();

			m_eventDeferrer.defer([&, reasonCode = packet.getVariableHeader().reasonCode]() {m_disconnectEvent({ reasonCode, true, false, ClientErrorCode::No_Error }); });
		}

		void MqttClient::handleExternalDisconnect(int closeCode, std::string reason)
		{
			LogInfo("MqttClient",
				"Starting external disconnect due to socket error code: %d, reason: %s",
				closeCode,
				reason.c_str());

			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			m_socket->close();
			m_sendQueue.clearQueue();
			clearState();

			m_eventDeferrer.defer([&]() {m_disconnectEvent({ packets::DisconnectReasonCode::UNSPECIFIED_ERROR, true, false, ClientErrorCode::Socket_Error }); });
		}

		void MqttClient::clearState() noexcept
		{
			for (auto& pSub : m_connectionInfo.pendingSubscriptions)
			{
				m_packetIdPool.releaseId(pSub.packetId);
			}

			m_connectionInfo.clear();
		}

		int MqttClient::sendPacket(const packets::BasePacket& packet)
		{
			LogInfo("MqttClient",
				"Sending packet. PacketType: %s, Size: %d",
				packetTypeToString(packet.getPacketType()),
				packet.getFixedHeader().getEncodedBytesSize() + packet.getFixedHeader().remainingLength.uint32Value());

			const ByteBuffer* bufferPtr{ packet.getDataBuffer() };
			if (bufferPtr != nullptr)
			{
				if (m_socket->send(*bufferPtr) == true)
				{
					LogTrace("MqttClient", "Packet sent, Bytes: %s.", bufferPtr->toString().c_str());
					return 0;
				}

				LogError("MqttClient", "Sending packet failed at socket level: %d", m_socket->getLastError());

				return m_socket->getLastError();
			}
			else
			{
				LogError("MqttClient", "Cannot send packet, buffer is nullptr. Packet Type: %d", static_cast<std::uint8_t>(packet.getPacketType()));
				return -1;
			}
		}

		void MqttClient::handleSocketConnectEvent(bool success)
		{
			if (success)
			{
				LogDebug("MqttClient", "Socket connected!");

				ConAckCallback conAckCallback{ std::bind(&MqttClient::handleReceivedConnectAcknowledge, this, std::placeholders::_1) };
				DisconnectCallback disconnectCallback{std::bind(&MqttClient::handleReceivedDisconnect, this, std::placeholders::_1) };
				PubCallback pubCallback{ std::bind(&MqttClient::handleReceivedPublish, this, std::placeholders::_1) };
				PingRespCallback pingRespCallback{ std::bind(&MqttClient::handleReceivedPingResponse, this, std::placeholders::_1) };
				PubAckCallback pubAckCallback{ std::bind(&MqttClient::handleReceivedPublishAck, this, std::placeholders::_1) };
				SubAckCallback subAckCallback{ std::bind(&MqttClient::handleReceivedSubscribeAcknowledge, this, std::placeholders::_1) };
				UnSubAckCallback unSubAckCallback{ std::bind(&MqttClient::handleReceivedUnSubscribeAcknowledge, this, std::placeholders::_1) };
				//TODO puback receive
				//TODO suback receive

				m_receiveQueue.setConnectAcknowledgeCallback(conAckCallback);
				m_receiveQueue.setDisconnectCallback(disconnectCallback);
				m_receiveQueue.setPublishCallback(pubCallback);
				m_receiveQueue.setPingResponseCallback(pingRespCallback);
				m_receiveQueue.setPublishAcknowledgeCallback(pubAckCallback);
				m_receiveQueue.setSubscribeAcknowledgeCallback(subAckCallback);
				m_receiveQueue.setUnSubscribeAcknowledgeCallback(unSubAckCallback);

				m_sendQueue.addToQueue(std::make_unique<SendConnectJob>(&m_connectionInfo,
					[this](const packets::BasePacket& packet) {return sendPacket(packet); }));

				LogTrace("MqttClient", "Connect packet send request queued, to be resolved on next tick.");
			}
			else
			{
				m_connectionStatus = ConnectionStatus::DISCONNECTED;
				LogError("MqttClient", "Socket failed to connect!");

				if (m_connectionStatus == ConnectionStatus::RECONNECTING)
				{
					reconnect();
				}
				else
				{
					//TODO maybe try reconnect to other addresses if provided rather than straight disconnect?

					m_eventDeferrer.defer([&, p = std::move(packets::ConnectAck{})]() {m_connectEvent({ false, false, ClientErrorCode::Socket_Connect_Failed }, p); });
				}
			}
		}

		void MqttClient::handleSocketDisconnectEvent()
		{
			handleExternalDisconnect(m_socket->getLastError(), m_socket->getLastCloseReason());
		}

        void MqttClient::handleSocketPacketReceivedEvent(ByteBuffer&& buffer)
		{
			LockGuard guard{ m_receiverMutex };

			ByteBuffer fullBuffer{ buffer.size() + m_leftOverBuffer.size()};
			fullBuffer.append(m_leftOverBuffer.bytes(), m_leftOverBuffer.size());
			fullBuffer.append(buffer.bytes(), buffer.size());

			std::vector<ByteBuffer> packets;
			std::size_t leftOver{ 0U };

			if (separateMqttPacketByteBuffers(fullBuffer, packets, leftOver))
			{
				for(auto& packetBuffer : packets)
				{
					m_receiveQueue.addToQueue(std::move(packetBuffer));
				}
			}

			if (leftOver > 0)
			{
				m_leftOverBuffer = ByteBuffer{ leftOver };
				m_leftOverBuffer.append(fullBuffer.bytes() + fullBuffer.size() - leftOver, leftOver);
			}
		}

		void MqttClient::handleSocketErrorEvent(int /*error*/)
		{
		}
	}
}