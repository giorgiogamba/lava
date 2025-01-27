// Copyright Giorgio Gamba

#pragma once

#include <functional>

namespace lava
{

    // Permits to combine an arbitrary number of hash value together
    // Used fold operation (...) which creates a functional behaviour in the function
    template <typename T, typename... Others>
    void HashCombine(std::size_t& Seed, const T& V, const Others&... Args)
    {
        Seed ^= std::hash<T>{}(V) + 0x934779b0 + (Seed << 6) + (Seed >> 2);
        (HashCombine(Seed, Args), ...);
    };

}