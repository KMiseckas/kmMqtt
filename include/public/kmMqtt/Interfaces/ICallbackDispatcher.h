// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

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
