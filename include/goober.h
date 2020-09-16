// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_H_)
#define GOOBER_H_
#pragma once

#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define GOOBER_API

typedef struct grVec2 {
    float x;
    float y;
} grVec2;

typedef enum grStatus {
    grStatus_OK,
    grStatus_BadArgument,
} grStatus;

typedef struct grContext grContext;
typedef struct grScope grScope;

typedef enum grMouseButton {
    grMouseButton_None,
    grMouseButton_Left = (1 << 0),
    grMouseButton_Right = (1 << 1),
    grMouseButton_Middle = (1 << 2)
} grMouseButton;

GOOBER_API grStatus grCreateContext(grContext** out_context);
GOOBER_API void grDeleteContext(grContext* context);

GOOBER_API grStatus
grContextSetMouseState(grContext* context, grVec2 pos, grMouseButton buttons, float wheel);
GOOBER_API grVec2 grGetMousePosition(grContext const* context);
GOOBER_API grVec2 grGetMouseDelta(grContext const* context);
GOOBER_API bool grIsMouseDown(grContext const* context, grMouseButton button);
GOOBER_API bool grIsMousePressed(grContext const* context, grMouseButton button);
GOOBER_API bool grIsMouseReleased(grContext const* context, grMouseButton button);

#if defined(__cplusplus)
} // extern "C"
#endif

#endif // defined(GOOBER_H_)
