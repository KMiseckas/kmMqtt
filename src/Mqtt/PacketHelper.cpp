// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include <kmMqtt/Mqtt/PacketHelper.h>

#include <kmMqtt/Mqtt/Params/PublishOptions.h>
#include <kmMqtt/Mqtt/Params/PubAckOptions.h>
#include <kmMqtt/Mqtt/Params/PubCompOptions.h>
#include <kmMqtt/Mqtt/Params/PubRecOptions.h>
#include <kmMqtt/Mqtt/Params/PubRelOptions.h>
#include <kmMqtt/Mqtt/Params/DisconnectArgs.h>
#include <kmMqtt/Mqtt/Params/SubscribeOptions.h>
#include <kmMqtt/Mqtt/Params/UnSubscribeOptions.h>
#include <kmMqtt/Mqtt/MqttConnectionInfo.h>

namespace kmMqtt
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
			connectProperties.tryAddProperty<PropertyType::SESSION_EXPIRY_INTERVAL>(conArgs.sessionExpiryInterval, conArgs.sessionExpiryInterval > 0);
			connectProperties.tryAddProperty<PropertyType::RECEIVE_MAXIMUM>(conArgs.receiveMaximum, conArgs.receiveMaximum > 0);
			connectProperties.tryAddProperty<PropertyType::MAXIMUM_PACKET_SIZE>(conArgs.maximumPacketSize, conArgs.maximumPacketSize >= 2);
			connectProperties.tryAddProperty<PropertyType::TOPIC_ALIAS_MAXIMUM>(conArgs.maximumTopicAliases, conArgs.maximumTopicAliases > 0);
			connectProperties.tryAddProperty<PropertyType::REQUEST_RESPONSE_INFORMATION>(conArgs.requestResponseInformation, conArgs.requestResponseInformation);
			connectProperties.tryAddProperty<PropertyType::REQUEST_PROBLEM_INFORMATION>(conArgs.requestProblemInformation, !conArgs.requestProblemInformation);

			for (const auto& property : conArgs.userProperties)
			{
				connectProperties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair{ property.first, property.second });
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
				willProperties.tryAddProperty<PropertyType::WILL_DELAY_INTERVAL>(conArgs.will->willDelayInterval, conArgs.will->willDelayInterval > 0);
				willProperties.tryAddProperty<PropertyType::PAYLOAD_FORMAT_INDICATOR>(static_cast<std::uint8_t>(conArgs.will->payloadFormat), conArgs.will->payloadFormat == PayloadFormatIndicator::UTF8);
				willProperties.tryAddProperty<PropertyType::MESSAGE_EXPIRY_INTERVAL>(conArgs.will->messageExpiryInterval, conArgs.will->messageExpiryInterval > 0);
				willProperties.tryAddProperty<PropertyType::CONTENT_TYPE>(UTF8String{ conArgs.will->contentType }, !conArgs.will->contentType.empty());
				willProperties.tryAddProperty<PropertyType::RESPONSE_TOPIC>(UTF8String{ conArgs.will->responseTopic }, !conArgs.will->responseTopic.empty());

				if (conArgs.will->correlationData != nullptr)
				{
					willProperties.tryAddProperty<PropertyType::CORRELATION_DATA>(BinaryData{ conArgs.will->correlationData->size(), conArgs.will->correlationData->bytes() });
				}

				for (const auto& property : conArgs.will->userProperties)
				{
					willProperties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair{ property.first, property.second });
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
			Properties properties;
			properties.tryAddProperty<PropertyType::SESSION_EXPIRY_INTERVAL>(args.sessionExpiryInterval, connectionInfo.connectArgs.sessionExpiryInterval != 0);
			properties.tryAddProperty<PropertyType::REASON_STRING>(UTF8String{ args.disconnectReasonText }, !args.disconnectReasonText.empty());

			for (const auto& property : args.userProperties)
			{
				properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair{ property.first, property.second });
			}

			DisconnectVariableHeader header{ reason, std::move(properties) };
			Disconnect disconnectPacket{ std::move(header) };

			return disconnectPacket;
		}

		PingReq createPingRequestPacket() noexcept
		{
			return PingReq{};
		}

		PingResp createPingResponsePacket() noexcept
		{
			return PingResp{};
		}

		Publish createPublishPacket(const MqttConnectionInfo& connectionInfo, bool isDup, const char* topic, const ByteBuffer& payload, const PublishOptions& options, std::uint16_t packetId) noexcept
		{
			(void)connectionInfo;

			PublishVariableHeader varHeader;
			PublishPayloadHeader payloadHeader;

			Properties properties;
			properties.tryAddProperty<PropertyType::PAYLOAD_FORMAT_INDICATOR>(static_cast<std::uint8_t>(options.payloadFormatIndicator));
			properties.tryAddProperty<PropertyType::MESSAGE_EXPIRY_INTERVAL>(options.messageExpiryInterval, options.addMessageExpiryInterval);
			properties.tryAddProperty<PropertyType::TOPIC_ALIAS>(options.topicAlias, options.topicAlias > 0);
			properties.tryAddProperty<PropertyType::RESPONSE_TOPIC>(UTF8String{ options.responseTopic }, !options.responseTopic.empty());

			if (options.correlationData != nullptr)
			{
				properties.tryAddProperty<PropertyType::CORRELATION_DATA>(BinaryData{ *options.correlationData.get() }, !options.responseTopic.empty());
			}

			for (const auto& property : options.userProperties)
			{
				properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair{ property.first, property.second });
			}

			varHeader.properties = std::move(properties);
			varHeader.packetIdentifier = packetId;
			varHeader.qos = options.qos;
			varHeader.topicName = topic;

			payloadHeader.payload = payload;

			EncodedPublishFlags flags{ isDup, options.qos, options.retain };

			return Publish{ std::move(payloadHeader), std::move(varHeader), flags };
		}

		
		PublishAck createPubAckPacket(std::uint16_t packetId, PubAckReasonCode reasonCode, const PubAckOptions& options) noexcept
		{
			Properties properties;
			properties.tryAddProperty<PropertyType::REASON_STRING>(UTF8String{ options.reasonString }, !options.reasonString.empty());

			for (const auto& property : options.userProperties)
			{
				properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair{ property.first, property.second });
			}

			PubAckVariableHeader varHeader{ packetId, reasonCode, std::move(properties) };
			return PublishAck{ std::move(varHeader) };
		}

		PublishRec createPubRecPacket(std::uint16_t packetId, PubRecReasonCode reasonCode, const PubRecOptions& options) noexcept
		{
			Properties properties;
			properties.tryAddProperty<PropertyType::REASON_STRING>(UTF8String{ options.reasonString }, !options.reasonString.empty());

			for (const auto& property : options.userProperties)
			{
				properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair{ property.first, property.second });
			}

			PubRecVariableHeader varHeader{ packetId, reasonCode, std::move(properties) };
			return PublishRec{ std::move(varHeader) };
		}

		PublishRel createPubRelPacket(std::uint16_t packetId, PubRelReasonCode reasonCode, const PubRelOptions& options) noexcept
		{
			Properties properties;
			properties.tryAddProperty<PropertyType::REASON_STRING>(UTF8String{ options.reasonString }, !options.reasonString.empty());

			for (const auto& property : options.userProperties)
			{
				properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair{ property.first, property.second });
			}

			PubRelVariableHeader varHeader{ packetId, reasonCode, std::move(properties) };
			return PublishRel{ std::move(varHeader) };
		}

		PublishComp createPubCompPacket(std::uint16_t packetId, PubCompReasonCode reasonCode, const PubCompOptions& options) noexcept
		{
			Properties properties;
			properties.tryAddProperty<PropertyType::REASON_STRING>(UTF8String{ options.reasonString }, !options.reasonString.empty());

			for (const auto& property : options.userProperties)
			{
				properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair{ property.first, property.second });
			}

			PubCompVariableHeader varHeader{ packetId, reasonCode, std::move(properties) };
			return PublishComp{ std::move(varHeader) };
		}

		Subscribe createSubscribePacket(std::uint16_t packetId, const std::vector<Topic>& topics, const SubscribeOptions& options) noexcept
		{
			Properties properties;
			properties.tryAddProperty<PropertyType::SUBSCRIPTION_IDENTIFIER>(VariableByteInteger(options.subscribeIdentifier), options.subscribeIdentifier.uint32Value() != 0);

			for (const auto& property : options.userProperties)
			{
				properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair{ property.first, property.second });
			}

			std::vector<Subscription> subscriptions;
			subscriptions.reserve(topics.size());

			for (const auto& t : topics)
			{
				EncodedSubscribeOptionsFlags flags;
				flags.setFlagValue(SubscribeOptionsFlags::QOS, static_cast<uint8_t>(t.options.qos));
				flags.setFlagValue(SubscribeOptionsFlags::NL, t.options.noLocal);
				flags.setFlagValue(SubscribeOptionsFlags::RAP, t.options.retainAsPublished);
				flags.setFlagValue(SubscribeOptionsFlags::RETAIN_HANDLING, static_cast<uint8_t>(t.options.retainHandling));

				subscriptions.emplace_back(t.topicFilter.c_str(), std::move(flags));
			}

			SubscribeVariableHeader varHeader{ packetId, std::move(properties) };
			SubscribePayloadHeader payloadHeader{ std::move(subscriptions) };

			return Subscribe{ std::move(varHeader), std::move(payloadHeader)};
		}

		UnSubscribe createUnSubscribePacket(std::uint16_t packetId, const std::vector<Topic>& topics, const UnSubscribeOptions& options) noexcept
		{
			Properties properties;
			for (const auto& property : options.userProperties)
			{
				properties.tryAddProperty<PropertyType::USER_PROPERTY>(UTF8StringPair{ property.first, property.second });
			}

			std::vector<UTF8String> topicsFilter;
			topicsFilter.reserve(topics.size());

			for (const auto& t : topics)
			{
				topicsFilter.emplace_back(t.topicFilter);
			}

			UnSubscribeVariableHeader varHeader{ packetId, std::move(properties) };
			UnSubscribePayloadHeader payloadHeader{ std::move(topicsFilter) };

			return UnSubscribe{ std::move(varHeader), std::move(payloadHeader) };
		}
	}
}