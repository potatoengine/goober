// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "catch.hpp"
#include "goober/goober.hh"

TEST_CASE("core initialization", "[core]") {
    grContext ctx;
    CHECK(grInitialize(ctx) == grStatus::Ok);
}

TEST_CASE("fnv1a", "[core][hash]") {
    static constexpr char test[] = "this is test input";
    static constexpr char empty[] = "";

    SECTION("runtime") {
        CHECK(grHashFnv1a(test, sizeof(test) - 1) == 0x77e875b1c7b6a32d);
        CHECK(grHashFnv1a(empty, sizeof(empty) - 1) == 0xcbf29ce484222325);
    }

    static_assert(
        grHashFnv1a(test, sizeof(test) - 1) == 0x77e875b1c7b6a32d,
        "grHashFnv1a(test) result incorrect");
    static_assert(
        grHashFnv1a(empty, sizeof(empty) - 1) == 0xcbf29ce484222325,
        "grHashFnv1a(empty) result incorrect");
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
