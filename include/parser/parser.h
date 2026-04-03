/**
 * @file parser.h
 * @brief Parser for Nova Compiler
 * @author Nova Compiler Team
 * @version 0.1.0
 * @date 2026-04-02
 * 
 * The parser transforms a stream of tokens into an Abstract Syntax Tree (AST).
 * This is the second phase of the compilation process.
 * 
 * Grammar Overview:
 *   program     → declaration* EOF
 *   declaration → varDecl | fnDecl | structDecl | stmt
 *   stmt        → exprStmt | ifStmt | whileStmt | forStmt | returnStmt | block
 *   expr        → assignment
 *   assignment  → IDENTIFIER "=" assignment | ternary
 *   ternary     → logicOr ("?" ternary ":" ternary)?
 *   logicOr     → logicAnd ("||" logicAnd)*
 *   logicAnd    → equality ("&&" equality)*
 *   equality    → comparison (("==" | "!=") comparison)*
 *   comparison  → term (("<" | "<=" | ">" | ">=") term)*
 *   term        → factor (("+" | "-") factor)*
 *   factor      → unary (("*" | "/" | "%") unary)*
 *   unary       → ("!" | "-" | "--" | "++") unary | postfix
 *   postfix     → primary ("[" expr "]" | "." IDENTIFIER | "(" args? ")" | "++" | "--")*
 *   primary     → NUMBER | STRING | IDENTIFIER | "(" expr ")" | "true" | "false"
 */

#ifndef NOVA_PARSER_H
#define NOVA_PARSER_H

#include "lexer/lexer.h"
#include "ast/ast.h"
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace nova {

/**
 * @class ParseError
 * @brief Exception thrown when parser encounters an error
 */
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message, const Location& loc)
        : std::runtime_error(message + " at " + loc.toString()),
          location(loc), errorMessage(message) {}
    
    Location location;
    std::string errorMessage;
};

/**
 * @class Parser
 * @brief Parses tokens into an AST
 * 
 * The parser uses recursive descent parsing, a top-down approach
 * that's easy to understand and extend.
 * 
 * Features:
 * - Detailed error reporting with source location
 * - Error synchronization to continue after errors
 * - Support for operator precedence
 * - Full AST construction
 */
class Parser {
public:
    /**
     * @brief Construct a parser from a token stream
     * @param tokens The tokens to parse
     */
    explicit Parser(const std::vector<Token>& tokens);
    
    /**
     * @brief Parse the entire token stream
     * @return The root Program node
     */
    std::shared_ptr<Program> parse();
    
    /**
     * @brief Check if there were any errors during parsing
     */
    bool hasErrors() const { return !errors_.empty(); }
    
    /**
     * @brief Get all parse errors
     */
    const std::vector<ParseError>& getErrors() const { return errors_; }

private:
    std::vector<Token> tokens_;
    size_t current_;
    std::vector<ParseError> errors_;
    
    // Token manipulation
    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(std::initializer_list<TokenType> types);
    Token consume(TokenType type, const std::string& message);
    
    // Error handling
    ParseError error(const Token& token, const std::string& message);
    void synchronize();
    
    // Parsing declarations
    DeclPtr parseDeclaration();
    DeclPtr parseFunctionDecl();
    DeclPtr parseVariableDecl(bool isConst);
    std::shared_ptr<ParamDecl> parseParameter();
    DeclPtr parseStructDecl();
    
    // Parsing statements
    StmtPtr parseStatement();
    StmtPtr parseExprStmt();
    StmtPtr parseBlockStmt();
    StmtPtr parseBlockStmtWithBrace();
    StmtPtr parseIfStmt();
    StmtPtr parseWhileStmt();
    StmtPtr parseForStmt();
    StmtPtr parseReturnStmt();
    std::vector<StmtPtr> parseBlock();
    
    // Parsing expressions
    ExprPtr parseExpression();
    ExprPtr parseAssignment();
    ExprPtr parseTernary();
    ExprPtr parseLogicOr();
    ExprPtr parseLogicAnd();
    ExprPtr parseEquality();
    ExprPtr parseComparison();
    ExprPtr parseTerm();
    ExprPtr parseFactor();
    ExprPtr parseUnary();
    ExprPtr parsePostfix();
    ExprPtr parsePrimary();
    ExprPtr parseCall(ExprPtr callee);
    
