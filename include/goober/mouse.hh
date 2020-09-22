// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_MOUSE_HH_)
#define GOOBER_MOUSE_HH_
#pragma once

#include "core.hh"

#include <cstdint>

inline namespace goober {
    // ------------------------------------------------------
    //  * public types *
    // ------------------------------------------------------

    struct grContext;

    enum class grMouseButton : uint16_t {
        None,
        Left = (1 << 0),
        Right = (1 << 1),
        Middle = (1 << 2)
    };
    constexpr grMouseButton operator|(grMouseButton l, grMouseButton r);
    constexpr grMouseButton operator&(grMouseButton l, grMouseButton r);

    // ------------------------------------------------------
    //  * public functions *
    // ------------------------------------------------------

    GOOBER_API bool grIsMouseDown(grContext const& context, grMouseButton button) noexcept;
    GOOBER_API bool grIsMousePressed(grContext const& context, grMouseButton button) noexcept;
    GOOBER_API bool grIsMouseReleased(grContext const& context, grMouseButton button) noexcept;

    // ------------------------------------------------------
    //  * implementation *
    // ------------------------------------------------------

    constexpr grMouseButton operator|(grMouseButton l, grMouseButton r) {
        return static_cast<grMouseButton>(static_cast<uint16_t>(l) | static_cast<uint16_t>(r));
    }
    constexpr grMouseButton operator&(grMouseButton l, grMouseButton r) {
        return static_cast<grMouseButton>(static_cast<uint16_t>(l) & static_cast<uint16_t>(r));
    }

} // namespace goober

#endif // defined(GOOBER_MOUSE_HH_)
