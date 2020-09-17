// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober.hh"

inline namespace goober {

    grStatus grInitialize(grContext& context, size_t contextSize) {
        if (contextSize != sizeof(grContext))
            return grStatus::AbiMismatch;

        context = grContext{};
        return grStatus::Ok;
    }

    grStatus grBeginFrame(grContext& context, float deltaTime) {
        context.mousePosDelta = context.mousePos - context.mousePosLast;
        context.deltaTime = deltaTime;
        return grStatus::Ok;
    }

    grStatus grEndFrame(grContext& context) {
        context.mousePosLast = context.mousePos;
        context.mouseButtonsLast = context.mouseButtons;
        return grStatus::Ok;
    }

} // namespace goober
