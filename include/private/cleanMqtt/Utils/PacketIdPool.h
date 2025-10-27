#ifndef INCLUDE_CLEANMQTT_UTILS_PACKETIDPOOL_HEADER
#define INCLUDE_CLEANMQTT_UTILS_PACKETIDPOOL_HEADER

#include <cleanMqtt/GlobalMacros.h>

#include <cstdint>
#include <stack>
#include <bitset>
#include <mutex>

namespace cleanMqtt
{
	/**
	 * @brief A pool for MQTT packet IDs (16-bit unsigned integers).
	 * Thread safe.
	 */
	struct PacketIdPool
	{
		/**
		 * @brief Get a new packet ID.
		 * Packet IDs are in the range [1, 65535].
		 * Reuses Ids that have been released.
		 * 
		 * @return A new packet ID.
		 */
		std::uint16_t getId() noexcept
		{
			LockGuard guard{ m_mutex };

			if (!m_availableIds.empty())
			{
				std::uint16_t nextId{ m_availableIds.top() };
				m_availableIds.pop();
				m_usedIds.flip(nextId);
				return nextId;
			}

			m_usedIds.flip(m_nextId);
			return m_nextId++;
		}

		/**
		 * @brief Release a packet ID back to the pool.
		 * 
		 * @param id The packet ID to release.
		 */
		void releaseId(std::uint16_t id) noexcept
		{
			if (id == 0U) return;

			LockGuard guard{ m_mutex };

			if (!m_usedIds.test(id)) return;

			m_availableIds.push(id);
			m_usedIds.reset(id);
		}

	private:
		std::uint16_t m_nextId{ 1U };//0 is not a valid MQTT packet ID.
		std::stack<std::uint16_t> m_availableIds{};
		std::bitset<65535> m_usedIds;
		std::mutex m_mutex;
	};
}

#endif
