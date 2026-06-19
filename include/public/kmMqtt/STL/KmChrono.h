// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_STL_CHRONO_H
#define INCLUDE_KMMQTT_STL_CHRONO_H

/**
 * @file KmChrono.h
 * @brief This file provides a wrapper around the C++ standard library's chrono facilities, only for parts
 * that are used in the kmMqtt project.
 *
 * To use a custom chrono implementation, define the CUSTOM_CHRONO_INCLUDE macro with the path
 * to the custom header file (Through CMake/Compiler flags/Preprocessor definitions).
 *
 * The custom header file should provide the same interface as the standard library's chrono facilities
 * used in this file.
 */
#if !defined(CUSTOM_CHRONO_INCLUDE)

#include <chrono>

namespace kmMqtt
{
    namespace kmStd
    {
        namespace chrono = ::std::chrono;
    }
}
#else
#include CUSTOM_CHRONO_INCLUDE
#endif // !defined(CUSTOM_CHRONO_INCLUDE)
#endif // INCLUDE_KMMQTT_STL_CHRONO_H
