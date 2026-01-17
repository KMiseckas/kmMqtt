// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_GLOBALTYPES_H
#define INCLUDE_KMMQTT_GLOBALTYPES_H

#include <chrono>

namespace kmMqtt
{
	using TimePoint = std::chrono::steady_clock::time_point;
	using Seconds = std::chrono::seconds;
	using Milliseconds = std::chrono::milliseconds;
}

#endif //INCLUDE_KMMQTT_GLOBALTYPES_H
