// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_UTILS_TEMPLATEUTILS_HEADER
#define INCLUDE_KMMQTT_UTILS_TEMPLATEUTILS_HEADER

#include <type_traits>
#include <utility>
#include <cstdint>

namespace kmMqtt
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

        /**
		 * @brief Checks if a function or callable object can be invoked with the specified arguments without throwing exceptions.
		 * 
		 * @tparam Func The type of the function or callable object.
		 * @tparam FArgs The types of the arguments to be passed to the function.
		 * @return true if the function can be invoked with the specified arguments without throwing exceptions, false otherwise.
		 * 
		 * Replacement for C++17's std::is_nothrow_invocable from <type_traits>.
         */
        template<typename Func, typename ...FArgs>
        struct is_NoThrow_Invocable
        {
        private:
            template <typename F, typename ...A>
            static auto test(int) -> decltype(static_cast<void>(std::declval<F>()(std::declval<A>()...)),
                std::true_type{});

            template <typename, typename ...>
            static std::false_type test(...);

        public:
			static constexpr bool value = noexcept(test<Func, FArgs...>(0));
        };
    }
}

#endif //INCLUDE_KMMQTT_UTILS_TEMPLATEUTILS_HEADER 