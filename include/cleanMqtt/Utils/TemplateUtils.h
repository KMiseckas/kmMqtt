#ifndef INCLUDE_CLEANMQTT_UTILS_TEMPLATEUTILS_HEADER
#define INCLUDE_CLEANMQTT_UTILS_TEMPLATEUTILS_HEADER

#include <cstdint>

namespace cleanMqtt
{
    namespace templateUtils
    {
        /**
        * @brief Checks if the specified flag is present in the given set of flags.
        *
        * @tparam TFlag The flag to check for (as a compile-time constant).
        * @tparam TOtherFlags The set of flags to check against (as a compile-time constant).
        * @return true if TFlag is set in TOtherFlags, false otherwise.
        */
        template<std::uint32_t TFlag, std::uint32_t TOtherFlags>
        constexpr bool hasFlag()
        {
            return (TFlag & TOtherFlags) != 0;
        }
    }
}

#endif //INCLUDE_CLEANMQTT_UTILS_TEMPLATEUTILS_HEADER 