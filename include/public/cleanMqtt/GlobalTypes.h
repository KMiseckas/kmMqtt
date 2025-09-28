#ifndef INCLUDE_CLEANMQTT_GLOBALTYPES_H
#define INCLUDE_CLEANMQTT_GLOBALTYPES_H

#include <chrono>

namespace cleanMqtt
{
	using TimePoint = std::chrono::steady_clock::time_point;
	using Seconds = std::chrono::seconds;
	using Milliseconds = std::chrono::milliseconds;
}

#endif //INCLUDE_CLEANMQTT_GLOBALTYPES_H
