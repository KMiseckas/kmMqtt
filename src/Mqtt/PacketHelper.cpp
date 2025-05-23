#include <cleanMqtt/Mqtt/PacketHelper.h>

namespace cleanMqtt
{
	namespace mqtt
	{
		Connect createConnectPacket(const MqttConnectionInfo& connectionInfo) noexcept
		{
			const auto& conArgs = connectionInfo.connectArgs;

			EncodedConnectFlags connectFlags;
			connectFlags.setFlagValue(ConnectFlags::CLEAN_START, conArgs.cleanStart);
			connectFlags.setFlagValue(ConnectFlags::PASSWORD, !conArgs.password.empty());
			connectFlags.setFlagValue(ConnectFlags::USER_NAME, !conArgs.username.empty());

			if (conArgs.will != nullptr)
			{
				assert(!conArgs.will->willTopic.empty());

				connectFlags.setFlagValue(ConnectFlags::WILL_FLAG, true);
				connectFlags.setFlagValue(ConnectFlags::WILL_QOS, static_cast<std::uint8_t>(conArgs.will->willQos));
				connectFlags.setFlagValue(ConnectFlags::WILL_RETAIN, conArgs.will->retainWillMessage);
			}

			Properties connectProperties;
			connectProperties.tryAddProperty(PropertyType::SESSION_EXPIRY_INTERVAL, conArgs.sessionExpiryInterval, conArgs.sessionExpiryInterval > 0);
			connectProperties.tryAddProperty(PropertyType::RECEIVE_MAXIMUM, conArgs.receiveMaximum, conArgs.receiveMaximum > 0);
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
			std::move(connectProperties) };

			Properties willProperties;
			if (conArgs.will != nullptr)
			{
				willProperties.tryAddProperty(PropertyType::WILL_DELAY_INTERVAL, conArgs.will->willDelayInterval, conArgs.will->willDelayInterval > 0);
				willProperties.tryAddProperty(PropertyType::PAYLOAD_FORMAT_INDICATOR, conArgs.will->payloadFormat, conArgs.will->payloadFormat == PayloadFormatIndicator::UTF8);
				willProperties.tryAddProperty(PropertyType::MESSAGE_EXPIRY_INTERVAL, conArgs.will->messageExpiryInterval, conArgs.will->messageExpiryInterval > 0);
				willProperties.tryAddProperty(PropertyType::CONTENT_TYPE, UTF8String{ conArgs.will->contentType }, !conArgs.will->contentType.empty());
				willProperties.tryAddProperty(PropertyType::RESPONSE_TOPIC, UTF8String{ conArgs.will->responseTopic }, !conArgs.will->responseTopic.empty());
				willProperties.tryAddProperty(PropertyType::CORRELATION_DATA, BinaryData{ conArgs.will->correlationData->size(), conArgs.will->correlationData->bytes() }, conArgs.will->correlationData != nullptr);

				for (const auto& property : conArgs.will->userProperties)
				{
					willProperties.tryAddProperty(PropertyType::USER_PROPERTY, UTF8StringPair{ property.first, property.second });
				}
			}

			ConnectPayloadHeader payloadHeader
			{
				UTF8String{conArgs.clientId},
				std::move(willProperties),
				UTF8String{conArgs.will == nullptr ? "" : conArgs.will->willTopic},
				conArgs.will == nullptr ? BinaryData() : BinaryData{conArgs.will->payload->size(), conArgs.will->payload->bytes()},
				UTF8String{conArgs.username},
				BinaryData{static_cast<std::uint16_t>(conArgs.password.size()), reinterpret_cast<const std::uint8_t*>(conArgs.password.c_str())}
			};

			Connect connect{ std::move(varHeader), std::move(payloadHeader) };

			return connect;
		}

		Disconnect createDisconnectPacket(const MqttConnectionInfo& connectionInfo, const DisconnectArgs& args, DisconnectReasonCode reason) noexcept
		{
			packets::Properties properties;
			properties.tryAddProperty(packets::PropertyType::SESSION_EXPIRY_INTERVAL, args.sessionExpiryInterval, connectionInfo.connectArgs.sessionExpiryInterval != 0);
			properties.tryAddProperty(packets::PropertyType::REASON_STRING, UTF8String{ args.disconnectReasonText }, !args.disconnectReasonText.empty());

			for (const auto& property : args.userProperties)
			{
				properties.tryAddProperty(PropertyType::USER_PROPERTY, UTF8StringPair{ property.first, property.second });
			}

			packets::DisconnectVariableHeader header{ reason, std::move(properties) };
			packets::Disconnect disconnectPacket{ std::move(header) };

			return disconnectPacket;
		}

		PingReq createPingRequestPacket() noexcept
		{
			return packets::PingReq{};
		}

		PingResp createPingResponsePacket() noexcept
		{
			return packets::PingResp{};
		}

		Publish createPublishPacket(const MqttConnectionInfo& connectionInfo, const char* topic, ByteBuffer&& payload, PublishOptions& options, PacketID packetId) noexcept
		{
			(void)connectionInfo;

			PublishVariableHeader varHeader;
			PublishPayloadHeader payloadHeader;

			packets::Properties properties;
			properties.tryAddProperty(packets::PropertyType::PAYLOAD_FORMAT_INDICATOR, options.payloadFormatIndicator);
			properties.tryAddProperty(packets::PropertyType::MESSAGE_EXPIRY_INTERVAL, options.messageExpiryInterval, options.addMessageExpiryInterval);
			properties.tryAddProperty(packets::PropertyType::TOPIC_ALIAS, options.topicAlias, options.topicAlias > 0);
			properties.tryAddProperty(packets::PropertyType::RESPONSE_TOPIC, options.responseTopic, !options.responseTopic.empty());
			properties.tryAddProperty(packets::PropertyType::CORRELATION_DATA, BinaryData{ *options.correlationData.get() }, options.correlationData != nullptr && !options.responseTopic.empty());

			for (const auto& property : options.userProperties)
			{
				properties.tryAddProperty(PropertyType::USER_PROPERTY, UTF8StringPair{ property.first, property.second });
			}

			varHeader.properties = std::move(properties);
			varHeader.packetIdentifier = packetId;
			varHeader.qos = options.qos;
			varHeader.topicName = topic;

			payloadHeader.payload = std::move(payload);

			EncodedPublishFlags flags{ false, options.qos, options.retain };

			return packets::Publish{ std::move(payloadHeader), std::move(varHeader), flags };
		}
	}
}