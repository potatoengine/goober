// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_DRAW_HH_)
#define GOOBER_DRAW_HH_
#pragma once

#include "core.hh"

inline namespace goober {

    // ------------------------------------------------------
    //  * grDrawList drawing helper *
    // ------------------------------------------------------

    struct grDrawList {
        using Index = std::uint16_t;
        using Offset = std::uint32_t;

        struct Vertex {
            grVec2 pos;
            grVec2 uv;
            grColor rgba;
        };

        struct Command {
            Offset indexStart = 0;
            Offset indexCount = 0;
            grTextureId textureId = 0;
        };

        grArray<Index> indices;
        grArray<Vertex> vertices;
        grArray<Command> commands;

        GOOBER_API void drawRect(grRect rect, grColor color);
        GOOBER_API void drawRect(
            grTextureId textureId,
            grRect rect,
            grRect texCoord,
            grColor color);
        GOOBER_API void drawText(
            grFont const* font,
            grTextureId textureId,
            grVec2 ul,
            grColor,
            grStringView text);

        void reset() noexcept {
            indices.clear();
            vertices.clear();
            commands.clear();
        }
    };

} // namespace goober

#endif // defined(GOOBER_DRAW_HH_)
