// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#if !defined(GOOBER_HH_)
#define GOOBER_HH_
#pragma once

#include "goober.h"

namespace goober {
    struct Vec2 {
        Vec2() = default;
        constexpr Vec2(float xi, float yi) noexcept : x(xi), y(yi) {}
        constexpr Vec2(grVec2 r) noexcept : x(r.x), y(r.y) {}
        constexpr operator grVec2() const noexcept { return {x, y}; }

        constexpr friend Vec2 operator+(Vec2 l, Vec2 r) noexcept { return {l.x + r.x, l.y + r.y}; }
        constexpr friend Vec2 operator-(Vec2 l, Vec2 r) noexcept { return {l.x - r.x, l.y - r.y}; }

        constexpr friend bool operator==(Vec2 l, Vec2 r) noexcept {
            return l.x == r.x && l.y == r.y;
        }
        constexpr friend bool operator!=(Vec2 l, Vec2 r) noexcept {
            return l.x != r.x || l.y != r.y;
        }

        float x = 0.f;
        float y = 0.f;
    };

    enum class MouseButton { None, Left = (1 << 0), Right = (1 << 1), Middle = (1 << 2) };
    constexpr MouseButton operator|(MouseButton l, MouseButton r);
    constexpr MouseButton operator&(MouseButton l, MouseButton r);

    class Context {
    public:
        Context() = default;
        ~Context() { grDeleteContext(_context); }

        inline static Context create();

        inline Context(Context&& rhs) noexcept;
        inline Context& operator=(Context&& rhs);

        inline grContext* get() const noexcept { return _context; }

    private:
        grContext* _context = nullptr;
    };

    // --- implementation ---

    constexpr MouseButton operator|(MouseButton l, MouseButton r) {
        return static_cast<MouseButton>(static_cast<unsigned>(l) | static_cast<unsigned>(r));
    }
    constexpr MouseButton operator&(MouseButton l, MouseButton r) {
        return static_cast<MouseButton>(static_cast<unsigned>(l) & static_cast<unsigned>(r));
    }

    Context Context::create() {
        Context ctx;
        // FIXME: handle errors (no, not exceptions)
        (void)grCreateContext(&ctx._context);
        return ctx;
    }

    Context::Context(Context&& rhs) noexcept {
        _context = rhs._context;
        rhs._context = nullptr;
    }

    Context& Context::operator=(Context&& rhs) {
        if (this != &rhs) {
            grDeleteContext(_context);
            _context = rhs._context;
            rhs._context = nullptr;
        }
        return *this;
    }
} // namespace goober

#endif // defined(GOOBER_HH_)
