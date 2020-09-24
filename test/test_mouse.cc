// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "catch.hpp"
#include "goober/core.hh"

TEST_CASE("mouse input", "[core][mouse]") {
    SECTION("position") {
        auto [result, ctx] = grCreateContext();

        ctx->mousePos = {10, 10};
        grBeginFrame(ctx, 0.f);
        grEndFrame(ctx);

        ctx->mousePos = {30, 30};
        grBeginFrame(ctx, 0.f);

        CHECK(ctx->mousePos == grVec2{30, 30});
        CHECK(ctx->mousePosDelta == grVec2{20, 20});
    }

    SECTION("button down") {
        auto [result, ctx] = grCreateContext();

        ctx->mouseButtons = grButtonMask::Left;
        grBeginFrame(ctx, 0.f);

        CHECK(grIsMouseDown(ctx, grButtonMask::Left));
        CHECK_FALSE(grIsMouseDown(ctx, grButtonMask::Right));

        grEndFrame(ctx);

        ctx->mouseButtons = grButtonMask::Right;
        grBeginFrame(ctx, 0.f);

        CHECK_FALSE(grIsMouseDown(ctx, grButtonMask::Left));
        CHECK(grIsMouseDown(ctx, grButtonMask::Right));
    }

    SECTION("button pressed") {
        auto [result, ctx] = grCreateContext();

        ctx->mouseButtons = grButtonMask::Left;
        grBeginFrame(ctx, 0.f);

        CHECK(grIsMousePressed(ctx, grButtonMask::Left));
        CHECK_FALSE(grIsMousePressed(ctx, grButtonMask::Right));

        grEndFrame(ctx);

        grBeginFrame(ctx, 0.f);

        CHECK_FALSE(grIsMousePressed(ctx, grButtonMask::Left));
        CHECK_FALSE(grIsMousePressed(ctx, grButtonMask::Right));

        grEndFrame(ctx);

        ctx->mouseButtons = grButtonMask::Right;
        grBeginFrame(ctx, 0.f);

        CHECK_FALSE(grIsMousePressed(ctx, grButtonMask::Left));
        CHECK(grIsMousePressed(ctx, grButtonMask::Right));
    }

    SECTION("button released") {
        auto [result, ctx] = grCreateContext();

        ctx->mouseButtons = grButtonMask::Left;
        grBeginFrame(ctx, 0.f);

        CHECK_FALSE(grIsMouseReleased(ctx, grButtonMask::Left));
        CHECK_FALSE(grIsMouseReleased(ctx, grButtonMask::Right));

        grEndFrame(ctx);

        ctx->mouseButtons = grButtonMask::None;
        grBeginFrame(ctx, 0.f);

        CHECK(grIsMouseReleased(ctx, grButtonMask::Left));
        CHECK_FALSE(grIsMouseReleased(ctx, grButtonMask::Right));
    }
}
