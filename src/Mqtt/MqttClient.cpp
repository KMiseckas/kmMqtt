#include <cleanMqtt/Mqtt/MqttClient.h>

#include <iostream>
#include <cleanMqtt/Mqtt/Packets/PacketUtils.h>
#include <cleanMqtt/Mqtt/PacketHelper.h>

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

			m_receiveQueue = std::make_unique<ReceiveQueue>(this);

			m_socket->setOnConnectCallback([this](bool success) { handleSocketConnectEvent(success); });
			m_socket->setOnDisconnectCallback([this]() { handleSocketDisconnectEvent(); });
			m_socket->setOnPacketRecvdCallback([this](ByteBuffer&& buffer) { handleSocketPacketReceivedEvent(std::move(buffer)); });
			m_socket->setOnErrorCallback([this](std::uint16_t error) { handleSocketErrorEvent(error); });
		}

		MqttClient::~MqttClient()
		{
			m_connectedEvent.removeAll();
			m_disconnectedEvent.removeAll();

			m_socket->close();
			m_socket = nullptr;

			m_sendQueue->clearQueue();
			m_sendQueue = nullptr;

			m_receiveQueue->clearQueue();
			m_receiveQueue = nullptr;
		}

		void MqttClient::connect(ConnectArgs&& args, const std::string& url)
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus == ConnectionStatus::CONNECTED || m_connectionStatus == ConnectionStatus::CONNECTING)
			{
				LogWarning("MqttClient", "Cannot connect() again while already connected or attempting to connect.");
				return;
			}

			LogInfo("MqttClient", "Starting MQTT connection proccess.");

			m_connectionStatus = ConnectionStatus::CONNECTING;
			m_connectionInfo.connectArgs = std::move(args);

			LogInfo("MqttClient", "Socket attempting to connect.");
			m_socket->connect(url);
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

		void MqttClient::tick(float /*deltaTime*/)
		{
			assert(m_sendQueue != nullptr);

			LockGuard guard{ m_mutex };

			if (m_socket->isConnected())
			{
				//TODO add encoding error codes returns.
				m_sendQueue->sendNextBatch(m_batchResultData);

				if (m_batchResultData.packetsSent != m_batchResultData.packetsAttemptedToSend)
				{
					LogWarning("MqttClient", "Failed to send all packets from the previous batch of packets queued for sending.");
				}

				if (!m_batchResultData.isRecoverable)
				{
					//TODO add proper logs once LOG Macro exists for ARGS... .
					DisconnectArgs args;
					args.disconnectReasonText = "Hit unrecoverable error: " + m_batchResultData.unrecoverableReasonStr;
					args.clearQueue = true;
					handleInternalDisconnect(packets::DisconnectReasonCode::UNSPECIFIED_ERROR, args);
				}

				//TODO cmakelist option for internal response queueing vs not queueuing and instant callback.
				m_receiveQueue->receiveAvailablePackets();
			}
		}

		inline ConnectionStatus MqttClient::getConnectionStatus() const noexcept
		{
			return m_connectionStatus;
		}

		void MqttClient::handleReceivedConnectAcknowledge(const ConnectAck& packet)
		{
			//Connection failed for any ConnectReasonCode > 128U, report back as connection failed.
			if (packet.getVariableHeader().reasonCode >= packets::ConnectReasonCode::UNSPECIFIED_ERROR)
			{
				m_connectionStatus = ConnectionStatus::DISCONNECTED;
				LogError("MqttClient", "Client could not connect to broker.");

				m_socket->close();
				m_sendQueue->clearQueue();

				m_connectedEvent.invoke(false, 0, packet);

				//TODO read mqtt5 spec for properties of Connect Ack (relating to publishing, subscribing and reconnection).
			}
			else
			{
				m_connectionStatus = ConnectionStatus::CONNECTED;
				LogError("MqttClient", "Client connected to broker.");

				m_connectedEvent.invoke(true, 0, packet);

				//TODO read mqtt5 spec for properties of Connect Ack (relating to publishing, subscribing and reconnection).
			}
		}

		void MqttClient::handleReceivedDisconnect(const Disconnect& packet)
		{
			handleExternalDisconnect(packet);
		}

		void MqttClient::handleInternalDisconnect(packets::DisconnectReasonCode reason, const DisconnectArgs& args)
		{
			//Bypass send queue and send packet ASAP to not wait for next tick to disconnect.
			sendPacket(createDisconnectPacket(m_connectionInfo, args, reason));

			if (args.clearQueue)
			{
				m_sendQueue->clearQueue();
			}

			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			m_socket->close();

			m_disconnectedEvent(reason);
		}

		void MqttClient::handleExternalDisconnect(const packets::Disconnect& packet)
		{
			LockGuard lockGuard{ m_mutex };

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				return;
			}

			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			m_socket->close();
			m_sendQueue->clearQueue();

			//TODO handle different reason codes here.

			m_disconnectedEvent(packet.getVariableHeader().reasonCode);
		}

		void MqttClient::handleExternalDisconnect(int /*closeCode*/, std::string /*reason*/)
		{
			LockGuard lockGuard{ m_mutex };

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				return;
			}

			m_connectionStatus = ConnectionStatus::DISCONNECTED;
			m_socket->close();
			m_sendQueue->clearQueue();

			//TODO use closeCode and reason.

			m_disconnectedEvent(packets::DisconnectReasonCode::UNSPECIFIED_ERROR);
		}

		int MqttClient::sendPacket(const packets::BasePacket& packet)
		{
			const ByteBuffer* bufferPtr{ packet.getDataBuffer() };
			if (bufferPtr != nullptr)
			{
				if (m_socket->send(*bufferPtr) == true)
				{
					return 0;
				}

				return m_socket->getLastError();
			}
			else
			{
				LogError("MqttClient", "Connect packet buffer is nullptr after encoding.");
				return -1;
			}
		}

		void MqttClient::handleSocketConnectEvent(bool success)
		{
			if (success)
			{
				LogInfo("MqttClient", "Socket connected to url.");

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
				LogError("MqttClient", "Socket failed to connect to url.");

				m_connectedEvent.invoke(false, m_socket->getLastError(), packets::ConnectAck{});
			}
		}

		void MqttClient::handleSocketDisconnectEvent()
		{
			handleExternalDisconnect(m_socket->getLastCloseCode(), m_socket->getLastCloseReason());
		}

		void MqttClient::handleSocketPacketReceivedEvent(ByteBuffer&& buffer)
		{
			m_receiveQueue->addToQueue(std::move(buffer));
		}

		void MqttClient::handleSocketErrorEvent(int /*error*/)
		{
		}
	}
}