#ifndef INCLUDE_CLEANMQTT_BYTEBUFFER_H
#define INCLUDE_CLEANMQTT_BYTEBUFFER_H

#include <cleanMqtt/GlobalMacros.h>
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
			throw std::out_of_range("The required size for encoding the bytes exceeds the headroom of buffer.");\
		}\

#define BYTEBUFFER_READ_CHECK(readCursor) \
		if (size() <= readCursor)\
		{\
			throw std::out_of_range("The buffer read is outside the bounds of buffer size.");\
		}\

	}
	

    /**
    * @brief A fixed-capacity byte buffer with optional small buffer optimization (SBO).
    *
    * ByteBuffer provides a contiguous memory area for storing bytes, with a capacity
    * defined at construction time. The buffer is not resizable after construction.
    * If SBO is enabled and the requested capacity is small enough, the buffer uses
    * stack memory; otherwise, it allocates on the heap.
    *
    * Provides methods for appending and reading bytes and multi-byte values,
    * as well as cursor management for sequential reading.
    *
    * The buffer's capacity is fixed after construction and cannot be changed.
    * 
    * SBO (Small Buffer Optimization) is enabled if ENABLE_BYTEBUFFER_SBO is defined.
    */
	struct PUBLIC_API ByteBuffer
	{

#ifdef ENABLE_BYTEBUFFER_SBO
#if BYTEBUFFER_SBO_MAX_SIZE < 8U
#define BYTEBUFFER_SBO_MAX_SIZE 64U
#endif
#else
#define BYTEBUFFER_SBO_MAX_SIZE 0U
#endif

	public:

		/**
		 * @brief Constructs a ByteBuffer with default capacity using SBO if enabled.
		 */
		ByteBuffer() noexcept : m_capacity(BYTEBUFFER_SBO_MAX_SIZE), m_size(0U)
		{
		}

		/**
		 * @brief Constructs a ByteBuffer with the specified capacity.
		 * @param capacity The desired capacity of the buffer in bytes.
		 * Uses SBO if enabled and capacity is small enough.
		 */
		ByteBuffer(std::size_t capacity) noexcept
			: m_capacity(capacity), m_size(0U)
		{
			if (capacity > BYTEBUFFER_SBO_MAX_SIZE)
			{
				m_bytes = new std::uint8_t[capacity];
			}
		}

		~ByteBuffer()
		{
#ifdef ENABLE_BYTEBUFFER_SBO
			if (m_bytes != m_sboBytes)
			{
				delete[] m_bytes;
			}
#else
			delete[] m_bytes;
#endif
		}

		ByteBuffer(const ByteBuffer& other) noexcept
			: m_size(other.m_size), m_capacity(other.m_capacity)
		{
			if(other.m_capacity > BYTEBUFFER_SBO_MAX_SIZE)
			{
				m_bytes = new std::uint8_t[other.m_capacity];
			}

			std::memcpy(m_bytes, other.m_bytes, other.m_size);
		}

		ByteBuffer(ByteBuffer&& other) noexcept
			: m_size(other.m_size), m_capacity(other.m_capacity)
		{
#ifdef ENABLE_BYTEBUFFER_SBO
			if(other.m_bytes == other.m_sboBytes)
			{
				std::memcpy(m_sboBytes, other.m_sboBytes, other.m_size);
				m_bytes = m_sboBytes;
			}
			else
			{
				m_bytes = other.m_bytes;
			}
#else
			m_bytes = other.m_bytes;
#endif
			other.m_bytes = nullptr;
			other.m_capacity = 0U;
			other.m_size = 0U;
		}

		ByteBuffer& operator=(const ByteBuffer& other) noexcept
		{
			if (this == &other)
			{
				return *this;
			}
			delete[] m_bytes;
			m_size = other.m_size;
			m_capacity = other.m_capacity;
#ifdef ENABLE_BYTEBUFFER_SBO
			if(other.m_capacity > BYTEBUFFER_SBO_MAX_SIZE)
			{
				m_bytes = new std::uint8_t[other.m_capacity];
			}
			else
			{
				m_bytes = m_sboBytes;
			}
#else
			m_bytes = new std::uint8_t[other.m_capacity];
#endif
			std::memcpy(m_bytes, other.m_bytes, other.m_size);
			return *this;
		}

		ByteBuffer& operator=(ByteBuffer&& other) noexcept
		{
			if (this == &other)
			{
				return *this;
			}

			m_size = other.m_size;
			m_capacity = other.m_capacity;
			
#ifdef ENABLE_BYTEBUFFER_SBO
			if (other.m_bytes == other.m_sboBytes)
			{
				std::memcpy(m_sboBytes, other.m_sboBytes, other.m_size);
				m_bytes = m_sboBytes;
			}
			else
			{
				m_bytes = other.m_bytes;
			}
#else
			m_bytes = other.m_bytes;
#endif

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

		/**
		 * @brief Reads a single byte from the buffer at the current read cursor position and advances the cursor.
		 */
		const std::uint8_t readUint8() const
		{
			BYTEBUFFER_READ_CHECK(m_readCursor)

			return m_bytes[m_readCursor++];
		}

		/**
		 * @brief Reads a 16-bit unsigned integer from the buffer at the current read cursor position and advances the cursor by 2 bytes.
		 */
		const std::uint16_t readUInt16() const
		{
			BYTEBUFFER_READ_CHECK(m_readCursor + 1)

            std::uint16_t val = (static_cast<std::uint16_t>(m_bytes[m_readCursor]) << 8) | static_cast<std::uint16_t>(m_bytes[m_readCursor + 1]);
            m_readCursor += 2;
            return val;
		}

		/**
		 * @brief Reads a 32-bit unsigned integer from the buffer at the current read cursor position and advances the cursor by 4 bytes.
		 */
		const std::uint32_t readUInt32() const
		{
			BYTEBUFFER_READ_CHECK(m_readCursor + 3)

			std::uint32_t b0{m_bytes[m_readCursor++]};
			std::uint32_t b1{m_bytes[m_readCursor++]};
			std::uint32_t b2{m_bytes[m_readCursor++]};
			std::uint32_t b3{m_bytes[m_readCursor++]};
			return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
		}

		/**
		 * @brief Increments the read cursor by the specified value.
		 */
		void incrementReadCursor(std::size_t shiftVal) const noexcept
		{
			m_readCursor += shiftVal;
		}

		/**
		 * @brief Resets the read cursor to the beginning of the buffer.
		 */
		void resetReadCursor() const noexcept
		{
			m_readCursor = 0U;
		}

		/**
		 * @brief Appends a 2 bytes to the end of the buffer.
		 * @param bytes The 2 bytes to append.
		 * @return Reference to the ByteBuffer for chaining.
		 */
		ByteBuffer& append(std::uint16_t bytes)
		{
			BYTEBUFFER_HEADROOM_CHECK(2)

			m_bytes[m_size++] = (bytes >> 8) & 0xffu;
			m_bytes[m_size++] = bytes & 0xffu;

			return *this;
		}

		/**
		 * @brief Appends 4 bytes to the end of the buffer.
		 * 
		 * @param bytes The 4 bytes to append.
		 * @return Reference to the ByteBuffer for chaining.
		 */
		ByteBuffer& append(std::uint32_t bytes)
		{
			BYTEBUFFER_HEADROOM_CHECK(4)

			m_bytes[m_size++] = (bytes >> 24) & 0xffu;
			m_bytes[m_size++] = (bytes >> 16) & 0xffu;
			m_bytes[m_size++] = (bytes >> 8) & 0xffu;
			m_bytes[m_size++] = bytes & 0xffu;

			return *this;
		}

		/**
		 * @brief Appends a sequence of bytes to the end of the buffer.
		 * 
		 * @param bytes Pointer to the bytes to append.
		 * @param size Number of bytes to append.
		 * @return Reference to the ByteBuffer for chaining.
		 */
		ByteBuffer& append(const std::uint8_t* bytes, std::size_t size)
		{
			if (size == 0)
			{
				throw std::runtime_error("Tried to append bytes with size 0.");
				return *this;
			}

			if (bytes == nullptr)
			{
				throw std::runtime_error("Tried to append nullptr bytes with size above 0.");
				return *this;
			}

			BYTEBUFFER_HEADROOM_CHECK(size)

			std::memcpy(m_bytes + m_size, bytes, size);
			m_size += size;

			return *this;
		}

		/**
		 * @brief Appends the contents of another ByteBuffer to this buffer.
		 * Make sure there is enough headroom in target buffer before calling this method.
		 * 
		 * @param byteBuffer The ByteBuffer whose contents to append.
		 * @return Reference to the ByteBuffer for chaining.
		 */
		ByteBuffer& append(const ByteBuffer& byteBuffer)
		{
			return append(byteBuffer.bytes(), byteBuffer.size());
		}

		/**
		 * @brief Get the size of the buffer (number of bytes currently stored).
		 */
		std::size_t size() const noexcept
		{
			return m_size;
		}

		/**
		 * @brief Get the capacity of the buffer (maximum number of bytes it can hold).
		 */
		std::size_t capacity() const noexcept
		{
			return m_capacity;
		}

		/**
		 * @brief Get the headroom of the buffer (remaining space available for writing).
		 */
		std::size_t headroom() const noexcept
		{
			return m_capacity - m_size;
		}

		/**
		 * @brief Get a pointer to the underlying byte array.
		 */
		const std::uint8_t* bytes() const noexcept
		{
			return m_bytes;
		}

		/**
		 * @brief Get the current read cursor position.
		 */
		std::size_t readCursor() const noexcept
		{
			return m_readCursor;
		}

		/**
		 * @brief Get the remaining bytes available to read from the current read cursor position.
		 */
		std::size_t readHeadroom() const noexcept
		{
			return m_size - m_readCursor;
		}

		/**
		 * @brief Converts the buffer contents to a string of bits for debugging.
		 */
		std::string toString() const noexcept
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
#ifdef ENABLE_BYTEBUFFER_SBO
		std::uint8_t m_sboBytes[BYTEBUFFER_SBO_MAX_SIZE];
		std::uint8_t* m_bytes{ &m_sboBytes[0]};
#else
		std::uint8_t* m_bytes{ nullptr };
#endif
		std::size_t m_capacity{ BYTEBUFFER_SBO_MAX_SIZE };
		std::size_t m_size{ 0U };
		mutable std::size_t m_readCursor{ 0U };
	};

}

#endif //INCLUDE_CLEANMQTT_BYTEBUFFER_H