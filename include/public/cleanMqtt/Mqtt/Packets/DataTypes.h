#ifndef INCLUDE_CLEANMQTT_MQTT_PACKETS_DATATYPES_H
#define INCLUDE_CLEANMQTT_MQTT_PACKETS_DATATYPES_H

#include <cleanMqtt/Logger/Log.h>
#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/ByteBuffer.h>

#include <cstdint>
#include <exception>
#include <string>
#include <stdexcept>
#include <cassert>

namespace cleanMqtt
{
	namespace mqtt
	{
		struct MqttDataType
		{
		public:
			virtual ~MqttDataType() {};
			virtual std::size_t encodingSize() const noexcept = 0;
			virtual void encode(ByteBuffer& buffer) const = 0;
		};

		struct BinaryData : public MqttDataType
		{
		public:
			~BinaryData()
			{
				delete[] m_bytes;
			}

			BinaryData() noexcept
				: m_size(0U), m_bytes(nullptr)
			{
			}

			explicit BinaryData(std::uint16_t size, const std::uint8_t* bytes) noexcept
				: m_size(size), m_bytes(new uint8_t[size])
			{
				std::memcpy(m_bytes, bytes, size);
			}

			explicit BinaryData(const ByteBuffer& buffer) :
				BinaryData(static_cast<std::uint16_t>(buffer.size()), buffer.bytes())
			{
				assert(buffer.size() <= 65535 && "ByteBuffer size exceeds uint16_t max value for conversion to binary data type");
			}

			BinaryData(BinaryData&& other) noexcept
				: m_size(other.m_size), m_bytes(other.m_bytes)
			{
				other.m_bytes = nullptr;
				other.m_size = 0U;
			}

			BinaryData(const BinaryData& other) noexcept
				: m_size(other.m_size)
			{
				if (m_size > 0)
				{
					m_bytes = new std::uint8_t[m_size];
					std::memcpy(m_bytes, other.m_bytes, m_size);
				}
			}

			BinaryData& operator=(BinaryData&& other) noexcept
			{
				if (this == &other)
				{
					return *this;
				}

				delete[] m_bytes;

				m_size = other.m_size;
				m_bytes = other.m_bytes;

				other.m_bytes = nullptr;
				other.m_size = 0U;

				return *this;
			}

			BinaryData& operator=(const BinaryData& other) noexcept
			{
				if (this == &other)
				{
					return *this;
				}

				delete[] m_bytes;
				m_bytes = nullptr;

				m_size = other.m_size;

				if (m_size > 0)
				{
					m_bytes = new std::uint8_t[m_size];
					std::memcpy(m_bytes, other.m_bytes, m_size);
				}

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

			void encode(ByteBuffer& buffer) const override
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
			static constexpr std::uint32_t kmaxByteSize{ 268435455U };

		public:
			VariableByteInteger() : m_value(0), m_size(1), m_encodedValue(0)
			{
			}

			static VariableByteInteger tryCreateFromBuffer(const ByteBuffer& buffer, bool* isSuccess = nullptr) noexcept
			{
				if (buffer.readHeadroom() <= 0)
				{
					LogError("VariableByteInteger", "Buffer has no remaining bytes to read.");
					if (isSuccess != nullptr)
					{
						*isSuccess = false;
					}
					return VariableByteInteger(0);
				}

				if (isSuccess != nullptr)
				{
					*isSuccess = true;
				}
				return VariableByteInteger(buffer);
			}

			static VariableByteInteger tryCreateFromValue(const std::uint32_t& val, bool* isSuccess = nullptr) noexcept
			{
				if (val > kmaxByteSize)
				{
					LogError("VariableByteInteger", "Invalid argument val (%d), max value allowed is %d", val, kmaxByteSize);
					if (isSuccess != nullptr)
					{
						*isSuccess = false;
					}
					return VariableByteInteger(0);
				}

				if (isSuccess != nullptr)
				{
					*isSuccess = true;
				}
				return VariableByteInteger(val);
			}

			static VariableByteInteger* tryCreateNewFromBuffer(const ByteBuffer& buffer) noexcept
			{
				if (buffer.readHeadroom() <= 0)
				{
					LogError("VariableByteInteger", "Buffer has no remaining bytes to read.");
					return nullptr;
				}

				return new VariableByteInteger(buffer);
			}

			static VariableByteInteger* tryCreateNewFromValue(const std::uint32_t& val) noexcept
			{
				if (val > kmaxByteSize)
				{
					LogError("VariableByteInteger", "Invalid argument val (%d), max value allowed is %d", val, kmaxByteSize);
					return nullptr;
				}

				return new VariableByteInteger(val);
			}

			constexpr std::uint32_t uint32Value() const noexcept
			{
				return m_value;
			}

			constexpr bool setValue(std::uint32_t value) noexcept
			{
				if (value > kmaxByteSize)
				{
					LogError("VariableByteInteger", "Cannot set value larger than max size: %d", kmaxByteSize);
					return false;
				}

				m_value = value;
				m_encodedValue = 0;
				m_size = 1;
				encodeValue(m_value);
				return true;
			}

			constexpr std::uint32_t uint32EncodedBytes() const noexcept
			{
				return m_encodedValue;
			}

			std::size_t encodingSize() const noexcept override
			{
				return static_cast<std::size_t>(m_size);
			}

			void encode(ByteBuffer& buffer) const override
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
				decodeBytes(buffer.bytes() + buffer.readCursor());
				buffer.incrementReadCursor(m_size);
			}

