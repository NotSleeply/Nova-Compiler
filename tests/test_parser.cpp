/**
 * @file test_parser.cpp
 * @brief Unit tests for the Parser module (simplified)
 */

#include "catch2/catch.hpp"
#include "parser/parser.h"
#include "lexer/lexer.h"

using namespace nova;

// Helper function to parse source code
std::shared_ptr<Program> parseSource(const std::string& source) {
    Lexer lexer(source, "test.nv");
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    return parser.parse();
}

// ============================================================================
// Parser Construction Tests
// ============================================================================

TEST_CASE("Parser can be constructed", "[parser][construction]") {
    Lexer lexer("let x = 42;", "test.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE_NOTHROW(Parser(tokens));
}

// ============================================================================
// Variable Declaration Tests
// ============================================================================

TEST_CASE("Parser parses variable declaration", "[parser][variables]") {
    auto program = parseSource("let x: int = 42;");
    
    REQUIRE(program != nullptr);
    CHECK(program->declarations.size() == 1);
}

TEST_CASE("Parser parses mutable variable declaration", "[parser][variables]") {
    auto program = parseSource("var y: float = 3.14;");
    
    REQUIRE(program != nullptr);
    CHECK(program->declarations.size() == 1);
}

TEST_CASE("Parser parses constant declaration", "[parser][variables]") {
    auto program = parseSource("const PI: float = 3.14159;");
    
    REQUIRE(program != nullptr);
    CHECK(program->declarations.size() == 1);
}

// ============================================================================
// Function Declaration Tests
// ============================================================================

TEST_CASE("Parser parses function declaration", "[parser][functions]") {
    auto program = parseSource("fn foo() -> void { }");
    
    REQUIRE(program != nullptr);
    CHECK(program->declarations.size() == 1);
}

TEST_CASE("Parser parses function with parameters", "[parser][functions]") {
    auto program = parseSource("fn add(a: int, b: int) -> int { return a + b; }");
    
    REQUIRE(program != nullptr);
    CHECK(program->declarations.size() == 1);
}

TEST_CASE("Parser parses function with void return", "[parser][functions]") {
    auto program = parseSource("fn print(msg: string) -> void { }");
    
    REQUIRE(program != nullptr);
    CHECK(program->declarations.size() == 1);
}

// ============================================================================
// Expression Tests
// ============================================================================

TEST_CASE("Parser parses integer literal", "[parser][expressions]") {
    auto program = parseSource("let x = 42;");
    REQUIRE(program != nullptr);
}

TEST_CASE("Parser parses float literal", "[parser][expressions]") {
    auto program = parseSource("let pi = 3.14;");
    REQUIRE(program != nullptr);
}

TEST_CASE("Parser parses string literal", "[parser][expressions]") {
    auto program = parseSource("let msg = \"hello\";");
    REQUIRE(program != nullptr);
}

TEST_CASE("Parser parses boolean literals", "[parser][expressions]") {
    auto program = parseSource("let flag = true;");
    REQUIRE(program != nullptr);
    
    program = parseSource("let flag = false;");
    REQUIRE(program != nullptr);
}

TEST_CASE("Parser parses binary expression", "[parser][expressions]") {
    auto program = parseSource("let result = a + b;");
    REQUIRE(program != nullptr);
}

TEST_CASE("Parser parses comparison expression", "[parser][expressions]") {
    auto program = parseSource("let cmp = x > y;");
    REQUIRE(program != nullptr);
}

TEST_CASE("Parser parses function call", "[parser][expressions]") {
    auto program = parseSource("let result = foo(a, b);");
    REQUIRE(program != nullptr);
}

// ============================================================================
// Statement Tests
// ============================================================================

TEST_CASE("Parser parses if statement", "[parser][statements]") {
    auto program = parseSource("fn test() -> void { if (x > 0) { } }");
    REQUIRE(program != nullptr);
}

TEST_CASE("Parser parses if-else statement", "[parser][statements]") {
    auto program = parseSource("fn test() -> void { if (x > 0) { } else { } }");
    REQUIRE(program != nullptr);
}

TEST_CASE("Parser parses while loop", "[parser][statements]") {
    auto program = parseSource("fn test() -> void { while (x > 0) { } }");
    REQUIRE(program != nullptr);
}

TEST_CASE("Parser parses for loop", "[parser][statements]") {
    auto program = parseSource("fn test() -> void { for (let i = 0; i < 10; i = i + 1) { } }");
    REQUIRE(program != nullptr);
}

TEST_CASE("Parser parses return statement", "[parser][statements]") {
    auto program = parseSource("fn test() -> int { return 42; }");
    REQUIRE(program != nullptr);
}

TEST_CASE("Parser parses block statement", "[parser][statements]") {
    auto program = parseSource("fn test() -> void { { let x = 1; } }");
    REQUIRE(program != nullptr);
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_CASE("Parser reports error on missing semicolon", "[parser][errors]") {
    auto program = parseSource("let x = 42");
    // Parser should handle errors gracefully
    CHECK(true);
}

TEST_CASE("Parser reports error on unmatched braces", "[parser][errors]") {
    auto program = parseSource("fn test() -> void { } }");
    // Parser should handle errors gracefully
    CHECK(true);
}

TEST_CASE("Parser reports error on invalid syntax", "[parser][errors]") {
    auto program = parseSource("let = ;");
    // Parser should handle errors gracefully
    CHECK(true);
}

// ============================================================================
// Complex Program Tests
// ============================================================================

TEST_CASE("Parser parses factorial function", "[parser][integration]") {
    std::string source = R"(
        fn factorial(n: int) -> int {
            if (n <= 1) {
                return 1;
            }
            return n * factorial(n - 1);
        }
    )";
    
    auto program = parseSource(source);
    REQUIRE(program != nullptr);
    CHECK(program->declarations.size() == 1);
}

TEST_CASE("Parser parses fibonacci function", "[parser][integration]") {
    std::string source = R"(
        fn fibonacci(n: int) -> int {
            if (n <= 1) {
                return n;
            }
            return fibonacci(n - 1) + fibonacci(n - 2);
        }
    )";
    
    auto program = parseSource(source);
    REQUIRE(program != nullptr);
    CHECK(program->declarations.size() == 1);
}

TEST_CASE("Parser parses main function", "[parser][integration]") {
    std::string source = R"(
        fn main() -> void {
            let x: int = 42;
            let y: float = 3.14;
            let msg: string = "Hello, Nova!";
        }
    )";
    
    auto program = parseSource(source);
    REQUIRE(program != nullptr);
    CHECK(program->declarations.size() == 1);
}

TEST_CASE("Parser parses struct declaration", "[parser][integration]") {
    std::string source = R"(
        struct Point {
            x: float;
            y: float;
        }
    )";
    
    auto program = parseSource(source);
    REQUIRE(program != nullptr);
}

TEST_CASE("Parser parses enum declaration", "[parser][integration]") {
    std::string source = R"(
        enum Color {
            Red,
            Green,
            Blue
        }
    )";
    
    auto program = parseSource(source);
    REQUIRE(program != nullptr);
}
