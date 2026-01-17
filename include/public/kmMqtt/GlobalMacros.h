// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#ifndef INCLUDE_KMMQTT_GLOBALMACROS_H
#define INCLUDE_KMMQTT_GLOBALMACROS_H

#include <type_traits>
#include <mutex>
#include <sstream>
#include <iostream>

namespace kmMqtt
{
    using LockGuard = std::lock_guard<std::mutex>;

#define MAX_PACKET_SIZE 268435455U //The maximum size of a MQTT packet as per the MQTT specification, 256 MB
#define RECEIVE_MAXIMUM_DEFAULT 65535U

#define ENUM_FLAG_OPERATORS(enumName)\
        constexpr enumName operator | (enumName lhs, enumName rhs)\
        {\
            using T = std::underlying_type_t <enumName>;\
            return static_cast<enumName>(static_cast<T>(lhs) | static_cast<T>(rhs));\
        };\
\
        constexpr enumName& operator |= (enumName & lhs, enumName rhs)\
        {\
            lhs = lhs | rhs;\
            return lhs;\
        };\
\
        constexpr enumName operator & (enumName lhs, enumName rhs)\
        {\
            using T = std::underlying_type_t <enumName>;\
            return static_cast<enumName>(static_cast<T>(lhs) & static_cast<T>(rhs));\
        };\

#define DELETE_COPY_CONSTRUCTOR(typeName)\
        typeName(const typeName& other) = delete;\

#define DELETE_COPY_ASSIGNMENT(typeName)\
        typeName& operator=(const typeName& other) = delete;\

#define DELETE_MOVE_CONSTRUCTOR(typeName)\
        typeName(typeName&& other) = delete;\

#define DELETE_MOVE_ASSIGNMENT(typeName)\
        typeName& operator=(typeName&& other) = delete;\

#define DELETE_COPY_ASSIGNMENT_AND_CONSTRUCTOR(typeName)\
        DELETE_COPY_CONSTRUCTOR(typeName)\
        DELETE_COPY_ASSIGNMENT(typeName)\

#define DELETE_MOVE_ASSIGNMENT_AND_CONSTRUCTOR(typeName)\
        DELETE_MOVE_CONSTRUCTOR(typeName)\
        DELETE_MOVE_ASSIGNMENT(typeName)\

#if defined(_WIN32) || defined(_WIN64)
    #if defined(PUBLIC_API_EXPORT)
        #define PUBLIC_API __declspec(dllexport)
    #elif defined(PUBLIC_API_IMPORT)
        #define PUBLIC_API __declspec(dllimport)
    #else
        #define PUBLIC_API
    #endif
#else
    #define PUBLIC_API
#endif
}

#endif //INCLUDE_KMMQTT_GLOBALMACROS_H