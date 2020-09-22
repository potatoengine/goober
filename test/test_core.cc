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
    REQUIRE(result == grStatus::Ok);
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
