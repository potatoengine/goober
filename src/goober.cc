// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober.h"
#include "context.h"

grStatus grCreateContext(grContext** out_context) {
    if (out_context == nullptr) {
        return grStatus_BadArgument;
    }

    *out_context = new grContext;
    return grStatus_OK;
}

void grDeleteContext(grContext* context) {
    delete context;
}

GOOBER_API grStatus
grContextSetMouseState(grContext* context, grVec2 pos, grMouseButton buttons, float wheel) {
    if (context == nullptr) {
        return grStatus_BadArgument;
    }

    context->mousePosLast = context->mousePos;
    context->mousePos = pos;

    context->mouseButtonsLast = context->mouseButtons;
    context->mouseButtons = static_cast<goober::MouseButton>(buttons);

    return grStatus_OK;
}

GOOBER_API grVec2 grGetMousePosition(grContext const* context) {
    if (context == nullptr) {
        return {0, 0};
    }

    return context->mousePos;
}

GOOBER_API grVec2 grGetMouseDelta(grContext const* context) {
    if (context == nullptr) {
        return {0, 0};
    }

    return context->mousePos - context->mousePosLast;
}

GOOBER_API bool grIsMouseDown(grContext const* context, grMouseButton button) {
    if (context == nullptr) {
        return false;
    }

    return (context->mouseButtons & static_cast<goober::MouseButton>(button)) !=
        goober::MouseButton{};
}

GOOBER_API bool grIsMousePressed(grContext const* context, grMouseButton button) {
    if (context == nullptr) {
        return false;
    }

    bool const isDown =
        (context->mouseButtons & static_cast<goober::MouseButton>(button)) != goober::MouseButton{};
    bool const wasDown = (context->mouseButtonsLast & static_cast<goober::MouseButton>(button)) !=
        goober::MouseButton{};

    return isDown && !wasDown;
}

GOOBER_API bool grIsMouseReleased(grContext const* context, grMouseButton button) {
    if (context == nullptr) {
        return false;
    }

    bool const isDown =
        (context->mouseButtons & static_cast<goober::MouseButton>(button)) != goober::MouseButton{};
    bool const wasDown = (context->mouseButtonsLast & static_cast<goober::MouseButton>(button)) !=
        goober::MouseButton{};

    return !isDown && wasDown;
}
