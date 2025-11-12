#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTIES_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTIES_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/Packets/PropertyType.h>
#include <cleanMqtt/Interfaces/ILogger.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>
#include <cleanMqtt/Mqtt/Packets/ErrorCodes.h>

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <type_traits>
#include <cassert>

namespace cleanMqtt
{
	namespace mqtt
	{
		class Properties
		{
		public:
			DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(Properties)

				Properties() noexcept
			{
			}

			virtual ~Properties()
			{
				for (const auto& pair : m_properties)
				{
					if (pair.second != nullptr)
					{
						delete pair.second;
					}
				}
				m_properties.clear();
			}

			Properties(Properties&& other) noexcept
				: m_properties(std::move(other.m_properties)), m_propertiesSizeInBytes(other.m_propertiesSizeInBytes)
			{
				other.m_properties.clear();
			}

			Properties& operator=(Properties&& other) noexcept
			{
				if (this != &other)
				{
					m_properties = std::move(other.m_properties);
					m_propertiesSizeInBytes = other.m_propertiesSizeInBytes;
					other.m_properties.clear();
				}
				return *this;
			}

			/**
			 * Specialise for each property type to ensure correct data type is used. Ran into problems where I would pass a value in but would
			 * not get a compile error until runtime because the data type did not match the property type expected data type 
			 * (e.g. passing in a uint8_t for a property that expects a uint32_t).
			 */

			template<PropertyType T>
			typename std::enable_if<std::is_same<std::uint8_t, typename type_of<T>::type>::value, bool>::type
				tryAddProperty(const std::uint8_t& value, bool conditionalCheck = true)
			{
				return tryAddPropertyImpl<T>(value, conditionalCheck);
			}

			template<PropertyType T>
			typename std::enable_if<std::is_same<std::uint16_t, typename type_of<T>::type>::value, bool>::type
				tryAddProperty(const std::uint16_t& value, bool conditionalCheck = true)
			{
				return tryAddPropertyImpl<T>(value, conditionalCheck);
			}

			template<PropertyType T>
			typename std::enable_if<std::is_same<std::uint32_t, typename type_of<T>::type>::value, bool>::type
				tryAddProperty(const std::uint32_t& value, bool conditionalCheck = true)
			{
				return tryAddPropertyImpl<T>(value, conditionalCheck);
			}

			template<PropertyType T>
			typename std::enable_if<std::is_same<UTF8String, typename type_of<T>::type>::value, bool>::type
				tryAddProperty(UTF8String&& value, bool conditionalCheck = true)
			{
				return tryAddPropertyImpl<T>(std::move(value), conditionalCheck);
			}

			template<PropertyType T>
			typename std::enable_if<std::is_same<UTF8StringPair, typename type_of<T>::type>::value, bool>::type
				tryAddProperty(UTF8StringPair&& value, bool conditionalCheck = true)
			{
				return tryAddPropertyImpl<T>(std::move(value), conditionalCheck);
			}

			template<PropertyType T>
			typename std::enable_if<std::is_same<VariableByteInteger, typename type_of<T>::type>::value, bool>::type
				tryAddProperty(VariableByteInteger&& value, bool conditionalCheck = true)
			{
				return tryAddPropertyImpl<T>(std::move(value), conditionalCheck);
			}

			template<PropertyType T>
			typename std::enable_if<std::is_same<BinaryData, typename type_of<T>::type>::value, bool>::type
				tryAddProperty(BinaryData&& value, bool conditionalCheck = true)
			{
				return tryAddPropertyImpl<T>(std::move(value), conditionalCheck);
			}

			template<typename TPropertyDataType>
			bool tryGetProperty(PropertyType type, const TPropertyDataType*& outVal) const
			{
				assert(!k_propertyTypeAllowDuplicatesZeroIndexed[k_propertyTypeZeroedId[static_cast<std::uint8_t>(type)]]);

				const auto iter = m_properties.find(type);

				if (iter != m_properties.end())
				{
					outVal = (static_cast<TPropertyDataType*>((iter->second)));
					return true;
				}

				return false;
			}

			template<typename TPropertyDataType>
			bool tryGetProperty(PropertyType type, std::vector<const TPropertyDataType*>& outVals) const
			{
				assert(k_propertyTypeAllowDuplicatesZeroIndexed[k_propertyTypeZeroedId[static_cast<std::uint8_t>(type)]]);

				const auto& rangePair = m_properties.equal_range(type);

				for (auto it = rangePair.first; it != rangePair.second; ++it)
				{
					const TPropertyDataType* value = static_cast<TPropertyDataType*>(it->second);
					outVals.push_back(value);
				}

				return !outVals.empty();
			}

			std::uint32_t encodingSize() const
			{
				auto varBytes{ VariableByteInteger::tryCreateFromValue(m_propertiesSizeInBytes) };
				return static_cast<std::uint32_t>(varBytes.encodingSize()) + m_propertiesSizeInBytes;
			}

			std::uint32_t size() const
			{
				return m_propertiesSizeInBytes;
			}

			void encode(ByteBuffer& buffer) const
			{
				//Encode properties combined length.
				bool b;
				const auto varBytesInt{ VariableByteInteger::tryCreateFromValue(m_propertiesSizeInBytes, &b) };
				varBytesInt.encode(buffer);

				//Encode properties one by one
				for (const auto& pair : m_properties)
				{
					propertyEncodings::encode(buffer, pair.first, pair.second);
				}
			}

