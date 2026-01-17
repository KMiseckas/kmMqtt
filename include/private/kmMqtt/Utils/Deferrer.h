// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_UTILS_DEFERRER_H
#define INCLUDE_KMMQTT_UTILS_DEFERRER_H

#include "kmMqtt/Utils/Event.h"
#include "kmMqtt/GlobalMacros.h"
#include <queue>
#include <mutex>
#include <type_traits>
#include <memory>

namespace kmMqtt
{

#define deferEvent(deferrer, lambda)\
	deferrer.defer(lambda)\

#define deferPacketEvent_wMove(deferrer, event, packet, ...)\
	deferrer.defer([&, p = std::move(packet)](){event(__VA_ARGS__, p);})\

#define deferEvent_wCaptures(deferrer, event, captures, ...)\
	deferrer.defer([captures](){event(__VA_ARGS__);})\

	namespace events
	{
		struct ICallable
		{
			virtual ~ICallable() {}
			virtual void call() noexcept = 0;
		};

		template<typename TFunc>
		struct Callable : ICallable
		{
			explicit Callable(TFunc&& f)
				: func(std::move(f))
			{
			}

			void call() noexcept override
			{
				func();
			}

			TFunc func;
		};

		class Deferrer
		{
		public:
			template<typename TFunc>
			void defer(TFunc&& event)
			{
				m_events.emplace(std::make_unique<Callable<TFunc>>(std::forward<TFunc>(event)));
			}

			void invokeEvents() noexcept;
			void clear() noexcept;

			void operator()() noexcept
			{
				invokeEvents();
			}

		private:
			std::queue<std::unique_ptr<ICallable>> m_events;
		};
	}
}

#endif //INCLUDE_KMMQTT_UTILS_DEFERRER_H 