    // Parsing types
    TypePtr parseType();
    TypePtr parseTypeAnnotation();
    
    // Helpers
    std::vector<ExprPtr> parseArguments();
    std::vector<std::shared_ptr<ParamDecl>> parseParameters();
};

// Implementation

inline Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens), current_(0) {}

inline Token Parser::peek() const {
    return tokens_[current_];
}

inline Token Parser::previous() const {
    return tokens_[current_ - 1];
}

inline bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

inline Token Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

inline bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

inline bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

inline Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

inline ParseError Parser::error(const Token& token, const std::string& message) {
    ParseError err(message, token.location);
    errors_.push_back(err);
    return err;
}

inline void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::KW_FN:
            case TokenType::KW_LET:
            case TokenType::KW_CONST:
            case TokenType::KW_STRUCT:
            case TokenType::KW_ENUM:
            case TokenType::KW_IF:
            case TokenType::KW_WHILE:
            case TokenType::KW_FOR:
            case TokenType::KW_RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

inline std::shared_ptr<Program> Parser::parse() {
    std::string filename = tokens_.empty() ? "source" : tokens_[0].location.filename;
    auto program = std::make_shared<Program>(filename, Location(filename, 1, 1, 0));
    
    while (!isAtEnd()) {
        try {
            auto decl = parseDeclaration();
            if (decl) {
                program->declarations.push_back(decl);
            }
        } catch (const ParseError& e) {
            // Error already recorded, synchronize and continue
            synchronize();
        }
    }
    
    return program;
}

inline DeclPtr Parser::parseDeclaration() {
    if (match({TokenType::KW_FN})) return parseFunctionDecl();
    if (match({TokenType::KW_LET})) return parseVariableDecl(false);
    if (match({TokenType::KW_CONST})) return parseVariableDecl(true);
    if (match({TokenType::KW_STRUCT})) return parseStructDecl();
    
    // If it's not a declaration, treat it as a statement
    // For now, we'll return nullptr for bare statements at top level
    // In a real compiler, we might handle this differently
    parseStatement();
    return nullptr;
}

inline DeclPtr Parser::parseFunctionDecl() {
    Location loc = previous().location;
    
    Token name = consume(TokenType::IDENTIFIER, "Expected function name");
    consume(TokenType::LPAREN, "Expected '(' after function name");
    
    auto params = parseParameters();
    
    consume(TokenType::RPAREN, "Expected ')' after parameters");
    
    TypePtr returnType = nullptr;
    if (match({TokenType::ARROW})) {
        returnType = parseType();
    }
    
    StmtPtr body = parseBlockStmtWithBrace();
    
    return std::make_shared<FunctionDecl>(name.lexeme, params, returnType, body, loc);
}

inline DeclPtr Parser::parseVariableDecl(bool isConst) {
    Location loc = previous().location;
    
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    TypePtr type = nullptr;
    if (match({TokenType::COLON})) {
        type = parseType();
    }
    
    ExprPtr init = nullptr;
    if (match({TokenType::OP_EQUAL})) {
        init = parseExpression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    return std::make_shared<VariableDecl>(name.lexeme, type, init, isConst, loc);
}

inline std::shared_ptr<ParamDecl> Parser::parseParameter() {
    Token name = consume(TokenType::IDENTIFIER, "Expected parameter name");
    
    TypePtr type = nullptr;
    if (match({TokenType::COLON})) {
        type = parseType();
    }
    
    return std::make_shared<ParamDecl>(name.lexeme, type, name.location);
}

inline DeclPtr Parser::parseStructDecl() {
    Location loc = previous().location;
    
    Token name = consume(TokenType::IDENTIFIER, "Expected struct name");
    consume(TokenType::LBRACE, "Expected '{' before struct body");
    
    std::vector<std::shared_ptr<FieldDecl>> fields;
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        Token fieldName = consume(TokenType::IDENTIFIER, "Expected field name");
        consume(TokenType::COLON, "Expected ':' after field name");
        TypePtr fieldType = parseType();
        
        fields.push_back(std::make_shared<FieldDecl>(
            fieldName.lexeme, fieldType, fieldName.location));
        
        // Optional comma
        match({TokenType::COMMA});
    }
    
    consume(TokenType::RBRACE, "Expected '}' after struct body");
    
    return std::make_shared<StructDecl>(name.lexeme, fields, loc);
}

