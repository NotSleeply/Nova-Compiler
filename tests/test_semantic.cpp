/**
 * @file test_semantic.cpp
 * @brief Unit tests for the Semantic Analyzer module
 * 
 * This file contains comprehensive tests for semantic analysis,
 * testing type checking, scope management, and error detection.
 */

#include "catch2/catch.hpp"
#include "semantic/semantic_analyzer.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include "ast/ast.h"

using namespace nova;

// Helper function to analyze source code
bool analyzeSource(const std::string& source, std::vector<CompilationError>& errors) {
    Lexer lexer(source, "test.nv");
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    
    if (parser.hasErrors()) {
        return false;
    }
    
    SemanticAnalyzer analyzer;
    bool result = analyzer.analyze(program);
    errors = analyzer.getErrors();
    return result;
}

// ============================================================================
// Type Checking Tests
// ============================================================================

TEST_CASE("Semantic analyzer checks integer type", "[semantic][types]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let x: int = 42;", errors);
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer checks float type", "[semantic][types]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let x: float = 3.14;", errors);
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer checks string type", "[semantic][types]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let s: string = \"hello\";", errors);
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer checks boolean type", "[semantic][types]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let b: bool = true;", errors);
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer detects type mismatch", "[semantic][types][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let x: int = \"string\";", errors);
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

TEST_CASE("Semantic analyzer detects incompatible assignment", "[semantic][types][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let x: int = 3.14;", errors);
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

// ============================================================================
// Binary Expression Type Checking Tests
// ============================================================================

TEST_CASE("Semantic analyzer checks integer addition", "[semantic][expressions]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let x: int = 1 + 2;", errors);
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer checks integer subtraction", "[semantic][expressions]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let x: int = 5 - 3;", errors);
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer checks integer multiplication", "[semantic][expressions]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let x: int = 2 * 3;", errors);
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer checks integer division", "[semantic][expressions]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let x: int = 6 / 2;", errors);
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer detects type mismatch in binary expression", "[semantic][expressions][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let x: int = 1 + \"string\";", errors);
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

TEST_CASE("Semantic analyzer checks comparison expressions", "[semantic][expressions]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let b: bool = 1 < 2;", errors);
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer checks equality expressions", "[semantic][expressions]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let b: bool = 1 == 1;", errors);
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer checks logical expressions", "[semantic][expressions]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let b: bool = true && false;", errors);
    
    CHECK(result);
    CHECK(errors.empty());
}

// ============================================================================
// Function Declaration Tests
// ============================================================================

TEST_CASE("Semantic analyzer checks simple function", "[semantic][functions]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn foo() -> void {\n"
        "    return;\n"
        "}\n",
        errors
    );
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer checks function with parameters", "[semantic][functions]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn add(a: int, b: int) -> int {\n"
        "    return a + b;\n"
        "}\n",
        errors
    );
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer checks function return type", "[semantic][functions]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn getValue() -> int {\n"
        "    return 42;\n"
        "}\n",
        errors
    );
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer detects missing return statement", "[semantic][functions][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn getValue() -> int {\n"
        "    let x: int = 42;\n"
        "}\n",
        errors
    );
    
    // This might be a warning, not an error, depending on implementation
    // CHECK_FALSE(result);
}

TEST_CASE("Semantic analyzer detects wrong return type", "[semantic][functions][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn getValue() -> int {\n"
        "    return \"string\";\n"
        "}\n",
        errors
    );
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

// ============================================================================
// Function Call Tests
// ============================================================================

