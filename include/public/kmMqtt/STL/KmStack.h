// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_STL_STACK_H
#define INCLUDE_KMMQTT_STL_STACK_H

#include "kmMqtt/STL/KmDeque.h"

#include <stack>

namespace kmMqtt
{
	namespace kmStd
	{
		template <typename T, typename Container = kmStd::deque<T>>
		using stack = ::std::stack<T, Container>;
	}
}

#endif // INCLUDE_KMMQTT_STL_STACK_H
