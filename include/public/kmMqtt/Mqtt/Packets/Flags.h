// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INTERFACE_KMMQTT_MQTT_PACKETS_FLAGS_H
#define INTERFACE_KMMQTT_MQTT_PACKETS_FLAGS_H

#include <kmMqtt/Mqtt/Packets/PacketUtils.h>
#include <kmMqtt/Logger/Log.h>

#include <type_traits>
#include <cstdint>
#include <cassert>
#include <stdexcept>

namespace kmMqtt
{
	namespace mqtt
	{
		/**
		 * @brief A utility class to manage flags within MQTT packets.
		 * This class allows for setting, getting, and overriding flag values.
		 * It uses a bitmask approach to store multiple flags in a single underlying type. Can use enum to define the flag bits.
		 * 
		 * @param TSizeType The underlying type used to store the flags (e.g., uint8_t, uint16_t).
		 * @param TFlagBitsEnum An enumeration type representing the individual flag bits.
		 * @param MaxFlagSize The maximum number of flags that can be represented.
		 */
		template<typename TSizeType, typename TFlagBitsEnum, std::size_t MaxFlagSize>
		struct Flags
		{
			Flags() noexcept = default;

			/**
			 * @brief Constructs a Flags object with the given initial flag values.
			 * @param flags The initial flag values as a bitmask.
			 */
			Flags(TSizeType flags) noexcept : m_flags(flags) {}

			/**
			 * @brief Retrieves the current flag values as a bitmask.
			 * 
			 * @return The current flag values.
			 */
			TSizeType getFlags() const
			{
				return m_flags;
			}

			/**
			 * @brief Retrieves the value of a specific flag. Can retrieve values larger than 1 for multi-bit flags aslong
			 * as the flag is defined as contiguous bits. For example, if the flag is defined as 0b00000110, this method can return values from 0 to 3 
			 * for the two bits that are positive (2 bits = 4 values: 00, 01, 10, 11) if the flag to check is passed in as such.
			 * 
			 * @tparam EnumT The enumeration type representing the flag bits.
			 * @tparam TReturnType The return type for the flag value.
			 * @param flagToCheck The specific flag to retrieve within the bitmask.
			 * @return The value of the specified flag.
			 */
			template<typename EnumT = TFlagBitsEnum, typename TReturnType = TSizeType>
			TReturnType getFlagValue(const EnumT& flagToCheck) const
			{
				const TSizeType bitmask = static_cast<TSizeType>(flagToCheck);
				assert(arePositiveBitsContiguous<TSizeType>(bitmask));

				return static_cast<TReturnType>((m_flags & bitmask) >> getFirstBitPosition<TSizeType>(bitmask));
			}

			/**
			 * @brief Sets the value of a specific flag. Can pass in values larger than 1 for multi-bit flags aslong
			 * as the flag is defined as contiguous bits. For example, if the flag is defined as 0b00000110, this method can accept values from 0 to 3
			 * for the two bits that are positive (2 bits = 4 values: 00, 01, 10, 11) if the flag to set is passed in as such.
			 * 
			 * @tparam TSizeType The underlying type used to store the flags.
			 * @param flagToSet The specific flag to set within the bitmask.
			 * @param val The value to set for the specified flag.
			 */
			void setFlagValue(const TFlagBitsEnum& flagToSet, TSizeType val)
			{
				if (static_cast<std::size_t>(flagToSet) > MaxFlagSize)
				{
					LogException("Flags", std::runtime_error("Cannot set flag that exceeds MaxFlagSize."));
				}

				const TSizeType bitmask = static_cast<TSizeType>(flagToSet);
				assert(arePositiveBitsContiguous<TSizeType>(bitmask));

				m_flags &= ~bitmask;
				m_flags |= (val << getFirstBitPosition<TSizeType>(bitmask)) & bitmask;
			}

			/**
			 * @brief Sets or clears multiple flags based on a boolean value. For example, 
			 * if the flagToSet is 0b00000110 and val is true, both bits will be set to 1 (0b00000110).
			 * The rest of the bits will remain unchanged. If val is false, both bits will be cleared to 0 (0b00000000).
			 * 
			 * @param flagToSet The specific flags to set or clear within the bitmask.
			 * @param val If true, sets the specified flags; if false, clears them.
			 */
			void setMultipleFlagsValue(const TFlagBitsEnum& flagToSet, bool val)
			{
				if (static_cast<std::size_t>(flagToSet) > MaxFlagSize)
				{
					LogException("Flags", std::runtime_error("Cannot set flag that exceeds MaxFlagSize."));
				}

				const TSizeType bitmask = static_cast<TSizeType>(flagToSet);

				m_flags = val ? m_flags | bitmask : m_flags & ~bitmask;
			}

			void overrideFlags(TSizeType bitmask) noexcept
			{
				m_flags = bitmask;
			}

		private:

			TSizeType m_flags{ 0 };
		};
	}
}

#endif //INTERFACE_KMMQTT_MQTT_PACKETS_FLAGS_H 