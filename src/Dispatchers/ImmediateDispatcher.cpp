#include "cleanMqtt/Dispatchers/ImmediateDispatcher.h"

namespace cleanMqtt
{
	void ImmediateDispatcher::dispatch(UniqueFunction callback) noexcept
	{
		callback(); //Invoke callback immediately from the thread mqtt is ticking on.
	}
}
