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
		namespace packets
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
						delete pair.second;
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

				template<typename TPropertyDataType>
				typename std::enable_if<!std::is_base_of<MqttDataType, TPropertyDataType>::value, bool>::type
				tryAddProperty(PropertyType type, const TPropertyDataType& value, bool conditionalCheck = true)
				{
#ifdef FORCE_ADD_PROPERTIES
					(void)conditionalCheck;
#else
					if (!conditionalCheck)
					{
						return false;
					}
#endif

					auto it = k_propertyTraits.find(type);
					if (!it->second.allowDuplicates)
					{
						const auto iter = m_properties.find(type);

						if (iter != m_properties.end())
						{
							LogError("Properties", "Cannot add duplicate property. Property Type: %d", static_cast<std::uint16_t>(type));
							return false;
						}
					}

					m_properties.insert(std::make_pair(type, new TPropertyDataType(value)));
					m_propertiesSizeInBytes += 1 + sizeof(value);

					return true;
				}

				template<typename TPropertyDataType>
				typename std::enable_if<std::is_base_of<MqttDataType, TPropertyDataType>::value, bool>::type
				tryAddProperty(PropertyType type, TPropertyDataType&& value, bool conditionalCheck = true)
				{
#ifdef FORCE_ADD_PROPERTIES
					(void)conditionalCheck;
#else
					if (!conditionalCheck)
					{
						return false;
					}
#endif

					auto it = k_propertyTraits.find(type);
					if (!it->second.allowDuplicates)
					{
						const auto iter = m_properties.find(type);

						if (iter != m_properties.end())
						{
							LogError("Properties", "Cannot add duplicate property. Property Type: %d", static_cast<std::uint16_t>(type));
							return false;
						}
					}

					m_propertiesSizeInBytes += 1 + static_cast<std::uint32_t>(value.encodingSize());
					m_properties.insert(std::make_pair(type, new TPropertyDataType(std::move(value))));

					return true;
				}

				template<typename TPropertyDataType>
				bool tryGetProperty(PropertyType type, const TPropertyDataType*& outVal) const
				{
					auto it = k_propertyTraits.find(type);
					assert(!it->second.allowDuplicates);

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
					auto it = k_propertyTraits.find(type);
					assert(it->second.allowDuplicates);

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
					auto varBytes{ VariableByteInteger(m_propertiesSizeInBytes) };
					return static_cast<std::uint32_t>(varBytes.encodingSize()) + m_propertiesSizeInBytes;
				}

				std::uint32_t size() const
				{
					return m_propertiesSizeInBytes;
				}

				void encode(ByteBuffer& buffer) const
				{
					//Encode properties combined length.
					const auto varBytesInt = VariableByteInteger(m_propertiesSizeInBytes);
					varBytesInt.encode(buffer);

					//Encode properties one by one
					for (const auto& pair : m_properties)
					{
						propertyEncodings::encode(buffer, pair.first, pair.second);
					}
				}

				DecodeResult decode(const ByteBuffer& buffer)
				{
					//Decode properties combined length
					const VariableByteInteger propertySizeBytes(buffer);
					m_propertiesSizeInBytes = propertySizeBytes.uint32Value();
					
					const std::size_t startingBufferCursor = buffer.readCursor();
					const std::size_t endBufferCursor = startingBufferCursor + m_propertiesSizeInBytes;

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
						//Property ID - 1 byte
						PropertyType type{ static_cast<PropertyType>(buffer.readUint8()) };

						//Property Data - Bytes based on property type
						void* data{ propertyDecodings::decode(buffer, type) };
						assert(data != nullptr);

						if (!tryAddProperty(type, data))
						{
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

				bool tryAddProperty(PropertyType type, void* data)
				{
					auto it = k_propertyTraits.find(type);
					if (!it->second.allowDuplicates)
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
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTIES_H