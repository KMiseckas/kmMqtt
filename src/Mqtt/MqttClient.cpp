#include <cleanMqtt/Mqtt/MqttClient.h>

#include <iostream>
#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		using namespace cleanMqtt::mqtt::packets;

namespace
{
#define TRY_ENFORCE_MAX_SEND_SIZE(allowEnforce, maxSizeAllowed, packetSize)\
if (allowEnforce == true)\
{\
	if (packetSize > maxSizeAllowed)\
	{\
		Log(LogLevel::Info, "Enforced max send size for queued packet.");\
		return interfaces::SendResultData{ packetSize, false, interfaces::NoSendReason::OVER_MAX_PACKET_SIZE, 0 };\
	}\
}\

}

		MqttClient::MqttClient(std::unique_ptr<interfaces::IWebSocket> socket, std::unique_ptr<interfaces::ISendQueue> sendQueue)
			: m_socket{ std::move(socket) }, m_sendQueue{ std::move(sendQueue) }
		{
		}

		MqttClient::~MqttClient()
		{
			m_connectedEvent.removeAll();
			m_disconnectedEvent.removeAll();

			m_socket->close();
			m_socket = nullptr;

			m_sendQueue->clearQueue();
			m_sendQueue = nullptr;
		}

		bool MqttClient::connect(ConnectArgs&& args, const std::string& url)
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus == ConnectionStatus::CONNECTED || m_connectionStatus == ConnectionStatus::CONNECTING)
			{
				Log(LogLevel::Warning, "MqttClient", "Cannot connect() again while already connected or attempting to connect.");
				return false;
			}

			Log(LogLevel::Info, "MqttClient", "Starting MQTT connection proccess.");

			m_connectionStatus = ConnectionStatus::CONNECTING;
			m_connectionInfo.connectArgs = std::move(args);

			Log(LogLevel::Info, "MqttClient", "Socket attempting to connect.");
			if (!m_socket->connect(url))
			{
				m_connectionStatus = ConnectionStatus::DISCONNECTED;
				Log(LogLevel::Error, "MqttClient", "Socket failed to connect to url.");
				return false;
			}
			Log(LogLevel::Info, "MqttClient", "Socket connected to url.");

			m_sendQueue->addToQueue([this](bool enforceMaxSendSize = false, std::size_t allowedSize = std::numeric_limits<std::size_t>::max())
				{
					Connect packet{ std::move(createConnectPacket(m_connectionInfo)) };
					packet.encode();

					std::size_t packetSize = packet.getFixedHeader().getEncodedBytesSize() + packet.getFixedHeader().remainingLength.uint32Value();

					TRY_ENFORCE_MAX_SEND_SIZE(enforceMaxSendSize, allowedSize, packetSize)

					int sendResult = sendPacket(packet) == 0;

					return interfaces::SendResultData{ 
						packetSize,
						sendResult == 0,
						sendResult == 0 ? interfaces::NoSendReason::NONE : interfaces::NoSendReason::SOCKET_SEND_ERROR,
						sendResult};
				}, PacketType::CONNECT);

			Log(LogLevel::Trace, "MqttClient", "Connect packet send request queued, to be resolved on next tick.");
			return true;
		}

		void MqttClient::publish(const char* topic, const char* payloadMsg)
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus != ConnectionStatus::CONNECTED)
			{
				Log(LogLevel::Warning, "MqttClient", "Client not connected, cannot publish()!");
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
				Log(LogLevel::Warning, "MqttClient", "Client not connected, cannot subscribe()!");
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
				Log(LogLevel::Warning, "MqttClient", "Client not connected, cannot unSubscribe()!");
				return;
			}

			//TODO
		}

		void MqttClient::disconnect(DisconnectArgs&& args)
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus == ConnectionStatus::DISCONNECTED)
			{
				Log(LogLevel::Warning, "MqttClient", "Client already disconnected, cannot disconnect()!");
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
				m_sendQueue->sendNextBatch(m_batchResultData);

				if (m_batchResultData.packetsSent != m_batchResultData.packetsAttemptedToSend)
				{
					Log(LogLevel::Warning, "MqttClient", "Failed to send all packets from the previous batch of packets queued for sending.");
				}

				if (!m_batchResultData.isRecoverable)
				{
					//TODO add proper logs once LOG Macro exists for ARGS... .
					DisconnectArgs args;
					args.disconnectReasonText = "Hit unrecoverable error.";
					args.clearQueue = true;
					handleInternalDisconnect(packets::DisconnectReasonCode::UNSPECIFIED_ERROR, args);
				}
			}
		}

		inline ConnectionStatus MqttClient::getConnectionStatus() const noexcept
		{
			return m_connectionStatus;
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
				Log(LogLevel::Error, "MqttClient", "Connect packet buffer is nullptr after encoding.");
				return -1;
			}
		}
	}
}