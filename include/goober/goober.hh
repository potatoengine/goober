// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_HH_)
#define GOOBER_HH_
#pragma once

#include "array.hh"
#include "core.hh"
#include "keyboard.hh"
#include "mouse.hh"

#include <cstdint>
#include <cstdlib>

inline namespace goober {
    // ------------------------------------------------------
    //  * public types *
    // ------------------------------------------------------

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

} // namespace goober

#endif // defined(GOOBER_HH_)
