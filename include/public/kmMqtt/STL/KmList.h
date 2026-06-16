// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_STL_LIST_H
#define INCLUDE_KMMQTT_STL_LIST_H

#include "kmMqtt/Memory/StdAllocator.h"

#include <list>

namespace kmMqtt
{
	namespace kmStd
	{
		template <typename T, typename Allocator = StdAllocator<T>>
		using list = ::std::list<T, Allocator>;
	}
}

#endif // INCLUDE_KMMQTT_STL_LIST_H
