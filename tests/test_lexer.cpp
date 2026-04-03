/**
 * @file test_lexer.cpp
 * @brief Unit tests for the Lexer module
 * 
 * This file contains comprehensive tests for the lexical analyzer,
 * testing all token types, error handling, and edge cases.
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "lexer/lexer.h"
#include "lexer/token.h"

using namespace nova;

// ============================================================================
// Lexer Construction Tests
// ============================================================================

TEST_CASE("Lexer can be constructed with source code", "[lexer][construction]") {
    std::string source = "let x = 42;";
    REQUIRE_NOTHROW(Lexer(source, "test.nv"));
}

TEST_CASE("Lexer handles empty source", "[lexer][edge-case]") {
    Lexer lexer("", "empty.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);  // Only EOF token
    REQUIRE(tokens[0].type == TokenType::END_OF_FILE);
}

TEST_CASE("Lexer handles whitespace-only source", "[lexer][edge-case]") {
    Lexer lexer("   \t\n\r\n  ", "whitespace.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 1);  // Only EOF token
    REQUIRE(tokens[0].type == TokenType::END_OF_FILE);
}

// ============================================================================
// Keyword Tests
// ============================================================================

TEST_CASE("Lexer recognizes all keywords", "[lexer][keywords]") {
    std::string source = "fn let const if else while for return struct enum "
                        "true false void int float string bool char";
    Lexer lexer(source, "keywords.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 18);  // 17 keywords + EOF
    
    CHECK(tokens[0].type == TokenType::FN);
    CHECK(tokens[1].type == TokenType::LET);
    CHECK(tokens[2].type == TokenType::CONST);
    CHECK(tokens[3].type == TokenType::IF);
    CHECK(tokens[4].type == TokenType::ELSE);
    CHECK(tokens[5].type == TokenType::WHILE);
    CHECK(tokens[6].type == TokenType::FOR);
    CHECK(tokens[7].type == TokenType::RETURN);
    CHECK(tokens[8].type == TokenType::STRUCT);
    CHECK(tokens[9].type == TokenType::ENUM);
    CHECK(tokens[10].type == TokenType::TRUE_LITERAL);
    CHECK(tokens[11].type == TokenType::FALSE_LITERAL);
    CHECK(tokens[12].type == TokenType::VOID);
    CHECK(tokens[13].type == TokenType::INT);
    CHECK(tokens[14].type == TokenType::FLOAT);
    CHECK(tokens[15].type == TokenType::STRING);
    CHECK(tokens[16].type == TokenType::BOOL);
    CHECK(tokens[17].type == TokenType::END_OF_FILE);
}

// ============================================================================
// Identifier Tests
// ============================================================================

TEST_CASE("Lexer recognizes identifiers", "[lexer][identifiers]") {
    Lexer lexer("foo bar _baz qux123 _", "identifiers.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 6);  // 5 identifiers + EOF
    
    CHECK(tokens[0].type == TokenType::IDENTIFIER);
    CHECK(std::get<std::string>(tokens[0].value) == "foo");
    
    CHECK(tokens[1].type == TokenType::IDENTIFIER);
    CHECK(std::get<std::string>(tokens[1].value) == "bar");
    
    CHECK(tokens[2].type == TokenType::IDENTIFIER);
    CHECK(std::get<std::string>(tokens[2].value) == "_baz");
    
    CHECK(tokens[3].type == TokenType::IDENTIFIER);
    CHECK(std::get<std::string>(tokens[3].value) == "qux123");
    
    CHECK(tokens[4].type == TokenType::IDENTIFIER);
    CHECK(std::get<std::string>(tokens[4].value) == "_");
}

TEST_CASE("Lexer distinguishes keywords from identifiers", "[lexer][identifiers]") {
    Lexer lexer("fn fnNotKeyword", "test.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0].type == TokenType::FN);
    CHECK(tokens[1].type == TokenType::IDENTIFIER);
    CHECK(std::get<std::string>(tokens[1].value) == "fnNotKeyword");
}

// ============================================================================
// Literal Tests
// ============================================================================

TEST_CASE("Lexer recognizes integer literals", "[lexer][literals]") {
    Lexer lexer("42 0 123456789", "integers.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 4);  // 3 integers + EOF
    
    CHECK(tokens[0].type == TokenType::INTEGER_LITERAL);
    CHECK(std::get<int64_t>(tokens[0].value) == 42);
    
    CHECK(tokens[1].type == TokenType::INTEGER_LITERAL);
    CHECK(std::get<int64_t>(tokens[1].value) == 0);
    
    CHECK(tokens[2].type == TokenType::INTEGER_LITERAL);
    CHECK(std::get<int64_t>(tokens[2].value) == 123456789);
}

TEST_CASE("Lexer recognizes floating-point literals", "[lexer][literals]") {
    Lexer lexer("3.14 0.5 123.456", "floats.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 4);  // 3 floats + EOF
    
    CHECK(tokens[0].type == TokenType::FLOAT_LITERAL);
    CHECK(std::get<double>(tokens[0].value) == Approx(3.14));
    
    CHECK(tokens[1].type == TokenType::FLOAT_LITERAL);
    CHECK(std::get<double>(tokens[1].value) == Approx(0.5));
    
    CHECK(tokens[2].type == TokenType::FLOAT_LITERAL);
    CHECK(std::get<double>(tokens[2].value) == Approx(123.456));
}

TEST_CASE("Lexer recognizes string literals", "[lexer][literals]") {
    Lexer lexer("\"hello\" \"world\" \"\"", "strings.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 4);  // 3 strings + EOF
    
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(std::get<std::string>(tokens[0].value) == "hello");
    
    CHECK(tokens[1].type == TokenType::STRING_LITERAL);
    CHECK(std::get<std::string>(tokens[1].value) == "world");
    
    CHECK(tokens[2].type == TokenType::STRING_LITERAL);
    CHECK(std::get<std::string>(tokens[2].value) == "");
}

TEST_CASE("Lexer handles escape sequences in strings", "[lexer][literals][escape]") {
    Lexer lexer("\"hello\\nworld\" \"tab\\there\" \"quote\\\"\"", "escapes.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 4);  // 3 strings + EOF
    
    CHECK(tokens[0].type == TokenType::STRING_LITERAL);
    CHECK(std::get<std::string>(tokens[0].value) == "hello\nworld");
    
    CHECK(tokens[1].type == TokenType::STRING_LITERAL);
    CHECK(std::get<std::string>(tokens[1].value) == "tab\there");
    
    CHECK(tokens[2].type == TokenType::STRING_LITERAL);
    CHECK(std::get<std::string>(tokens[2].value) == "quote\"");
}

TEST_CASE("Lexer recognizes character literals", "[lexer][literals]") {
    Lexer lexer("'a' 'Z' '0' '\\n'", "chars.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 5);  // 4 chars + EOF
    
    CHECK(tokens[0].type == TokenType::CHAR_LITERAL);
    CHECK(std::get<char>(tokens[0].value) == 'a');
    
    CHECK(tokens[1].type == TokenType::CHAR_LITERAL);
    CHECK(std::get<char>(tokens[1].value) == 'Z');
    
    CHECK(tokens[2].type == TokenType::CHAR_LITERAL);
    CHECK(std::get<char>(tokens[2].value) == '0');
    
    CHECK(tokens[3].type == TokenType::CHAR_LITERAL);
    CHECK(std::get<char>(tokens[3].value) == '\n');
}

TEST_CASE("Lexer recognizes boolean literals", "[lexer][literals]") {
    Lexer lexer("true false", "bools.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 3);  // 2 bools + EOF
    
    CHECK(tokens[0].type == TokenType::TRUE_LITERAL);
    CHECK(tokens[1].type == TokenType::FALSE_LITERAL);
}

// ============================================================================
// Operator Tests
// ============================================================================

TEST_CASE("Lexer recognizes arithmetic operators", "[lexer][operators]") {
    Lexer lexer("+ - * / %", "arith.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 6);  // 5 operators + EOF
    
    CHECK(tokens[0].type == TokenType::PLUS);
    CHECK(tokens[1].type == TokenType::MINUS);
    CHECK(tokens[2].type == TokenType::STAR);
    CHECK(tokens[3].type == TokenType::SLASH);
    CHECK(tokens[4].type == TokenType::PERCENT);
}

TEST_CASE("Lexer recognizes comparison operators", "[lexer][operators]") {
    Lexer lexer("== != < <= > >=", "compare.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 7);  // 6 operators + EOF
    
    CHECK(tokens[0].type == TokenType::EQUAL_EQUAL);
    CHECK(tokens[1].type == TokenType::BANG_EQUAL);
    CHECK(tokens[2].type == TokenType::LESS);
    CHECK(tokens[3].type == TokenType::LESS_EQUAL);
    CHECK(tokens[4].type == TokenType::GREATER);
    CHECK(tokens[5].type == TokenType::GREATER_EQUAL);
}

TEST_CASE("Lexer recognizes logical operators", "[lexer][operators]") {
    Lexer lexer("&& || !", "logical.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 4);  // 3 operators + EOF
    
    CHECK(tokens[0].type == TokenType::AND);
    CHECK(tokens[1].type == TokenType::OR);
    CHECK(tokens[2].type == TokenType::BANG);
}

TEST_CASE("Lexer recognizes assignment operators", "[lexer][operators]") {
    Lexer lexer("= += -= *= /= %=", "assign.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 7);  // 6 operators + EOF
    
    CHECK(tokens[0].type == TokenType::EQUAL);
    CHECK(tokens[1].type == TokenType::PLUS_EQUAL);
    CHECK(tokens[2].type == TokenType::MINUS_EQUAL);
    CHECK(tokens[3].type == TokenType::STAR_EQUAL);
    CHECK(tokens[4].type == TokenType::SLASH_EQUAL);
    CHECK(tokens[5].type == TokenType::PERCENT_EQUAL);
}

TEST_CASE("Lexer recognizes bitwise operators", "[lexer][operators]") {
    Lexer lexer("& | ^ ~ << >>", "bitwise.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 7);  // 6 operators + EOF
    
    CHECK(tokens[0].type == TokenType::AMPERSAND);
    CHECK(tokens[1].type == TokenType::PIPE);
    CHECK(tokens[2].type == TokenType::CARET);
    CHECK(tokens[3].type == TokenType::TILDE);
    CHECK(tokens[4].type == TokenType::LEFT_SHIFT);
    CHECK(tokens[5].type == TokenType::RIGHT_SHIFT);
}

// ============================================================================
// Delimiter Tests
// ============================================================================

TEST_CASE("Lexer recognizes delimiters", "[lexer][delimiters]") {
    Lexer lexer("( ) { } [ ] ; , . : ->", "delims.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 12);  // 11 delimiters + EOF
    
    CHECK(tokens[0].type == TokenType::LEFT_PAREN);
    CHECK(tokens[1].type == TokenType::RIGHT_PAREN);
    CHECK(tokens[2].type == TokenType::LEFT_BRACE);
    CHECK(tokens[3].type == TokenType::RIGHT_BRACE);
    CHECK(tokens[4].type == TokenType::LEFT_BRACKET);
    CHECK(tokens[5].type == TokenType::RIGHT_BRACKET);
    CHECK(tokens[6].type == TokenType::SEMICOLON);
    CHECK(tokens[7].type == TokenType::COMMA);
    CHECK(tokens[8].type == TokenType::DOT);
    CHECK(tokens[9].type == TokenType::COLON);
    CHECK(tokens[10].type == TokenType::ARROW);
}

// ============================================================================
// Comment Tests
// ============================================================================

TEST_CASE("Lexer handles single-line comments", "[lexer][comments]") {
    Lexer lexer("let x = 42; // This is a comment\nlet y = 10;", "comment.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    // Should not include comment tokens in the output
    REQUIRE(tokens.size() >= 8);  // let, x, =, 42, ;, let, y, ...
    CHECK(tokens[0].type == TokenType::LET);
    CHECK(tokens[4].type == TokenType::SEMICOLON);
    CHECK(tokens[5].type == TokenType::LET);
}

TEST_CASE("Lexer handles multi-line comments", "[lexer][comments]") {
    Lexer lexer("let x = 42; /* This is a\nmulti-line comment */ let y = 10;", "comment.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    // Should skip the multi-line comment
    REQUIRE(tokens.size() >= 8);
    CHECK(tokens[0].type == TokenType::LET);
    CHECK(tokens[4].type == TokenType::SEMICOLON);
    CHECK(tokens[5].type == TokenType::LET);
}

