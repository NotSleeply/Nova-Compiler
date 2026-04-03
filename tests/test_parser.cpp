/**
 * @file test_parser.cpp
 * @brief Unit tests for the Parser module
 * 
 * This file contains comprehensive tests for the parser,
 * testing AST generation, error recovery, and edge cases.
 */

#include "catch2/catch.hpp"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include "ast/ast.h"

using namespace nova;

// Helper function to parse source code
std::shared_ptr<Program> parseSource(const std::string& source, const std::string& filename = "test.nv") {
    Lexer lexer(source, filename);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    return parser.parse();
}

// ============================================================================
// Parser Construction Tests
// ============================================================================

TEST_CASE("Parser can be constructed with tokens", "[parser][construction]") {
    Lexer lexer("let x = 42;", "test.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE_NOTHROW(Parser(tokens));
}

TEST_CASE("Parser handles empty token list", "[parser][edge-case]") {
    std::vector<Token> tokens;
    Parser parser(tokens);
    
    auto program = parser.parse();
    REQUIRE(program != nullptr);
    REQUIRE(program->declarations.empty());
}

// ============================================================================
// Function Declaration Tests
// ============================================================================

TEST_CASE("Parser parses simple function declaration", "[parser][functions]") {
    auto program = parseSource("fn foo() -> void { }");
    
    REQUIRE(program->declarations.size() == 1);
    
    auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    REQUIRE(funcDecl != nullptr);
    CHECK(funcDecl->name == "foo");
    CHECK(funcDecl->params.empty());
    CHECK(funcDecl->returnType != nullptr);
}

TEST_CASE("Parser parses function with parameters", "[parser][functions]") {
    auto program = parseSource("fn add(a: int, b: int) -> int { return a + b; }");
    
    REQUIRE(program->declarations.size() == 1);
    
    auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    REQUIRE(funcDecl != nullptr);
    CHECK(funcDecl->name == "add");
    CHECK(funcDecl->params.size() == 2);
    CHECK(funcDecl->params[0]->name == "a");
    CHECK(funcDecl->params[1]->name == "b");
}

TEST_CASE("Parser parses function with no return type", "[parser][functions]") {
    auto program = parseSource("fn doSomething() { }");
    
    REQUIRE(program->declarations.size() == 1);
    
    auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    REQUIRE(funcDecl != nullptr);
    CHECK(funcDecl->name == "doSomething");
}

TEST_CASE("Parser parses multiple functions", "[parser][functions]") {
    auto program = parseSource(
        "fn foo() -> void { }\n"
        "fn bar() -> void { }\n"
        "fn baz() -> void { }"
    );
    
    REQUIRE(program->declarations.size() == 3);
    
    auto func1 = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    auto func2 = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[1]);
    auto func3 = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[2]);
    
    REQUIRE(func1 != nullptr);
    REQUIRE(func2 != nullptr);
    REQUIRE(func3 != nullptr);
    
    CHECK(func1->name == "foo");
    CHECK(func2->name == "bar");
    CHECK(func3->name == "baz");
}

// ============================================================================
// Variable Declaration Tests
// ============================================================================

TEST_CASE("Parser parses variable declaration with initializer", "[parser][variables]") {
    auto program = parseSource("let x: int = 42;");
    
    REQUIRE(program->declarations.size() == 1);
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    CHECK(varDecl->name == "x");
    CHECK(varDecl->initializer != nullptr);
}

TEST_CASE("Parser parses variable declaration without initializer", "[parser][variables]") {
    auto program = parseSource("let x: int;");
    
    REQUIRE(program->declarations.size() == 1);
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    CHECK(varDecl->name == "x");
    CHECK(varDecl->initializer == nullptr);
}

TEST_CASE("Parser parses const declaration", "[parser][variables]") {
    auto program = parseSource("const PI: float = 3.14;");
    
    REQUIRE(program->declarations.size() == 1);
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    CHECK(varDecl->name == "PI");
    CHECK(varDecl->isConst);
    CHECK(varDecl->initializer != nullptr);
}

