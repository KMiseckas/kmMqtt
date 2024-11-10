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

		MqttClient::MqttClient(std::unique_ptr<interfaces::IWebSocket> socket, std::unique_ptr<interfaces::ISendQueue> sendQueue)
			: m_socket{ std::move(socket) }, m_sendQueue{ std::move(sendQueue) }
		{
			assert(m_socket != nullptr);
			assert(m_sendQueue != nullptr);

			m_socket->setOnConnectCallback([this](bool success) { handleSocketConnectEvent(success); });
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

			if (m_connectionStatus == ConnectionStatus::CONNECTED || m_connectionStatus == ConnectionStatus::CONNECTING)
			{
				LogWarning("MqttClient", "Cannot connect() again while already connected or attempting to connect.");
				return;
			}

			m_receiveQueue.clear();

			LogDebug("MqttClient", "Starting MQTT connection proccess.");
			LogInfo("MqttClient",
				"URL: %s\nUsername: %s\nClientId: %s\nVersion: %d\nProtocol Name: %s",
				address.primaryAddress.value().c_str(),
				args.username.c_str(),
				args.clientId.c_str(),
				static_cast<std::uint8_t>(args.version),
				args.protocolName.c_str());


			m_connectionStatus = ConnectionStatus::CONNECTING;
			m_connectionInfo.connectArgs = std::move(args);
			m_connectionInfo.connectAddress = std::move(address);
			m_connectionInfo.reconnectAddress.reset(m_connectionInfo.connectAddress);

			LogInfo("MqttClient", "Socket attempting to connect.");
			m_socket->connect(m_connectionInfo.connectAddress.primaryAddress.value());
		}

		void MqttClient::publish(const char* topic, const char* payloadMsg)
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				LogWarning("MqttClient", "Client not connected, cannot publish()!");
				return;
			}

			//TODO

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

			//TODO

			(void)topic;
		}

		void MqttClient::unSubscribe()
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				LogWarning("MqttClient", "Client not connected, cannot unSubscribe()!");
				return;
			}

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

			m_deferrer.clear();
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

			if (m_socket->isConnected())
			{
				//Send queued encoded packets.
				m_sendQueue->sendNextBatch(m_batchResultData);

#if ENABLE_LOGS && LOG_LEVEL == INFO
				if (m_batchResultData.packetsSent > 0)
				{
					LogInfo("MqttClient", "Packets Sent: %d\nPackets Expected to send: %d", m_batchResultData.packetsSent, m_batchResultData.packetsAttemptedToSend);
				}
#endif

				//Check for potential issues
				if (m_batchResultData.packetsSent != m_batchResultData.packetsAttemptedToSend)
				{
					LogWarning("MqttClient",
						"Failed to send all packets from the previous batch of packets queued for sending.\nPackets Sent: %d\nPackets Expected: %d",
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
			else
			{
				handleExternalDisconnect(m_socket->getLastCloseCode(), m_socket->getLastCloseReason());
			}

			m_deferrer.invokeEvents();
		}

		inline ConnectionStatus MqttClient::getConnectionStatus() const noexcept
		{
			return m_connectionStatus;
		}

		const MqttConnectionInfo& MqttClient::getConnectionInfo() const noexcept
		{
			return m_connectionInfo;
		}

		void MqttClient::handleReceivedConnectAcknowledge(const ConnectAck& packet)
		{
			//Connection succeeded for any ConnectReasonCode < 128U, report back as connection success, else start failure proccess.
			if (packet.getVariableHeader().reasonCode < packets::ConnectReasonCode::UNSPECIFIED_ERROR)
			{
				m_connectionStatus = ConnectionStatus::CONNECTED;
				m_connectionInfo.m_hasBeenConnected = true;

				m_connectionInfo.reconnectAddress.reset(m_connectionInfo.connectAddress);

				if (m_connectionStatus == ConnectionStatus::RECONNECTING)
				{
					LogDebug("MqttClient", "Reconnection complete to broker: %s", m_connectionInfo.reconnectAddress.primaryAddress.value().c_str());
					deferEvent(m_deferrer, m_reconnectEvent, ReconnectionStatus::SUCCEEDED, 0, packet);
				}
				else
				{
					LogDebug("MqttClient", "Client connected to broker: %s", m_connectionInfo.connectAddress.primaryAddress.value().c_str());
					deferEvent(m_deferrer, m_connectEvent, true, 0, packet);
				}

				//TODO read mqtt5 spec for properties of Connect Ack (relating to publishing, subscribing and reconnection).
			}
			else
			{
				assert(m_connectionStatus == ConnectionStatus::RECONNECTING || m_connectionStatus == ConnectionStatus::CONNECTING);

				if (m_connectionStatus == ConnectionStatus::RECONNECTING)
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
						deferEvent(m_deferrer, m_reconnectEvent, ReconnectionStatus::FAILED, 0, packet);

						if (m_connectionInfo.m_hasBeenConnected)
						{
							deferEvent(m_deferrer, m_disconnectEvent, DisconnectReasonCode::NORMAL_DISCONNECTION);
						}
					}
				}
				else
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
					m_connectionInfo.m_hasBeenConnected = false;

					deferEvent(m_deferrer, m_connectEvent, false, 0, packet);
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
			deferEvent(m_deferrer, m_reconnectEvent, ReconnectionStatus::RECONNECTING, 0, packets::ConnectAck{});

			m_connectionStatus = ConnectionStatus::RECONNECTING;
			m_receiveQueue.clear();

			LogDebug("MqttClient", "Starting MQTT re-connection proccess.");
			LogInfo("MqttClient",
				"URL: %s\nUsername: %s\nClientId: %s\nVersion: %d\nProtocol Name: %s",
				m_connectionInfo.reconnectAddress.primaryAddress.value().c_str(),
				m_connectionInfo.connectArgs.username.c_str(),
				m_connectionInfo.connectArgs.clientId.c_str(),
				static_cast<std::uint8_t>(m_connectionInfo.connectArgs.version),
				m_connectionInfo.connectArgs.protocolName.c_str());

			LogInfo("MqttClient", "Socket attempting to connect.");
			m_socket->connect(m_connectionInfo.reconnectAddress.primaryAddress.value());
		}

		void MqttClient::handleInternalDisconnect(packets::DisconnectReasonCode reason, const DisconnectArgs& args)
		{
			LogInfo("MqttClient", "Starting internal disconnect: %s", args.disconnectReasonText.c_str());

			//Bypass send queue and send packet ASAP to not wait for next tick before performing disconnect.
			sendPacket(createDisconnectPacket(m_connectionInfo, args, reason));

			if (args.clearQueue)
			{
				m_sendQueue->clearQueue();
			}

			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			m_socket->close();

			m_connectionInfo.m_hasBeenConnected = false;

			deferEvent(m_deferrer, m_disconnectEvent, reason);
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

			m_socket->close();
			m_receiveQueue.clear();

			const bool brokerRedirectionStarted = tryStartBrokerRedirection(static_cast<std::uint8_t>(code), packet.getVariableHeader().properties);
			if (brokerRedirectionStarted)
			{
				return;
			}

			m_connectionInfo.m_hasBeenConnected = false;
			m_sendQueue->clearQueue();
			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			deferEvent(m_deferrer, m_disconnectEvent, packet.getVariableHeader().reasonCode);
		}

		void MqttClient::handleExternalDisconnect(int closeCode, std::string reason)
		{
			LogInfo("MqttClient",
				"Starting external disconnect from socket error code: %d, reason %s",
				closeCode,
				reason.c_str());

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				return;
			}

			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			m_socket->close();
			m_sendQueue->clearQueue();

			m_connectionInfo.m_hasBeenConnected = false;

			deferEvent(m_deferrer, m_disconnectEvent, packets::DisconnectReasonCode::UNSPECIFIED_ERROR);
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

				m_receiveQueue.setConnectAcknowledgeCallback(conAckCallback);
				m_receiveQueue.setDisconnectCallback(disconnectCallback);
				m_receiveQueue.setPublishCallback(pubCallback);

				m_sendQueue->addToQueue([this](bool enforceMaxSendSize = false, std::size_t allowedSize = std::numeric_limits<std::size_t>::max())
					{
						Connect packet{ std::move(createConnectPacket(m_connectionInfo)) };
						packet.encode();

						const std::size_t packetSize{ packet.getFixedHeader().getEncodedBytesSize() + packet.getFixedHeader().remainingLength.uint32Value() };

						if (enforceMaxSendSize == true) 
						{
							if (packetSize > allowedSize) 
							{
								LogInfo("", "Enforced max send size for queued packet.");
								return interfaces::SendResultData{ packetSize, false, interfaces::NoSendReason::OVER_MAX_PACKET_SIZE, 0 };
							}
						}

						int sendResult = sendPacket(packet) == 0;

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
					deferEvent(m_deferrer, m_connectEvent, false, m_socket->getLastError(), packets::ConnectAck{});
				}
			}
		}

		void MqttClient::handleSocketDisconnectEvent()
		{
			
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