// kmMqtt (https://github.com/KMiseckas/kmMqtt)
// Copyright (c) 2026 Klaudijus Miseckas
// Licensed under the Apache License, Version 2.0
// See LICENSE file in the project root for full license information.

#pragma once

template <size_t N>
struct Functor
{
    char data[N];
    void operator()() { int a = 2 + N; }
};
