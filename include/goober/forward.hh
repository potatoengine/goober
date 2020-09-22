// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_FORWARD_HH_)
#define GOOBER_FORWARD_HH_
#pragma once

#include <cstdlib>

inline namespace goober {
    template <typename T>
    struct grArray;
    struct grContext;
    template <typename T>
    struct grResult;
    struct grVec2;

    enum class grId : std::uint64_t;
    enum class grModMask : std::uint16_t;
    enum class grButtonMask : std::uint16_t;
    enum class grStatus : std::uint16_t;

} // namespace goober

#endif // defined(GOOBER_FORWARD_HH_)
