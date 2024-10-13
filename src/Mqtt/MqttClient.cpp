#include <cleanMqtt/Mqtt/MqttClient.h>

#include <iostream>

namespace cleanMqtt
{
	namespace mqtt
	{
		using namespace cleanMqtt::mqtt::packets;

		MqttClient::MqttClient(std::unique_ptr<interfaces::IWebSocket> socket) 
			: m_socket(std::move(socket))
		{
		}

		MqttClient::~MqttClient()
		{

		}

		void MqttClient::connect(ConnectArgs&& args)
		{
			LockGuard guard{ m_mutex };

			if (m_connectionStatus == ConnectionStatus::CONNECTED || m_connectionStatus == ConnectionStatus::CONNECTING)
			{
				Log(LogLevel::Warning, "MqttClient", "Cannot connect() again while already connected or attempting to connect.");
				return;
			}

			Log(LogLevel::Info, "MqttClient", "Starting MQTT connection proccess.");

			m_connectionStatus = ConnectionStatus::CONNECTING;
			m_connectionInfo.connectArgs = std::move(args);


			//TODO -> do encoding queue, then send queue seperately? (we can rate limit encoding)
			//TODO seperate out into its own functions for packet creation (Factory?)
			m_queuedRequests.push(
				[this]()
				{
					const auto& conArgs = m_connectionInfo.connectArgs;

					EncodedConnectFlags connectFlags;
					connectFlags.setFlagValue(ConnectFlags::CLEAN_START, conArgs.cleanStart);
					connectFlags.setFlagValue(ConnectFlags::PASSWORD, !conArgs.password.empty());
					connectFlags.setFlagValue(ConnectFlags::USER_NAME, !conArgs.username.empty());

					if (conArgs.will != nullptr)
					{
						if (conArgs.will->willTopic.empty())
						{
							Exception(LogLevel::Error,
								"MqttClient",
								std::runtime_error("Ignoring Will - Attempted to add a Will to the Connect packet, but Will Topic has not been set!"));
						}
						else
						{
							connectFlags.setFlagValue(ConnectFlags::WILL_FLAG, true);
							connectFlags.setFlagValue(ConnectFlags::WILL_QOS, static_cast<std::uint8_t>(conArgs.will->willQos));
							connectFlags.setFlagValue(ConnectFlags::WILL_RETAIN, conArgs.will->retainWillMessage);
						}
					}

					Properties connectProperties;
					connectProperties.tryAddProperty(PropertyType::SESSION_EXPIRY_INTERVAL, conArgs.sessionExpiryInterval, conArgs.sessionExpiryInterval > 0);
					connectProperties.tryAddProperty(PropertyType::RECEIVE_MAXIMUM, conArgs.receiveMaximum);
					connectProperties.tryAddProperty(PropertyType::MAXIMUM_PACKET_SIZE, conArgs.maximumPacketSize, conArgs.maximumPacketSize >= 2);
					connectProperties.tryAddProperty(PropertyType::TOPIC_ALIAS_MAXIMUM, conArgs.maximumTopicAliases, conArgs.maximumTopicAliases > 0);
					connectProperties.tryAddProperty(PropertyType::REQUEST_RESPONSE_INFORMATION, conArgs.requestResponseInformation, conArgs.requestResponseInformation);
					connectProperties.tryAddProperty(PropertyType::REQUEST_PROBLEM_INFORMATION, conArgs.requestProblemInformation, !conArgs.requestProblemInformation);

					for (const auto& property : conArgs.userProperties)
					{
						connectProperties.tryAddProperty(PropertyType::USER_PROPERTY, UTF8StringPair{ property.first, property.second });
					}

					ConnectVariableHeader varHeader{
					conArgs.protocolName.c_str(),
					conArgs.version,
					conArgs.keepAliveInSec,
					std::move(connectFlags),
					std::move(connectProperties)};

					ConnectPayloadHeader payloadHeader;
					//TODO payload header

					Connect connect{ std::move(varHeader), std::move(payloadHeader)};
					connect.encode();

					const auto* bufferPtr = connect.getDataBuffer();
					if (bufferPtr != nullptr)
					{
						sendPacket(*bufferPtr);
					}
					else
					{
						Log(LogLevel::Error, "MqttClient", "Connect packet buffer is nullptr after encoding.");
					}
				});

			Log(LogLevel::Trace, "MqttClient", "Packet queued to be resolved for next tick.");
		}

		void MqttClient::publish(const char* topic, const char* payloadMsg)
		{
			(void)topic;
			(void)payloadMsg;
		}

		void MqttClient::subscribe(const char* topic)
		{
			(void)topic;
		}

		void MqttClient::unSubscribe()
		{
		}

		void MqttClient::disconnect()
		{
		}

		void MqttClient::tick(float deltaTime)
		{
			(void)deltaTime;

			if (m_socket->isConnected())
			{
				//TODO:: Tick queue for packet proccessing.
			}
		}

		int MqttClient::sendPacket(const ByteBuffer& data)
		{
			if (m_socket->send(data) == true)
			{
				return 0;
			}

			return m_socket->getLastError();
		}
	}
}