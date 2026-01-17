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