inline StmtPtr Parser::parseStatement() {
    if (match({TokenType::LBRACE})) return parseBlockStmt();
    if (match({TokenType::KW_IF})) return parseIfStmt();
    if (match({TokenType::KW_WHILE})) return parseWhileStmt();
    if (match({TokenType::KW_FOR})) return parseForStmt();
    if (match({TokenType::KW_RETURN})) return parseReturnStmt();
    
    return parseExprStmt();
}

inline StmtPtr Parser::parseExprStmt() {
    ExprPtr expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_shared<ExpressionStmt>(expr, expr->location);
}

inline StmtPtr Parser::parseBlockStmt() {
    // This is called when '{' has already been consumed (from parseStatement)
    Location loc = previous().location;
    auto stmts = parseBlock();
    return std::make_shared<BlockStmt>(stmts, loc);
}

inline StmtPtr Parser::parseBlockStmtWithBrace() {
    // This is called when '{' has NOT been consumed yet
    Location loc = peek().location;
    consume(TokenType::LBRACE, "Expected '{' before block");
    auto stmts = parseBlock();
    return std::make_shared<BlockStmt>(stmts, loc);
}

inline std::vector<StmtPtr> Parser::parseBlock() {
    std::vector<StmtPtr> statements;
    
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        try {
            statements.push_back(parseStatement());
        } catch (const ParseError& e) {
            synchronize();
        }
    }
    
    consume(TokenType::RBRACE, "Expected '}' after block");
    return statements;
}

inline StmtPtr Parser::parseIfStmt() {
    Location loc = previous().location;
    
    consume(TokenType::LPAREN, "Expected '(' after 'if'");
    ExprPtr condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after if condition");
    
    StmtPtr thenBranch = parseStatement();
    StmtPtr elseBranch = nullptr;
    
    if (match({TokenType::KW_ELSE})) {
        elseBranch = parseStatement();
    }
    
    return std::make_shared<IfStmt>(condition, thenBranch, elseBranch, loc);
}

inline StmtPtr Parser::parseWhileStmt() {
    Location loc = previous().location;
    
    consume(TokenType::LPAREN, "Expected '(' after 'while'");
    ExprPtr condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after while condition");
    
    StmtPtr body = parseStatement();
    
    return std::make_shared<WhileStmt>(condition, body, loc);
}

inline StmtPtr Parser::parseForStmt() {
    Location loc = previous().location;
    
    consume(TokenType::LPAREN, "Expected '(' after 'for'");
    
    // Initializer
    std::variant<std::monostate, StmtPtr, DeclPtr> initializer;
    if (match({TokenType::SEMICOLON})) {
        // No initializer - already default initialized to monostate
    } else if (match({TokenType::KW_LET})) {
        initializer = parseVariableDecl(false);
        // Don't consume semicolon again, parseVariableDecl already did
    } else {
        initializer = parseExprStmt();
    }
    
    // Condition
    ExprPtr condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after loop condition");
    
    // Increment
    ExprPtr increment = nullptr;
    if (!check(TokenType::RPAREN)) {
        increment = parseExpression();
    }
    consume(TokenType::RPAREN, "Expected ')' after for clauses");
    
    StmtPtr body = parseStatement();
    
    return std::make_shared<ForStmt>(initializer, condition, increment, body, loc);
}

inline StmtPtr Parser::parseReturnStmt() {
    Location loc = previous().location;
    
    ExprPtr value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = parseExpression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after return value");
    return std::make_shared<ReturnStmt>(value, loc);
}

inline ExprPtr Parser::parseExpression() {
    return parseAssignment();
}

inline ExprPtr Parser::parseAssignment() {
    ExprPtr expr = parseTernary();
    
    if (match({TokenType::OP_EQUAL})) {
        Token equals = previous();
        ExprPtr value = parseAssignment();
        
        if (auto ident = std::dynamic_pointer_cast<Identifier>(expr)) {
            return std::make_shared<BinaryExpr>(expr, TokenType::OP_EQUAL, 
                                               value, equals.location);
        }
        
        error(equals, "Invalid assignment target");
    }
    
    return expr;
}