TEST_CASE("Lexer handles nested comments", "[lexer][comments]") {
    Lexer lexer("let x = 42; /* Outer /* nested */ still outer */ let y = 10;", "nested.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() >= 8);
    CHECK(tokens[0].type == TokenType::LET);
    CHECK(tokens[5].type == TokenType::LET);
}

// ============================================================================
// Location Tracking Tests
// ============================================================================

TEST_CASE("Lexer tracks line numbers correctly", "[lexer][location]") {
    Lexer lexer("let x = 42;\nlet y = 10;\nlet z = 20;", "lines.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    // Find tokens on each line
    int line1Count = 0, line2Count = 0, line3Count = 0;
    for (const auto& token : tokens) {
        if (token.line == 1) line1Count++;
        else if (token.line == 2) line2Count++;
        else if (token.line == 3) line3Count++;
    }
    
    CHECK(line1Count > 0);
    CHECK(line2Count > 0);
    CHECK(line3Count > 0);
}

TEST_CASE("Lexer tracks column numbers correctly", "[lexer][location]") {
    Lexer lexer("let x = 42;", "columns.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() >= 5);
    
    // Check column positions
    CHECK(tokens[0].column == 1);   // 'let'
    CHECK(tokens[1].column == 5);   // 'x'
    CHECK(tokens[2].column == 7);   // '='
    CHECK(tokens[3].column == 9);   // '42'
    CHECK(tokens[4].column == 11);  // ';'
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_CASE("Lexer throws error on unterminated string", "[lexer][errors]") {
    Lexer lexer("\"unterminated string", "error.nv");
    
    REQUIRE_THROWS_AS(lexer.tokenize(), LexerError);
}

TEST_CASE("Lexer throws error on unterminated character literal", "[lexer][errors]") {
    Lexer lexer("'a", "error.nv");
    
    REQUIRE_THROWS_AS(lexer.tokenize(), LexerError);
}

TEST_CASE("Lexer throws error on invalid escape sequence", "[lexer][errors]") {
    Lexer lexer("\"invalid\\escape\"", "error.nv");
    
    REQUIRE_THROWS_AS(lexer.tokenize(), LexerError);
}

TEST_CASE("Lexer throws error on unexpected character", "[lexer][errors]") {
    Lexer lexer("let x = @;", "error.nv");
    
    REQUIRE_THROWS_AS(lexer.tokenize(), LexerError);
}

// ============================================================================
// Complex Code Tests
// ============================================================================

TEST_CASE("Lexer tokenizes function declaration", "[lexer][integration]") {
    Lexer lexer("fn add(a: int, b: int) -> int { return a + b; }", "func.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 18);  // Count all tokens
    
    CHECK(tokens[0].type == TokenType::FN);
    CHECK(tokens[1].type == TokenType::IDENTIFIER);
    CHECK(tokens[2].type == TokenType::LEFT_PAREN);
    CHECK(tokens[3].type == TokenType::IDENTIFIER);
    CHECK(tokens[4].type == TokenType::COLON);
    CHECK(tokens[5].type == TokenType::INT);
    // ... etc
}

TEST_CASE("Lexer tokenizes if-else statement", "[lexer][integration]") {
    Lexer lexer("if (x > 0) { return x; } else { return -x; }", "ifelse.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() >= 15);
    
    CHECK(tokens[0].type == TokenType::IF);
    CHECK(tokens[1].type == TokenType::LEFT_PAREN);
    CHECK(tokens[2].type == TokenType::IDENTIFIER);
    CHECK(tokens[3].type == TokenType::GREATER);
    CHECK(tokens[4].type == TokenType::INTEGER_LITERAL);
}

TEST_CASE("Lexer tokenizes complex expression", "[lexer][integration]") {
    Lexer lexer("result = (a + b) * (c - d) / e;", "expr.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() >= 15);
    
    CHECK(tokens[0].type == TokenType::IDENTIFIER);
    CHECK(tokens[1].type == TokenType::EQUAL);
    CHECK(tokens[2].type == TokenType::LEFT_PAREN);
    CHECK(tokens[3].type == TokenType::IDENTIFIER);
    CHECK(tokens[4].type == TokenType::PLUS);
}
