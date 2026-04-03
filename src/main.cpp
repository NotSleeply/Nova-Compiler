/**
 * @file main.cpp
 * @brief Main entry point for Nova Compiler
 * @author Nova Compiler Team
 * @version 0.2.0
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
#include "semantic/semantic_analyzer.h"
#include "ir/ir.h"
#include "ir/ir_generator.h"
#include "codegen/code_generator.h"

using namespace nova;

/**
 * @brief Print usage information
 */
void printUsage(const std::string& programName) {
    std::cout << R"(
Nova Compiler v0.2.0
A modern compiler frontend framework

Usage: )" << programName << R"( [options] <source-file>

Options:
  -h, --help          Show this help message
  -l, --lex           Run lexer only and print tokens
  -p, --parse         Run parser and print AST
  -s, --semantic      Run semantic analysis
  -o, --output FILE   Output file (not implemented yet)
  -v, --version       Show version information

Examples:
  )" << programName << R"( example.nv
  )" << programName << R"( --lex example.nv
  )" << programName << R"( --parse example.nv
  )" << programName << R"( --semantic example.nv
  )" << programName << R"( --ir example.nv
  )" << programName << R"( --codegen example.nv
  )" << programName << R"( --codegen -o output.c example.nv

For more information, visit: https://github.com/NotSleeply/Nova-Compiler
)" << std::endl;
}

/**
 * @brief Print version information
 */
void printVersion() {
    std::cout << "Nova Compiler v0.2.0" << std::endl;
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
 * @brief Run semantic analysis and print results
 */
int runSemantic(const std::string& source, const std::string& filename);

/**
 * @brief Generate and print IR
 */
int generateIR(const std::string& source, const std::string& filename);

/**
 * @brief Generate target code
 */
int generateCode(const std::string& source, const std::string& filename, const std::string& outputFile);

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
        
        // Phase 3: Semantic Analysis
        std::cout << "Phase 3: Semantic Analysis..." << std::endl;
        SemanticAnalyzer analyzer;
        if (!analyzer.analyze(program)) {
            std::cerr << "  ✗ Semantic errors found:" << std::endl;
            for (const auto& error : analyzer.getErrors()) {
                std::cerr << "    " << error.toString() << std::endl;
            }
            return 1;
        }
        std::cout << "  ✓ Semantic analysis passed" << std::endl;
        
        // Phase 4: IR Generation
        std::cout << "Phase 4: IR Generation..." << std::endl;
        IRGenerator irGen;
        auto irModule = irGen.generate(program);
        std::cout << "  ✓ IR generated successfully" << std::endl;
        
        // Phase 5: Code Generation
        std::cout << "Phase 5: Code Generation..." << std::endl;
        nova::codegen::CCodeGenerator codeGen;
        auto generatedCode = codeGen.generate(irModule);
        
        // Write to default output file
        std::ofstream outFile(generatedCode.filename);
        if (!outFile) {
            std::cerr << "Error: Cannot open output file: " << generatedCode.filename << std::endl;
            return 1;
        }
        outFile << generatedCode.code;
        outFile.close();
        std::cout << "  ✓ Code generated to: " << generatedCode.filename << std::endl;
        
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
    std::string outputFile;
    bool lexOnly = false;
    bool parseOnly = false;
    bool semanticOnly = false;
    bool irOnly = false;
    bool codegenOnly = false;
    
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
        } else if (arg == "-s" || arg == "--semantic") {
            semanticOnly = true;
        } else if (arg == "-i" || arg == "--ir") {
            irOnly = true;
        } else if (arg == "-c" || arg == "--codegen") {
            codegenOnly = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: --output requires a filename" << std::endl;
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
    } else if (semanticOnly) {
        return runSemantic(source, sourceFile);
    } else if (irOnly) {
        return generateIR(source, sourceFile);
    } else if (codegenOnly) {
        return generateCode(source, sourceFile, outputFile);
    } else {
        return compile(source, sourceFile);
    }
}

/**
 * @brief Run semantic analysis and print results
 */
int runSemantic(const std::string& source, const std::string& filename) {
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
        
        // Then semantic analysis
        SemanticAnalyzer analyzer;
        if (!analyzer.analyze(program)) {
            std::cerr << "\n=== Semantic Errors ===\n" << std::endl;
            for (const auto& error : analyzer.getErrors()) {
                std::cerr << error.toString() << std::endl;
            }
            return 1;
        }
        
        std::cout << "\n=== Semantic Analysis ===\n" << std::endl;
        std::cout << "✓ No semantic errors found" << std::endl;
        std::cout << "✓ Type checking passed" << std::endl;
        std::cout << "✓ Symbol resolution completed" << std::endl;
        
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
 * @brief Generate and print IR
 */
int generateIR(const std::string& source, const std::string& filename) {
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
        
        // Semantic analysis
        SemanticAnalyzer analyzer;
        if (!analyzer.analyze(program)) {
            std::cerr << "\n=== Semantic Errors ===\n" << std::endl;
            for (const auto& error : analyzer.getErrors()) {
                std::cerr << error.toString() << std::endl;
            }
            return 1;
        }
        
        // Generate IR
        IRGenerator irGen;
        auto irModule = irGen.generate(program);
        
        std::cout << "\n=== Intermediate Representation ===\n" << std::endl;
        std::cout << irModule->toString() << std::endl;
        
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
 * @brief Generate target code
 */
int generateCode(const std::string& source, const std::string& filename, const std::string& outputFile) {
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
        
        // Semantic analysis
        SemanticAnalyzer analyzer;
        if (!analyzer.analyze(program)) {
            std::cerr << "\n=== Semantic Errors ===\n" << std::endl;
            for (const auto& error : analyzer.getErrors()) {
                std::cerr << error.toString() << std::endl;
            }
            return 1;
        }
        
        // Generate IR
        IRGenerator irGen;
        auto irModule = irGen.generate(program);
        
        // Generate target code
        nova::codegen::CCodeGenerator codeGen;
        if (!outputFile.empty()) {
            codeGen.setOutputFile(outputFile);
        }
        auto generatedCode = codeGen.generate(irModule);
        
        // Write to file or stdout
        std::string outFile = outputFile.empty() ? generatedCode.filename : outputFile;
        std::ofstream out(outFile);
        if (!out) {
            std::cerr << "Error: Cannot open output file: " << outFile << std::endl;
            return 1;
        }
        out << generatedCode.code;
        out.close();
        
        std::cout << "\n=== Code Generation ===\n" << std::endl;
        std::cout << "✓ Target: " << generatedCode.language << std::endl;
        std::cout << "✓ Output: " << outFile << std::endl;
        std::cout << "\nCompile with: gcc -o " << outFile.substr(0, outFile.find_last_of('.')) 
                  << " " << outFile << std::endl;
        
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