TEST_CASE("Parser parses multiple variable declarations", "[parser][variables]") {
    auto program = parseSource(
        "let x: int = 1;\n"
        "let y: int = 2;\n"
        "let z: int = 3;"
    );
    
    REQUIRE(program->declarations.size() == 3);
    
    for (int i = 0; i < 3; i++) {
        auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[i]);
        REQUIRE(varDecl != nullptr);
    }
}

// ============================================================================
// Expression Tests
// ============================================================================

TEST_CASE("Parser parses integer literal", "[parser][expressions][literals]") {
    auto program = parseSource("let x: int = 42;");
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    
    auto literal = std::dynamic_pointer_cast<IntegerLiteral>(varDecl->initializer);
    REQUIRE(literal != nullptr);
    CHECK(literal->value == 42);
}

TEST_CASE("Parser parses float literal", "[parser][expressions][literals]") {
    auto program = parseSource("let x: float = 3.14;");
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    
    auto literal = std::dynamic_pointer_cast<FloatLiteral>(varDecl->initializer);
    REQUIRE(literal != nullptr);
    CHECK(literal->value == Approx(3.14));
}

TEST_CASE("Parser parses string literal", "[parser][expressions][literals]") {
    auto program = parseSource("let s: string = \"hello\";");
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    
    auto literal = std::dynamic_pointer_cast<StringLiteral>(varDecl->initializer);
    REQUIRE(literal != nullptr);
    CHECK(literal->value == "hello");
}

TEST_CASE("Parser parses boolean literals", "[parser][expressions][literals]") {
    auto program1 = parseSource("let b: bool = true;");
    auto varDecl1 = std::dynamic_pointer_cast<VarDecl>(program1->declarations[0]);
    REQUIRE(varDecl1 != nullptr);
    auto boolLit1 = std::dynamic_pointer_cast<BooleanLiteral>(varDecl1->initializer);
    REQUIRE(boolLit1 != nullptr);
    CHECK(boolLit1->value == true);
    
    auto program2 = parseSource("let b: bool = false;");
    auto varDecl2 = std::dynamic_pointer_cast<VarDecl>(program2->declarations[0]);
    REQUIRE(varDecl2 != nullptr);
    auto boolLit2 = std::dynamic_pointer_cast<BooleanLiteral>(varDecl2->initializer);
    REQUIRE(boolLit2 != nullptr);
    CHECK(boolLit2->value == false);
}

TEST_CASE("Parser parses binary expressions with correct precedence", "[parser][expressions][operators]") {
    auto program = parseSource("let result: int = 1 + 2 * 3;");
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    
    // Should parse as: 1 + (2 * 3)
    auto addExpr = std::dynamic_pointer_cast<BinaryExpr>(varDecl->initializer);
    REQUIRE(addExpr != nullptr);
    CHECK(addExpr->op == BinaryOp::ADD);
    
    auto leftLit = std::dynamic_pointer_cast<IntegerLiteral>(addExpr->left);
    REQUIRE(leftLit != nullptr);
    CHECK(leftLit->value == 1);
    
    auto rightMul = std::dynamic_pointer_cast<BinaryExpr>(addExpr->right);
    REQUIRE(rightMul != nullptr);
    CHECK(rightMul->op == BinaryOp::MUL);
}

TEST_CASE("Parser parses parenthesized expressions", "[parser][expressions]") {
    auto program = parseSource("let result: int = (1 + 2) * 3;");
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    
    // Should parse as: (1 + 2) * 3
    auto mulExpr = std::dynamic_pointer_cast<BinaryExpr>(varDecl->initializer);
    REQUIRE(mulExpr != nullptr);
    CHECK(mulExpr->op == BinaryOp::MUL);
    
    auto leftAdd = std::dynamic_pointer_cast<BinaryExpr>(mulExpr->left);
    REQUIRE(leftAdd != nullptr);
    CHECK(leftAdd->op == BinaryOp::ADD);
}

TEST_CASE("Parser parses unary expressions", "[parser][expressions][operators]") {
    auto program = parseSource("let x: int = -42;");
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    
    auto unaryExpr = std::dynamic_pointer_cast<UnaryExpr>(varDecl->initializer);
    REQUIRE(unaryExpr != nullptr);
    CHECK(unaryExpr->op == UnaryOp::NEGATE);
    
    auto operand = std::dynamic_pointer_cast<IntegerLiteral>(unaryExpr->operand);
    REQUIRE(operand != nullptr);
    CHECK(operand->value == 42);
}

