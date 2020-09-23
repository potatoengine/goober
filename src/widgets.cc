// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober/core.hh"

inline namespace goober {

    bool grButton(grContext* ctx, char const* label, grVec4 aabb, grColor rgba) {
        bool const over = grIsMouseOver(ctx, aabb);

        ctx->draw.drawRect({aabb.x, aabb.y}, {aabb.z, aabb.w}, rgba);
        grColor color = over ? grColors::yellow : grColors::grey;
        ctx->draw.drawRect({aabb.x + 2, aabb.y + 2}, {aabb.z - 2, aabb.w - 2}, color);

        return over && grIsMouseDown(ctx, grButtonMask::Left);
    }

} // namespace goober
