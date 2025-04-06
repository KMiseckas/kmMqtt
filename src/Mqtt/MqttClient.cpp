#include <cleanMqtt/Mqtt/MqttClient.h>

#include <iostream>
#include <cleanMqtt/Mqtt/Packets/PacketUtils.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>
#include <functional>

namespace cleanMqtt
{
	namespace mqtt
	{
		using namespace cleanMqtt::mqtt::packets;
		using Milliseconds = std::chrono::milliseconds;

		MqttClient::MqttClient(const Config config, std::unique_ptr<interfaces::IWebSocket> socket, std::unique_ptr<interfaces::ISendQueue> sendQueue)
			: m_config{ config }, m_socket { std::move(socket) }, m_sendQueue{ std::move(sendQueue) }
		{
			assert(m_socket != nullptr);
			assert(m_sendQueue != nullptr);

			m_socket->setOnConnectCallback([this](bool success){handleSocketConnectEvent(success);});
			m_socket->setOnDisconnectCallback([this]() { handleSocketDisconnectEvent(); });
			m_socket->setOnPacketRecvdCallback([this](ByteBuffer&& buffer) { handleSocketPacketReceivedEvent(std::move(buffer)); });
			m_socket->setOnErrorCallback([this](std::uint16_t error) { handleSocketErrorEvent(error); });
		}

		MqttClient::~MqttClient()
		{
			shutdown();

			m_socket = nullptr;
			m_sendQueue = nullptr;
		}

		void MqttClient::connect(ConnectArgs&& args, ConnectAddress&& address)
		{
			LockGuard guard{ m_mutex };

			if (address.primaryAddress.hostname().empty())
			{
				LogWarning("MqttClient", "Cannot connect() without primary address.");
				return;
			}

			if (args.clientId.empty())
			{
				LogWarning("MqttClient", "Cannot connect() without client ID argument.");
				return;
			}

			if (m_connectionStatus == ConnectionStatus::CONNECTED || m_connectionStatus == ConnectionStatus::CONNECTING)
			{
				LogWarning("MqttClient", "Cannot connect() again while already connected or attempting to connect.");
				return;
			}

			m_receiveQueue.clear();

			LogDebug("MqttClient", "Starting MQTT connection proccess.");
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

			m_socket->connect(m_connectionInfo.connectAddress.primaryAddress.hostname(), m_connectionInfo.connectAddress.primaryAddress.port());

			m_connectionInfo.connectionStartTime = std::chrono::steady_clock::now();
		}

		void MqttClient::publish(const char* topic, const char* payloadMsg)
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				LogWarning("MqttClient", "Client not connected, cannot publish()!");
				return;
			}

			LogTrace("MqttClient", "Started publish(): Topic: %s, Msg: %s", topic, payloadMsg);

			

