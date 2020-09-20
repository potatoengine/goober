// goober - by Sean Middleditch
// This is free and unencumbered software released into the public domain.
// See LICENSE.md for more details.

#include "array.hh"
#include "catch.hpp"

TEST_CASE("grArray", "[array]") {
    grAllocator alloc;

    SECTION("push_back trivial") {
        grArray<int> test(alloc);

        int const iterations = 1000;

        for (int index = 0; index != iterations; ++index) {
            test.push_back(index);
            REQUIRE(test.size() == index + 1);
            REQUIRE(test.capacity() >= test.size());
            REQUIRE(test.first[index] == index);
        }

        for (int index = 0; index != iterations; ++index)
            REQUIRE(test.first[index] == index);
    }

    SECTION("resize") {
        grArray<int> test(alloc);

        int const size1 = 1000;
        int const size2 = 4000;
        int const size3 = 2000;

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
        grArray<int> test(alloc);

        int const capacity = 1000;

        test.reserve(capacity);
        REQUIRE(test.capacity() == capacity);

        test.reserve(0);
        REQUIRE(test.capacity() == capacity);

        test.shrink_to_fit();
        REQUIRE(test.capacity() == 0);
    }
}
