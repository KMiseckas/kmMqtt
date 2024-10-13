#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_DATATYPES_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_DATATYPES_H

#include <cleanMqtt/Interfaces/ILogger.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/ByteBuffer.h>

#include <cstdint>
#include <exception>
#include <string>
#include <stdexcept>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			struct MqttDataType
			{
			public:
				virtual std::size_t encodingSize() const noexcept = 0;
				virtual void encode(ByteBuffer& buffer) const = 0;
			};

			struct BinaryData : public MqttDataType
			{
			public:
				DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(BinaryData)

				~BinaryData()
				{
					delete[] m_bytes;
				}

				BinaryData() noexcept
					: m_size(0U), m_bytes(nullptr)
				{
				}

				BinaryData(std::uint16_t size, const std::uint8_t* bytes) noexcept
					: m_size(size), m_bytes(new uint8_t[size])
				{
					std::memcpy(m_bytes, bytes, size);
				}

				BinaryData(const ByteBuffer& buffer)
				{
					decode(buffer);
				}

				BinaryData(BinaryData&& other) noexcept
					: m_size(other.m_size), m_bytes(other.m_bytes)
				{
					other.m_bytes = nullptr;
					other.m_size = 0U;
				}

				BinaryData& operator=(BinaryData&& other) noexcept
				{
					if (this == &other)
					{
						return *this;
					}

					m_size = other.m_size;
					m_bytes = other.m_bytes;

					other.m_bytes = nullptr;
					other.m_size = 0U;

					return *this;
				}

				std::size_t encodingSize() const noexcept override
				{
					return static_cast<std::size_t>(m_size) + sizeof(m_size);
				}

				std::uint16_t size() const noexcept
				{
					return m_size;
				}

				const std::uint8_t* bytes() const noexcept
				{
					return m_bytes;
				}

				void encode(ByteBuffer& buffer) const
				{
					buffer.append(m_size);
					buffer.append(m_bytes, m_size);
				}

				void decode(const ByteBuffer& buffer)
				{
					m_size = buffer.readUInt16();

					if (m_size > 0)
					{
						delete[] m_bytes;
						m_bytes = new std::uint8_t[m_size];
						std::memcpy(m_bytes, &buffer[buffer.readCursor()], m_size);
						buffer.incrementReadCursor(m_size);
					}
				}

			private:
				std::uint16_t m_size{ 0U };
				std::uint8_t* m_bytes{ nullptr };
			};

			struct VariableByteInteger : public MqttDataType
			{
			public:
				VariableByteInteger(std::uint32_t value = 0U)
					: m_value(value), m_size(1)
				{
					if (m_value > 0)
					{
						encodeValue(m_value);
					}
				}

				VariableByteInteger(const ByteBuffer& buffer)
					:m_size(1), m_value(0), m_encodedValue(0)
				{
					decode(buffer);
				}

				VariableByteInteger& operator=(const std::uint32_t& other)
				{
					m_value = other;
					encodeValue(m_value);
					return *this;
				}

				constexpr std::uint32_t uint32Value() const noexcept
				{
					return m_value;
				}

				constexpr std::uint32_t uint32EncodedBytes() const noexcept
				{
					return m_encodedValue;
				}

				std::size_t encodingSize() const noexcept override
				{
					return static_cast<std::size_t>(m_size);
				}

				void encode(ByteBuffer& buffer) const
				{
					const std::uint32_t length = uint32EncodedBytes();
					static constexpr std::size_t maxBytesShift = sizeof(std::uint32_t) - 1;
					for (size_t i = 0; i < encodingSize(); ++i)
					{
						buffer += ((length >> ((maxBytesShift - i) * 8)) & 0xff);
					}
				}

				void decode(const ByteBuffer& buffer)
				{
					decodeBytes(buffer.bytes());
					buffer.incrementReadCursor(m_size);
				}

				void decode(const std::uint8_t* buffer)
				{
					decodeBytes(buffer);
				}

			private:

				constexpr void encodeValue(std::uint32_t value)
				{
					constexpr std::uint32_t maxByteSize = 268435455;

					if (value > maxByteSize)
					{
						Exception(LogLevel::Error, "DataTypes", std::runtime_error("Cannot encode value larger than 268,435,455"));
					}

					std::uint8_t encodedByte{ 0U };
					std::uint8_t i{ 4 };

					do
					{
						--i;
						encodedByte = value % 128;
						value = value / 128;

						if (value > 0)
						{
							encodedByte = encodedByte | 128;
						}

						m_encodedValue |= encodedByte << (i * 8);
					} while (value > 0);

					m_size = 4U - i;
				}

				constexpr void decodeBytes(const std::uint8_t* bytes)
				{
					std::uint32_t multiplier{ 1 };
					m_value = 0;
					m_size = 0;

					std::uint8_t encodedByte{ 0U };

					do
					{
						m_size++;
						encodedByte = *(bytes++);

						m_value += (encodedByte & 127) * multiplier;

						if (multiplier > (128 * 128 * 128))
						{
							throw std::exception("Could not decode malformed variable byte integer data.");
						}

						multiplier *= 128;

					} while ((encodedByte & 128) != 0);
				}

				std::uint8_t m_size{ 1 };
				std::uint32_t m_value{ 0 };
				std::uint32_t m_encodedValue{ 0 };
			};

			struct UTF8String : public MqttDataType
			{
			public:
				UTF8String() noexcept
				{
				}

				UTF8String(std::uint16_t size, const char* val)
					: m_size(size), m_bytes(new std::uint8_t[size])
				{
					if (m_size > 0)
					{
						std::memcpy(m_bytes, val, m_size);
					}
				}

				UTF8String(const ByteBuffer& buffer)
				{
					decode(buffer);
				}

				UTF8String(const std::string& val)
				{
					if (val.size() > 65535)
					{
						Exception(LogLevel::Error, "UTF8String", std::runtime_error("Construction from string failed. Cannot exceed size of 65535 (uint16_t)."));
					}

					m_size = static_cast<std::uint16_t>(val.size());
					m_bytes = new std::uint8_t[val.size()];

					if (m_size > 0)
					{
						std::memcpy(m_bytes, val.c_str(), m_size);
					}
				}

				UTF8String(const UTF8String& other)
					:m_size(other.m_size), m_bytes(new std::uint8_t[other.m_size])
				{
					if (m_size > 0)
					{
						std::memcpy(m_bytes, other.m_bytes, m_size);
					}
				}

				UTF8String(UTF8String&& other) noexcept
					:m_size(other.m_size), m_bytes(other.m_bytes)
				{
					other.m_size = 0U;
					other.m_bytes = nullptr;
				}

				UTF8String& operator=(UTF8String&& other) noexcept
				{
					m_size = other.m_size;
					m_bytes = other.m_bytes;

					other.m_size = 0U;
					other.m_bytes = nullptr;

					return *this;
				}

				UTF8String& operator=(const UTF8String& other) noexcept
				{
					if (this == &other)
					{
						return *this;
					}

					delete[] m_bytes;

					m_size = other.m_size;
					if (m_size > 0)
					{
						m_bytes = new std::uint8_t[m_size];
						std::memcpy(m_bytes, other.m_bytes, m_size);
					}
					else
					{
						m_bytes = nullptr;
					}

					return *this;
				}

				UTF8String& operator=(const std::string& other)
				{
					if (other.size() > 65535)
					{
						Exception(LogLevel::Error, "UTF8String", std::runtime_error("Construction from string failed. Cannot exceed size of 65535 (uint16_t)."));
					}

					delete[] m_bytes;

					m_size = static_cast<std::uint16_t>(other.size());
					if (other.size() > 0)
					{
						m_bytes = new std::uint8_t[m_size];
						std::memcpy(m_bytes, other.c_str(), m_size);
					}
					else
					{
						m_bytes = nullptr;
					}

					return *this;
				}

				~UTF8String()
				{
					delete[] m_bytes;
				}

				std::size_t encodingSize() const noexcept override
				{
					return static_cast<std::size_t>(m_size) + sizeof(m_size);
				}

				void encode(ByteBuffer& buffer) const
				{
					buffer.append(m_size);
					buffer.append(m_bytes, m_size);
				}

				void decode(const ByteBuffer& buffer)
				{
					m_size = buffer.readUInt16();

					if (m_size > 0)
					{
						delete[] m_bytes;
						m_bytes = new std::uint8_t[m_size];
						std::memcpy(m_bytes, &buffer[buffer.readCursor()], m_size);
						buffer.incrementReadCursor(m_size);
					}
				}

				std::uint16_t stringSize() const noexcept
				{
					return static_cast<std::size_t>(m_size);
				}

				const std::uint8_t* stringBytes() const noexcept
				{
					return m_bytes;
				}

				std::string getString() const noexcept
				{
					return std::string(reinterpret_cast<const char*>(m_bytes), m_size);
				}

			private:
				std::uint16_t m_size{ 0U };
				std::uint8_t* m_bytes{ nullptr };
			};

			struct UTF8StringPair : public MqttDataType
			{
				UTF8StringPair() noexcept
				{
				}

				UTF8StringPair(std::uint16_t firstSize, const char* firstVal, std::uint16_t secondSize, const char* secondVal)
					:m_first(firstSize, firstVal), m_second(secondSize, secondVal)
				{
				}

				UTF8StringPair(const std::string& firstVal, const std::string& secondVal)
					: m_first(firstVal), m_second(secondVal)
				{
				}

				UTF8StringPair(const ByteBuffer& buffer)
				{
					m_first.decode(buffer);
					m_second.decode(buffer);
				}

				UTF8StringPair(const UTF8StringPair& other)
					:m_first(other.m_first), m_second(other.m_second)
				{
				}

				UTF8StringPair(UTF8StringPair&& other) noexcept
					:m_first(std::move(other.m_first)), m_second(std::move(other.m_second))
				{
				}

				std::size_t encodingSize() const noexcept override
				{
					return m_first.encodingSize() + m_second.encodingSize();
				}

				void encode(ByteBuffer& buffer) const
				{
					m_first.encode(buffer);
					m_second.encode(buffer);
				}

				const UTF8String& first() const noexcept
				{
					return m_first;
				}

				const UTF8String& second() const noexcept
				{
					return m_second;
				}

			private:
				UTF8String m_first;
				UTF8String m_second;
			};
		}
	}
}

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_DATATYPES_H