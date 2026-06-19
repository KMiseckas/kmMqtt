// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_STL_ARRAY_H
#define INCLUDE_KMMQTT_STL_ARRAY_H

#include <array>
#include <cstddef>

namespace kmMqtt
{
	namespace kmStd
	{
		template <typename T, ::std::size_t Size>
		using array = ::std::array<T, Size>;
	}
}

#endif // INCLUDE_KMMQTT_STL_ARRAY_H