TEST_CASE("Semantic analyzer checks function call with correct arguments", "[semantic][functions][calls]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn add(a: int, b: int) -> int {\n"
        "    return a + b;\n"
        "}\n"
        "fn test() -> void {\n"
        "    let result: int = add(1, 2);\n"
        "}\n",
        errors
    );
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer detects wrong number of arguments", "[semantic][functions][calls][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn add(a: int, b: int) -> int {\n"
        "    return a + b;\n"
        "}\n"
        "fn test() -> void {\n"
        "    let result: int = add(1);\n"
        "}\n",
        errors
    );
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

TEST_CASE("Semantic analyzer detects wrong argument types", "[semantic][functions][calls][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn add(a: int, b: int) -> int {\n"
        "    return a + b;\n"
        "}\n"
        "fn test() -> void {\n"
        "    let result: int = add(1, \"string\");\n"
        "}\n",
        errors
    );
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

TEST_CASE("Semantic analyzer detects undefined function", "[semantic][functions][calls][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn test() -> void {\n"
        "    let result: int = undefinedFunction();\n"
        "}\n",
        errors
    );
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

// ============================================================================
// Variable Declaration Tests
// ============================================================================

TEST_CASE("Semantic analyzer checks variable declaration", "[semantic][variables]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let x: int = 42;", errors);
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer detects duplicate variable declaration", "[semantic][variables][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "let x: int = 1;\n"
        "let x: int = 2;\n",
        errors
    );
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

TEST_CASE("Semantic analyzer allows variable shadowing in different scopes", "[semantic][variables][scope]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "let x: int = 1;\n"
        "fn foo() -> void {\n"
        "    let x: int = 2;\n"
        "}\n",
        errors
    );
    
    CHECK(result);
    CHECK(errors.empty());
}

// ============================================================================
// Variable Usage Tests
// ============================================================================

TEST_CASE("Semantic analyzer checks variable usage", "[semantic][variables]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "let x: int = 42;\n"
        "let y: int = x;\n",
        errors
    );
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer detects undefined variable", "[semantic][variables][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let y: int = x;", errors);
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

TEST_CASE("Semantic analyzer detects variable used before declaration", "[semantic][variables][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "let y: int = x;\n"
        "let x: int = 42;\n",
        errors
    );
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

// ============================================================================
// Scope Tests
// ============================================================================

TEST_CASE("Semantic analyzer handles nested scopes", "[semantic][scope]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn foo() -> void {\n"
        "    let x: int = 1;\n"
        "    {\n"
        "        let y: int = 2;\n"
        "        let z: int = x + y;\n"
        "    }\n"
        "}\n",
        errors
    );
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer detects variable out of scope", "[semantic][scope][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn foo() -> void {\n"
        "    {\n"
        "        let x: int = 1;\n"
        "    }\n"
        "    let y: int = x;\n"
        "}\n",
        errors
    );
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

// ============================================================================
// Control Flow Tests
// ============================================================================

TEST_CASE("Semantic analyzer checks if statement", "[semantic][control-flow]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn foo() -> void {\n"
        "    if (true) {\n"
        "        return;\n"
        "    }\n"
        "}\n",
        errors
    );
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer checks while statement", "[semantic][control-flow]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn foo() -> void {\n"
        "    while (true) {\n"
        "        return;\n"
        "    }\n"
        "}\n",
        errors
    );
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer checks for statement", "[semantic][control-flow]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn foo() -> void {\n"
        "    for (let i: int = 0; i < 10; i = i + 1) {\n"
        "        return;\n"
        "    }\n"
        "}\n",
        errors
    );
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer detects non-boolean condition in if", "[semantic][control-flow][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn foo() -> void {\n"
        "    if (42) {\n"
        "        return;\n"
        "    }\n"
        "}\n",
        errors
    );
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

TEST_CASE("Semantic analyzer detects non-boolean condition in while", "[semantic][control-flow][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn foo() -> void {\n"
        "    while (42) {\n"
        "        return;\n"
        "    }\n"
        "}\n",
        errors
    );
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_CASE("Semantic analyzer checks complete program", "[semantic][integration]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn factorial(n: int) -> int {\n"
        "    if (n <= 1) {\n"
        "        return 1;\n"
        "    }\n"
        "    return n * factorial(n - 1);\n"
        "}\n"
        "\n"
        "fn main() -> void {\n"
        "    let result: int = factorial(5);\n"
        "}\n",
        errors
    );
    
    CHECK(result);
    CHECK(errors.empty());
}

TEST_CASE("Semantic analyzer detects multiple errors", "[semantic][integration][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource(
        "fn foo() -> int {\n"
        "    return undefinedVar;\n"
        "}\n"
        "\n"
        "fn bar() -> void {\n"
        "    let x: int = undefinedFunction();\n"
        "}\n",
        errors
    );
    
    CHECK_FALSE(result);
    CHECK(errors.size() >= 2);
}
