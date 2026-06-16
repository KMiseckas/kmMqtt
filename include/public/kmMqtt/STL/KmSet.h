// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_STL_SET_H
#define INCLUDE_KMMQTT_STL_SET_H

#include "kmMqtt/Memory/StdAllocator.h"

#include <functional>
#include <set>

namespace kmMqtt
{
	namespace kmStd
	{
		template <
			typename Key,
			typename Compare = ::std::less<Key>,
			typename Allocator = StdAllocator<Key>>
		using set = ::std::set<Key, Compare, Allocator>;

		template <
			typename Key,
			typename Compare = ::std::less<Key>,
			typename Allocator = StdAllocator<Key>>
		using multiset = ::std::multiset<Key, Compare, Allocator>;
	}
}

#endif // INCLUDE_KMMQTT_STL_SET_H
