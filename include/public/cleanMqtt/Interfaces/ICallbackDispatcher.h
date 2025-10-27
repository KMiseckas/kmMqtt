#ifndef INCLUDE_CLEANMQTT_INTERFACES_ICALLBACKDISPATCHER_H
#define INCLUDE_CLEANMQTT_INTERFACES_ICALLBACKDISPATCHER_H

#include "cleanMqtt/Utils/UniqueFunction.h"
#include "cleanMqtt/GlobalMacros.h"
#include <functional>

namespace cleanMqtt
{
	class PUBLIC_API ICallbackDispatcher
	{
	public:
		ICallbackDispatcher() noexcept = default;
		virtual ~ICallbackDispatcher() noexcept = default;

		virtual void dispatch(UniqueFunction callback) noexcept = 0;
	};
}

#endif // INCLUDE_CLEANMQTT_INTERFACES_ICALLBACKDISPATCHER_H
