#ifndef INCLUDE_CLEANMQTT_UTILS_TEMPLATEUTILS_HEADER
#define INCLUDE_CLEANMQTT_UTILS_TEMPLATEUTILS_HEADER

#include <cstdint>

namespace cleanMqtt
{
	namespace templateUtils
	{
		template<std::uint32_t TFlag, std::uint32_t TOtherFlags>
		constexpr bool hasFlag()
		{
			return (TFlag & TOtherFlags) != 0;
		}
	}
}

#endif //INCLUDE_CLEANMQTT_UTILS_TEMPLATEUTILS_HEADER 