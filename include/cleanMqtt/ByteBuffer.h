#ifndef INCLUDE_CLEANMQTT_BYTEBUFFER_H
#define INCLUDE_CLEANMQTT_BYTEBUFFER_H

#include <cleanMqtt/GlobalMacros.h>
#include <cleanMqtt/Logger/Log.h>
#include <cstdint>
#include <stdexcept>
#include <exception>
#include <bitset>

namespace cleanMqtt
{
	namespace
	{
#define BYTEBUFFER_HEADROOM_CHECK(spaceRequired) \
		if (headroom() < spaceRequired)\
		{\
			LogException("ByteBuffer", std::out_of_range("The required size for encoding the bytes exceeds the headroom of buffer."));\
		}\

#define BYTEBUFFER_READ_CHECK(readCursor) \
		if (size() <= readCursor)\
		{\
			LogException("ByteBuffer", std::out_of_range("The buffer read is outside the bounds of buffer size."));\
		}\

	}

	struct PUBLIC_API ByteBuffer
	{
	public:
		DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(ByteBuffer)

		explicit ByteBuffer(std::size_t capacity) noexcept
			: m_capacity(capacity), m_size(0U), m_bytes(new std::uint8_t[capacity])
		{
		}

		~ByteBuffer()
		{
			delete[] m_bytes;
		}

		ByteBuffer(ByteBuffer&& other) noexcept
			: m_size(other.m_size), m_capacity(other.m_capacity), m_bytes(other.m_bytes)
		{
			other.m_bytes = nullptr;
			other.m_capacity = 0U;
			other.m_size = 0U;
		}

		ByteBuffer& operator=(ByteBuffer&& other) noexcept
		{
			if (this == &other)
			{
				return *this;
			}

			m_size = other.m_size;
			m_capacity = other.m_capacity;
			m_bytes = other.m_bytes;

			other.m_bytes = nullptr;
			other.m_capacity = 0U;
			other.m_size = 0U;

			return *this;
		}

		const std::uint8_t& operator[](std::size_t n) const
		{
			return m_bytes[n];
		}

		ByteBuffer& operator+=(std::uint8_t byte)
		{
			BYTEBUFFER_HEADROOM_CHECK(1)

			m_bytes[m_size++] = byte;

			return *this;
		}

		const std::uint8_t readUint8() const
		{
			BYTEBUFFER_READ_CHECK(m_readCursor)

			return m_bytes[m_readCursor++];
		}

		const std::uint16_t readUInt16() const
		{
			BYTEBUFFER_READ_CHECK(m_readCursor + 1)

			std::uint16_t val = (m_bytes[m_readCursor] << 8) | m_bytes[m_readCursor + 1];
			m_readCursor += 2;
			return val;
		}

		const std::uint32_t readUInt32() const
		{
			BYTEBUFFER_READ_CHECK(m_readCursor + 3)

			return (m_bytes[m_readCursor++] << 24) |
			(m_bytes[m_readCursor++] << 16) |
			(m_bytes[m_readCursor++] << 8) |
			m_bytes[m_readCursor++];
		}

		void incrementReadCursor(std::size_t shiftVal) const
		{
			m_readCursor += shiftVal;
		}

		void resetReadCursor() const
		{
			m_readCursor = 0U;
		}

		ByteBuffer& append(std::uint16_t bytes)
		{
			BYTEBUFFER_HEADROOM_CHECK(2)

			m_bytes[m_size++] = (bytes >> 8) & 0xffu;
			m_bytes[m_size++] = bytes & 0xffu;

			return *this;
		}

		ByteBuffer& append(std::uint32_t bytes)
		{
			BYTEBUFFER_HEADROOM_CHECK(4)

			m_bytes[m_size++] = (bytes >> 24) & 0xffu;
			m_bytes[m_size++] = (bytes >> 16) & 0xffu;
			m_bytes[m_size++] = (bytes >> 8) & 0xffu;
			m_bytes[m_size++] = bytes & 0xffu;

			return *this;
		}

		ByteBuffer& append(const std::uint8_t* bytes, std::size_t size)
		{
			if (size == 0)
			{
				LogWarning("ByteBuffer", "Tried to append bytes with size 0.");
				return *this;
			}

			if (bytes == nullptr)
			{
				LogException("ByteBuffer", std::runtime_error("Tried to append nullptr bytes with size above 0."));
				return *this;
			}

			BYTEBUFFER_HEADROOM_CHECK(size)

			std::memcpy(m_bytes + m_size, bytes, size);
			m_size += size;

			return *this;
		}

		ByteBuffer& append(const ByteBuffer& byteBuffer)
		{
			return append(byteBuffer.bytes(), byteBuffer.size());
		}

		std::size_t size() const noexcept
		{
			return m_size;
		}

		std::size_t capacity() const noexcept
		{
			return m_capacity;
		}

		std::size_t headroom() const noexcept
		{
			return m_capacity - m_size;
		}

		const std::uint8_t* bytes() const noexcept
		{
			return m_bytes;
		}

		std::size_t readCursor() const noexcept
		{
			return m_readCursor;
		}

		std::size_t readHeadroom() const noexcept
		{
			return m_size - m_readCursor;
		}

		std::string toString() const
		{
			std::string result{ "" };

			for (size_t i = 0; i < m_size; ++i)
			{
				result += std::bitset<8>(m_bytes[i]).to_string();
				result += " ";
			}

			return result;
		}

	private:
		std::uint8_t* m_bytes{ nullptr };
		std::size_t m_capacity{ 0U };
		std::size_t m_size{ 0U };
		mutable std::size_t m_readCursor{ 0U };
	};

}

#endif //INCLUDE_CLEANMQTT_BYTEBUFFER_H