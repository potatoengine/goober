// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#pragma once

#include "goober.hh"

struct grContext {
    goober::Vec2 mousePosLast;
    goober::Vec2 mousePos;
    goober::MouseButton mouseButtonsLast{};
    goober::MouseButton mouseButtons{};
};
