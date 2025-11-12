#include "cleanMqtt/Mqtt/MqttClientImpl.h"

#include "cleanMqtt/MqttClientOptions.h"
#include <cleanMqtt/Mqtt/Transport/Jobs/ConnectComposer.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/PublishComposer.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/PingComposer.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/PubAckComposer.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/SubscribeComposer.h>
#include <cleanMqtt/Mqtt/Transport/Jobs/UnSubscribeComposer.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>
#include <cleanMqtt/Logger/LoggerInstance.h>
#include <cleanMqtt/Logger/DefaultLogger.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		MqttClientImpl::MqttClientImpl(const IMqttEnvironment* const env, const MqttClientOptions& clientOptions)
			: m_config(env->createConfig()),
			m_socket(env->createWebSocket()),
			m_clientOptions{clientOptions}
		{
			if (getLogger() == nullptr)
			{
				setLogger(new DefaultLogger());
			}

			assert(m_socket != nullptr);

			m_socket->setOnConnectCallback([this](bool success) {handleSocketConnectEvent(success); });
			m_socket->setOnDisconnectCallback([this]() { handleSocketDisconnectEvent(); });
			m_socket->setOnRecvdCallback([this](ByteBuffer&& buffer) { handleSocketDataReceivedEvent(std::move(buffer)); });
			m_socket->setOnErrorCallback([this](std::uint16_t error) { handleSocketErrorEvent(error); });

			m_sendPubAckEvent.add([this](std::uint16_t packetId) { pubAck(packetId, PubAckReasonCode::SUCCESS, PubAckOptions{}); });

			m_sendQueue.setSocket(m_socket);
			m_sendQueue.setOnPingSentCallback([this]() { handlePingSentEvent(); });
		}

		MqttClientImpl::~MqttClientImpl()
		{
			shutdown();

			if (m_socket != nullptr)
			{
				m_socket->close();
			}
		}

		ClientError MqttClientImpl::connect(ConnectArgs&& args, ConnectAddress&& address) noexcept
		{
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
			}

			m_mqttMainThreadCondition.notify_all();

			return ClientErrorCode::No_Error;
		}

		ClientError MqttClientImpl::publish(const char* topic, ByteBuffer&& payload, PublishOptions&& options) noexcept
		{
			LogTrace("MqttClient", "Started publish(): Topic: %s", topic);

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				LogError("MqttClient", "Client not connected, cannot publish()!");
				return { ClientErrorCode::Not_Connected, "Client not connected, cannot publish()!" };
			}

			if (options.topicAlias > m_connectionInfo.maxServerTopicAlias)
			{
				LogError("MqttClient", "Topic alias exceeds `max server topic alias` received from broker.");
			}

			std::uint16_t packetId{ 0U };
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

			m_sendQueue.addToQueue(std::make_unique<PublishComposer>(&m_connectionInfo,
				&m_packetIdPool,
				packetId,
				topic,
				std::move(payload),
				std::move(options)));

			LogInfo("MqttClient", "Queued publish message for sending, Topic: %s, Packet ID: %d, QOS: %d", topic, packetId, static_cast<std::uint8_t>(options.qos));

			m_mqttMainThreadCondition.notify_all();

			return ClientErrorCode::No_Error;
		}

		ClientError MqttClientImpl::subscribe(const std::vector<Topic>& topics, SubscribeOptions&& options) noexcept
		{
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

			{
				LockGuard guard{ m_mutex };
				m_connectionInfo.pendingSubscriptions.push_back(PendingSubscription{ id, topics });
			}

			m_sendQueue.addToQueue(std::make_unique<SubscribeComposer>(&m_connectionInfo,
				&m_packetIdPool,
				id,
				topics,
				std::move(options)));

			LogTrace("MqttClient", "Started subscribe: Subscribing to; %s", allTopicsToStr(topics).c_str());
			m_mqttMainThreadCondition.notify_all();

			return ClientErrorCode::No_Error;
		}

		ClientError MqttClientImpl::unSubscribe(const std::vector<Topic>& topics, UnSubscribeOptions&& options) noexcept
		{
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

			{
				LockGuard guard{ m_mutex };
				m_connectionInfo.pendingUnSubscriptions.push_back(PendingUnSubscription{ id, topics });
			}

			m_sendQueue.addToQueue(std::make_unique<UnSubscribeComposer>(&m_connectionInfo,
				&m_packetIdPool,
				id,
				topics,
				std::move(options)));

			LogTrace("MqttClient", "Started unSubscribe: Unsubscribing from %s", allTopicsToStr(topics).c_str());

			m_mqttMainThreadCondition.notify_all();

			return ClientErrorCode::No_Error;
		}

		ClientError MqttClientImpl::disconnect(DisconnectArgs&& args) noexcept
		{
			if (m_connectionStatus == ConnectionStatus::DISCONNECTED)
			{
				LogWarning("MqttClient", "Client already disconnected, cannot disconnect()!");
				return { ClientErrorCode::Not_Connected, "Client not connected, cannot disconnect()!" };
			}

			handleInternalDisconnect(
				args.willPublish ? DisconnectReasonCode::DISCONNECT_WITH_WILL_MESSAGE : DisconnectReasonCode::NORMAL_DISCONNECTION,
				args);

			m_mqttMainThreadCondition.notify_all();

			return ClientErrorCode::No_Error;
		}

		ClientError MqttClientImpl::shutdown() noexcept
		{
			if (m_clientOptions.getTickMode() == TickMode::ASYNC)
			{
				return shutdownAsync();
			}

			if (m_connectionStatus == ConnectionStatus::DISCONNECTED)
			{
				return { ClientErrorCode::MQTT_Not_Active };
			}

			assert(m_socket != nullptr);

			LogInfo("MqttClient", "Client shutdown.");

			shutdownCleanup();

			return ClientErrorCode::No_Error;
		}

		ClientError MqttClientImpl::shutdownAsync() noexcept
		{
			LogInfo("MqttClient", "Client shutting down async.");

			bool expectedVal{ true };
			if (!m_isRunningAsync.compare_exchange_strong(expectedVal, false))
			{
				LogInfo("MqttClient", "Client not running asynchronously, cannot shutdownAsync(). Ignoring.");
				return ClientErrorCode::No_Error;
			}

			m_mqttMainThreadCondition.notify_all();

			if (m_mqttMainThread.joinable()) 
			{
				try 
				{
					m_mqttMainThread.join();
				}
				catch (...) 
				{
				}
			}

			return ClientErrorCode::No_Error;
		}

		ClientError MqttClientImpl::shutdownCleanup() noexcept
		{
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

		ClientError MqttClientImpl::tick() noexcept
		{
			assert(m_clientOptions.getTickMode() == TickMode::SYNC && "tick() should only be called when client is not configured to tick asynchronously.");

			if (m_clientOptions.getTickMode() == TickMode::ASYNC)
			{
				LogWarning("MqttClient", "Cannot call tick() when client is configured to tick asynchronously (tickAsync set as true in constructor).");
				return ClientErrorCode::Using_Tick_Async;
			}

			assert(m_socket != nullptr);

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

			if (m_clientOptions.isUsingInternalCallbackDeferrer())
			{
				LockGuard guard{ m_mutex };
				m_eventDeferrer.invokeEvents();
			}

			return ClientErrorCode::No_Error;
		}

        void MqttClientImpl::tickAsync() noexcept
		{
			assert(m_clientOptions.getTickMode() == TickMode::ASYNC && "tickAsync() should only be called when client is configured to tick asynchronously.");

			if (m_isRunningAsync.exchange(true))
			{
				LogWarning("MqttClient", "tickAsync() called while already ticking asynchronously.");
				return;
			}

			try
			{
				m_mqttMainThread = std::thread([this]() {
					while (true)
					{
						{
							std::unique_lock<std::mutex> lock{ m_tickMutex };

							m_mqttMainThreadCondition.wait_for(lock, std::chrono::milliseconds(m_config.tickAsyncWaitForMS), [this] {
								return !m_isRunningAsync;
								});
						}

						if (!m_isRunningAsync)
						{
							LogInfo("MqttClient", "Client shutdown.");
							shutdownCleanup();
							break; //Exit thread loop
						}

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

						if(m_clientOptions.isUsingInternalCallbackDeferrer())
						{
							LockGuard guard{ m_mutex };
							m_eventDeferrer.invokeEvents();
						}
					}
					});
			}
			catch (const std::exception& e)
			{
				LogError("MqttClient", "Exception in tickAsync thread: %s", e.what());
				shutdownAsync();
				m_isRunningAsync = false;
			}
			catch (...)
			{
				LogError("MqttClient", "Unknown exception in tickAsync thread.");
				shutdownAsync();
				m_isRunningAsync = false;
			}
		}

		ErrorEvent& MqttClientImpl::onErrorEvent() noexcept
		{
			return m_errorEvent;
		}

		ConnectEvent& MqttClientImpl::onConnectEvent() noexcept
		{
			return m_connectEvent;
		}

		DisconnectEvent& MqttClientImpl::onDisconnectEvent() noexcept
		{
			return m_disconnectEvent;
		}

		ReconnectEvent& MqttClientImpl::onReconnectEvent() noexcept
		{
			return m_reconnectEvent;
		}

		PublishEvent& MqttClientImpl::onPublishEvent() noexcept
		{
			return m_publishEvent;
		}

		PublishAckEvent& MqttClientImpl::onPublishAckEvent() noexcept
		{
			return m_pubAckEvent;
		}

		SubscribeAckEvent& MqttClientImpl::onSubscribeAckEvent() noexcept
		{
			return m_subAckEvent;
		}

		UnSubscribeAckEvent& MqttClientImpl::onUnSubscribeAckEvent() noexcept
		{
			return m_unSubAckEvent;
		}

		ConnectionStatus MqttClientImpl::getConnectionStatus() const noexcept
		{
			return m_connectionStatus;
		}

		const MqttConnectionInfo& MqttClientImpl::getConnectionInfo() const noexcept
		{
			return m_connectionInfo;
		}

		bool MqttClientImpl::getIsTickingAsync() const noexcept
		{
			return m_clientOptions.getTickMode() == TickMode::ASYNC;
		}

		void MqttClientImpl::pubAck(std::uint16_t packetId, PubAckReasonCode code, PubAckOptions&& options) noexcept
		{
			m_sendQueue.addToQueue(std::make_unique<PubAckComposer>(&m_connectionInfo,
				packetId,
				code,
				std::move(options)));
		}

		bool MqttClientImpl::tryStartBrokerRedirection(std::uint8_t failedConnectionReasonCode, const Properties& properties) noexcept
		{
			static_assert(static_cast<std::uint8_t>(ConnectReasonCode::SERVER_MOVED) == static_cast<std::uint8_t>(DisconnectReasonCode::SERVER_MOVED),
				"ConnectReasonCode::SERVER_MOVED value no longer matches DisconnectReasonCode::SERVER_MOVED value: Required `tryBrokerRedirection()` logic to be amended.");
			static_assert(static_cast<std::uint8_t>(ConnectReasonCode::USE_ANOTHER_SERVER) == static_cast<std::uint8_t>(DisconnectReasonCode::USE_ANOTHER_SERVER),
				"ConnectReasonCode::USE_ANOTHER_SERVER value no longer matches DisconnectReasonCode::USE_ANOTHER_SERVER value: Required `tryBrokerRedirection()` logic to be amended.");

			static constexpr std::uint8_t SERVER_MOVED_VAL = static_cast<std::uint8_t>(DisconnectReasonCode::SERVER_MOVED);
			static constexpr std::uint8_t USE_ANOTHER_SERVER_VAL = static_cast<std::uint8_t>(DisconnectReasonCode::USE_ANOTHER_SERVER);

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

		void MqttClientImpl::reconnect()
		{
			DISPATCH_EVENT_TO_CONSUMER([&]() {m_reconnectEvent({ ReconnectionStatus::RECONNECTING, false, 0, ClientErrorCode::No_Error }, ConnectAck{}); });

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

		void MqttClientImpl::handleInternalDisconnect(DisconnectReasonCode reason, const DisconnectArgs& args) noexcept
		{
			{
				LockGuard guard{ m_mutex };

				LogInfo("MqttClient", "Starting internal disconnect: %s", args.disconnectReasonText.c_str());

				if (m_connectionStatus == ConnectionStatus::DISCONNECTED)
				{
					LogWarning("MqttClient", "Client not connected, cannot perform internal disconnect. Ignoring.");
					return;
				}

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
			}

			DISPATCH_EVENT_TO_CONSUMER([&, reasonCode = reason]() {m_disconnectEvent({ reasonCode, false, false, ClientErrorCode::No_Error }); });
		}

		void MqttClientImpl::handleExternalDisconnect(const Disconnect& packet)
		{
			{
				LockGuard guard{ m_mutex };

				if (m_connectionStatus == ConnectionStatus::DISCONNECTED)
				{
					LogWarning("MqttClient", "Client not connected, cannot perform internal disconnect. Ignoring.");
					return;
				}

				const DisconnectReasonCode code{ packet.getVariableHeader().reasonCode };

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
			}

			DISPATCH_EVENT_TO_CONSUMER([&, reasonCode = packet.getVariableHeader().reasonCode]() {m_disconnectEvent({ reasonCode, true, false, ClientErrorCode::No_Error }); });
		}

		void MqttClientImpl::handleExternalDisconnect(int closeCode, std::string reason)
		{
			{
				LockGuard guard{ m_mutex };

				if (m_connectionStatus == ConnectionStatus::DISCONNECTED)
				{
					LogWarning("MqttClient", "Client not connected, cannot perform internal disconnect. Ignoring.");
					return;
				}

				LogInfo("MqttClient",
					"Starting external disconnect due to socket error code: %d, reason: %s",
					closeCode,
					reason.c_str());

				m_connectionStatus = ConnectionStatus::DISCONNECTED;
				m_socket->close();
				m_sendQueue.clearQueue();
				clearState();
			}

			DISPATCH_EVENT_TO_CONSUMER([&]() {m_disconnectEvent({ DisconnectReasonCode::UNSPECIFIED_ERROR, true, false, ClientErrorCode::Socket_Error }); });
		}

		void MqttClientImpl::clearState() noexcept
		{
			for (auto& pSub : m_connectionInfo.pendingSubscriptions)
			{
				m_packetIdPool.releaseId(pSub.packetId);
			}

			m_connectionInfo.clear();
		}

		void MqttClientImpl::handleSocketConnectEvent(bool success)
		{
			if (success)
			{
				LogDebug("MqttClient", "Socket connected!");

				ConAckCallback conAckCallback{ std::bind(&MqttClientImpl::handleReceivedConnectAcknowledge, this, std::placeholders::_1) };
				DisconnectCallback disconnectCallback{ std::bind(&MqttClientImpl::handleReceivedDisconnect, this, std::placeholders::_1) };
				PubCallback pubCallback{ std::bind(&MqttClientImpl::handleReceivedPublish, this, std::placeholders::_1) };
				PingRespCallback pingRespCallback{ std::bind(&MqttClientImpl::handleReceivedPingResponse, this, std::placeholders::_1) };
				PubAckCallback pubAckCallback{ std::bind(&MqttClientImpl::handleReceivedPublishAck, this, std::placeholders::_1) };
				SubAckCallback subAckCallback{ std::bind(&MqttClientImpl::handleReceivedSubscribeAcknowledge, this, std::placeholders::_1) };
				UnSubAckCallback unSubAckCallback{ std::bind(&MqttClientImpl::handleReceivedUnSubscribeAcknowledge, this, std::placeholders::_1) };
				//TODO puback receive

				m_receiveQueue.setConnectAcknowledgeCallback(conAckCallback);
				m_receiveQueue.setDisconnectCallback(disconnectCallback);
				m_receiveQueue.setPublishCallback(pubCallback);
				m_receiveQueue.setPingResponseCallback(pingRespCallback);
				m_receiveQueue.setPublishAcknowledgeCallback(pubAckCallback);
				m_receiveQueue.setSubscribeAcknowledgeCallback(subAckCallback);
				m_receiveQueue.setUnSubscribeAcknowledgeCallback(unSubAckCallback);

				m_sendQueue.addToQueue(std::make_unique<ConnectComposer>(&m_connectionInfo));

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

					//DISPATCH_EVENT_TO_CONSUMER([&, p = std::move(ConnectAck{})]() {m_connectEvent({ false, false, ClientErrorCode::Socket_Connect_Failed }, p); });
				}
			}
		}

		void MqttClientImpl::handleSocketDisconnectEvent()
		{
			handleExternalDisconnect(m_socket->getLastError(), m_socket->getLastCloseReason());
		}

		void MqttClientImpl::handleSocketDataReceivedEvent(ByteBuffer&& buffer)
		{
			LockGuard guard{ m_receiverMutex };

			ByteBuffer fullBuffer{ buffer.size() + m_leftOverBuffer.size() };
			fullBuffer.append(m_leftOverBuffer.bytes(), m_leftOverBuffer.size());
			fullBuffer.append(buffer.bytes(), buffer.size());

			std::vector<ByteBuffer> packets;
			std::size_t leftOver{ 0U };

			if (separateMqttPacketByteBuffers(fullBuffer, packets, leftOver))
			{
				for (auto& packetBuffer : packets)
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

		void MqttClientImpl::handleSocketErrorEvent(int /*error*/)
		{
			// TODO: Implement handleSocketErrorEvent
		}

		void MqttClientImpl::handlePingSentEvent()
		{
			m_connectionInfo.lastPingReqSentTime = std::chrono::steady_clock::now();
			m_connectionInfo.awaitingPingResponse = true;
		}

		void MqttClientImpl::handleReceivedConnectAcknowledge(ConnectAck&& packet)
		{
			//Connection counts as succeeded for any ConnectReasonCode < 128U so report back as connection success, else start failure proccess.
			if (packet.getVariableHeader().reasonCode < ConnectReasonCode::UNSPECIFIED_ERROR)
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

					DISPATCH_EVENT_TO_CONSUMER([&, p = std::move(packet)]() {m_reconnectEvent({ ReconnectionStatus::SUCCEEDED, true, 0, ClientErrorCode::No_Error }, p); });
				}
				else
				{
					LogDebug("MqttClient", "Client connected to broker: %s Port: %s",
						m_connectionInfo.connectAddress.primaryAddress.hostname().c_str(),
						m_connectionInfo.reconnectAddress.primaryAddress.port().c_str());

					DISPATCH_EVENT_TO_CONSUMER([&, p = std::move(packet)]() {m_connectEvent({ true, true, ClientErrorCode::No_Error }, p); });
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

		void MqttClientImpl::handleReceivedDisconnect(Disconnect&& packet)
		{
			handleExternalDisconnect(packet);
		}

		void MqttClientImpl::handleReceivedPublish(Publish&& packet)
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
						[this](const BasePacket& packet) {return sendPacket(packet); },
						packetId,
						m_config.enforceMaxPacketSizeOnSend,
						m_config.maxAllowedPacketSize));
				}
				else if (packet.getVariableHeader().qos == Qos::QOS_2)
				{
					m_sendQueue.addToQueue(std::make_unique<SendPublishReceivedJob>(&m_connectionInfo,
						[this](const BasePacket& packet) {return sendPacket(packet); },
						packetId,
						m_config.enforceMaxPacketSizeOnSend,
						m_config.maxAllowedPacketSize));
				}*/
			}
		}

		void MqttClientImpl::handleReceivedPublishAck(PublishAck&& packet)
		{
			const auto packetId{ packet.getVariableHeader().packetId };

			m_connectionInfo.sessionState.removeMessage(packetId);
			m_packetIdPool.releaseId(packetId);

			DISPATCH_EVENT_TO_CONSUMER([&, id = packetId, p = std::move(packet)]() {m_pubAckEvent({ id }, p); });
		}

		void MqttClientImpl::handleReceivedSubscribeAcknowledge(SubscribeAck&& packet)
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

			DISPATCH_EVENT_TO_CONSUMER([&, id = packetId, res = std::move(results), p = std::move(packet)]() {m_subAckEvent({ id, std::move(res) }, p); });
		}

		void MqttClientImpl::handleReceivedUnSubscribeAcknowledge(UnSubscribeAck&& packet)
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

			DISPATCH_EVENT_TO_CONSUMER([&, id = packetId, res = std::move(results), p = std::move(packet)]() {m_unSubAckEvent({ id, std::move(res) }, p); });
		}

		void MqttClientImpl::handleReceivedPingResponse(PingResp&& packet)
		{
			(void)packet;

			m_connectionInfo.awaitingPingResponse = false;
		}

		void MqttClientImpl::firePublishReceivedEvent(Publish&& packet) noexcept
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

					DISPATCH_EVENT_TO_CONSUMER([&, reasonText = args.disconnectReasonText.c_str()]() {m_errorEvent({ ClientErrorCode::Recv_Topic_Alias_Disallowed, reasonText }, {}); });
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

					DISPATCH_EVENT_TO_CONSUMER([&, reasonText = args.disconnectReasonText.c_str()]() {m_errorEvent({ ClientErrorCode::Recv_Invalid_Topic_Alias, reasonText }, {}); });
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

				DISPATCH_EVENT_TO_CONSUMER([&, reasonText = args.disconnectReasonText.c_str()]() {m_errorEvent({ ClientErrorCode::Recv_Empty_Topic_Name_And_Invalid_Alias, reasonText }, {}); });
				handleInternalDisconnect(DisconnectReasonCode::PROTOCOL_ERROR, std::move(args));
				return;
			}

			const auto qos{ packet.getVariableHeader().qos };
			const auto id{ packet.getVariableHeader().packetIdentifier };

			DISPATCH_EVENT_TO_CONSUMER([&, tName = std::move(topicName), pload = &packet.getPayloadHeader().payload, p = std::move(packet)]() {m_publishEvent({ std::move(tName), pload }, p); });

			if (qos == Qos::QOS_1)
			{
				//After packet is sent to application layer (previous deferEvent), send PUBACK packet back to the server.
				DISPATCH_EVENT_TO_CONSUMER([&]() {m_sendPubAckEvent(id); });
			}
		}

		void MqttClientImpl::tickCheckTimeOut()
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

		void MqttClientImpl::tickCheckKeepAlive()
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
					m_sendQueue.addToQueue(std::make_unique<PingComposer>(&m_connectionInfo));
				}
			}
		}

		void MqttClientImpl::tickSendPackets()
		{
			//Send queued encoded packets.
			m_sendQueue.sendNextBatch(m_batchResultData);

			if (m_batchResultData.controlPacketSent)
			{
				m_connectionInfo.lastControlPacketTime = std::chrono::steady_clock::now();
			}

			//If result from sending action was specified as unrecoverable, then log error and start disconnect.
			if (!m_batchResultData.isRecoverable)
			{
				DisconnectArgs args;
				args.disconnectReasonText = "Hit unrecoverable error in sending packet: " + m_batchResultData.unrecoverableReasonStr +
					", Encode result: " + m_batchResultData.lastSendResult.encodeResult.reason;
				args.clearQueue = true;

				m_clientOptions.getCallbackDispatcher()->dispatch([&, reasonText = args.disconnectReasonText.c_str(), res = m_batchResultData.lastSendResult]() {m_errorEvent({ClientErrorCode::Failed_Sending_Packet, reasonText}, std::move(res)); });
				handleInternalDisconnect(DisconnectReasonCode::UNSPECIFIED_ERROR, args);
			}
		}

		void MqttClientImpl::tickReceivePackets()
		{
			//Check, decode, and handle received packets.
			const DecodeResult result{ m_receiveQueue.receiveNextBatch() };
			if (!result.isSuccess())
			{
				handleDecodeError(result);
			}
		}

		void MqttClientImpl::tickPendingPublishMessageRetries()
		{
			const auto& msgs = m_connectionInfo.sessionState.messages();

			for(const auto& msg : msgs)
			{
				if (msg.nextRetryTime < std::chrono::steady_clock::now())
				{
					PacketType type{ msg.getRetryPacketType() };

					LogTrace("MqttClient", "Retrying message: Type: %s, Packet ID: %d, Topic: %s",
						packetTypeToString(type),
						msg.data.packetID,
						msg.data.publishMsgData.topic);

					if (type == PacketType::PUBLISH)
					{
						PublishOptions options{ msg.data.publishMsgData.options };
						ByteBuffer payloadCopy{ msg.data.publishMsgData.payload.size()};
						payloadCopy.append(msg.data.publishMsgData.payload);

						m_sendQueue.addToQueue(std::make_unique<PublishComposer>(&m_connectionInfo,
							&m_packetIdPool,
							msg.data.packetID,
							msg.data.publishMsgData.topic,
							std::move(payloadCopy),
							std::move(options)));
					}
					else if (type == PacketType::PUBLISH_RECEIVED)
					{
						//TODO handle retrying PUBLISH_RECEIVED packet.
					}
					else if(type == PacketType::PUBLISH_RELEASED)
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

		void MqttClientImpl::handleFailedReconnect(ConnectAck&& packet)
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
				DISPATCH_EVENT_TO_CONSUMER([&, p = std::move(packet)]() {m_reconnectEvent({ ReconnectionStatus::FAILED, false, 0, ClientErrorCode::No_Error }, p); });

				if (m_connectionInfo.hasBeenConnected)
				{
					DISPATCH_EVENT_TO_CONSUMER([&]() {m_disconnectEvent({ DisconnectReasonCode::NORMAL_DISCONNECTION, false, false, ClientErrorCode::No_Error }); });
				}
			}
		}

		void MqttClientImpl::handleFailedConnect(ConnectAck&& packet)
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

			DISPATCH_EVENT_TO_CONSUMER([&, p = std::move(packet)]() {m_connectEvent({ false, true, ClientErrorCode::No_Error }, p); });
		}

		void MqttClientImpl::handleTimeOutConnect()
		{
			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			LogDebug("MqttClient", "Client could not connect to broker. Connection timed out");

			m_socket->setOnDisconnectCallback(nullptr);
			m_socket->close();
			m_receiveQueue.clear();
			m_sendQueue.clearQueue();
			m_connectionInfo.hasBeenConnected = false;

			DISPATCH_EVENT_TO_CONSUMER([&]() {m_connectEvent({ false, false, ClientErrorCode::TimeOut }, {}); });
		}

		void MqttClientImpl::handleTimeOutReconnect()
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
				DISPATCH_EVENT_TO_CONSUMER([&]() {m_reconnectEvent({ ReconnectionStatus::FAILED, false, 0, ClientErrorCode::No_Error }, {}); });

				if (m_connectionInfo.hasBeenConnected)
				{
					DISPATCH_EVENT_TO_CONSUMER([&]() {m_disconnectEvent({ DisconnectReasonCode::NORMAL_DISCONNECTION, false, false, ClientErrorCode::TimeOut }); });
				}
			}
		}

		void MqttClientImpl::handleDecodeError(const DecodeResult& result) noexcept
		{
			DisconnectArgs args{ false, true };
			args.disconnectReasonText = result.reason;

			LogInfo("MqttClient", "Failed to decode received packet. Packet Type: %d, Reason: %s", static_cast<std::uint8_t>(result.packetType), result.reason.c_str());

			if (m_connectionStatus == ConnectionStatus::CONNECTING)
			{
				const bool hasAck{ result.packetType == PacketType::CONNECT_ACKNOWLEDGE };

				DISPATCH_EVENT_TO_CONSUMER([&, hA = hasAck, p = std::move(ConnectAck{})]() {m_connectEvent({ false, hA, ClientErrorCode::Failed_Decoding_Packet }, p); });
			}

			DISPATCH_EVENT_TO_CONSUMER([&]() {m_errorEvent({ ClientErrorCode::Failed_Decoding_Packet, args.disconnectReasonText.c_str() }, {}); });
			handleInternalDisconnect(result.getDisconnectReason(), args);
		}

		int MqttClientImpl::sendPacket(const BasePacket& packet)
		{
			if (m_socket == nullptr)
			{
				LogError("MqttClient", "Cannot send packet, socket is nullptr. Packet Type: %d", static_cast<std::uint8_t>(packet.getPacketType()));
				return -1;
			}

			LogInfo("MqttClient",
				"Sending packet. PacketType: %s, Size: %d",
				packetTypeToString(packet.getPacketType()),
				packet.getFixedHeader().getEncodedBytesSize() + packet.getFixedHeader().remainingLength.uint32Value());

			const ByteBuffer& bufferRef{ packet.getDataBuffer() };

			if (m_socket->send(bufferRef) == bufferRef.size())
			{
				LogTrace("MqttClient", "Packet sent, Bytes: %s.", bufferRef.toString().c_str());
				return 0;
			}

			LogError("MqttClient", "Sending packet failed at socket level: %d", m_socket->getLastError());

			return m_socket->getLastError();
		}
	}
}