			DecodeResult decode(const ByteBuffer& buffer) noexcept
			{
				//Decode properties combined length
				bool isSuccess;
				const auto varBytes{ VariableByteInteger::tryCreateFromBuffer(buffer, &isSuccess) };

				if (!isSuccess)
				{
					return DecodeResult{ DecodeErrorCode::MALFORMED_PACKET,
						"Failed to get value from variable byte integer encoded bytes. Has Property length been included in packet?" };
				}

				m_propertiesSizeInBytes = varBytes.uint32Value();

				const std::size_t startingBufferCursor{ buffer.readCursor() };
				const std::size_t endBufferCursor{ startingBufferCursor + m_propertiesSizeInBytes };

				if (buffer.size() < endBufferCursor)
				{
					LogError("Properties", "Decoding properties would cause buffer overflow. Buffer Size: %d, Properties Decode End: %d", buffer.size(), endBufferCursor);
					return DecodeResult{ DecodeErrorCode::MALFORMED_PACKET,
						"Decoding properties would cause buffer overflow, buffer size: " +
						std::to_string(static_cast<std::size_t>(buffer.size())) +
						", Properties Decode End: " +
						std::to_string(static_cast<std::size_t>(endBufferCursor)) };
				}

				//Decode properties one by one
				while (buffer.readCursor() < endBufferCursor)
				{
					PropertyType type{};
					void* data{ nullptr };

					try
					{
						//Property ID - 1 byte
						type = static_cast<PropertyType>(buffer.readUint8());

						//Property Data - Bytes based on property type
						data = propertyDecodings::decode(buffer, type);
						assert(data != nullptr);
					}
					catch (const std::exception& e)
					{
						LogError("Properties", "Failed to decode property from buffer. Exception: %s", e.what());
						return DecodeResult{ DecodeErrorCode::MALFORMED_PACKET, "Failed to decode property from buffer: " + std::string(e.what()) };
					}

					if (!tryAddProperty(type, data))
					{
						delete data;
						LogError("Properties", "Failed to add property to properties list due to it already existing.");
						return DecodeResult{ DecodeErrorCode::PROTOCOL_ERROR, "Duplicate property not allowed for property type: " + std::to_string(static_cast<std::uint8_t>(type)) };
					}
				}

				if (buffer.readCursor() != endBufferCursor)
				{
					if (buffer.readCursor() > endBufferCursor)
					{
						LogError("Properties", "Decoded more data than expected from properties! Any subsequent decoding will be broken from this buffer!");
						return DecodeResult{ DecodeErrorCode::MALFORMED_PACKET, "Properties size is smaller than the bytes of data decoded for properties part of header." };
					}
					else
					{
						LogError("Properties", "Decoded less data than expected from properties! Any subsequent decoding will be broken from this buffer!");
						return DecodeResult{ DecodeErrorCode::MALFORMED_PACKET, "Properties size is bigger than the bytes of data decoded for properties part of header." };
					}
				}

				return DecodeResult{ DecodeErrorCode::NO_ERROR };
			}

		protected:

			template<PropertyType T, typename DataT>
			typename std::enable_if<!std::is_base_of<MqttDataType, DataT>::value, bool>::type
				tryAddPropertyImpl(const DataT& value, bool conditionalCheck = true)
			{
#ifdef FORCE_ADD_PROPERTIES
				(void)conditionalCheck;
#else
				if (!conditionalCheck)
				{
					return false;
				}
#endif

				static constexpr bool allowDuplicate{ k_propertyTypeAllowDuplicatesZeroIndexed[k_propertyTypeZeroedId[static_cast<std::uint8_t>(T)]] };
				if (!allowDuplicate)
				{
					const auto iter = m_properties.find(T);

					if (iter != m_properties.end())
					{
						LogError("Properties", "Cannot add duplicate property. Property Type: %d", static_cast<std::uint16_t>(T));
						return false;
					}
				}

				m_properties.insert(std::make_pair(T, new DataT(value)));
				m_propertiesSizeInBytes += 1 + sizeof(value);

				return true;
			}

			template<PropertyType T, typename DataT>
			typename std::enable_if<std::is_base_of<MqttDataType, DataT>::value, bool>::type
				tryAddPropertyImpl(DataT&& value, bool conditionalCheck = true)
			{
#ifdef FORCE_ADD_PROPERTIES
				(void)conditionalCheck;
#else
				if (!conditionalCheck)
				{
					return false;
				}
#endif

				static constexpr bool allowDuplicate{ k_propertyTypeAllowDuplicatesZeroIndexed[k_propertyTypeZeroedId[static_cast<std::uint8_t>(T)]] };
				if (!allowDuplicate)
				{
					const auto iter = m_properties.find(T);

					if (iter != m_properties.end())
					{
						LogError("Properties", "Cannot add duplicate property. Property Type: %d", static_cast<std::uint16_t>(T));
						return false;
					}
				}

				m_propertiesSizeInBytes += 1 + static_cast<std::uint32_t>(value.encodingSize());
				m_properties.insert(std::make_pair(T, new DataT(std::move(value))));

				return true;
			}

			bool tryAddProperty(PropertyType type, void* data)
			{
				bool allowDuplicate{ k_propertyTypeAllowDuplicatesZeroIndexed[k_propertyTypeZeroedId[static_cast<std::uint8_t>(type)]] };
				if (!allowDuplicate)
				{
					const auto iter = m_properties.find(type);

					if (iter != m_properties.end())
					{
						LogError("Properties", "Cannot add duplicate property. Property Type:", static_cast<std::uint16_t>(type));
						return false;
					}
				}

				m_properties.insert(std::make_pair(type, data));

				return true;
			}

			std::uint32_t m_propertiesSizeInBytes{ 0U };
			std::unordered_multimap<PropertyType, void*> m_properties;
		};
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTIES_H