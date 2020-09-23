// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "catch.hpp"
#include "goober/core.hh"

#include <string>

TEST_CASE("grArray", "[array]") {
    grAllocator alloc;

    SECTION("push_back trivial") {
        grArray<std::size_t> test(alloc);

        std::size_t const iterations = 1000;

        for (std::size_t index = 0; index != iterations; ++index) {
            test.push_back(index);
            REQUIRE(test.size() == index + 1);
            REQUIRE(test.capacity() >= test.size());
            REQUIRE(test[index] == index);
        }

        for (std::size_t index = 0; index != iterations; ++index)
            REQUIRE(test[index] == index);
    }

    SECTION("push_back non-trivial") {
        grArray<std::string> test(alloc);

        std::string const input = "test";

        std::size_t const iterations = 1000;

        for (std::size_t index = 0; index != iterations; ++index) {
            test.push_back(input);
            REQUIRE(test.size() == index + 1);
            REQUIRE(test.capacity() >= test.size());
            REQUIRE(test[index] == input);
        }

        for (std::size_t index = 0; index != iterations; ++index)
            REQUIRE(test[index] == input);
    }

    SECTION("resize") {
        grArray<std::size_t> test(alloc);

        std::size_t const size1 = 1000;
        std::size_t const size2 = 4000;
        std::size_t const size3 = 2000;

        test.resize(size1);
        REQUIRE(test.size() == size1);
        REQUIRE(test.capacity() >= size1);

        test.resize(size2);
        REQUIRE(test.size() == size2);
        REQUIRE(test.capacity() >= size2);

        test.resize(size3);
        REQUIRE(test.size() == size3);
        REQUIRE(test.capacity() >= size2); // capacity should not have shrunk
    }

    SECTION("shrink_to_fit") {
        grArray<std::size_t> test(alloc);

        std::size_t const capacity = 1000;

        test.reserve(capacity);
        REQUIRE(test.capacity() == capacity);

        test.reserve(0);
        REQUIRE(test.capacity() == capacity);

        test.shrink_to_fit();
        REQUIRE(test.capacity() == 0);
    }
}
