#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTIES_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_PROPERTIES_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Mqtt/Packets/PropertyType.h>
#include <cleanMqtt/Interfaces/ILogger.h>
#include <cleanMqtt/Mqtt/Packets/DataTypes.h>

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

					//Property identifiers are actually Variable Byte Integers, but this library assumes they fit within 0U - 127U and encodes using a byte.
					//MQTT spec 5.1 at time of writting has all property IDs below 127U.
					//Rewrite to use variable byte integer if property IDs increase beyond 127U.
					assert(static_cast<std::uint8_t>(type) < 127);

					auto it = k_propertyTraits.find(type);
					if (!it->second.allowDuplicates)
					{
						const auto iter = m_properties.find(type);

						if (iter != m_properties.end())
						{
							Log(LogLevel::Error, "Properties", "Cannot add duplicate property.");
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

					//Property identifiers are actually Variable Byte Integers, but this library assumes they fit within 0U - 127U and encodes using a byte.
					//MQTT spec 5.1 at time of writting has all property IDs below 127U.
					//Rewrite to use variable byte integer if property IDs increase beyond 127U.
					assert(static_cast<std::uint8_t>(type) < 127);

					auto it = k_propertyTraits.find(type);
					if (!it->second.allowDuplicates)
					{
						const auto iter = m_properties.find(type);

						if (iter != m_properties.end())
						{
							Log(LogLevel::Error, "Properties", "Cannot add duplicate property.");
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

				void decode(const ByteBuffer& buffer)
				{
					//Decode properties combined length
					const VariableByteInteger propertySizeBytes(buffer);
					m_propertiesSizeInBytes = propertySizeBytes.uint32Value();
					
					const std::size_t startingBufferCursor = buffer.readCursor();
					const std::size_t endBufferCursor = startingBufferCursor + m_propertiesSizeInBytes;

					//Decode properties one by one
					while (buffer.readCursor() < endBufferCursor)
					{
						//Property ID - 1 byte
						PropertyType type{ static_cast<PropertyType>(buffer.readUint8()) };

						//Property Data - Bytes based on property type
						void* data = propertyDecodings::decode(buffer, type);

						if (!tryAddProperty(type, data))
						{
							Exception(LogLevel::Error, "Properties", std::runtime_error("Something went wrong! Failed to add decoded property to properties list."));
						}
					}

					if (buffer.readCursor() > endBufferCursor)
					{
						Exception(
							LogLevel::Fatal,
							"Properties",
							std::out_of_range("Decoded more data than excpected! Any subsequent decoding will be broken from this buffer!"));
					}
				}

			protected:

				bool tryAddProperty(PropertyType type, void* data)
				{
					//Property identifiers are actually Variable Byte Integers, but this library assumes they fit within 0U - 127U and encodes using a byte.
					//MQTT spec 5.1 at time of writting has all property IDs below 127U.
					//Rewrite to use variable byte integer if property IDs increase beyond 127U.
					assert(static_cast<std::uint8_t>(type) < 127);

					auto it = k_propertyTraits.find(type);
					if (!it->second.allowDuplicates)
					{
						const auto iter = m_properties.find(type);

						if (iter != m_properties.end())
						{
							Log(LogLevel::Error, "Properties", "Cannot add duplicate property.");
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