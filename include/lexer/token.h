/**
 * @file token.h
 * @brief Token definitions for Nova Compiler
 * @author Nova Compiler Team
 * @version 0.1.0
 * @date 2026-04-02
 * 
 * This file defines all token types used in the lexical analysis phase.
 * Tokens are the fundamental building blocks of the compiler's frontend.
 */

#ifndef NOVA_TOKEN_H
#define NOVA_TOKEN_H

#include <string>
#include <unordered_map>
#include <variant>
#include <memory>

namespace nova {

/**
 * @enum TokenType
 * @brief Enumeration of all possible token types in the source language
 */
enum class TokenType {
    // Literals
    INTEGER_LITERAL,     // 42, 0, 123
    FLOAT_LITERAL,       // 3.14, 0.5
    STRING_LITERAL,      // "hello", 'world'
    CHAR_LITERAL,        // 'a', '\n'
    BOOL_LITERAL,        // true, false
    
    // Identifiers and Keywords
    IDENTIFIER,          // variable names, function names, etc.
    
    // Keywords
    KW_IF,               // if
    KW_ELSE,             // else
    KW_WHILE,            // while
    KW_FOR,              // for
    KW_RETURN,           // return
    KW_BREAK,            // break
    KW_CONTINUE,         // continue
    KW_FN,               // fn (function declaration)
    KW_LET,              // let (variable declaration)
    KW_CONST,            // const (constant declaration)
    KW_STRUCT,           // struct
    KW_ENUM,             // enum
    KW_MATCH,            // match (pattern matching)
    KW_CASE,             // case
    KW_DEFAULT,          // default
    KW_TRUE,             // true
    KW_FALSE,            // false
    KW_NULL,             // null
    KW_VOID,             // void
    KW_INT,              // int
    KW_FLOAT,            // float
    KW_STRING,           // string
    KW_BOOL,             // bool
    KW_CHAR,             // char
    
    // Operators
    // Arithmetic
    OP_PLUS,             // +
    OP_MINUS,            // -
    OP_STAR,             // *
    OP_SLASH,            // /
    OP_PERCENT,          // %
    OP_STAR_STAR,        // ** (power)
    
    // Comparison
    OP_EQUAL_EQUAL,      // ==
    OP_BANG_EQUAL,       // !=
    OP_LESS,             // <
    OP_LESS_EQUAL,       // <=
    OP_GREATER,          // >
    OP_GREATER_EQUAL,    // >=
    
    // Logical
    OP_AND,              // &&
    OP_OR,               // ||
    OP_BANG,             // !
    
    // Bitwise
    OP_AMPERSAND,        // &
    OP_PIPE,             // |
    OP_CARET,            // ^
    OP_TILDE,            // ~
    OP_LESS_LESS,        // <<
    OP_GREATER_GREATER,  // >>
    
    // Assignment
    OP_EQUAL,            // =
    OP_PLUS_EQUAL,       // +=
    OP_MINUS_EQUAL,      // -=
    OP_STAR_EQUAL,       // *=
    OP_SLASH_EQUAL,      // /=
    OP_PERCENT_EQUAL,    // %=
    
    // Increment/Decrement
    OP_PLUS_PLUS,        // ++
    OP_MINUS_MINUS,      // --
    
    // Delimiters
    LPAREN,              // (
    RPAREN,              // )
    LBRACE,              // {
    RBRACE,              // }
    LBRACKET,            // [
    RBRACKET,            // ]
    COMMA,               // ,
    SEMICOLON,           // ;
    COLON,               // :
    COLON_COLON,         // ::
    DOT,                 // .
    ARROW,               // ->
    FAT_ARROW,           // =>
    
    // Special
    END_OF_FILE,         // EOF
    UNKNOWN,             // Unknown token
    NEWLINE,             // Newline (for significant whitespace languages)
    COMMENT,             // Single-line comment
    BLOCK_COMMENT        // Multi-line comment
};

/**
 * @brief Map for keyword string to TokenType conversion
 */
static const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"if", TokenType::KW_IF},
    {"else", TokenType::KW_ELSE},
    {"while", TokenType::KW_WHILE},
    {"for", TokenType::KW_FOR},
    {"return", TokenType::KW_RETURN},
    {"break", TokenType::KW_BREAK},
    {"continue", TokenType::KW_CONTINUE},
    {"fn", TokenType::KW_FN},
    {"let", TokenType::KW_LET},
    {"const", TokenType::KW_CONST},
    {"struct", TokenType::KW_STRUCT},
    {"enum", TokenType::KW_ENUM},
    {"match", TokenType::KW_MATCH},
    {"case", TokenType::KW_CASE},
    {"default", TokenType::KW_DEFAULT},
    {"true", TokenType::KW_TRUE},
    {"false", TokenType::KW_FALSE},
    {"null", TokenType::KW_NULL},
    {"void", TokenType::KW_VOID},
    {"int", TokenType::KW_INT},
    {"float", TokenType::KW_FLOAT},
    {"string", TokenType::KW_STRING},
    {"bool", TokenType::KW_BOOL},
    {"char", TokenType::KW_CHAR}
};

