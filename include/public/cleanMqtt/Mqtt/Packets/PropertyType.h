#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTYTYPE_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTYTYPE_H

#include <cleanMqtt/Utils/TemplateUtils.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/ByteBuffer.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>
#include <cstdint>
#include <map>

namespace cleanMqtt
{
	namespace mqtt
	{
		enum class PropertyType : std::uint8_t
		{
			PAYLOAD_FORMAT_INDICATOR = 1U, //UINT8 - PUBLISH, Will Properties
			MESSAGE_EXPIRY_INTERVAL = 2U, //UINT32 - PUBLISH, Will Properties
			CONTENT_TYPE = 3U, //UTF8 - PUBLISH, Will Properties
			RESPONSE_TOPIC = 8U, //UTF8 - PUBLISH, Will Properties
			CORRELATION_DATA = 9U, //BinaryData - PUBLISH, Will Properties
			SUBSCRIPTION_IDENTIFIER = 11U, //VariableByteInteger - PUBLISH, SUBSCRIBE
			SESSION_EXPIRY_INTERVAL = 17U, //UINT32 - CONNECT, CONNACK, DISCONNECT
			ASSIGNED_CLIENT_IDENTIFIER = 18U, //UTF8 - CONNACK
			SERVER_KEEP_ALIVE = 19U, //UINT16 - CONNACK
			AUTHENTICATION_METHOD = 21U, //UTF8 - CONNECT, CONNACK, AUTH
			AUTHENTICATION_DATA = 22U, //BinaryData - CONNECT, CONNACK, AUTH
			REQUEST_PROBLEM_INFORMATION = 23U, //UINT8 - CONNECT
			WILL_DELAY_INTERVAL = 24U, //UINT32 - Will Properties
			REQUEST_RESPONSE_INFORMATION = 25U, //UINT8 - CONNECT
			RESPONSE_INFORMATION = 26U, //UTF8 - CONNACK
			SERVER_REFERENCE = 28U, //UTF8 - CONNACK, DISCONNECT
			REASON_STRING = 31U, //UTF8 - CONNACK, PUBACK, PUBREC, PUBREL, PUBCOMP, SUBACK, UNSUBACK, DISCONNECT, AUTH
			RECEIVE_MAXIMUM = 33U, //UINT16 - CONNECT, CONNACK
			TOPIC_ALIAS_MAXIMUM = 34U, //UINT16 - CONNECT, CONNACK
			TOPIC_ALIAS = 35U, //UINT16 - PUBLISH
			MAXIMUM_QOS = 36U, //UINT8 - CONNACK
			RETAIN_AVAILABLE = 37U, //UINT8 - CONNACK
			USER_PROPERTY = 38U, //UTF8 Pair - CONNECT, CONNACK, PUBLISH, Will Properties, PUBACK, PUBREC, PUBREL, PUBCOMP, SUBSCRIBE, SUBACK, UNSUBSCRIBE, UNSUBACK, DISCONNECT, AUTH
			MAXIMUM_PACKET_SIZE = 39U, //UINT32 - CONNECT, CONNACK
			WILDCARD_SUBSCRIPTION_AVAILABLE = 40U, //UINT8 - CONNACK
			SUBSCRIPTION_IDENTIFIER_AVAILABLE = 41U, //UINT8 - CONNACK
			SHARED_SUBSCRIPTION_AVAILABLE = 42U, //UINT8 - CONNACK

			_COUNT
		};

		//Property identifiers are actually Variable Byte Integers, but this library assumes they fit within 0U - 127U and encodes using a byte.
		//MQTT spec 5.1 at time of writting has all property IDs below 127U.
		//Rewrite to use variable byte integer if property IDs increase beyond 127U.
		static_assert(static_cast<std::uint16_t>(PropertyType::_COUNT) <= 127, "PropertyType values exceed 127, rewrite to use variable byte integer!");

		enum class PropertyDataType : std::uint8_t
		{
			UINT8 = 1 << 0,
			UINT16 = 1 << 1,
			UINT32 = 1 << 2,
			VARIABLE_BYTE = 1 << 3,
			UTF8 = 1 << 4,
			UTF8_PAIR = 1 << 5,
			BINARY_DATA = 1 << 6
		};

		ENUM_FLAG_OPERATORS(PropertyDataType)

			constexpr PropertyDataType k_numericalDataType
		{
			PropertyDataType::UINT8 |
				PropertyDataType::UINT16 |
				PropertyDataType::UINT32
		};

		constexpr PropertyDataType k_mqttDataType
		{
			PropertyDataType::VARIABLE_BYTE |
			PropertyDataType::UTF8 |
			PropertyDataType::UTF8_PAIR |
			PropertyDataType::BINARY_DATA
		};

