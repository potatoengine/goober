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

        context->fontAtlas = new (grAlloc(sizeof(grFontAtlas))) grFontAtlas;

        return context;
    }

    grStatus grDestroyContext(grContext* context) {
        if (context == nullptr)
            return grStatus::NullArgument;

        context->fontAtlas->~grFontAtlas();
        grFree(context->fontAtlas);

        context->~grContext();
        grFree(context);

        return grStatus::Ok;
    }

    static void grResetPortal(grPortal* portal) {
        portal->idStack.clear();
        portal->elements.clear();
        portal->elementStack.clear();
        portal->lastElement = -1;
        portal->draw->reset();

        portal->elements.push_back({});
        portal->currentElement = 0;
    }

    grResult<grId> grBeginPortal(grContext* context, grStringView name, grRect bounds) {
        if (context == nullptr)
            return grStatus::NullArgument;

        grId id = grHashFnv1a(name);

        grPortal* port = nullptr;

        for (grPortal* sportal : context->portals) {
            if (sportal->id == id) {
                port = sportal;
                break;
            }
        }

        if (port == nullptr) {
            port = context->portals.push_back(new (grAlloc(sizeof(grPortal))) grPortal);
            port->name = grString(name);
            port->id = id;
            port->draw.reset(new (grAlloc(sizeof(grDrawList))) grDrawList());
            grResetPortal(port);
        }

        port->bounds = bounds;

        context->portalStack.push_back(port);
        context->currentPortal = port;
        context->currentDrawList = port->draw.get();

        return id;
    }

    grStatus grEndPortal(grContext* context) {
        if (context == nullptr)
            return grStatus::NullArgument;
        if (context->portalStack.empty())
            return grStatus::Empty;

        context->portalStack.pop_back();
        context->currentPortal =
            context->portalStack.empty() ? nullptr : context->portalStack.back();
        context->currentDrawList =
            context->currentPortal != nullptr ? context->currentPortal->draw.get() : nullptr;

        return grStatus::Ok;
    }

    grPortal* grCurrentPortal(grContext* context) {
        if (context == nullptr)
            return nullptr;
        return context->currentPortal;
    }

    grDrawList* grCurrentDrawList(grContext* context) {
        if (context == nullptr)
            return nullptr;
        return context->currentDrawList;
    }

    grStatus grBeginFrame(grContext* context, float deltaTime) {
        if (context == nullptr)
            return grStatus::NullArgument;

        for (grPortal* port : context->portals)
            grResetPortal(port);

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

        context->currentPortal = nullptr;

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

    static int grAddElement(grPortal* portal, grElementKind kind, grVec2 size, grColor bgColor) {
        int const elementIndex = static_cast<int>(portal->elements.size());
        grElement& element = portal->elements.push_back({});
        element.kind = kind;
        element.size = size;
        element.bgColor = bgColor;

        if (portal->currentElement != -1) {
            grElement& current = portal->elements[portal->currentElement];
            if (current.firstChild == -1) {
                current.firstChild = current.lastChild = elementIndex;
            }
            else {
                grElement& prevSibling = portal->elements[current.lastChild];
                prevSibling.nextSibling = elementIndex;
            }
        }
        else if (portal->lastElement != -1) {
            grElement& prevSibling = portal->elements[portal->lastElement];
            prevSibling.nextSibling = elementIndex;
        }

        portal->lastElement = elementIndex;

        return elementIndex;
    }

    void grAddInlineElement(grContext* context, grVec2 size, grColor bgColor) {
        if (context == nullptr)
            return;

        grPortal* portal = context->currentPortal;
        if (portal == nullptr)
            return;

        grAddElement(portal, grElementKind::Inline, size, bgColor);
    }

    void grAddBlockElement(grContext* context, grVec2 size, grColor bgColor) {
        if (context == nullptr)
            return;

        grPortal* portal = context->currentPortal;
        if (portal == nullptr)
            return;

        grAddElement(portal, grElementKind::Block, size, bgColor);
    }

    bool grBeginBlockElement(grContext* context, grVec2 size, grColor bgColor) {
        if (context == nullptr)
            return false;

        grPortal* portal = context->currentPortal;
        if (portal == nullptr)
            return false;

        int const elementIndex = grAddElement(portal, grElementKind::Block, size, bgColor);

        portal->elementStack.push_back(portal->currentElement);
        portal->currentElement = elementIndex;
        return true;
    }

    void grEndElement(grContext* context) noexcept {
        if (context == nullptr)
            return;

        grPortal* portal = context->currentPortal;
        if (portal == nullptr)
            return;

        if (portal->elementStack.empty()) {
            portal->currentElement = -1;
        }
        else {
            portal->currentElement = portal->elementStack.back();
            portal->elementStack.pop_back();
        }
    }

    static void grLayoutPortalElements(grPortal* portal) {
        grRect const bounds = portal->bounds;
        grVec2 cursor = bounds.minimum;
        float runMaxHeight = 0;

        for (grElement& elem : portal->elements) {
            grVec2 pos = cursor;

            if (elem.kind == grElementKind::Inline) {
                if (elem.size.y > runMaxHeight)
                    runMaxHeight = elem.size.y;
                cursor.x += elem.size.x;
                if (cursor.x >= bounds.maximum.x && elem.size.x < bounds.size().x) {
                    cursor.x = bounds.minimum.x;
                    cursor.y += runMaxHeight;
                    runMaxHeight = 0;
                }
            }
            else {
                cursor.x = pos.x = bounds.minimum.x;
                pos.y += runMaxHeight;
                cursor.y = pos.y + elem.size.y;
                runMaxHeight = 0;
            }

            elem.layout.minimum = pos;
            elem.layout.maximum = pos + elem.size;
        }
    }

    static void grRenderPortalElements(grPortal* portal) {
        grDrawList* draw = portal->draw.get();

        for (grElement const& elem : portal->elements) {
            if (elem.bgColor != grColors::transparent)
                draw->drawRect(elem.layout, elem.bgColor);
        }
    }

    grStatus grRenderElements(grContext* context) {
        if (context == nullptr)
            return grStatus::NullArgument;

        for (grPortal* portal : context->portals) {
            grLayoutPortalElements(portal);
            grRenderPortalElements(portal);
        }

        return grStatus::Ok;
    }

} // namespace goober
