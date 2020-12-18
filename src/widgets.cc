// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober/core.hh"
#include "goober/draw.hh"
#include "goober/font.hh"

inline namespace goober {

    bool grButton(grContext* context, grStringView label, grVec2 pos, grColor rgba) {
        grPortal* port = grCurrentPortal(context);
        if (port == nullptr)
            return false;

        grId const id = grGetId(context, label);

        grVec2 const labelSize = grFontMeasureText(context, 1, label);
        grRect const aabb(pos, pos + labelSize + grVec2(8, 8));

        bool const over = grIsMouseOver(context, aabb);

        bool const activated = over && grIsMousePressed(context, grButtonMask::Left);
        if (activated)
            context->activeId = id;

        bool const active = context->activeId == id;
        if (active && grIsMouseDown(context, grButtonMask::Left))
            context->activeIdNext = id;

        port->draw->drawRect({pos, aabb.maximum}, rgba);
        grColor color = (active && over) ? grColors::red
            : (active || over)           ? grColors::yellow
                                         : grColors::grey;
        port->draw->drawRect(
            {{pos.x + 2, pos.y + 2}, {aabb.maximum.x - 2, aabb.maximum.y - 2}},
            color);
        port->draw->drawText(
            grGetFont(context, 1),
            context->fontAtlas->texture,
            {pos.x + 4, pos.y + 4},
            rgba,
            label);

        bool const clicked = over && active && grIsMouseReleased(context, grButtonMask::Left);
        if (clicked)
            context->activeId = {};

        return clicked;
    }

    void grImage(
        grContext* context,
        grTextureId textureId,
        grRect pos,
        grRect texCoord,
        grColor rgba) {
        grDrawList* draw = grCurrentDrawList(context);
        if (draw == nullptr)
            return;

        draw->drawRect(textureId, pos, texCoord, rgba);
    }

    void grText(grContext* context, grStringView text, grVec2 pos, grColor rgba) {
        grDrawList* draw = grCurrentDrawList(context);
        if (draw == nullptr)
            return;

        grFont const* font = grGetFont(context, 1);
        if (font == nullptr)
            return;

        draw->drawText(font, context->fontAtlas->texture, pos, rgba, text);
    }

} // namespace goober