		using DecoderFunc = void* (*)(const ByteBuffer& buffer);
		using EncoderFunc = void (*)(ByteBuffer& buffer, PropertyType propertyType, const void* data);

		struct PropertyTraits
		{
			constexpr PropertyTraits(PropertyDataType type, EncoderFunc encoderFunc, DecoderFunc decoderFunc, bool allowDuplicates = false)
				: allowDuplicates{ allowDuplicates }, dataType{ type }, encoder{ encoderFunc }, decoder{ decoderFunc }
			{
			}

			const bool allowDuplicates;
			const PropertyDataType dataType;
			const EncoderFunc encoder;
			const DecoderFunc decoder;
		};

		namespace propertyEncodings
		{
			void encode(ByteBuffer& buffer, PropertyType property, const void* data);

			namespace
			{
				//TODO maybe we can reuse pointers rather than allocating new ones?

				void encodeUInt8(ByteBuffer& buffer, PropertyType property, const void* data)
				{
					const auto castData = static_cast<const std::uint8_t*>(data);
					buffer += static_cast<std::uint8_t>(property);
					buffer += *castData;
				}

				void encodeUInt16(ByteBuffer& buffer, PropertyType property, const void* data)
				{
					const auto castData = static_cast<const std::uint16_t*>(data);
					buffer += static_cast<std::uint8_t>(property);
					buffer.append(*castData);
				}

				void encodeUInt32(ByteBuffer& buffer, PropertyType property, const void* data)
				{
					const auto castData = static_cast<const std::uint32_t*>(data);
					buffer += static_cast<std::uint8_t>(property);
					buffer.append(*castData);
				}

				void encodeUTF8String(ByteBuffer& buffer, PropertyType property, const void* data)
				{
					const auto castData = static_cast<const UTF8String*>(data);
					buffer += static_cast<std::uint8_t>(property);
					castData->encode(buffer);
				}

				void encodeUTF8StringPair(ByteBuffer& buffer, PropertyType property, const void* data)
				{
					const auto castData = static_cast<const UTF8StringPair*>(data);
					buffer += static_cast<std::uint8_t>(property);
					castData->encode(buffer);
				}

				void encodeVariableByteInteger(ByteBuffer& buffer, PropertyType property, const void* data)
				{
					const auto castData = static_cast<const VariableByteInteger*>(data);
					buffer += static_cast<std::uint8_t>(property);
					castData->encode(buffer);
				}

				void encodeBinaryData(ByteBuffer& buffer, PropertyType property, const void* data)
				{
					const auto castData = static_cast<const BinaryData*>(data);
					buffer += static_cast<std::uint8_t>(property);
					castData->encode(buffer);
				}
			}
		}

		namespace propertyDecodings
		{
			void* decode(const ByteBuffer& buffer, PropertyType property);

			namespace
			{
				//TODO maybe we can reuse return pointers rather than allocating new ones? (careful if multithreaded!)

				void* decodeUInt8(const ByteBuffer& buffer)
				{
					return new std::uint8_t(buffer.readUint8());
				}

				void* decodeUInt16(const ByteBuffer& buffer)
				{
					return new std::uint16_t(buffer.readUInt16());
				}

				void* decodeUInt32(const ByteBuffer& buffer)
				{
					return new std::uint32_t(buffer.readUInt32());
				}

				void* decodeUTF8String(const ByteBuffer& buffer)
				{
					return new UTF8String(buffer);
				}

				void* decodeUTF8StringPair(const ByteBuffer& buffer)
				{
					return new UTF8StringPair(buffer);
				}

				void* decodeVariableByteInteger(const ByteBuffer& buffer)
				{
					return VariableByteInteger::tryCreateNewFromBuffer(buffer);
				}

				void* decodeBinaryData(const ByteBuffer& buffer)
				{
					return new BinaryData(buffer);
				}
			}
		}

