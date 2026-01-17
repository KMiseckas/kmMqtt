// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#include "kmMqtt/Dispatchers/ImmediateDispatcher.h"

namespace kmMqtt
{
	void ImmediateDispatcher::dispatch(UniqueFunction callback) noexcept
	{
		callback(); //Invoke callback immediately from the thread mqtt is ticking on.
	}
}
