#ifndef INCLUDE_CLEANMQTT_UTILS_PACKETIDPOOL_HEADER
#define INCLUDE_CLEANMQTT_UTILS_PACKETIDPOOL_HEADER

#include <cstdint>
#include <stack>
#include <bitset>


namespace cleanMqtt
{
	using PacketID = std::uint16_t;

	//Not thread safe by design as currently not needed to be thread safe due to how its used internally.
	struct PacketIdPool
	{
		PacketID getId() noexcept
		{
			if (!m_availableIds.empty())
			{
				PacketID nextId{ m_availableIds.top() };
				m_availableIds.pop();
				return nextId;
			}

			return m_nextId++;
		}

		void releaseId(PacketID id) noexcept
		{
			if (id == 0U) return;
			if (!m_usedIds.test(id)) return;

			m_availableIds.push(id);
			m_usedIds.reset(id);
		}

	private:
		PacketID m_nextId{ 1U };//0 is not a valid MQTT packet ID.
		std::stack<PacketID> m_availableIds{};
		std::bitset<65535> m_usedIds;
	};
}

#endif
