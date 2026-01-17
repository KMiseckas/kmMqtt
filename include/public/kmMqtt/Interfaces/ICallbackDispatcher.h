#ifndef INCLUDE_KMMQTT_INTERFACES_ICALLBACKDISPATCHER_H
#define INCLUDE_KMMQTT_INTERFACES_ICALLBACKDISPATCHER_H

#include "kmMqtt/Utils/UniqueFunction.h"
#include "kmMqtt/GlobalMacros.h"
#include <functional>

namespace kmMqtt
{
	class PUBLIC_API ICallbackDispatcher
	{
	public:
		ICallbackDispatcher() noexcept = default;
		virtual ~ICallbackDispatcher() noexcept = default;

		virtual void dispatch(UniqueFunction callback) noexcept = 0;
	};
}

#endif // INCLUDE_KMMQTT_INTERFACES_ICALLBACKDISPATCHER_H
