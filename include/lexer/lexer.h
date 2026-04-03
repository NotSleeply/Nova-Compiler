/**
 * @file lexer.h
 * @brief Lexer (Scanner) for Nova Compiler
 * @author Nova Compiler Team
 * @version 0.1.0
 * @date 2026-04-02
 *
 * The lexer transforms source code into a stream of tokens.
 * This is the first phase of the compilation process.
 */

#ifndef NOVA_LEXER_H
#define NOVA_LEXER_H

#include "token.h"
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace nova
{

    /**
     * @class LexerError
     * @brief Exception thrown when lexer encounters an error
     */
    class LexerError : public std::runtime_error
    {
    public:
        LexerError(const std::string &message, const Location &loc)
            : std::runtime_error(message + " at " + loc.toString()),
              location(loc), errorMessage(message) {}

        Location location;
        std::string errorMessage;
    };

    /**
     * @class Lexer
     * @brief Tokenizes source code into a stream of tokens
     *
     * The lexer performs lexical analysis, converting raw source text
     * into tokens that can be processed by the parser.
     *
     * Features:
     * - Tracks source location (line, column, offset)
     * - Handles comments (single-line and multi-line)
     * - Supports escape sequences in strings
     * - Reports detailed error messages
     */
    class Lexer
    {
    public:
        /**
         * @brief Construct a lexer from a source string
         * @param source The source code to tokenize
         * @param filename The name of the source file (for error reporting)
         */
        explicit Lexer(const std::string &source, const std::string &filename = "<string>");

        /**
         * @brief Construct a lexer from a file
         * @param filename Path to the source file
         * @return Lexer instance
         * @throws std::runtime_error if file cannot be opened
         */
        static Lexer fromFile(const std::string &filename);

        /**
         * @brief Tokenize the entire source code
         * @return Vector of all tokens
         */
        std::vector<Token> tokenize();

        /**
         * @brief Get the next token
         * @return The next token in the stream
         */
        Token nextToken();

        /**
         * @brief Peek at the next token without consuming it
         * @return The next token
         */
        Token peekToken();

        /**
         * @brief Peek at a future token
         * @param offset How many tokens ahead to peek
         * @return The token at the specified offset
         */
        Token peekToken(size_t offset);

        /**
         * @brief Check if we've reached the end of the source
         */
        bool isAtEnd() const;

        /**
         * @brief Get the current location in the source
         */
        Location getCurrentLocation() const;

    private:
        std::string source_;
        std::string filename_;
        size_t start_;   // Start position of current token
        size_t current_; // Current position in source
        int line_;       // Current line number
        int column_;     // Current column number
        int offset_;     // Character offset from start

        // Peek buffer for lookahead
        std::vector<Token> tokenBuffer_;

        // Helper methods
        char peek() const;
        char peekNext() const;
        char advance();
        bool match(char expected);

        void skipWhitespace();
        void skipComment();
        void skipBlockComment();

        Token scanToken();
        Token scanNumber();
        Token scanString();
        Token scanChar();
        Token scanIdentifier();
        Token scanOperator();

        bool isDigit(char c) const;
        bool isAlpha(char c) const;
        bool isAlphaNumeric(char c) const;

        char escapeSequence();
    };

    // Implementation

    inline Lexer::Lexer(const std::string &source, const std::string &filename)
        : source_(source), filename_(filename), start_(0), current_(0),
          line_(1), column_(1), offset_(0) {}

    inline Lexer Lexer::fromFile(const std::string &filename)
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return Lexer(buffer.str(), filename);
    }

    inline bool Lexer::isAtEnd() const
    {
        return current_ >= source_.length();
    }

    inline Location Lexer::getCurrentLocation() const
    {
        return Location(filename_, line_, column_, offset_);
    }

    inline char Lexer::peek() const
    {
        if (isAtEnd())
            return '\0';
        return source_[current_];
    }

    inline char Lexer::peekNext() const
    {
        if (current_ + 1 >= source_.length())
            return '\0';
        return source_[current_ + 1];
    }

    inline char Lexer::advance()
    {
        char c = source_[current_];
        current_++;
        offset_++;

        if (c == '\n')
        {
            line_++;
            column_ = 1;
        }
        else
        {
            column_++;
        }

        return c;
    }

    inline bool Lexer::match(char expected)
    {
        if (isAtEnd() || source_[current_] != expected)
        {
            return false;
        }
        advance();
        return true;
    }

    inline bool Lexer::isDigit(char c) const
    {
        return c >= '0' && c <= '9';
    }

    inline bool Lexer::isAlpha(char c) const
    {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               c == '_';
    }

    inline bool Lexer::isAlphaNumeric(char c) const
    {
        return isAlpha(c) || isDigit(c);
    }

    inline void Lexer::skipWhitespace()
    {
        while (!isAtEnd())
        {
            char c = peek();
            switch (c)
            {
            case ' ':
            case '\t':
            case '\r':
                advance();
                break;
            case '\n':
                advance();
                break;
            case '/':
                if (peekNext() == '/')
                {
                    skipComment();
                }
                else if (peekNext() == '*')
                {
                    skipBlockComment();
                }
                else
                {
                    return;
                }
                break;
            default:
                return;
            }
        }
    }

    inline void Lexer::skipComment()
    {
        // Skip the "//"
        advance();
        advance();

        // Consume characters until end of line or end of file
        while (!isAtEnd() && peek() != '\n')
        {
            advance();
        }
    }

    inline void Lexer::skipBlockComment()
    {
        // Skip the "/*"
        advance();
        advance();

        int nesting = 1;
        while (!isAtEnd() && nesting > 0)
        {
            if (peek() == '/' && peekNext() == '*')
            {
                advance();
                advance();
                nesting++;
            }
            else if (peek() == '*' && peekNext() == '/')
            {
                advance();
                advance();
                nesting--;
            }
            else
            {
                advance();
            }
        }

        if (nesting > 0)
        {
            throw LexerError("Unterminated block comment", getCurrentLocation());
        }
    }

    inline Token Lexer::scanNumber()
    {
        Location loc = getCurrentLocation();

        // Scan integer part
        while (!isAtEnd() && isDigit(peek()))
        {
            advance();
        }

        bool isFloat = false;

        // Check for decimal point
        if (peek() == '.' && isDigit(peekNext()))
        {
            isFloat = true;
            advance(); // Consume the '.'

            while (!isAtEnd() && isDigit(peek()))
            {
                advance();
            }
        }

        // Check for exponent
        if (peek() == 'e' || peek() == 'E')
        {
            isFloat = true;
            advance(); // Consume the 'e' or 'E'

            if (peek() == '+' || peek() == '-')
            {
                advance(); // Consume sign
            }

            while (!isAtEnd() && isDigit(peek()))
            {
                advance();
            }
        }

        std::string lexeme = source_.substr(start_, current_ - start_);

        if (isFloat)
        {
            double value = std::stod(lexeme);
            return Token(TokenType::FLOAT_LITERAL, value, loc, lexeme);
        }
        else
        {
            int64_t value = std::stoll(lexeme);
            return Token(TokenType::INTEGER_LITERAL, value, loc, lexeme);
        }
    }

    inline char Lexer::escapeSequence()
    {
        char c = advance();
        switch (c)
        {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'r':
            return '\r';
        case '\\':
            return '\\';
        case '"':
            return '"';
        case '\'':
            return '\'';
        case '0':
            return '\0';
        default:
            throw LexerError("Invalid escape sequence: \\" + std::string(1, c),
                             getCurrentLocation());
        }
    }

    inline Token Lexer::scanString()
    {
        Location loc = getCurrentLocation();
        advance(); // Consume opening quote

        std::string value;

        while (!isAtEnd() && peek() != '"')
        {
            if (peek() == '\n')
            {
                throw LexerError("Unterminated string", loc);
            }

            if (peek() == '\\')
            {
                advance();
                value += escapeSequence();
            }
            else
            {
                value += advance();
            }
        }

        if (isAtEnd())
        {
            throw LexerError("Unterminated string", loc);
        }

        advance(); // Consume closing quote

        std::string lexeme = source_.substr(start_, current_ - start_);
        return Token(TokenType::STRING_LITERAL, value, loc, lexeme);
    }

    inline Token Lexer::scanChar()
    {
        Location loc = getCurrentLocation();
        advance(); // Consume opening quote

        char value;
        if (peek() == '\\')
        {
            advance();
            value = escapeSequence();
        }
        else
        {
            value = advance();
        }

        if (peek() != '\'')
        {
            throw LexerError("Unterminated character literal", loc);
        }

        advance(); // Consume closing quote

        std::string lexeme = source_.substr(start_, current_ - start_);
        return Token(TokenType::CHAR_LITERAL, value, loc, lexeme);
    }

    inline Token Lexer::scanIdentifier()
    {
        Location loc = getCurrentLocation();

        while (!isAtEnd() && isAlphaNumeric(peek()))
        {
            advance();
        }

        std::string lexeme = source_.substr(start_, current_ - start_);

        // Check if it's a keyword
        auto it = KEYWORDS.find(lexeme);
        if (it != KEYWORDS.end())
        {
            return Token(it->second, std::monostate{}, loc, lexeme);
        }

        return Token(TokenType::IDENTIFIER, lexeme, loc, lexeme);
    }

    inline Token Lexer::scanOperator()
    {
        Location loc = getCurrentLocation();
        char c = advance();

        switch (c)
        {
        // Single-character tokens
        case '(':
            return Token(TokenType::LPAREN, std::monostate{}, loc, "(");
        case ')':
            return Token(TokenType::RPAREN, std::monostate{}, loc, ")");
        case '{':
            return Token(TokenType::LBRACE, std::monostate{}, loc, "{");
        case '}':
            return Token(TokenType::RBRACE, std::monostate{}, loc, "}");
        case '[':
            return Token(TokenType::LBRACKET, std::monostate{}, loc, "[");
        case ']':
            return Token(TokenType::RBRACKET, std::monostate{}, loc, "]");
        case ',':
            return Token(TokenType::COMMA, std::monostate{}, loc, ",");
        case ';':
            return Token(TokenType::SEMICOLON, std::monostate{}, loc, ";");
        case '.':
            return Token(TokenType::DOT, std::monostate{}, loc, ".");

        // Potential multi-character operators
        case '+':
            if (match('+'))
                return Token(TokenType::OP_PLUS_PLUS, std::monostate{}, loc, "++");
            if (match('='))
                return Token(TokenType::OP_PLUS_EQUAL, std::monostate{}, loc, "+=");
            return Token(TokenType::OP_PLUS, std::monostate{}, loc, "+");

        case '-':
            if (match('-'))
                return Token(TokenType::OP_MINUS_MINUS, std::monostate{}, loc, "--");
            if (match('='))
                return Token(TokenType::OP_MINUS_EQUAL, std::monostate{}, loc, "-=");
            if (match('>'))
                return Token(TokenType::ARROW, std::monostate{}, loc, "->");
            return Token(TokenType::OP_MINUS, std::monostate{}, loc, "-");

        case '*':
            if (match('*'))
                return Token(TokenType::OP_STAR_STAR, std::monostate{}, loc, "**");
            if (match('='))
                return Token(TokenType::OP_STAR_EQUAL, std::monostate{}, loc, "*=");
            return Token(TokenType::OP_STAR, std::monostate{}, loc, "*");

        case '/':
            if (match('='))
                return Token(TokenType::OP_SLASH_EQUAL, std::monostate{}, loc, "/=");
            return Token(TokenType::OP_SLASH, std::monostate{}, loc, "/");

        case '%':
            if (match('='))
                return Token(TokenType::OP_PERCENT_EQUAL, std::monostate{}, loc, "%=");
            return Token(TokenType::OP_PERCENT, std::monostate{}, loc, "%");

        case '=':
            if (match('='))
                return Token(TokenType::OP_EQUAL_EQUAL, std::monostate{}, loc, "==");
            if (match('>'))
                return Token(TokenType::FAT_ARROW, std::monostate{}, loc, "=>");
            return Token(TokenType::OP_EQUAL, std::monostate{}, loc, "=");

        case '!':
            if (match('='))
                return Token(TokenType::OP_BANG_EQUAL, std::monostate{}, loc, "!=");
            return Token(TokenType::OP_BANG, std::monostate{}, loc, "!");

        case '<':
            if (match('<'))
                return Token(TokenType::OP_LESS_LESS, std::monostate{}, loc, "<<");
            if (match('='))
                return Token(TokenType::OP_LESS_EQUAL, std::monostate{}, loc, "<=");
            return Token(TokenType::OP_LESS, std::monostate{}, loc, "<");

        case '>':
            if (match('>'))
                return Token(TokenType::OP_GREATER_GREATER, std::monostate{}, loc, ">>");
            if (match('='))
                return Token(TokenType::OP_GREATER_EQUAL, std::monostate{}, loc, ">=");
            return Token(TokenType::OP_GREATER, std::monostate{}, loc, ">");

        case '&':
            if (match('&'))
                return Token(TokenType::OP_AND, std::monostate{}, loc, "&&");
            return Token(TokenType::OP_AMPERSAND, std::monostate{}, loc, "&");

        case '|':
            if (match('|'))
                return Token(TokenType::OP_OR, std::monostate{}, loc, "||");
            return Token(TokenType::OP_PIPE, std::monostate{}, loc, "|");

        case '^':
            return Token(TokenType::OP_CARET, std::monostate{}, loc, "^");
        case '~':
            return Token(TokenType::OP_TILDE, std::monostate{}, loc, "~");

        case ':':
            if (match(':'))
                return Token(TokenType::COLON_COLON, std::monostate{}, loc, "::");
            return Token(TokenType::COLON, std::monostate{}, loc, ":");

        default:
            throw LexerError("Unexpected character: " + std::string(1, c), loc);
        }
    }

    inline Token Lexer::scanToken()
    {
        skipWhitespace();
        start_ = current_;

        if (isAtEnd())
        {
            return Token(TokenType::END_OF_FILE, std::monostate{}, getCurrentLocation(), "");
        }

        char c = peek();

        // Numbers
        if (isDigit(c))
        {
            return scanNumber();
        }

        // Identifiers and keywords
        if (isAlpha(c))
        {
            return scanIdentifier();
        }

        // String literals
        if (c == '"')
        {
            return scanString();
        }

        // Character literals
        if (c == '\'')
        {
            return scanChar();
        }

        // Operators and punctuation
        return scanOperator();
    }

    inline std::vector<Token> Lexer::tokenize()
    {
        std::vector<Token> tokens;

        while (true)
        {
            Token token = scanToken();
            tokens.push_back(token);

            if (token.type == TokenType::END_OF_FILE)
            {
                break;
            }
        }

        return tokens;
    }

    inline Token Lexer::nextToken()
    {
        if (!tokenBuffer_.empty())
        {
            Token token = tokenBuffer_.front();
            tokenBuffer_.erase(tokenBuffer_.begin());
            return token;
        }

        return scanToken();
    }

    inline Token Lexer::peekToken()
    {
        return peekToken(0);
    }

    inline Token Lexer::peekToken(size_t offset)
    {
        while (tokenBuffer_.size() <= offset)
        {
            tokenBuffer_.push_back(scanToken());
        }
        return tokenBuffer_[offset];
    }

} // namespace nova

#endif // NOVA_LEXER_H
