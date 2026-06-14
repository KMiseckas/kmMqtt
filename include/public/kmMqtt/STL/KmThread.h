// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_STL_THREAD_H
#define INCLUDE_KMMQTT_STL_THREAD_H

/**
 * @file KmThread.h
 * @brief This file provides a wrapper around the C++ standard library's threading facilities, only for parts
 * that are used in the kmMqtt project.
 * 
 * To use a custom thread implementation, define the CUSTOM_THREAD_INCLUDE macro with the path
 * to the custom header file (Through CMake/Compiler flags/Preprocessor definitions).
 * 
 * The custom header file should provide the same interface as the standard library's threading facilities
 * used in this file.
 */
#if !defined(CUSTOM_THREAD_INCLUDE)

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <utility>

namespace kmMqtt
{
    namespace kmStd
    {
        using condition_variable = ::std::condition_variable;
        using mutex = ::std::mutex;
        using thread = ::std::thread;

        template <typename T>
        using atomic = ::std::atomic<T>;

        template <typename Mutex>
        using lock_guard = ::std::lock_guard<Mutex>;

        template <typename Mutex>
        using unique_lock = ::std::unique_lock<Mutex>;

        namespace this_thread
        {
            template<typename Duration>
            void sleep_for(const Duration& duration)
            {
                ::std::this_thread::sleep_for(duration);
            }
        }

        template<typename T, typename... Args>
        kmStd::thread make_thread(T&& func, Args&&... args)
        {
            return kmStd::thread(::std::forward<T>(func), ::std::forward<Args>(args)...);
        }
    }
}
#else
#include CUSTOM_THREAD_INCLUDE
#endif // !defined(CUSTOM_THREAD_INCLUDE)
#endif // INCLUDE_KMMQTT_STL_THREAD_H
