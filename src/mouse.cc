// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "mouse.hh"
#include "goober.hh"

inline namespace goober {

    bool grIsMouseDown(grContext const& context, grMouseButton button) noexcept {
        return (context.mouseButtons & button) != grMouseButton{};
    }

    bool grIsMousePressed(grContext const& context, grMouseButton button) noexcept {
        bool const isDown = (context.mouseButtons & button) != grMouseButton{};
        bool const wasDown = (context.mouseButtonsLast & button) != grMouseButton{};

        return isDown && !wasDown;
    }

    bool grIsMouseReleased(grContext const& context, grMouseButton button) noexcept {
        bool const isDown = (context.mouseButtons & button) != grMouseButton{};
        bool const wasDown = (context.mouseButtonsLast & button) != grMouseButton{};

        return !isDown && wasDown;
    }

} // namespace goober
