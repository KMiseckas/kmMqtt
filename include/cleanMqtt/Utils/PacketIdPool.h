#ifndef INCLUDE_CLEANMQTT_UTILS_PACKETIDPOOL_HEADER
#define INCLUDE_CLEANMQTT_UTILS_PACKETIDPOOL_HEADER

#include <cstdint>
#include <stack>
#include <bitset>

namespace cleanMqtt
{
	//Not thread safe by design as currently not needed to be thread safe due to how its used internally.
	struct PacketIdPool
	{
		std::uint16_t getId() noexcept
		{
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

		void releaseId(std::uint16_t id) noexcept
		{
			if (id == 0U) return;
			if (!m_usedIds.test(id)) return;

			m_availableIds.push(id);
			m_usedIds.reset(id);
		}

	private:
		std::uint16_t m_nextId{ 1U };//0 is not a valid MQTT packet ID.
		std::stack<std::uint16_t> m_availableIds{};
		std::bitset<65535> m_usedIds;
	};
}

#endif
