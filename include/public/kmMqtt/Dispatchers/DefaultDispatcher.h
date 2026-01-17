#ifndef INCLUDE_KMMQTT_DISPATCHERS_DEFAULTDISPATCHER_H
#define INCLUDE_KMMQTT_DISPATCHERS_DEFAULTDISPATCHER_H

#include "kmMqtt/GlobalMacros.h"
#include "kmMqtt/Interfaces/ICallbackDispatcher.h"

namespace kmMqtt
{
	class PUBLIC_API DefaultDispatcher : public ICallbackDispatcher
	{
	public:
		void dispatch(UniqueFunction callback) noexcept override;
	};
}

#endif //INCLUDE_KMMQTT_DISPATCHERS_DEFAULTDISPATCHER_H
