// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "goober/core.hh"

inline namespace goober {

    grAllocator::grAllocator() noexcept {
        _allocate = [](size_t sizeInBytes, void*) -> void* {
            return std::malloc(sizeInBytes);
        };
        _deallocate = [](void* memory, size_t sizeInBytes, void*) {
            std::free(memory);
        };
    }

    grResult<grContext*> grCreateContext(grAllocator& allocator) {
        grContext* context = new (allocator.allocate(sizeof(grContext))) grContext(allocator);
        if (context == nullptr)
            return grStatus::BadAlloc;

        return context;
    }

    grStatus grDestroyContext(grContext* context) {
        if (context == nullptr)
            return grStatus::NullArgument;

        context->~grContext();
        context->allocator.deallocate(context, sizeof(grContext));

        return grStatus::Ok;
    }

    grStatus grBeginFrame(grContext* context, float deltaTime) {
        if (context == nullptr)
            return grStatus::NullArgument;

        context->draw.reset();

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

    void grDrawList::drawRect(grVec2 ul, grVec2 br, grColor color) {
        Offset vertex = static_cast<Offset>(vertices.size());
        vertices.push_back({ul, {}, color});
        vertices.push_back({{br.x, ul.y}, {}, color});
        vertices.push_back({br, {}, color});
        vertices.push_back({{ul.x, br.y}, {}, color});

        Offset index = static_cast<Offset>(indices.size());
        indices.push_back(vertex);
        indices.push_back(vertex + 1);
        indices.push_back(vertex + 2);
        indices.push_back(vertex + 2);
        indices.push_back(vertex + 3);
        indices.push_back(vertex + 0);

        commands.push_back({index, vertex, 6});
    }

} // namespace goober
