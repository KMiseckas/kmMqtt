// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_UTILS_PACKETIDPOOL_HEADER
#define INCLUDE_KMMQTT_UTILS_PACKETIDPOOL_HEADER

#include "kmMqtt/GlobalMacros.h"

#include <cstdint>
#include <stack>
#include <bitset>
#include <mutex>

//Macro (instead of const) to allow changing in build tools. If QOS > 0 is rarely or never used, can reduce memory footprint by lowering this value.
#if !defined(PACKET_POOL_ID_SIZE) || (PACKET_POOL_ID_SIZE > 65535U)
#define PACKET_POOL_ID_SIZE 65535U
#endif

namespace kmMqtt
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
		std::stack<std::uint16_t> m_availableIds{}; //Pool of available Ids for reuse.
		std::bitset<PACKET_POOL_ID_SIZE> m_usedIds; //Tracks used Ids to prevent duplicates.
		std::mutex m_mutex;
	};
}

#endif
