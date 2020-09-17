// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "catch.hpp"
#include "goober.hh"

TEST_CASE("core initialization", "[core]") {
    grContext ctx;
    CHECK(grInitialize(ctx) == grStatus::Ok);
}

TEST_CASE("mouse input", "[core][mouse]") {
    SECTION("position") {
        grContext ctx;
        grInitialize(ctx);

        ctx.mousePos = {10, 10};
        grBeginFrame(ctx, 0.f);
        grEndFrame(ctx);

        ctx.mousePos = {30, 30};
        grBeginFrame(ctx, 0.f);

        CHECK(ctx.mousePos == grVec2{30, 30});
        CHECK(ctx.mousePosDelta == grVec2{20, 20});
    }

    SECTION("button down") {
        grContext ctx;
        grInitialize(ctx);

        ctx.mouseButtons = grMouseButton::Left;
        grBeginFrame(ctx, 0.f);

        CHECK(grIsMouseDown(ctx, grMouseButton::Left));
        CHECK_FALSE(grIsMouseDown(ctx, grMouseButton::Right));

        grEndFrame(ctx);

        ctx.mouseButtons = grMouseButton::Right;
        grBeginFrame(ctx, 0.f);

        CHECK_FALSE(grIsMouseDown(ctx, grMouseButton::Left));
        CHECK(grIsMouseDown(ctx, grMouseButton::Right));
    }

    SECTION("button pressed") {
        grContext ctx;
        grInitialize(ctx);

        ctx.mouseButtons = grMouseButton::Left;
        grBeginFrame(ctx, 0.f);

        CHECK(grIsMousePressed(ctx, grMouseButton::Left));
        CHECK_FALSE(grIsMousePressed(ctx, grMouseButton::Right));

        grEndFrame(ctx);

        grBeginFrame(ctx, 0.f);

        CHECK_FALSE(grIsMousePressed(ctx, grMouseButton::Left));
        CHECK_FALSE(grIsMousePressed(ctx, grMouseButton::Right));

        grEndFrame(ctx);

        ctx.mouseButtons = grMouseButton::Right;
        grBeginFrame(ctx, 0.f);

        CHECK_FALSE(grIsMousePressed(ctx, grMouseButton::Left));
        CHECK(grIsMousePressed(ctx, grMouseButton::Right));
    }

    SECTION("button released") {
        grContext ctx;
        grInitialize(ctx);

        ctx.mouseButtons = grMouseButton::Left;
        grBeginFrame(ctx, 0.f);

        CHECK_FALSE(grIsMouseReleased(ctx, grMouseButton::Left));
        CHECK_FALSE(grIsMouseReleased(ctx, grMouseButton::Right));

        grEndFrame(ctx);

        ctx.mouseButtons = grMouseButton::None;
        grBeginFrame(ctx, 0.f);

        CHECK(grIsMouseReleased(ctx, grMouseButton::Left));
        CHECK_FALSE(grIsMouseReleased(ctx, grMouseButton::Right));
    }
}
