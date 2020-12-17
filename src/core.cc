// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober/core.hh"
#include "goober/draw.hh"
#include "goober/font.hh"

inline namespace goober {

    grResult<grContext*> grCreateContext() {
        grContext* context = new (grAlloc(sizeof(grContext))) grContext;
        if (context == nullptr)
            return grStatus::BadAlloc;

        return context;
    }

    grStatus grDestroyContext(grContext* context) {
        if (context == nullptr)
            return grStatus::NullArgument;

        context->~grContext();
        grFree(context);

        return grStatus::Ok;
    }

    grResult<grId> grBeginPortal(grContext* context, grStringView name) {
        if (context == nullptr)
            return grStatus::NullArgument;

        grId id = grHashFnv1a(name);

        for (grPortal* port : context->portals) {
            if (port->id == id) {
                context->portalStack.push_back(port);
                return id;
            }
        }

        grPortal* port = context->portals.push_back(new (grAlloc(sizeof(grPortal))) grPortal);
        port->context = context;
        port->name = grString(name);
        port->id = id;
        port->draw.reset(new (grAlloc(sizeof(grDrawList))) grDrawList());
        context->portalStack.push_back(port);
        return id;
    }

    grStatus grEndPortal(grContext* context) {
        if (context == nullptr)
            return grStatus::NullArgument;
        if (context->portalStack.empty())
            return grStatus::Empty;

        context->portalStack.pop_back();
        return grStatus::Ok;
    }

    grStatus grBeginFrame(grContext* context, float deltaTime) {
        if (context == nullptr)
            return grStatus::NullArgument;

        for (grPortal* port : context->portals) {
            port->idStack.clear();
            port->widgetStack.clear();
            port->draw->reset();
        }

        context->activeId = context->activeIdNext;
        context->activeIdNext = {};

        context->mousePosDelta = context->mousePos - context->mousePosLast;
        context->deltaTime = deltaTime;
        return grStatus::Ok;
    }

    grStatus grEndFrame(grContext* context) {
        if (context == nullptr)
            return grStatus::NullArgument;

        context->mousePosLast = context->mousePos;
        context->mouseButtonsLast = context->mouseButtons;
        return grStatus::Ok;
    }

    grId grGetId(grContext const* context, uint64_t hash) noexcept {
        if (context == nullptr)
            return static_cast<grId>(hash);
        if (context->portalStack.empty())
            return static_cast<grId>(hash);

        grPortal* port = context->portalStack.back();
        if (port->idStack.empty())
            return grHashCombine(static_cast<std::uint64_t>(port->id), hash);

        return static_cast<grId>(
            grHashCombine(static_cast<std::uint64_t>(port->idStack.back()), hash));
    }

    grStatus grPushId(grContext* context, grId id) {
        if (context == nullptr)
            return grStatus::NullArgument;
        if (context->portalStack.empty())
            return grStatus::Empty;

        context->portalStack.back()->idStack.push_back(id);
        return grStatus::Ok;
    }

    grStatus grPopId(grContext* context) noexcept {
        if (context == nullptr)
            return grStatus::NullArgument;
        if (context->portalStack.empty())
            return grStatus::Empty;
        if (context->portalStack.back()->idStack.empty())
            return grStatus::Empty;

        context->portalStack.back()->idStack.pop_back();
        return grStatus::Ok;
    }

    bool grIsMouseDown(grContext const* context, grButtonMask button) noexcept {
        if (context == nullptr)
            return false;

        return (context->mouseButtons & button) != grButtonMask{};
    }

    bool grIsMousePressed(grContext const* context, grButtonMask button) noexcept {
        if (context == nullptr)
            return false;

        bool const isDown = (context->mouseButtons & button) != grButtonMask{};
        bool const wasDown = (context->mouseButtonsLast & button) != grButtonMask{};

        return isDown && !wasDown;
    }

    bool grIsMouseReleased(grContext const* context, grButtonMask button) noexcept {
        if (context == nullptr)
            return false;

        bool const isDown = (context->mouseButtons & button) != grButtonMask{};
        bool const wasDown = (context->mouseButtonsLast & button) != grButtonMask{};

        return !isDown && wasDown;
    }

    bool grIsMouseOver(grContext const* context, grRect area) noexcept {
        if (context == nullptr)
            return false;

        return grIsContained(area, context->mousePos);
    }

    bool grIsMouseEntering(grContext const* context, grRect area) noexcept {
        if (context == nullptr)
            return false;

        return grIsContained(area, context->mousePos) &&
            !grIsContained(area, context->mousePosLast);
    }

    bool grIsMouseLeaving(grContext const* context, grRect area) noexcept {
        if (context == nullptr)
            return false;

        return !grIsContained(area, context->mousePos) &&
            grIsContained(area, context->mousePosLast);
    }

} // namespace goober
