#ifndef INCLUDE_CLEANMQTT_DISPATCHERS_IMMEDIATEDISPATCHER_H
#define INCLUDE_CLEANMQTT_DISPATCHERS_IMMEDIATEDISPATCHER_H

#include "cleanMqtt/GlobalMacros.h"
#include "cleanMqtt/Interfaces/ICallbackDispatcher.h"

namespace cleanMqtt
{
	class PUBLIC_API ImmediateDispatcher : public ICallbackDispatcher
	{
		void dispatch(UniqueFunction callback) noexcept override;
	};
}

#endif //INCLUDE_CLEANMQTT_DISPATCHERS_IMMEDIATEDISPATCHER_H
