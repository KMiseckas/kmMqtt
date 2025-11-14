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
		/**
		 * Property types as defined in the MQTT 5 spec with exact int IDs.
		 */
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

		/**
		 * Property types in the zero indexed order as they appear in the MQTT 5 spec.
		 */
		enum class PropertyTypeOrdered : std::uint8_t
		{
			PAYLOAD_FORMAT_INDICATOR, //UINT8 - PUBLISH, Will Properties
			MESSAGE_EXPIRY_INTERVAL, //UINT32 - PUBLISH, Will Properties
			CONTENT_TYPE, //UTF8 - PUBLISH, Will Properties
			RESPONSE_TOPIC, //UTF8 - PUBLISH, Will Properties
			CORRELATION_DATA, //BinaryData - PUBLISH, Will Properties
			SUBSCRIPTION_IDENTIFIER, //VariableByteInteger - PUBLISH, SUBSCRIBE
			SESSION_EXPIRY_INTERVAL, //UINT32 - CONNECT, CONNACK, DISCONNECT
			ASSIGNED_CLIENT_IDENTIFIER, //UTF8 - CONNACK
			SERVER_KEEP_ALIVE, //UINT16 - CONNACK
			AUTHENTICATION_METHOD, //UTF8 - CONNECT, CONNACK, AUTH
			AUTHENTICATION_DATA, //BinaryData - CONNECT, CONNACK, AUTH
			REQUEST_PROBLEM_INFORMATION, //UINT8 - CONNECT
			WILL_DELAY_INTERVAL, //UINT32 - Will Properties
			REQUEST_RESPONSE_INFORMATION, //UINT8 - CONNECT
			RESPONSE_INFORMATION, //UTF8 - CONNACK
			SERVER_REFERENCE, //UTF8 - CONNACK, DISCONNECT
			REASON_STRING, //UTF8 - CONNACK, PUBACK, PUBREC, PUBREL, PUBCOMP, SUBACK, UNSUBACK, DISCONNECT, AUTH
			RECEIVE_MAXIMUM, //UINT16 - CONNECT, CONNACK
			TOPIC_ALIAS_MAXIMUM, //UINT16 - CONNECT, CONNACK
			TOPIC_ALIAS, //UINT16 - PUBLISH
			MAXIMUM_QOS, //UINT8 - CONNACK
			RETAIN_AVAILABLE, //UINT8 - CONNACK
			USER_PROPERTY, //UTF8 Pair - CONNECT, CONNACK, PUBLISH, Will Properties, PUBACK, PUBREC, PUBREL, PUBCOMP, SUBSCRIBE, SUBACK, UNSUBSCRIBE, UNSUBACK, DISCONNECT, AUTH
			MAXIMUM_PACKET_SIZE, //UINT32 - CONNECT, CONNACK
			WILDCARD_SUBSCRIPTION_AVAILABLE, //UINT8 - CONNACK
			SUBSCRIPTION_IDENTIFIER_AVAILABLE, //UINT8 - CONNACK
			SHARED_SUBSCRIPTION_AVAILABLE, //UINT8 - CONNACK

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

		template<PropertyType>
		struct type_of;
		
		template<> struct type_of<PropertyType::PAYLOAD_FORMAT_INDICATOR> { using type = std::uint8_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT8; };
		template<> struct type_of<PropertyType::MESSAGE_EXPIRY_INTERVAL> { using type = std::uint32_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT32; };
		template<> struct type_of<PropertyType::CONTENT_TYPE> { using type = UTF8String; static constexpr PropertyDataType enumType = PropertyDataType::UTF8; };
		template<> struct type_of<PropertyType::RESPONSE_TOPIC> { using type = UTF8String; static constexpr PropertyDataType enumType = PropertyDataType::UTF8; };
		template<> struct type_of<PropertyType::CORRELATION_DATA> { using type = BinaryData; static constexpr PropertyDataType enumType = PropertyDataType::BINARY_DATA;};
		template<> struct type_of<PropertyType::SUBSCRIPTION_IDENTIFIER> { using type = VariableByteInteger; static constexpr PropertyDataType enumType = PropertyDataType::VARIABLE_BYTE; };
		template<> struct type_of<PropertyType::SESSION_EXPIRY_INTERVAL> { using type = std::uint32_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT32; };
		template<> struct type_of<PropertyType::ASSIGNED_CLIENT_IDENTIFIER> { using type = UTF8String; static constexpr PropertyDataType enumType = PropertyDataType::UTF8; };
		template<> struct type_of<PropertyType::SERVER_KEEP_ALIVE> { using type = std::uint16_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT16; };
		template<> struct type_of<PropertyType::AUTHENTICATION_METHOD> { using type = UTF8String; static constexpr PropertyDataType enumType = PropertyDataType::UTF8; };
		template<> struct type_of<PropertyType::AUTHENTICATION_DATA> { using type = BinaryData; static constexpr PropertyDataType enumType = PropertyDataType::BINARY_DATA; };
		template<> struct type_of<PropertyType::REQUEST_PROBLEM_INFORMATION> { using type = std::uint8_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT8; };
		template<> struct type_of<PropertyType::WILL_DELAY_INTERVAL> { using type = std::uint32_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT32; };
		template<> struct type_of<PropertyType::REQUEST_RESPONSE_INFORMATION> { using type = std::uint8_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT8; };
		template<> struct type_of<PropertyType::RESPONSE_INFORMATION> { using type = UTF8String; static constexpr PropertyDataType enumType = PropertyDataType::UTF8; };
		template<> struct type_of<PropertyType::SERVER_REFERENCE> { using type = UTF8String; static constexpr PropertyDataType enumType = PropertyDataType::UTF8; };
		template<> struct type_of<PropertyType::REASON_STRING> { using type = UTF8String; static constexpr PropertyDataType enumType = PropertyDataType::UTF8; };
		template<> struct type_of<PropertyType::RECEIVE_MAXIMUM> { using type = std::uint16_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT16; };
		template<> struct type_of<PropertyType::TOPIC_ALIAS_MAXIMUM> { using type = std::uint16_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT16; };
		template<> struct type_of<PropertyType::TOPIC_ALIAS> { using type = std::uint16_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT16; };
		template<> struct type_of<PropertyType::MAXIMUM_QOS> { using type = std::uint8_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT8; };
		template<> struct type_of<PropertyType::RETAIN_AVAILABLE> { using type = std::uint8_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT8; };
		template<> struct type_of<PropertyType::USER_PROPERTY> { using type = UTF8StringPair; static constexpr PropertyDataType enumType = PropertyDataType::UTF8_PAIR; };
		template<> struct type_of<PropertyType::MAXIMUM_PACKET_SIZE> { using type = std::uint32_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT32; };
		template<> struct type_of<PropertyType::WILDCARD_SUBSCRIPTION_AVAILABLE> { using type = std::uint8_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT8; };
		template<> struct type_of<PropertyType::SUBSCRIPTION_IDENTIFIER_AVAILABLE> { using type = std::uint8_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT8; };
		template<> struct type_of<PropertyType::SHARED_SUBSCRIPTION_AVAILABLE> { using type = std::uint8_t; static constexpr PropertyDataType enumType = PropertyDataType::UINT8; };

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

		namespace propertyEncodings
		{
			void encode(ByteBuffer& buffer, PropertyType property, const void* data);

			namespace
			{
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
					auto bd = new BinaryData();
					bd->decode(buffer);
					return bd;
				}
			}
		}

