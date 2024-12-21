#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETHELPER_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETHELPER_H

#include <cleanMqtt/Mqtt/MqttConnectionInfo.h>
#include <cleanMqtt/Mqtt/Packets/Connection/Connect.h>
#include <cleanMqtt/Mqtt/Packets/Connection/ConnectAck.h>
#include <cleanMqtt/Mqtt/Packets/Connection/Disconnect.h>
#include "cleanMqtt/Mqtt/Params/DisconnectArgs.h"

namespace cleanMqtt
{
	namespace mqtt
	{	
		using namespace packets;

		Connect createConnectPacket(const MqttConnectionInfo& connectionInfo) noexcept
		{
			const auto& conArgs = connectionInfo.connectArgs;

			if (conArgs.will != nullptr)
			{
				if (conArgs.will->payload != nullptr && conArgs.will->payload->size() <= 0)
				{
					LogException("MqttClient", std::runtime_error("Payload size must be bigger than 0! - Payload is included in the will, but payload size is 0."));
				}

				if (conArgs.will->correlationData != nullptr && conArgs.will->correlationData->size() <= 0)
				{
					LogException("MqttClient", std::runtime_error("Correlatation data size must be bigger than 0! - Correlatation data is included in the will, but Correlatation data size is 0."));
				}
			}

			EncodedConnectFlags connectFlags;
			connectFlags.setFlagValue(ConnectFlags::CLEAN_START, conArgs.cleanStart);
			connectFlags.setFlagValue(ConnectFlags::PASSWORD, !conArgs.password.empty());
			connectFlags.setFlagValue(ConnectFlags::USER_NAME, !conArgs.username.empty());

			if (conArgs.will != nullptr)
			{
				if (conArgs.will->willTopic.empty())
				{
					LogException(
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
				willProperties.tryAddProperty(PropertyType::RESPONSE_TOPIC, BinaryData{ conArgs.will->correlationData->size(), conArgs.will->correlationData->bytes() }, conArgs.will->correlationData != nullptr);

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

		Disconnect createDisconnectPacket(const MqttConnectionInfo& connectionInfo, const DisconnectArgs& args, DisconnectReasonCode reason)
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
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETHELPER_H 