inline ExprPtr Parser::parseTernary() {
    ExprPtr expr = parseLogicOr();
    
    if (match({TokenType::OP_BANG_EQUAL})) {
        // This is actually ? for ternary, but we're using BANG_EQUAL as placeholder
        // In a real implementation, we'd have a proper QUESTION token
        Token op = previous();
        ExprPtr thenBranch = parseTernary();
        consume(TokenType::COLON, "Expected ':' in ternary expression");
        ExprPtr elseBranch = parseTernary();
        // Return ternary expression (we'd need a TernaryExpr AST node)
    }
    
    return expr;
}

inline ExprPtr Parser::parseLogicOr() {
    ExprPtr expr = parseLogicAnd();
    
    while (match({TokenType::OP_OR})) {
        Token op = previous();
        ExprPtr right = parseLogicAnd();
        expr = std::make_shared<BinaryExpr>(expr, op.type, right, op.location);
    }
    
    return expr;
}

inline ExprPtr Parser::parseLogicAnd() {
    ExprPtr expr = parseEquality();
    
    while (match({TokenType::OP_AND})) {
        Token op = previous();
        ExprPtr right = parseEquality();
        expr = std::make_shared<BinaryExpr>(expr, op.type, right, op.location);
    }
    
    return expr;
}

inline ExprPtr Parser::parseEquality() {
    ExprPtr expr = parseComparison();
    
    while (match({TokenType::OP_EQUAL_EQUAL, TokenType::OP_BANG_EQUAL})) {
        Token op = previous();
        ExprPtr right = parseComparison();
        expr = std::make_shared<BinaryExpr>(expr, op.type, right, op.location);
    }
    
    return expr;
}

inline ExprPtr Parser::parseComparison() {
    ExprPtr expr = parseTerm();
    
    while (match({TokenType::OP_LESS, TokenType::OP_LESS_EQUAL,
                  TokenType::OP_GREATER, TokenType::OP_GREATER_EQUAL})) {
        Token op = previous();
        ExprPtr right = parseTerm();
        expr = std::make_shared<BinaryExpr>(expr, op.type, right, op.location);
    }
    
    return expr;
}

inline ExprPtr Parser::parseTerm() {
    ExprPtr expr = parseFactor();
    
    while (match({TokenType::OP_PLUS, TokenType::OP_MINUS})) {
        Token op = previous();
        ExprPtr right = parseFactor();
        expr = std::make_shared<BinaryExpr>(expr, op.type, right, op.location);
    }
    
    return expr;
}

inline ExprPtr Parser::parseFactor() {
    ExprPtr expr = parseUnary();
    
    while (match({TokenType::OP_STAR, TokenType::OP_SLASH, TokenType::OP_PERCENT})) {
        Token op = previous();
        ExprPtr right = parseUnary();
        expr = std::make_shared<BinaryExpr>(expr, op.type, right, op.location);
    }
    
    return expr;
}

inline ExprPtr Parser::parseUnary() {
    if (match({TokenType::OP_BANG, TokenType::OP_MINUS,
               TokenType::OP_MINUS_MINUS, TokenType::OP_PLUS_PLUS})) {
        Token op = previous();
        ExprPtr right = parseUnary();
        return std::make_shared<UnaryExpr>(op.type, right, true, op.location);
    }
    
    return parsePostfix();
}

inline ExprPtr Parser::parsePostfix() {
    ExprPtr expr = parsePrimary();
    
    while (true) {
        if (match({TokenType::LPAREN})) {
            expr = parseCall(expr);
        } else if (match({TokenType::LBRACKET})) {
            ExprPtr index = parseExpression();
            consume(TokenType::RBRACKET, "Expected ']' after index");
            // Return index expression
        } else if (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, 
                                "Expected property name after '.'");
            // Return member expression
        } else if (match({TokenType::OP_MINUS_MINUS, TokenType::OP_PLUS_PLUS})) {
            Token op = previous();
            expr = std::make_shared<UnaryExpr>(op.type, expr, false, op.location);
        } else {
            break;
        }
    }
    
    return expr;
}

