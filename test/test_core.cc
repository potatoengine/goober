// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "catch.hpp"
#include "goober.hh"

TEST_CASE("core initialization", "[core]") {
    auto ctx = goober::Context::create();
}

TEST_CASE("mouse input", "[core][mouse]") {
    using namespace goober;

    auto ctx = Context::create();

    SECTION("position") {
        grContextSetMouseState(ctx.get(), {10, 10}, grMouseButton_None, 0.f);
        grContextSetMouseState(ctx.get(), {30, 30}, grMouseButton_Left, 0.f);

        CHECK(grGetMousePosition(ctx.get()) == Vec2{30, 30});
        CHECK(grGetMouseDelta(ctx.get()) == Vec2{20, 20});
    }

    SECTION("button down") {
        grContextSetMouseState(ctx.get(), {}, grMouseButton_None, 0.f);
        grContextSetMouseState(ctx.get(), {}, grMouseButton_Left, 0.f);

        CHECK(grIsMouseDown(ctx.get(), grMouseButton_Left));
        CHECK_FALSE(grIsMouseDown(ctx.get(), grMouseButton_Right));

        grContextSetMouseState(ctx.get(), {}, grMouseButton_Right, 0.f);

        CHECK_FALSE(grIsMouseDown(ctx.get(), grMouseButton_Left));
        CHECK(grIsMouseDown(ctx.get(), grMouseButton_Right));
    }

    SECTION("button pressed") {
        grContextSetMouseState(ctx.get(), {}, grMouseButton_None, 0.f);
        grContextSetMouseState(ctx.get(), {}, grMouseButton_Left, 0.f);

        CHECK(grIsMousePressed(ctx.get(), grMouseButton_Left));
        CHECK_FALSE(grIsMousePressed(ctx.get(), grMouseButton_Right));

        grContextSetMouseState(ctx.get(), {}, grMouseButton_Right, 0.f);

        CHECK_FALSE(grIsMousePressed(ctx.get(), grMouseButton_Left));
        CHECK(grIsMousePressed(ctx.get(), grMouseButton_Right));
    }

    SECTION("button released") {
        grContextSetMouseState(ctx.get(), {}, grMouseButton_None, 0.f);
        grContextSetMouseState(ctx.get(), {}, grMouseButton_Left, 0.f);

        CHECK_FALSE(grIsMouseReleased(ctx.get(), grMouseButton_Left));
        CHECK_FALSE(grIsMouseReleased(ctx.get(), grMouseButton_Right));

        grContextSetMouseState(ctx.get(), {}, grMouseButton_Right, 0.f);

        CHECK(grIsMouseReleased(ctx.get(), grMouseButton_Left));
        CHECK_FALSE(grIsMouseReleased(ctx.get(), grMouseButton_Right));
    }
}