TEST_CASE("Parser parses function call expression", "[parser][expressions][calls]") {
    auto program = parseSource("let result: int = foo(1, 2, 3);");
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    
    auto callExpr = std::dynamic_pointer_cast<CallExpr>(varDecl->initializer);
    REQUIRE(callExpr != nullptr);
    CHECK(callExpr->functionName == "foo");
    CHECK(callExpr->arguments.size() == 3);
}

TEST_CASE("Parser parses nested function calls", "[parser][expressions][calls]") {
    auto program = parseSource("let result: int = outer(inner(1), 2);");
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    
    auto outerCall = std::dynamic_pointer_cast<CallExpr>(varDecl->initializer);
    REQUIRE(outerCall != nullptr);
    CHECK(outerCall->functionName == "outer");
    CHECK(outerCall->arguments.size() == 2);
    
    auto innerCall = std::dynamic_pointer_cast<CallExpr>(outerCall->arguments[0]);
    REQUIRE(innerCall != nullptr);
    CHECK(innerCall->functionName == "inner");
}

// ============================================================================
// Statement Tests
// ============================================================================

TEST_CASE("Parser parses return statement", "[parser][statements]") {
    auto program = parseSource("fn foo() -> int { return 42; }");
    
    auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    REQUIRE(funcDecl != nullptr);
    REQUIRE(funcDecl->body != nullptr);
    REQUIRE(funcDecl->body->statements.size() == 1);
    
    auto returnStmt = std::dynamic_pointer_cast<ReturnStmt>(funcDecl->body->statements[0]);
    REQUIRE(returnStmt != nullptr);
    CHECK(returnStmt->value != nullptr);
}

TEST_CASE("Parser parses return statement without value", "[parser][statements]") {
    auto program = parseSource("fn foo() -> void { return; }");
    
    auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    REQUIRE(funcDecl != nullptr);
    
    auto returnStmt = std::dynamic_pointer_cast<ReturnStmt>(funcDecl->body->statements[0]);
    REQUIRE(returnStmt != nullptr);
    CHECK(returnStmt->value == nullptr);
}

TEST_CASE("Parser parses expression statement", "[parser][statements]") {
    auto program = parseSource("fn foo() -> void { bar(42); }");
    
    auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    REQUIRE(funcDecl != nullptr);
    
    auto exprStmt = std::dynamic_pointer_cast<ExpressionStmt>(funcDecl->body->statements[0]);
    REQUIRE(exprStmt != nullptr);
    CHECK(exprStmt->expression != nullptr);
}

TEST_CASE("Parser parses if statement", "[parser][statements][control-flow]") {
    auto program = parseSource("fn foo() -> void { if (true) { return; } }");
    
    auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    REQUIRE(funcDecl != nullptr);
    
    auto ifStmt = std::dynamic_pointer_cast<IfStmt>(funcDecl->body->statements[0]);
    REQUIRE(ifStmt != nullptr);
    CHECK(ifStmt->condition != nullptr);
    CHECK(ifStmt->thenBranch != nullptr);
    CHECK(ifStmt->elseBranch == nullptr);
}

TEST_CASE("Parser parses if-else statement", "[parser][statements][control-flow]") {
    auto program = parseSource("fn foo() -> void { if (true) { return; } else { return; } }");
    
    auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    REQUIRE(funcDecl != nullptr);
    
    auto ifStmt = std::dynamic_pointer_cast<IfStmt>(funcDecl->body->statements[0]);
    REQUIRE(ifStmt != nullptr);
    CHECK(ifStmt->condition != nullptr);
    CHECK(ifStmt->thenBranch != nullptr);
    CHECK(ifStmt->elseBranch != nullptr);
}

