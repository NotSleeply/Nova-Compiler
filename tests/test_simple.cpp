/**
 * @file test_simple.cpp
 * @brief Simple test to verify the test framework works
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

TEST_CASE("Simple test - framework verification", "[test]") {
    REQUIRE(1 == 1);
    REQUIRE(true);
    CHECK(2 + 2 == 4);
}

TEST_CASE("String test", "[test]") {
    std::string str = "hello";
    REQUIRE(str == "hello");
    REQUIRE(str.length() == 5);
}
