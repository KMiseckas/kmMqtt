#include "kmMqtt/Dispatchers/ImmediateDispatcher.h"

namespace kmMqtt
{
	void ImmediateDispatcher::dispatch(UniqueFunction callback) noexcept
	{
		callback(); //Invoke callback immediately from the thread mqtt is ticking on.
	}
}
