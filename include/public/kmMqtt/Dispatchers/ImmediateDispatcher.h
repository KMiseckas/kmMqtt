// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_DISPATCHERS_IMMEDIATEDISPATCHER_H
#define INCLUDE_KMMQTT_DISPATCHERS_IMMEDIATEDISPATCHER_H

#include "kmMqtt/GlobalMacros.h"
#include "kmMqtt/Interfaces/ICallbackDispatcher.h"

namespace kmMqtt
{
	class PUBLIC_API ImmediateDispatcher : public ICallbackDispatcher
	{
	public:
		void dispatch(UniqueFunction callback) noexcept override;
	};
}

#endif //INCLUDE_KMMQTT_DISPATCHERS_IMMEDIATEDISPATCHER_H
