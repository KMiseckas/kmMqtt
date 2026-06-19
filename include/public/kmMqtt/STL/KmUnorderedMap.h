// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_STL_UNORDEREDMAP_H
#define INCLUDE_KMMQTT_STL_UNORDEREDMAP_H

#include "kmMqtt/Memory/StdAllocator.h"

#include <functional>
#include <unordered_map>

namespace kmMqtt
{
	namespace kmStd
	{
		template <
			typename Key,
			typename T,
			typename Hash = ::std::hash<Key>,
			typename KeyEqual = ::std::equal_to<Key>,
			typename Allocator = StdAllocator<::std::pair<const Key, T>>>
		using unordered_map = ::std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

		template <
			typename Key,
			typename T,
			typename Hash = ::std::hash<Key>,
			typename KeyEqual = ::std::equal_to<Key>,
			typename Allocator = StdAllocator<::std::pair<const Key, T>>>
		using unordered_multimap = ::std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>;
	}
}

#endif // INCLUDE_KMMQTT_STL_UNORDEREDMAP_H