			(void)topic;
			(void)payloadMsg;
		}

		void MqttClient::subscribe(const char* topic)
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				LogWarning("MqttClient", "Client not connected, cannot subscribe()!");
				return;
			}

			LogTrace("MqttClient", "Started subscribe: Subscribing to %s", topic);

			//TODO

			(void)topic;
		}

		void MqttClient::unSubscribe(const char* topic)
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				LogWarning("MqttClient", "Client not connected, cannot unSubscribe()!");
				return;
			}

			LogTrace("MqttClient", "Started un-subscribe: Unsubscribing from %s", topic);

			//TODO
		}

		void MqttClient::disconnect(DisconnectArgs&& args)
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus == ConnectionStatus::DISCONNECTED)
			{
				LogWarning("MqttClient", "Client already disconnected, cannot disconnect()!");
				return;
			}

			handleInternalDisconnect(
				args.willPublish ? packets::DisconnectReasonCode::DISCONNECT_WITH_WILL_MESSAGE : packets::DisconnectReasonCode::NORMAL_DISCONNECTION,
				args);
		}

		void MqttClient::shutdown() noexcept
		{
			LockGuard guard{ m_mutex };

			LogInfo("MqttClient", "Client shutdown.");

			m_connectionStatus = ConnectionStatus::DISCONNECTED;

			m_eventDeferrer.clear();
			m_connectEvent.removeAll();
			m_disconnectEvent.removeAll();

			m_socket->close();
			m_sendQueue->clearQueue();
			m_receiveQueue.clear();
		}

		void MqttClient::tick(float /*deltaTime*/)
		{
			assert(m_sendQueue != nullptr);

			LockGuard guard{ m_mutex };

			tickCheckTimeOut();

			if (m_connectionStatus != ConnectionStatus::DISCONNECTED)
			{
				if (m_socket->isConnected())
				{
					tickSendPackets();
					tickReceivePackets();
					tickCheckKeepAlive();
				}
			}

			if (m_socket != nullptr && m_socket->isConnected())
			{
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

		void MqttClient::handleReceivedConnectAcknowledge(const ConnectAck& packet)
		{
			//Connection counts as succeeded for any ConnectReasonCode < 128U so report back as connection success, else start failure proccess.
			if (packet.getVariableHeader().reasonCode < packets::ConnectReasonCode::UNSPECIFIED_ERROR)
			{
				if (m_connectionStatus == ConnectionStatus::RECONNECTING)
				{
					LogDebug("MqttClient", "Reconnection complete to broker: %s Port: %s",
						m_connectionInfo.reconnectAddress.primaryAddress.hostname().c_str(),
						m_connectionInfo.reconnectAddress.primaryAddress.port().c_str());
					deferEvent(m_eventDeferrer, m_reconnectEvent, ReconnectionStatus::SUCCEEDED, 0, packet);
				}
				else
				{
					LogDebug("MqttClient", "Client connected to broker: %s Port: %s",
						m_connectionInfo.connectAddress.primaryAddress.hostname().c_str(),
						m_connectionInfo.reconnectAddress.primaryAddress.port().c_str());
					deferEvent(m_eventDeferrer, m_connectEvent, true, 0, packet);
				}

				m_connectionStatus = ConnectionStatus::CONNECTED;
				m_connectionInfo.hasBeenConnected = true;

				m_connectionInfo.reconnectAddress.reset(m_connectionInfo.connectAddress);

				//Handle properties received from the server.
				const std::uint16_t* serverKeepAlive;
				if (packet.getVariableHeader().properties.tryGetProperty<std::uint16_t>(PropertyType::SERVER_KEEP_ALIVE, serverKeepAlive))
				{
					m_connectionInfo.connectArgs.keepAliveInSec = *serverKeepAlive;
				}

				//Set-up ping interval
				m_connectionInfo.pingInterval = MqttConnectionInfo::Seconds{ m_connectionInfo.connectArgs.keepAliveInSec };
				if (m_config.pingAlways && m_connectionInfo.connectArgs.keepAliveInSec <= 0)
				{
					m_connectionInfo.pingInterval = std::chrono::duration_cast<MqttConnectionInfo::Seconds>(MqttConnectionInfo::Milliseconds{ m_config.defaultPingInterval });
				}

				//TODO read mqtt5 spec for rest of properties of Connect Ack (relating to publishing, subscribing and reconnection).
			}
			else
			{
				assert(m_connectionStatus == ConnectionStatus::RECONNECTING || m_connectionStatus == ConnectionStatus::CONNECTING);

				if (m_connectionStatus == ConnectionStatus::RECONNECTING)
				{
					handleFailedReconnect(packet);
				}
				else
				{
					handleFailedConnect(packet);
				}

				//TODO read mqtt5 spec for properties of Connect Ack (relating to publishing, subscribing and reconnection).
			}
		}

		void MqttClient::handleReceivedDisconnect(const Disconnect& packet)
		{
			handleExternalDisconnect(packet);
		}

		void MqttClient::handleReceivedPublish(const packets::Publish& /*packet*/)
		{
		}

		void MqttClient::handleReceivedPingResponse(const packets::PingResp& packet)
		{
			(void)packet;

			m_connectionInfo.awaitingPingResponse = false;
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
				if (m_connectionInfo.connectArgs.keepAliveInSec == 0 && !m_config.pingAlways)
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

				MqttConnectionInfo::Milliseconds elapsedTimeSinceControlPacket;

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
					m_sendQueue->addToQueue([this](bool enforceMaxSendSize = false, std::size_t allowedSize = std::numeric_limits<std::size_t>::max())
						{
							PingReq packet{ createPingRequestPacket() };
							packet.encode();

							const std::size_t packetSize{ PACKET_SIZE_POST_ENCODE(packet) };

							CHECK_ENFORCE_MAX_PACKET_SIZE(enforceMaxSendSize, packetSize, allowedSize);

							int sendResult{ sendPacket(packet) };

							if (sendResult == 0)
							{
								m_connectionInfo.lastPingReqSentTime = std::chrono::steady_clock::now();
								m_connectionInfo.awaitingPingResponse = true;
							}

							return interfaces::SendResultData{
								packetSize,
								sendResult == 0,
								sendResult == 0 ? interfaces::NoSendReason::NONE : interfaces::NoSendReason::SOCKET_SEND_ERROR,
								sendResult };
						}, PacketType::CONNECT);
				}
			}
		}

		void MqttClient::tickSendPackets()
		{
			//Send queued encoded packets.
			m_sendQueue->sendNextBatch(m_batchResultData);

			if (m_batchResultData.packetsSent > 0)
			{
				m_connectionInfo.lastControlPacketTime = std::chrono::steady_clock::now();

				LogInfo("MqttClient", "Packets Sent: %d\tPackets Expected to Send: %d", m_batchResultData.packetsSent, m_batchResultData.packetsAttemptedToSend);
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
				args.disconnectReasonText = "Hit unrecoverable error: " + m_batchResultData.unrecoverableReasonStr;
				args.clearQueue = true;

				handleInternalDisconnect(packets::DisconnectReasonCode::UNSPECIFIED_ERROR, args);
			}
		}

		void MqttClient::tickReceivePackets()
		{
			//Check, decode, and handle received packets.
			const DecodeResult result = m_receiveQueue.receiveNextBatch();
			if (!result.isSuccess())
			{
				DisconnectArgs args{ false, true };
				args.disconnectReasonText = result.reason;

				LogInfo("MqttClient", "Failed to decode received packet. Packet Type: %d, Reason: %s", static_cast<std::uint8_t>(result.packetType), result.reason.c_str());
				handleInternalDisconnect(result.getDisconnectReason(), args);
			}
		}

		void MqttClient::handleFailedReconnect(const packets::ConnectAck& packet)
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
				m_sendQueue->clearQueue();
				m_connectionStatus = ConnectionStatus::DISCONNECTED;
				deferEvent(m_eventDeferrer, m_reconnectEvent, ReconnectionStatus::FAILED, 0, packet);

				if (m_connectionInfo.hasBeenConnected)
				{
					deferEvent(m_eventDeferrer, m_disconnectEvent, DisconnectReasonCode::NORMAL_DISCONNECTION);
				}
			}
		}

		void MqttClient::handleFailedConnect(const packets::ConnectAck& packet)
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

			m_sendQueue->clearQueue();
			m_connectionInfo.hasBeenConnected = false;

			deferEvent(m_eventDeferrer, m_connectEvent, false, 0, packet);
		}

		void MqttClient::handleTimeOutConnect()
		{
			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			LogDebug("MqttClient", "Client could not connect to broker. Connection timed out");

			m_socket->setOnDisconnectCallback(nullptr);
			m_socket->close();
			m_receiveQueue.clear();
			m_sendQueue->clearQueue();
			m_connectionInfo.hasBeenConnected = false;

			deferEvent(m_eventDeferrer, m_connectEvent, false, 0, {});
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
				m_sendQueue->clearQueue();
				m_connectionStatus = ConnectionStatus::DISCONNECTED;
				deferEvent(m_eventDeferrer, m_reconnectEvent, ReconnectionStatus::FAILED, 0, {});

				if (m_connectionInfo.hasBeenConnected)
				{
					deferEvent(m_eventDeferrer, m_disconnectEvent, DisconnectReasonCode::NORMAL_DISCONNECTION);
				}
			}
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
			deferEvent(m_eventDeferrer, m_reconnectEvent, ReconnectionStatus::RECONNECTING, 0, packets::ConnectAck{});

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

		void MqttClient::handleInternalDisconnect(packets::DisconnectReasonCode reason, const DisconnectArgs& args)
		{
			LogInfo("MqttClient", "Starting internal disconnect: %s", args.disconnectReasonText.c_str());

			//Bypass send queue and send packet ASAP to not wait for next tick before performing disconnect.
			Disconnect packet{ createDisconnectPacket(m_connectionInfo, args, reason) };
			packet.encode();

			sendPacket(packet);

			if (args.clearQueue)
			{
				m_sendQueue->clearQueue();
			}

			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			m_socket->setOnDisconnectCallback(nullptr);
			m_socket->close();

			m_connectionInfo.hasBeenConnected = false;

			deferEvent(m_eventDeferrer, m_disconnectEvent, reason);
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

			m_connectionInfo.hasBeenConnected = false;
			m_sendQueue->clearQueue();
			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			deferEvent(m_eventDeferrer, m_disconnectEvent, packet.getVariableHeader().reasonCode);
		}

		void MqttClient::handleExternalDisconnect(int closeCode, std::string reason)
		{
			LogInfo("MqttClient",
				"Starting external disconnect from socket error code: %d reason: %s",
				closeCode,
				reason.c_str());

			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			m_socket->close();
			m_sendQueue->clearQueue();

			m_connectionInfo.hasBeenConnected = false;

			deferEvent(m_eventDeferrer, m_disconnectEvent, packets::DisconnectReasonCode::UNSPECIFIED_ERROR);
		}

		int MqttClient::sendPacket(const packets::BasePacket& packet)
		{
			LogInfo("MqttClient",
				"Sending packet. PacketType: %d, Size: %d",
				static_cast<std::uint8_t>(packet.getPacketType()),
				packet.getFixedHeader().getEncodedBytesSize() + packet.getFixedHeader().remainingLength.uint32Value());

			const ByteBuffer* bufferPtr{ packet.getDataBuffer() };
			if (bufferPtr != nullptr)
			{
				if (m_socket->send(*bufferPtr) == true)
				{
					LogTrace("MqttClient", "Packet sent:\n\tBytes: %s.", bufferPtr->toString().c_str());
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

				ConAckCallback conAckCallback = std::bind(&MqttClient::handleReceivedConnectAcknowledge, this, std::placeholders::_1);
				DisconnectCallback disconnectCallback = std::bind(&MqttClient::handleReceivedDisconnect, this, std::placeholders::_1);
				PubCallback pubCallback = std::bind(&MqttClient::handleReceivedPublish, this, std::placeholders::_1);
				PingRespCallback pingRespCallback = std::bind(&MqttClient::handleReceivedPingResponse, this, std::placeholders::_1);

				m_receiveQueue.setConnectAcknowledgeCallback(conAckCallback);
				m_receiveQueue.setDisconnectCallback(disconnectCallback);
				m_receiveQueue.setPublishCallback(pubCallback);
				m_receiveQueue.setPingResponseCallback(pingRespCallback);

				m_sendQueue->addToQueue([this](bool enforceMaxSendSize = false, std::size_t allowedSize = std::numeric_limits<std::size_t>::max())
					{
						Connect packet{ createConnectPacket(m_connectionInfo) };
						packet.encode();

						const std::size_t packetSize{ PACKET_SIZE_POST_ENCODE(packet) };

						CHECK_ENFORCE_MAX_PACKET_SIZE(enforceMaxSendSize, packetSize, allowedSize);

						int sendResult = sendPacket(packet);

						return interfaces::SendResultData{
							packetSize,
							sendResult == 0,
							sendResult == 0 ? interfaces::NoSendReason::NONE : interfaces::NoSendReason::SOCKET_SEND_ERROR,
							sendResult };
					}, PacketType::CONNECT);

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

					deferEvent(m_eventDeferrer, m_connectEvent, false, m_socket->getLastError(), packets::ConnectAck{});
				}
			}
		}

		void MqttClient::handleSocketDisconnectEvent()
		{
			handleExternalDisconnect(m_socket->getLastError(), m_socket->getLastCloseReason());
		}

		void MqttClient::handleSocketPacketReceivedEvent(ByteBuffer&& buffer)
		{
			m_receiveQueue.addToQueue(std::move(buffer));
		}

		void MqttClient::handleSocketErrorEvent(int /*error*/)
		{
		}
	}
}