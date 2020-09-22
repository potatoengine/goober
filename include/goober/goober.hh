// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_HH_)
#define GOOBER_HH_
#pragma once

#include "array.hh"
#include "core.hh"

#include <cstdint>
#include <cstdlib>

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

    enum class grMouseButton : uint16_t {
        None,
        Left = (1 << 0),
        Right = (1 << 1),
        Middle = (1 << 2)
    };
    constexpr grMouseButton operator|(grMouseButton l, grMouseButton r);
    constexpr grMouseButton operator&(grMouseButton l, grMouseButton r);

    enum class grId : grHash {};

    struct grContext {
        grAllocator allocator;
        grVec2 mousePosLast;
        grVec2 mousePos;
        grVec2 mousePosDelta;
        grMod modifiers{};
        grMouseButton mouseButtonsLast{};
        grMouseButton mouseButtons{};
        float deltaTime = 0.f;
    };

    // ------------------------------------------------------
    //  * public functions *
    // ------------------------------------------------------

    GOOBER_API grStatus grInitialize(grContext& context, size_t contextSize = sizeof(grContext));

    GOOBER_API grStatus grBeginFrame(grContext& context, float deltaTime);
    GOOBER_API grStatus grEndFrame(grContext& context);

    constexpr bool grIsMouseDown(grContext const& context, grMouseButton button) noexcept;
    constexpr bool grIsMousePressed(grContext const& context, grMouseButton button) noexcept;
    constexpr bool grIsMouseReleased(grContext const& context, grMouseButton button) noexcept;

    // ------------------------------------------------------
    //  * implementation *
    // ------------------------------------------------------

    constexpr grMod operator|(grMod l, grMod r) {
        return static_cast<grMod>(static_cast<uint16_t>(l) | static_cast<uint16_t>(r));
    }
    constexpr grMod operator&(grMod l, grMod r) {
        return static_cast<grMod>(static_cast<uint16_t>(l) & static_cast<uint16_t>(r));
    }

    constexpr grMouseButton operator|(grMouseButton l, grMouseButton r) {
        return static_cast<grMouseButton>(static_cast<uint16_t>(l) | static_cast<uint16_t>(r));
    }
    constexpr grMouseButton operator&(grMouseButton l, grMouseButton r) {
        return static_cast<grMouseButton>(static_cast<uint16_t>(l) & static_cast<uint16_t>(r));
    }

    constexpr bool grIsMouseDown(grContext const& context, grMouseButton button) noexcept {
        return (context.mouseButtons & button) != grMouseButton{};
    }

    constexpr bool grIsMousePressed(grContext const& context, grMouseButton button) noexcept {
        bool const isDown = (context.mouseButtons & button) != grMouseButton{};
        bool const wasDown = (context.mouseButtonsLast & button) != grMouseButton{};

        return isDown && !wasDown;
    }

    constexpr bool grIsMouseReleased(grContext const& context, grMouseButton button) noexcept {
        bool const isDown = (context.mouseButtons & button) != grMouseButton{};
        bool const wasDown = (context.mouseButtonsLast & button) != grMouseButton{};

        return !isDown && wasDown;
    }

} // namespace goober

#endif // defined(GOOBER_HH_)
