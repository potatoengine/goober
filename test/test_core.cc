// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "catch.hpp"
#include "goober/core.hh"

TEST_CASE("core initialization", "[core]") {
    grAllocator alloc;
    auto [result, ctx] = grCreateContext(alloc);
    REQUIRE(result == grStatus::Ok);
    REQUIRE(ctx != nullptr);
    auto result2 = grDestroyContext(ctx);
    REQUIRE(result2 == grStatus::Ok);
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

TEST_CASE("grIsContained", "[core][math]") {
    static constexpr grVec4 aabb = {10, 10, 20, 20};

    SECTION("contained") {
        // on the upper-left edge on one or both axes
        CHECK(grIsContained(aabb, {10, 10}));
        CHECK(grIsContained(aabb, {10, 15}));
        CHECK(grIsContained(aabb, {15, 10}));

        // clearly in the middle
        CHECK(grIsContained(aabb, {15, 15}));
    }

    SECTION("uncontained") {
        // clearly outside the bounds
        CHECK_FALSE(grIsContained(aabb, {0, 0}));
        CHECK_FALSE(grIsContained(aabb, {30, 30}));

        // on the upper-left edge on one axis but side bounds
        CHECK_FALSE(grIsContained(aabb, {10, 0}));
        CHECK_FALSE(grIsContained(aabb, {0, 10}));

        // on the bottom-right edge, which is not included
        CHECK_FALSE(grIsContained(aabb, {20, 20}));

        // outside the bounds on one axis
        CHECK_FALSE(grIsContained(aabb, {30, 15}));
        CHECK_FALSE(grIsContained(aabb, {15, 30}));
        CHECK_FALSE(grIsContained(aabb, {30, 5}));
        CHECK_FALSE(grIsContained(aabb, {5, 30}));
    }
}
