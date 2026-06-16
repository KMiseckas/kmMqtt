// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_STL_UNORDEREDSET_H
#define INCLUDE_KMMQTT_STL_UNORDEREDSET_H

#include "kmMqtt/Memory/StdAllocator.h"

#include <functional>
#include <unordered_set>

namespace kmMqtt
{
	namespace kmStd
	{
		template <
			typename Key,
			typename Hash = ::std::hash<Key>,
			typename KeyEqual = ::std::equal_to<Key>,
			typename Allocator = StdAllocator<Key>>
		using unordered_set = ::std::unordered_set<Key, Hash, KeyEqual, Allocator>;

		template <
			typename Key,
			typename Hash = ::std::hash<Key>,
			typename KeyEqual = ::std::equal_to<Key>,
			typename Allocator = StdAllocator<Key>>
		using unordered_multiset = ::std::unordered_multiset<Key, Hash, KeyEqual, Allocator>;
	}
}

#endif // INCLUDE_KMMQTT_STL_UNORDEREDSET_H
