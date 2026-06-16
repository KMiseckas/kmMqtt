// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_GLOBALTYPES_H
#define INCLUDE_KMMQTT_GLOBALTYPES_H

#include <kmMqtt/STL/KmChrono.h>

namespace kmMqtt
{
	using TimePoint = kmStd::chrono::steady_clock::time_point;
	using Seconds = kmStd::chrono::seconds;
	using Milliseconds = kmStd::chrono::milliseconds;
}

#endif //INCLUDE_KMMQTT_GLOBALTYPES_H