TEST_CASE("Parser parses while statement", "[parser][statements][control-flow]") {
    auto program = parseSource("fn foo() -> void { while (true) { break; } }");
    
    auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    REQUIRE(funcDecl != nullptr);
    
    auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(funcDecl->body->statements[0]);
    REQUIRE(whileStmt != nullptr);
    CHECK(whileStmt->condition != nullptr);
    CHECK(whileStmt->body != nullptr);
}

TEST_CASE("Parser parses for statement", "[parser][statements][control-flow]") {
    auto program = parseSource("fn foo() -> void { for (let i: int = 0; i < 10; i = i + 1) { } }");
    
    auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    REQUIRE(funcDecl != nullptr);
    
    auto forStmt = std::dynamic_pointer_cast<ForStmt>(funcDecl->body->statements[0]);
    REQUIRE(forStmt != nullptr);
    CHECK(forStmt->initializer != nullptr);
    CHECK(forStmt->condition != nullptr);
    CHECK(forStmt->update != nullptr);
    CHECK(forStmt->body != nullptr);
}

TEST_CASE("Parser parses block statement", "[parser][statements]") {
    auto program = parseSource("fn foo() -> void { { let x: int = 42; } }");
    
    auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    REQUIRE(funcDecl != nullptr);
    
    auto blockStmt = std::dynamic_pointer_cast<BlockStmt>(funcDecl->body->statements[0]);
    REQUIRE(blockStmt != nullptr);
    CHECK(blockStmt->statements.size() == 1);
}

// ============================================================================
// Type Tests
// ============================================================================

TEST_CASE("Parser parses basic types", "[parser][types]") {
    auto program = parseSource(
        "let a: int = 0;\n"
        "let b: float = 0.0;\n"
        "let c: bool = true;\n"
        "let d: string = \"\";\n"
        "let e: void;"
    );
    
    REQUIRE(program->declarations.size() == 5);
}

TEST_CASE("Parser parses array types", "[parser][types]") {
    auto program = parseSource("let arr: int[] = [1, 2, 3];");
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    
    auto arrayType = std::dynamic_pointer_cast<ArrayType>(varDecl->type);
    REQUIRE(arrayType != nullptr);
}

TEST_CASE("Parser parses named types", "[parser][types]") {
    auto program = parseSource("let obj: MyClass = MyClass();");
    
    auto varDecl = std::dynamic_pointer_cast<VarDecl>(program->declarations[0]);
    REQUIRE(varDecl != nullptr);
    
    auto namedType = std::dynamic_pointer_cast<NamedType>(varDecl->type);
    REQUIRE(namedType != nullptr);
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_CASE("Parser handles missing semicolon", "[parser][errors]") {
    Lexer lexer("let x: int = 42", "error.nv");
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    
    REQUIRE_THROWS_AS(parser.parse(), ParseError);
}

TEST_CASE("Parser handles missing closing brace", "[parser][errors]") {
    Lexer lexer("fn foo() -> void {", "error.nv");
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    
    REQUIRE_THROWS_AS(parser.parse(), ParseError);
}

TEST_CASE("Parser handles invalid expression", "[parser][errors]") {
    Lexer lexer("let x: int = +;", "error.nv");
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    
    REQUIRE_THROWS_AS(parser.parse(), ParseError);
}

TEST_CASE("Parser handles multiple errors", "[parser][errors]") {
    Lexer lexer("fn foo( -> void { let x = }", "error.nv");
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    
    REQUIRE_THROWS_AS(parser.parse(), ParseError);
    CHECK(parser.hasErrors());
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST_CASE("Parser parses complete program", "[parser][integration]") {
    auto program = parseSource(
        "fn add(a: int, b: int) -> int {\n"
        "    return a + b;\n"
        "}\n"
        "\n"
        "fn main() -> void {\n"
        "    let x: int = add(10, 20);\n"
        "    if (x > 0) {\n"
        "        return;\n"
        "    }\n"
        "}\n"
    );
    
    REQUIRE(program->declarations.size() == 2);
    
    auto addFunc = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    REQUIRE(addFunc != nullptr);
    CHECK(addFunc->name == "add");
    
    auto mainFunc = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[1]);
    REQUIRE(mainFunc != nullptr);
    CHECK(mainFunc->name == "main");
}
