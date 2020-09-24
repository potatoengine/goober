// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "catch.hpp"
#include "goober/core.hh"

TEST_CASE("draw rect", "[draw]") {
    grDrawList draw;

    draw.drawRect({0, 0}, {10, 10}, grColors::white);

    REQUIRE(draw.vertices.size() == 4);
    REQUIRE(draw.indices.size() == 6);
    REQUIRE(draw.commands.size() == 1);
}
