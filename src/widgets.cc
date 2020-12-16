// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober/core.hh"
#include "goober/draw.hh"

inline namespace goober {

    bool grButton(grContext* context, grStringView label, grVec4 aabb, grColor rgba) {
        if (context == nullptr)
            return false;
        if (context->portalStack.empty())
            return false;

        grPortal* port = context->portalStack.back();
        grId const id = grGetId(context, label);

        bool const over = grIsMouseOver(port->context, aabb);

        bool const activated = over && grIsMousePressed(context, grButtonMask::Left);
        if (activated)
            context->activeId = id;

        bool const active = context->activeId == id;
        if (active && grIsMouseDown(context, grButtonMask::Left))
            context->activeIdNext = id;

        port->draw->drawRect({{aabb.x, aabb.y}, {aabb.z, aabb.w}}, rgba);
        grColor color = active ? grColors::red : over ? grColors::yellow : grColors::grey;
        port->draw->drawRect({{aabb.x + 2, aabb.y + 2}, {aabb.z - 2, aabb.w - 2}}, color);

        bool const clicked = over && active && grIsMouseReleased(context, grButtonMask::Left);
        if (clicked)
            context->activeId = {};

        return clicked;
    }

} // namespace goober