			void decode(const std::uint8_t* buffer)
			{
				decodeBytes(buffer);
			}

		private:

			VariableByteInteger(std::uint32_t value)
				: m_value(value), m_size(1)
			{
				if (m_value > 0)
				{
					encodeValue(m_value);
				}
			}

			VariableByteInteger(const ByteBuffer& buffer)
				: m_value(0), m_size(1), m_encodedValue(0)
			{
				decode(buffer);
			}

			constexpr void encodeValue(std::uint32_t value)
			{
				if (value > kmaxByteSize)
				{
					LogError("DataTypes", "Cannot encode value larger than max size: %d", kmaxByteSize);
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

				bool found{ false };
				while (!found)
				{
					m_size++;
					encodedByte = *(bytes++);

					m_value += (encodedByte & 127) * multiplier;

					if (multiplier > (128 * 128 * 128))
					{
						LogError("DataTypes", "Could not decode malformed variable byte integer data.");
					}

					multiplier *= 128;
					found = (encodedByte & 128) == 0;
				}
			}

			std::uint32_t m_value{ 0 };
			std::uint8_t m_size{ 1 };
			std::uint32_t m_encodedValue{ 0 };
		};

		struct UTF8String : public MqttDataType
		{
		public:
			UTF8String() noexcept
			{
			}

			explicit UTF8String(std::uint16_t size, const char* val)
				: m_size(size)
			{
				if (m_size > 0)
				{
					m_bytes = new std::uint8_t[size];
					std::memcpy(m_bytes, val, m_size);
				}
			}

			explicit UTF8String(const ByteBuffer& buffer)
			{
				decode(buffer);
			}

			explicit UTF8String(const std::string& val)
			{
				if (val.size() > 65535)
				{
					LogException("UTF8String", std::runtime_error("Construction from string failed. Cannot exceed size of 65535 (uint16_t)."));
				}

				m_size = static_cast<std::uint16_t>(val.size());

				if (m_size > 0)
				{
					m_bytes = new std::uint8_t[val.size()];
					std::memcpy(m_bytes, val.c_str(), m_size);
				}
			}

			UTF8String(const UTF8String& other)
				:m_size(other.m_size)
			{
				if (m_size > 0)
				{
					m_bytes = new std::uint8_t[other.m_size];
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
				if (this == &other)
				{
					return *this;
				}

				delete[] m_bytes;

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
					LogException("UTF8String", std::runtime_error("Construction from string failed. Cannot exceed size of 65535 (uint16_t)."));
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

			void encode(ByteBuffer& buffer) const override
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

			explicit UTF8StringPair(const ByteBuffer& buffer)
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

			void encode(ByteBuffer& buffer) const override
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

#endif //INCLUDE_CLEANMQTT_MQTT_PACKETS_DATATYPES_H