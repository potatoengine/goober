// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_CORE_HH_)
#define GOOBER_CORE_HH_
#pragma once

#include <cstdint>
#include <cstdlib>

#define GOOBER_API

inline namespace goober {
    // ------------------------------------------------------
    //  * public types *
    // ------------------------------------------------------
    struct grVec2 {
        grVec2() = default;
        constexpr grVec2(float xi, float yi) noexcept : x(xi), y(yi) {}

        constexpr friend grVec2 operator+(grVec2 l, grVec2 r) noexcept;
        constexpr friend grVec2 operator-(grVec2 l, grVec2 r) noexcept;

        constexpr friend bool operator==(grVec2 l, grVec2 r) noexcept;
        constexpr friend bool operator!=(grVec2 l, grVec2 r) noexcept;

        float x = 0.f;
        float y = 0.f;
    };

    enum class grStatus : uint16_t {
        Ok,
        NullArgument,
        AbiMismatch,
    };

    using grHash = uint64_t;

    struct grAllocator {
        using Allocate = void* (*)(size_t sizeInBytes, void* userData);
        using Deallocate = void (*)(void* memory, size_t sizeInBytes, void* userData);

        Allocate alloc = nullptr;
        Deallocate free = nullptr;
        void* userData = nullptr;

        inline grAllocator() noexcept;
    };

    // ------------------------------------------------------
    //  * public functions *
    // ------------------------------------------------------

    constexpr grHash grHashFnv1a(char const* data, size_t length) noexcept;

    // ------------------------------------------------------
    //  * implementation *
    // ------------------------------------------------------

    constexpr grVec2 operator+(grVec2 l, grVec2 r) noexcept { return {l.x + r.x, l.y + r.y}; }
    constexpr grVec2 operator-(grVec2 l, grVec2 r) noexcept { return {l.x - r.x, l.y - r.y}; }

    constexpr bool operator==(grVec2 l, grVec2 r) noexcept { return l.x == r.x && l.y == r.y; }
    constexpr bool operator!=(grVec2 l, grVec2 r) noexcept { return l.x != r.x || l.y != r.y; }

    constexpr grHash grHashFnv1a(char const* data, size_t length) noexcept {
        grHash state = 14695981039346656037ull;
        for (size_t index = 0; index != length; ++index)
            state = (state ^ data[length]) * 1099511628211ull;
        return state;
    }

    grAllocator::grAllocator() noexcept {
        alloc = [](size_t sizeInBytes, void*) -> void* {
            return std::malloc(sizeInBytes);
        };
        free = [](void* memory, size_t sizeInBytes, void*) {
            std::free(memory);
        };
    }

} // namespace goober

#endif // defined(GOOBER_CORE_HH_)
