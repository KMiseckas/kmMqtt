// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_STL_MAP_H
#define INCLUDE_KMMQTT_STL_MAP_H

#include "kmMqtt/Memory/StdAllocator.h"

#include <functional>
#include <map>

namespace kmMqtt
{
	namespace kmStd
	{
		template <
			typename Key,
			typename T,
			typename Compare = ::std::less<Key>,
			typename Allocator = StdAllocator<::std::pair<const Key, T>>>
		using map = ::std::map<Key, T, Compare, Allocator>;

		template <
			typename Key,
			typename T,
			typename Compare = ::std::less<Key>,
			typename Allocator = StdAllocator<::std::pair<const Key, T>>>
		using multimap = ::std::multimap<Key, T, Compare, Allocator>;
	}
}

#endif // INCLUDE_KMMQTT_STL_MAP_H
