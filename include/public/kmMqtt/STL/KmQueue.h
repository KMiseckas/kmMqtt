// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_STL_QUEUE_H
#define INCLUDE_KMMQTT_STL_QUEUE_H

#include "kmMqtt/STL/KmDeque.h"
#include "kmMqtt/STL/KmVector.h"

#include <functional>
#include <queue>

namespace kmMqtt
{
	namespace kmStd
	{
		template <typename T, typename Container = kmStd::deque<T>>
		using queue = ::std::queue<T, Container>;

		template <
			typename T,
			typename Container = kmStd::vector<T>,
			typename Compare = ::std::less<typename Container::value_type>>
		using priority_queue = ::std::priority_queue<T, Container, Compare>;
	}
}

#endif // INCLUDE_KMMQTT_STL_QUEUE_H