#define AsInt(enum) static_cast<std::uint8_t>(enum)

		/**
		 * Mapping from PropertyType to its data type.
		 */
		constexpr PropertyDataType k_propertyTypeDataZeroIndexed[43/**PropertyType::_COUNT*/]
		{
			PropertyDataType::UINT8,//PAYLOAD_FORMAT_INDICATOR
			PropertyDataType::UINT32,//MESSAGE_EXPIRY_INTERVAL
			PropertyDataType::UTF8,//CONTENT_TYPE
			PropertyDataType::UTF8,//RESPONSE_TOPIC
			PropertyDataType::BINARY_DATA,//CORRELATION_DATA
			PropertyDataType::VARIABLE_BYTE,//SUBSCRIPTION_IDENTIFIER
			PropertyDataType::UINT32,//SESSION_EXPIRY_INTERVAL
			PropertyDataType::UTF8,//ASSIGNED_CLIENT_IDENTIFIER
			PropertyDataType::UINT16,//SERVER_KEEP_ALIVE
			PropertyDataType::UTF8,//AUTHENTICATION_METHOD
			PropertyDataType::BINARY_DATA,//AUTHENTICATION_DATA
			PropertyDataType::UINT8,//REQUEST_PROBLEM_INFORMATION
			PropertyDataType::UINT32,//WILL_DELAY_INTERVAL
			PropertyDataType::UINT8,//REQUEST_RESPONSE_INFORMATION
			PropertyDataType::UTF8,//RESPONSE_INFORMATION
			PropertyDataType::UTF8,//SERVER_REFERENCE
			PropertyDataType::UTF8,//REASON_STRING
			PropertyDataType::UINT16,//RECEIVE_MAXIMUM
			PropertyDataType::UINT16,//TOPIC_ALIAS_MAXIMUM
			PropertyDataType::UINT16,//TOPIC_ALIAS
			PropertyDataType::UINT8,//MAXIMUM_QOS
			PropertyDataType::UINT8,//RETAIN_AVAILABLE
			PropertyDataType::UTF8_PAIR,//USER_PROPERTY
			PropertyDataType::UINT32,//MAXIMUM_PACKET_SIZE
			PropertyDataType::UINT8,//WILDCARD_SUBSCRIPTION_AVAILABLE
			PropertyDataType::UINT8,//SUBSCRIPTION_IDENTIFIER_AVAILABLE
			PropertyDataType::UINT8//SHARED_SUBSCRIPTION_AVAILABLE
		};

		static_assert(static_cast<std::size_t>(PropertyType::_COUNT) == sizeof(k_propertyTypeDataZeroIndexed) / sizeof(PropertyDataType), "k_propertyTypeDataZeroIndexed size mismatch!");

		/**
		 * Mapping from PropertyType to its encoder function.
		 */
		constexpr EncoderFunc k_propertyTypeEncodersZeroIndexed[43/**PropertyType::_COUNT*/]
		{
			propertyEncodings::encodeUInt8,//PAYLOAD_FORMAT_INDICATOR
			propertyEncodings::encodeUInt32,//MESSAGE_EXPIRY_INTERVAL
			propertyEncodings::encodeUTF8String,//CONTENT_TYPE
			propertyEncodings::encodeUTF8String,//RESPONSE_TOPIC
			propertyEncodings::encodeBinaryData,//CORRELATION_DATA
			propertyEncodings::encodeVariableByteInteger,//SUBSCRIPTION_IDENTIFIER
			propertyEncodings::encodeUInt32,//SESSION_EXPIRY_INTERVAL
			propertyEncodings::encodeUTF8String,//ASSIGNED_CLIENT_IDENTIFIER
			propertyEncodings::encodeUInt16,//SERVER_KEEP_ALIVE
			propertyEncodings::encodeUTF8String,//AUTHENTICATION_METHOD
			propertyEncodings::encodeBinaryData,//AUTHENTICATION_DATA
			propertyEncodings::encodeUInt8,//REQUEST_PROBLEM_INFORMATION
			propertyEncodings::encodeUInt32,//WILL_DELAY_INTERVAL
			propertyEncodings::encodeUInt8,//REQUEST_RESPONSE_INFORMATION
			propertyEncodings::encodeUTF8String,//RESPONSE_INFORMATION
			propertyEncodings::encodeUTF8String,//SERVER_REFERENCE
			propertyEncodings::encodeUTF8String,//REASON_STRING
			propertyEncodings::encodeUInt16,//RECEIVE_MAXIMUM
			propertyEncodings::encodeUInt16,//TOPIC_ALIAS_MAXIMUM
			propertyEncodings::encodeUInt16,//TOPIC_ALIAS
			propertyEncodings::encodeUInt8,//MAXIMUM_QOS
			propertyEncodings::encodeUInt8,//RETAIN_AVAILABLE
			propertyEncodings::encodeUTF8StringPair,//USER_PROPERTY
			propertyEncodings::encodeUInt32,//MAXIMUM_PACKET_SIZE
			propertyEncodings::encodeUInt8,//WILDCARD_SUBSCRIPTION_AVAILABLE
			propertyEncodings::encodeUInt8,//SUBSCRIPTION_IDENTIFIER_AVAILABLE
			propertyEncodings::encodeUInt8//SHARED_SUBSCRIPTION_AVAILABLE
		};

		static_assert(static_cast<std::size_t>(PropertyType::_COUNT) == sizeof(k_propertyTypeEncodersZeroIndexed) / sizeof(EncoderFunc), "k_propertyTypeEncodersZeroIndexed size mismatch!");

		/**
		 * Mapping from PropertyType to its decoder function.
		 */
		constexpr DecoderFunc k_propertyTypeDecodersZeroIndexed[43/**PropertyType::_COUNT*/]
		{
			propertyDecodings::decodeUInt8,//PAYLOAD_FORMAT_INDICATOR
			propertyDecodings::decodeUInt32,//MESSAGE_EXPIRY_INTERVAL
			propertyDecodings::decodeUTF8String,//CONTENT_TYPE
			propertyDecodings::decodeUTF8String,//RESPONSE_TOPIC
			propertyDecodings::decodeBinaryData,//CORRELATION_DATA
			propertyDecodings::decodeVariableByteInteger,//SUBSCRIPTION_IDENTIFIER
			propertyDecodings::decodeUInt32,//SESSION_EXPIRY_INTERVAL
			propertyDecodings::decodeUTF8String,//ASSIGNED_CLIENT_IDENTIFIER
			propertyDecodings::decodeUInt16,//SERVER_KEEP_ALIVE
			propertyDecodings::decodeUTF8String,//AUTHENTICATION_METHOD
			propertyDecodings::decodeBinaryData,//AUTHENTICATION_DATA
			propertyDecodings::decodeUInt8,//REQUEST_PROBLEM_INFORMATION
			propertyDecodings::decodeUInt32,//WILL_DELAY_INTERVAL
			propertyDecodings::decodeUInt8,//REQUEST_RESPONSE_INFORMATION
			propertyDecodings::decodeUTF8String,//RESPONSE_INFORMATION
			propertyDecodings::decodeUTF8String,//SERVER_REFERENCE
			propertyDecodings::decodeUTF8String,//REASON_STRING
			propertyDecodings::decodeUInt16,//RECEIVE_MAXIMUM
			propertyDecodings::decodeUInt16,//TOPIC_ALIAS_MAXIMUM
			propertyDecodings::decodeUInt16,//TOPIC_ALIAS
			propertyDecodings::decodeUInt8,//MAXIMUM_QOS
			propertyDecodings::decodeUInt8,//RETAIN_AVAILABLE
			propertyDecodings::decodeUTF8StringPair,//USER_PROPERTY
			propertyDecodings::decodeUInt32,//MAXIMUM_PACKET_SIZE
			propertyDecodings::decodeUInt8,//WILDCARD_SUBSCRIPTION_AVAILABLE
			propertyDecodings::decodeUInt8,//SUBSCRIPTION_IDENTIFIER_AVAILABLE
			propertyDecodings::decodeUInt8//SHARED_SUBSCRIPTION_AVAILABLE
		};

		static_assert(static_cast<std::size_t>(PropertyType::_COUNT) == sizeof(k_propertyTypeDecodersZeroIndexed) / sizeof(DecoderFunc), "k_propertyTypeDecodersZeroIndexed size mismatch!");

		/**
		 * Mapping from PropertyType to its encoder function.
		 */
		constexpr bool k_propertyTypeAllowDuplicatesZeroIndexed [43 /**PropertyType::_COUNT*/]
		{
			false,//PAYLOAD_FORMAT_INDICATOR
			false,//MESSAGE_EXPIRY_INTERVAL
			false,//CONTENT_TYPE
			false,//RESPONSE_TOPIC
			false,//CORRELATION_DATA
			false,//SUBSCRIPTION_IDENTIFIER
			false,//SESSION_EXPIRY_INTERVAL
			false,//ASSIGNED_CLIENT_IDENTIFIER
			false,//SERVER_KEEP_ALIVE
			false,//AUTHENTICATION_METHOD
			false,//AUTHENTICATION_DATA
			false,//REQUEST_PROBLEM_INFORMATION
			false,//WILL_DELAY_INTERVAL
			false,//REQUEST_RESPONSE_INFORMATION
			false,//RESPONSE_INFORMATION
			false,//SERVER_REFERENCE
			false,//REASON_STRING
			false,//RECEIVE_MAXIMUM
			false,//TOPIC_ALIAS_MAXIMUM
			false,//TOPIC_ALIAS
			false,//MAXIMUM_QOS
			false,//RETAIN_AVAILABLE
			true,//USER_PROPERTY
			false,//MAXIMUM_PACKET_SIZE
			false,//WILDCARD_SUBSCRIPTION_AVAILABLE
			false,//SUBSCRIPTION_IDENTIFIER_AVAILABLE
			false//SHARED_SUBSCRIPTION_AVAILABLE
		};

		static_assert(static_cast<std::size_t>(PropertyType::_COUNT) == sizeof(k_propertyTypeAllowDuplicatesZeroIndexed) / sizeof(bool), "k_propertyTypeAllowDuplicatesZeroIndexed size mismatch!");

		/**
		 * Mapping from PropertyType to its ordered index.
		 */
		constexpr std::uint8_t k_propertyTypeZeroedId[43/**PropertyType::_COUNT*/] =
		{
			0xFF, // 0 - unused
			AsInt(PropertyTypeOrdered::PAYLOAD_FORMAT_INDICATOR), // 1 - PAYLOAD_FORMAT_INDICATOR
			AsInt(PropertyTypeOrdered::MESSAGE_EXPIRY_INTERVAL), // 2 - MESSAGE_EXPIRY_INTERVAL
			AsInt(PropertyTypeOrdered::CONTENT_TYPE), // 3 - CONTENT_TYPE
			0xFF, 0xFF, 0xFF, 0xFF, // 4-7 - unused
			AsInt(PropertyTypeOrdered::RESPONSE_TOPIC), // 8 - RESPONSE_TOPIC
			AsInt(PropertyTypeOrdered::CORRELATION_DATA), // 9 - CORRELATION_DATA
			0xFF, // 10 - unused
			AsInt(PropertyTypeOrdered::SUBSCRIPTION_IDENTIFIER), // 11 - SUBSCRIPTION_IDENTIFIER
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // 12-16 - unused
			AsInt(PropertyTypeOrdered::SESSION_EXPIRY_INTERVAL), // 17 - SESSION_EXPIRY_INTERVAL
			AsInt(PropertyTypeOrdered::ASSIGNED_CLIENT_IDENTIFIER), // 18 - ASSIGNED_CLIENT_IDENTIFIER
			AsInt(PropertyTypeOrdered::SERVER_KEEP_ALIVE), // 19 - SERVER_KEEP_ALIVE
			0xFF, // 20 - unused
			AsInt(PropertyTypeOrdered::AUTHENTICATION_METHOD), // 21 - AUTHENTICATION_METHOD
			AsInt(PropertyTypeOrdered::AUTHENTICATION_DATA), // 22 - AUTHENTICATION_DATA
			AsInt(PropertyTypeOrdered::REQUEST_PROBLEM_INFORMATION), // 23 - REQUEST_PROBLEM_INFORMATION
			AsInt(PropertyTypeOrdered::WILL_DELAY_INTERVAL), // 24 - WILL_DELAY_INTERVAL
			AsInt(PropertyTypeOrdered::REQUEST_RESPONSE_INFORMATION), // 25 - REQUEST_RESPONSE_INFORMATION
			AsInt(PropertyTypeOrdered::RESPONSE_INFORMATION), // 26 - RESPONSE_INFORMATION
			0xFF, // 27 - unused
			AsInt(PropertyTypeOrdered::SERVER_REFERENCE), // 28 - SERVER_REFERENCE
			0xFF, 0xFF, // 29-30 - unused
			AsInt(PropertyTypeOrdered::REASON_STRING), // 31 - REASON_STRING
			0xFF, // 32 - unused
			AsInt(PropertyTypeOrdered::RECEIVE_MAXIMUM), // 33 - RECEIVE_MAXIMUM
			AsInt(PropertyTypeOrdered::TOPIC_ALIAS_MAXIMUM), // 34 - TOPIC_ALIAS_MAXIMUM
			AsInt(PropertyTypeOrdered::TOPIC_ALIAS), // 35 - TOPIC_ALIAS
			AsInt(PropertyTypeOrdered::MAXIMUM_QOS), // 36 - MAXIMUM_QOS
			AsInt(PropertyTypeOrdered::RETAIN_AVAILABLE), // 37 - RETAIN_AVAILABLE
			AsInt(PropertyTypeOrdered::USER_PROPERTY), // 38 - USER_PROPERTY
			AsInt(PropertyTypeOrdered::MAXIMUM_PACKET_SIZE), // 39 - MAXIMUM_PACKET_SIZE
			AsInt(PropertyTypeOrdered::WILDCARD_SUBSCRIPTION_AVAILABLE), // 40 - WILDCARD_SUBSCRIPTION_AVAILABLE
			AsInt(PropertyTypeOrdered::SUBSCRIPTION_IDENTIFIER_AVAILABLE), // 41 - SUBSCRIPTION_IDENTIFIER_AVAILABLE
			AsInt(PropertyTypeOrdered::SHARED_SUBSCRIPTION_AVAILABLE) // 42 - SHARED_SUBSCRIPTION_AVAILABLE
		};

		static_assert(static_cast<std::size_t>(PropertyType::_COUNT) == sizeof(k_propertyTypeZeroedId) / sizeof(std::uint8_t), "k_propertyTypeZeroedId size mismatch!");

#undef AsInt
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTYTYPE_H