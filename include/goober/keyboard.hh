// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_KEYBOARD_HH_)
#define GOOBER_KEYBOARD_HH_
#pragma once

#include <cstdint>

inline namespace goober {
    // ------------------------------------------------------
    //  * public types *
    // ------------------------------------------------------
    enum class grMod : uint16_t {
        None,
        LeftShift = (1 << 0),
        RightShift = (1 << 1),
        Shift = LeftShift | RightShift,
        LeftAlt = (1 << 2),
        RightAlt = (1 << 3),
        Alt = LeftAlt | RightAlt,
        LeftCtrl = (1 << 4),
        RightCtrl = (1 << 5),
        Ctrl = LeftCtrl | RightCtrl,
    };
    constexpr grMod operator|(grMod l, grMod r);
    constexpr grMod operator&(grMod l, grMod r);

    // ------------------------------------------------------
    //  * implementation *
    // ------------------------------------------------------

    constexpr grMod operator|(grMod l, grMod r) {
        return static_cast<grMod>(static_cast<uint16_t>(l) | static_cast<uint16_t>(r));
    }
    constexpr grMod operator&(grMod l, grMod r) {
        return static_cast<grMod>(static_cast<uint16_t>(l) & static_cast<uint16_t>(r));
    }

} // namespace goober

#endif // defined(GOOBER_KEYBOARD_HH_)
