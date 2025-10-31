#pragma once

template <size_t N>
struct Functor
{
    char data[N];
    void operator()() { int a = 2 + N; }
};
