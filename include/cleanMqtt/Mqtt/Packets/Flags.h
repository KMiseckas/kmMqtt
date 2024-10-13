#ifndef INTERFACE_CLEANMQTT_MQTT_PACKETS_FLAGS_H
#define INTERFACE_CLEANMQTT_MQTT_PACKETS_FLAGS_H

#include <cleanMqtt/Mqtt/Packets/PacketUtils.h>
#include <cleanMqtt/Interfaces/ILogger.h>

#include <type_traits>
#include <cstdint>
#include <cassert>
#include <stdexcept>

namespace cleanMqtt
{
	namespace mqtt
	{
		namespace packets
		{
			template<typename TSizeType, typename TFlagBitsEnum, std::size_t MaxFlagSize>
			struct Flags
			{
				Flags() noexcept = default;
				Flags(TSizeType flags) noexcept : m_flags(flags) {}

				TSizeType getFlags() const
				{
					return m_flags;
				}

				template<typename TReturnType = TSizeType>
				TReturnType getFlagValue(const TFlagBitsEnum& flagToCheck) const
				{
					const TSizeType bitmask = static_cast<TSizeType>(flagToCheck);
					assert(arePositiveBitsContiguous<TSizeType>(bitmask));

					return static_cast<TReturnType>((m_flags & bitmask) >> getFirstBitPosition<TSizeType>(bitmask));
				}

				void setFlagValue(const TFlagBitsEnum& flagToSet, TSizeType val)
				{
					if (static_cast<std::size_t>(flagToSet) > MaxFlagSize)
					{
						Exception(LogLevel::Error, "Flags", std::runtime_error("Cannot set flag that exceeds MaxFlagSize."));
					}

					const TSizeType bitmask = static_cast<TSizeType>(flagToSet);
					assert(arePositiveBitsContiguous<TSizeType>(bitmask));

					m_flags &= ~bitmask;
					m_flags |= (val << getFirstBitPosition<TSizeType>(bitmask)) & bitmask;
				}

				void setMultipleFlagsValue(const TFlagBitsEnum& flagToSet, bool val)
				{
					if (static_cast<std::size_t>(flagToSet) > MaxFlagSize)
					{
						Exception(LogLevel::Error, "Flags", std::runtime_error("Cannot set flag that exceeds MaxFlagSize."));
					}

					const TSizeType bitmask = static_cast<TSizeType>(flagToSet);

					m_flags = val ? m_flags | bitmask : m_flags & ~bitmask;
				}

				void overrideFlags(TSizeType bitmask)
				{
					m_flags = bitmask;
				}

			private:

				TSizeType m_flags{ 0 };
			};
		}
	}
}

#endif //INTERFACE_CLEANMQTT_MQTT_PACKETS_FLAGS_H 