/**
 * @brief Convert TokenType to string for debugging
 * @param type The token type to convert
 * @return String representation of the token type
 */
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::INTEGER_LITERAL: return "INTEGER_LITERAL";
        case TokenType::FLOAT_LITERAL: return "FLOAT_LITERAL";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::CHAR_LITERAL: return "CHAR_LITERAL";
        case TokenType::BOOL_LITERAL: return "BOOL_LITERAL";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::KW_IF: return "KW_IF";
        case TokenType::KW_ELSE: return "KW_ELSE";
        case TokenType::KW_WHILE: return "KW_WHILE";
        case TokenType::KW_FOR: return "KW_FOR";
        case TokenType::KW_RETURN: return "KW_RETURN";
        case TokenType::KW_BREAK: return "KW_BREAK";
        case TokenType::KW_CONTINUE: return "KW_CONTINUE";
        case TokenType::KW_FN: return "KW_FN";
        case TokenType::KW_LET: return "KW_LET";
        case TokenType::KW_CONST: return "KW_CONST";
        case TokenType::KW_STRUCT: return "KW_STRUCT";
        case TokenType::KW_ENUM: return "KW_ENUM";
        case TokenType::KW_MATCH: return "KW_MATCH";
        case TokenType::KW_CASE: return "KW_CASE";
        case TokenType::KW_DEFAULT: return "KW_DEFAULT";
        case TokenType::KW_TRUE: return "KW_TRUE";
        case TokenType::KW_FALSE: return "KW_FALSE";
        case TokenType::KW_NULL: return "KW_NULL";
        case TokenType::KW_VOID: return "KW_VOID";
        case TokenType::KW_INT: return "KW_INT";
        case TokenType::KW_FLOAT: return "KW_FLOAT";
        case TokenType::KW_STRING: return "KW_STRING";
        case TokenType::KW_BOOL: return "KW_BOOL";
        case TokenType::KW_CHAR: return "KW_CHAR";
        case TokenType::OP_PLUS: return "OP_PLUS";
        case TokenType::OP_MINUS: return "OP_MINUS";
        case TokenType::OP_STAR: return "OP_STAR";
        case TokenType::OP_SLASH: return "OP_SLASH";
        case TokenType::OP_PERCENT: return "OP_PERCENT";
        case TokenType::OP_STAR_STAR: return "OP_STAR_STAR";
        case TokenType::OP_EQUAL_EQUAL: return "OP_EQUAL_EQUAL";
        case TokenType::OP_BANG_EQUAL: return "OP_BANG_EQUAL";
        case TokenType::OP_LESS: return "OP_LESS";
        case TokenType::OP_LESS_EQUAL: return "OP_LESS_EQUAL";
        case TokenType::OP_GREATER: return "OP_GREATER";
        case TokenType::OP_GREATER_EQUAL: return "OP_GREATER_EQUAL";
        case TokenType::OP_AND: return "OP_AND";
        case TokenType::OP_OR: return "OP_OR";
        case TokenType::OP_BANG: return "OP_BANG";
        case TokenType::OP_AMPERSAND: return "OP_AMPERSAND";
        case TokenType::OP_PIPE: return "OP_PIPE";
        case TokenType::OP_CARET: return "OP_CARET";
        case TokenType::OP_TILDE: return "OP_TILDE";
        case TokenType::OP_LESS_LESS: return "OP_LESS_LESS";
        case TokenType::OP_GREATER_GREATER: return "OP_GREATER_GREATER";
        case TokenType::OP_EQUAL: return "OP_EQUAL";
        case TokenType::OP_PLUS_EQUAL: return "OP_PLUS_EQUAL";
        case TokenType::OP_MINUS_EQUAL: return "OP_MINUS_EQUAL";
        case TokenType::OP_STAR_EQUAL: return "OP_STAR_EQUAL";
        case TokenType::OP_SLASH_EQUAL: return "OP_SLASH_EQUAL";
        case TokenType::OP_PERCENT_EQUAL: return "OP_PERCENT_EQUAL";
        case TokenType::OP_PLUS_PLUS: return "OP_PLUS_PLUS";
        case TokenType::OP_MINUS_MINUS: return "OP_MINUS_MINUS";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::COLON_COLON: return "COLON_COLON";
        case TokenType::DOT: return "DOT";
        case TokenType::ARROW: return "ARROW";
        case TokenType::FAT_ARROW: return "FAT_ARROW";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
        default: return "UNKNOWN_TOKEN_TYPE";
    }
}

/**
 * @struct Location
 * @brief Represents the source code location of a token
 */
struct Location {
    std::string filename;
    int line;
    int column;
    int offset;  // Character offset from the beginning of the file
    
    Location(const std::string& file = "", int ln = 1, int col = 1, int off = 0)
        : filename(file), line(ln), column(col), offset(off) {}
    
    std::string toString() const {
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

/**
 * @brief Token value type using std::variant for type safety
 */
using TokenValue = std::variant<
    std::monostate,      // No value (keywords, operators, etc.)
    int64_t,             // Integer literal
    double,              // Float literal
    std::string,         // String literal or identifier
    char,                // Char literal
    bool                 // Bool literal
>;

/**
 * @struct Token
 * @brief Represents a single token with its type, value, and location
 */
struct Token {
    TokenType type;
    TokenValue value;
    Location location;
    std::string lexeme;  // The original source text
    
    Token(TokenType t = TokenType::UNKNOWN, 
          const TokenValue& v = std::monostate{},
          const Location& loc = Location(),
          const std::string& lex = "")
        : type(t), value(v), location(loc), lexeme(lex) {}
    
    /**
     * @brief Check if this token is of a specific type
     */
    bool is(TokenType t) const { return type == t; }
    
    /**
     * @brief Check if this token is one of several types
     */
    template<typename... Types>
    bool isOneOf(Types... types) const {
        return (... || (type == types));
    }
    
    /**
     * @brief Get string representation for debugging
     */
    std::string toString() const {
        std::string result = tokenTypeToString(type);
        result += " '" + lexeme + "'";
        result += " at " + location.toString();
        return result;
    }
};

} // namespace nova

#endif // NOVA_TOKEN_H