inline ExprPtr Parser::parsePrimary() {
    if (match({TokenType::KW_FALSE})) {
        return std::make_shared<BoolLiteral>(false, previous().location);
    }
    if (match({TokenType::KW_TRUE})) {
        return std::make_shared<BoolLiteral>(true, previous().location);
    }
    if (match({TokenType::KW_NULL})) {
        return std::make_shared<Identifier>("null", previous().location);
    }
    
    if (match({TokenType::INTEGER_LITERAL})) {
        int64_t value = std::get<int64_t>(previous().value);
        return std::make_shared<IntegerLiteral>(value, previous().location);
    }
    if (match({TokenType::FLOAT_LITERAL})) {
        double value = std::get<double>(previous().value);
        return std::make_shared<FloatLiteral>(value, previous().location);
    }
    if (match({TokenType::STRING_LITERAL})) {
        std::string value = std::get<std::string>(previous().value);
        return std::make_shared<StringLiteral>(value, previous().location);
    }
    if (match({TokenType::CHAR_LITERAL})) {
        char value = std::get<char>(previous().value);
        (void)value; // Suppress unused variable warning
        // Return char literal - TODO
    }
    
    if (match({TokenType::IDENTIFIER})) {
        return std::make_shared<Identifier>(previous().lexeme, previous().location);
    }
    
    if (match({TokenType::LPAREN})) {
        ExprPtr expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw error(peek(), "Expected expression");
}

inline ExprPtr Parser::parseCall(ExprPtr callee) {
    auto args = parseArguments();
    consume(TokenType::RPAREN, "Expected ')' after arguments");
    return std::make_shared<CallExpr>(callee, args, callee->location);
}

inline std::vector<ExprPtr> Parser::parseArguments() {
    std::vector<ExprPtr> args;
    
    if (!check(TokenType::RPAREN)) {
        do {
            args.push_back(parseExpression());
        } while (match({TokenType::COMMA}));
    }
    
    return args;
}

inline std::vector<std::shared_ptr<ParamDecl>> Parser::parseParameters() {
    std::vector<std::shared_ptr<ParamDecl>> params;
    
    if (!check(TokenType::RPAREN)) {
        do {
            params.push_back(parseParameter());
        } while (match({TokenType::COMMA}));
    }
    
    return params;
}

inline TypePtr Parser::parseType() {
    Token name;
    
    // Handle primitive type keywords
    if (match({TokenType::KW_INT})) {
        name = previous();
    } else if (match({TokenType::KW_FLOAT})) {
        name = previous();
    } else if (match({TokenType::KW_STRING})) {
        name = previous();
    } else if (match({TokenType::KW_BOOL})) {
        name = previous();
    } else if (match({TokenType::KW_VOID})) {
        name = previous();
    } else {
        name = consume(TokenType::IDENTIFIER, "Expected type name");
    }
    
    // Check for array type
    if (match({TokenType::LBRACKET})) {
        consume(TokenType::RBRACKET, "Expected ']' for array type");
        auto baseType = std::make_shared<NamedType>(name.lexeme, name.location);
        return std::make_shared<ArrayType>(baseType, name.location);
    }
    
    // Check for primitive types
    if (name.lexeme == "int" || name.type == TokenType::KW_INT) {
        return std::make_shared<PrimitiveType>(PrimitiveTypeKind::INT, name.location);
    } else if (name.lexeme == "float" || name.type == TokenType::KW_FLOAT) {
        return std::make_shared<PrimitiveType>(PrimitiveTypeKind::FLOAT, name.location);
    } else if (name.lexeme == "string" || name.type == TokenType::KW_STRING) {
        return std::make_shared<PrimitiveType>(PrimitiveTypeKind::STRING, name.location);
    } else if (name.lexeme == "bool" || name.type == TokenType::KW_BOOL) {
        return std::make_shared<PrimitiveType>(PrimitiveTypeKind::BOOL, name.location);
    } else if (name.lexeme == "void" || name.type == TokenType::KW_VOID) {
        return std::make_shared<PrimitiveType>(PrimitiveTypeKind::VOID, name.location);
    }
    
    // User-defined type
    return std::make_shared<NamedType>(name.lexeme, name.location);
}

inline TypePtr Parser::parseTypeAnnotation() {
    if (match({TokenType::COLON})) {
        return parseType();
    }
    return nullptr;
}

} // namespace nova

#endif // NOVA_PARSER_H
