/**
 * @file main.cpp
 * @brief Main entry point for Nova Compiler
 * @author Nova Compiler Team
 * @version 0.1.0
 * @date 2026-04-02
 * 
 * Nova Compiler - A modern compiler frontend framework
 * Built from scratch with ❤️ for the open-source community
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include "lexer/token.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/ast.h"

using namespace nova;

/**
 * @brief Print usage information
 */
void printUsage(const std::string& programName) {
    std::cout << R"(
Nova Compiler v0.1.0
A modern compiler frontend framework

Usage: )" << programName << R"( [options] <source-file>

Options:
  -h, --help          Show this help message
  -l, --lex           Run lexer only and print tokens
  -p, --parse         Run parser and print AST
  -o, --output FILE   Output file (not implemented yet)
  -v, --version       Show version information

Examples:
  )" << programName << R"( example.nv
  )" << programName << R"( --lex example.nv
  )" << programName << R"( --parse example.nv

For more information, visit: https://github.com/NotSleeply/Nova-Compiler
)" << std::endl;
}

/**
 * @brief Print version information
 */
void printVersion() {
    std::cout << "Nova Compiler v0.1.0" << std::endl;
    std::cout << "Built on " << __DATE__ << " at " << __TIME__ << std::endl;
    std::cout << "Copyright (c) 2026 Nova Compiler Team" << std::endl;
}

/**
 * @brief Read source file into string
 */
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
 * @brief Run lexer and print tokens
 */
int runLexer(const std::string& source, const std::string& filename) {
    try {
        Lexer lexer(source, filename);
        std::vector<Token> tokens = lexer.tokenize();
        
        std::cout << "\n=== Tokens ===\n" << std::endl;
        for (const Token& token : tokens) {
            std::cout << token.toString() << std::endl;
        }
        
        std::cout << "\nTotal tokens: " << tokens.size() << std::endl;
        return 0;
        
    } catch (const LexerError& e) {
        std::cerr << "Lexer Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

/**
 * @brief Run parser and print AST
 */
int runParser(const std::string& source, const std::string& filename) {
    try {
        // First, tokenize
        Lexer lexer(source, filename);
        std::vector<Token> tokens = lexer.tokenize();
        
        // Then parse
        Parser parser(tokens);
        auto program = parser.parse();
        
        if (parser.hasErrors()) {
            std::cerr << "\n=== Parse Errors ===\n" << std::endl;
            for (const auto& error : parser.getErrors()) {
                std::cerr << "Error: " << error.what() << std::endl;
            }
            return 1;
        }
        
        std::cout << "\n=== Abstract Syntax Tree ===\n" << std::endl;
        std::cout << program->toString() << std::endl;
        
        return 0;
        
    } catch (const LexerError& e) {
        std::cerr << "Lexer Error: " << e.what() << std::endl;
        return 1;
    } catch (const ParseError& e) {
        std::cerr << "Parse Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

/**
 * @brief Full compilation pipeline
 */
int compile(const std::string& source, const std::string& filename) {
    try {
        // Phase 1: Lexical Analysis
        std::cout << "Phase 1: Lexical Analysis..." << std::endl;
        Lexer lexer(source, filename);
        std::vector<Token> tokens = lexer.tokenize();
        std::cout << "  ✓ Tokenized " << tokens.size() << " tokens" << std::endl;
        
        // Phase 2: Parsing
        std::cout << "Phase 2: Parsing..." << std::endl;
        Parser parser(tokens);
        auto program = parser.parse();
        
        if (parser.hasErrors()) {
            std::cerr << "  ✗ Parse errors found:" << std::endl;
            for (const auto& error : parser.getErrors()) {
                std::cerr << "    " << error.what() << std::endl;
            }
            return 1;
        }
        std::cout << "  ✓ Parsed successfully" << std::endl;
        
        // Phase 3: Semantic Analysis (TODO)
        std::cout << "Phase 3: Semantic Analysis... (Not implemented yet)" << std::endl;
        
        // Phase 4: Code Generation (TODO)
        std::cout << "Phase 4: Code Generation... (Not implemented yet)" << std::endl;
        
        std::cout << "\nCompilation completed successfully!" << std::endl;
        return 0;
        
    } catch (const LexerError& e) {
        std::cerr << "Lexer Error: " << e.what() << std::endl;
        return 1;
    } catch (const ParseError& e) {
        std::cerr << "Parse Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string sourceFile;
    bool lexOnly = false;
    bool parseOnly = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-l" || arg == "--lex") {
            lexOnly = true;
        } else if (arg == "-p" || arg == "--parse") {
            parseOnly = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                // Output file specified (not implemented yet)
                ++i;
            } else {
                std::cerr << "Error: Missing output file" << std::endl;
                return 1;
            }
        } else if (arg[0] != '-') {
            sourceFile = arg;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (sourceFile.empty()) {
        std::cerr << "Error: No source file specified" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    // Read source file
    std::string source;
    try {
        source = readFile(sourceFile);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    // Run the appropriate phase
    if (lexOnly) {
        return runLexer(source, sourceFile);
    } else if (parseOnly) {
        return runParser(source, sourceFile);
    } else {
        return compile(source, sourceFile);
    }
}