		const std::map<PropertyType, PropertyTraits> k_propertyTraits = {
			{PropertyType::PAYLOAD_FORMAT_INDICATOR, {PropertyDataType::UINT8, propertyEncodings::encodeUInt8, propertyDecodings::decodeUInt8, false}},
			{PropertyType::REQUEST_PROBLEM_INFORMATION, {PropertyDataType::UINT8, propertyEncodings::encodeUInt8, propertyDecodings::decodeUInt8, false}},
			{PropertyType::REQUEST_RESPONSE_INFORMATION, {PropertyDataType::UINT8, propertyEncodings::encodeUInt8, propertyDecodings::decodeUInt8, false}},
			{PropertyType::MAXIMUM_QOS, {PropertyDataType::UINT8, propertyEncodings::encodeUInt8, propertyDecodings::decodeUInt8, false}},
			{PropertyType::RETAIN_AVAILABLE, {PropertyDataType::UINT8, propertyEncodings::encodeUInt8, propertyDecodings::decodeUInt8, false}},
			{PropertyType::WILDCARD_SUBSCRIPTION_AVAILABLE, {PropertyDataType::UINT8, propertyEncodings::encodeUInt8, propertyDecodings::decodeUInt8, false}},
			{PropertyType::SUBSCRIPTION_IDENTIFIER_AVAILABLE, {PropertyDataType::UINT8, propertyEncodings::encodeUInt8, propertyDecodings::decodeUInt8, false}},
			{PropertyType::SHARED_SUBSCRIPTION_AVAILABLE, {PropertyDataType::UINT8, propertyEncodings::encodeUInt8, propertyDecodings::decodeUInt8, false}},
			{PropertyType::MESSAGE_EXPIRY_INTERVAL, {PropertyDataType::UINT32, propertyEncodings::encodeUInt32, propertyDecodings::decodeUInt32, false}},
			{PropertyType::SESSION_EXPIRY_INTERVAL, {PropertyDataType::UINT32, propertyEncodings::encodeUInt32, propertyDecodings::decodeUInt32, false}},
			{PropertyType::WILL_DELAY_INTERVAL, {PropertyDataType::UINT32, propertyEncodings::encodeUInt32, propertyDecodings::decodeUInt32, false}},
			{PropertyType::MAXIMUM_PACKET_SIZE, {PropertyDataType::UINT32, propertyEncodings::encodeUInt32, propertyDecodings::decodeUInt32, false}},
			{PropertyType::CONTENT_TYPE, {PropertyDataType::UTF8, propertyEncodings::encodeUTF8String, propertyDecodings::decodeUTF8String, false}},
			{PropertyType::RESPONSE_TOPIC, {PropertyDataType::UTF8, propertyEncodings::encodeUTF8String, propertyDecodings::decodeUTF8String, false}},
			{PropertyType::ASSIGNED_CLIENT_IDENTIFIER, {PropertyDataType::UTF8, propertyEncodings::encodeUTF8String, propertyDecodings::decodeUTF8String, false}},
			{PropertyType::AUTHENTICATION_METHOD, {PropertyDataType::UTF8, propertyEncodings::encodeUTF8String, propertyDecodings::decodeUTF8String, false}},
			{PropertyType::RESPONSE_INFORMATION, {PropertyDataType::UTF8, propertyEncodings::encodeUTF8String, propertyDecodings::decodeUTF8String, false}},
			{PropertyType::SERVER_REFERENCE, {PropertyDataType::UTF8, propertyEncodings::encodeUTF8String, propertyDecodings::decodeUTF8String, false}},
			{PropertyType::REASON_STRING, {PropertyDataType::UTF8, propertyEncodings::encodeUTF8String, propertyDecodings::decodeUTF8String, false}},
			{PropertyType::CORRELATION_DATA, {PropertyDataType::BINARY_DATA, propertyEncodings::encodeBinaryData, propertyDecodings::decodeBinaryData, false}},
			{PropertyType::AUTHENTICATION_DATA, {PropertyDataType::BINARY_DATA, propertyEncodings::encodeBinaryData, propertyDecodings::decodeBinaryData, false}},
			{PropertyType::SUBSCRIPTION_IDENTIFIER, {PropertyDataType::VARIABLE_BYTE, propertyEncodings::encodeVariableByteInteger, propertyDecodings::decodeVariableByteInteger, false}},
			{PropertyType::SERVER_KEEP_ALIVE, {PropertyDataType::UINT16, propertyEncodings::encodeUInt16, propertyDecodings::decodeUInt16, false}},
			{PropertyType::RECEIVE_MAXIMUM, {PropertyDataType::UINT16, propertyEncodings::encodeUInt16, propertyDecodings::decodeUInt16, false}},
			{PropertyType::TOPIC_ALIAS_MAXIMUM, {PropertyDataType::UINT16, propertyEncodings::encodeUInt16, propertyDecodings::decodeUInt16, false}},
			{PropertyType::TOPIC_ALIAS, {PropertyDataType::UINT16, propertyEncodings::encodeUInt16, propertyDecodings::decodeUInt16, false}},
			{PropertyType::USER_PROPERTY, {PropertyDataType::UTF8_PAIR, propertyEncodings::encodeUTF8StringPair, propertyDecodings::decodeUTF8StringPair, true}}
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTYTYPE_H