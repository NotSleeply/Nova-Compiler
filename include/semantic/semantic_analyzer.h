/**
 * @file semantic_analyzer.h
 * @brief Semantic analysis for Nova Compiler
 * @author Nova Compiler Team
 * @version 0.2.0
 * @date 2026-04-02
 * 
 * The semantic analyzer performs type checking, symbol resolution,
 * and other semantic validations on the AST.
 */

#ifndef NOVA_SEMANTIC_ANALYZER_H
#define NOVA_SEMANTIC_ANALYZER_H

#include "ast/ast.h"
#include "symbol_table.h"
#include "type_checker.h"
#include "lexer/token.h"
#include <vector>
#include <memory>
#include <functional>

namespace nova {

/**
 * @struct SemanticError
 * @brief Represents a semantic error
 */
struct SemanticError {
    std::string message;
    Location location;
    
    SemanticError(const std::string& msg, const Location& loc)
        : message(msg), location(loc) {}
    
    std::string toString() const {
        return "Semantic Error: " + message + " at " + location.toString();
    }
};

/**
 * @class SemanticAnalyzer
 * @brief Performs semantic analysis on the AST
 * 
 * The semantic analyzer:
 * - Builds and maintains the symbol table
 * - Performs type checking
 * - Detects semantic errors
 * - Annotates AST nodes with type information
 */
class SemanticAnalyzer {
public:
    SemanticAnalyzer() : symbolTable_(std::make_unique<SymbolTable>()) {}
    
    /**
     * @brief Analyze the entire program
     * @return true if no errors, false otherwise
     */
    bool analyze(const std::shared_ptr<Program>& program);
    
    /**
     * @brief Get all semantic errors
     */
    const std::vector<SemanticError>& getErrors() const { return errors_; }
    
    /**
     * @brief Check if there were any errors
     */
    bool hasErrors() const { return !errors_.empty(); }
    
    /**
     * @brief Print all errors to stderr
     */
    void printErrors() const {
        for (const auto& error : errors_) {
            std::cerr << error.toString() << std::endl;
        }
    }

private:
    std::unique_ptr<SymbolTable> symbolTable_;
    std::vector<SemanticError> errors_;
    
    // Current function return type (for return statement checking)
    TypePtr currentFunctionReturnType_ = nullptr;
    bool inLoop_ = false;  // Track if we're inside a loop
    
    // Helper methods
    void reportError(const std::string& message, const Location& loc);
    
    // Declaration analysis
    void analyzeDeclaration(const DeclPtr& decl);
    void analyzeFunctionDecl(const std::shared_ptr<FunctionDecl>& func);
    void analyzeVariableDecl(const std::shared_ptr<VariableDecl>& var);
    void analyzeStructDecl(const std::shared_ptr<StructDecl>& struc);
    
    // Statement analysis
    void analyzeStatement(const StmtPtr& stmt);
    void analyzeBlockStmt(const std::shared_ptr<BlockStmt>& block);
    void analyzeIfStmt(const std::shared_ptr<IfStmt>& ifStmt);
    void analyzeWhileStmt(const std::shared_ptr<WhileStmt>& whileStmt);
    void analyzeForStmt(const std::shared_ptr<ForStmt>& forStmt);
    void analyzeReturnStmt(const std::shared_ptr<ReturnStmt>& ret);
    void analyzeExpressionStmt(const std::shared_ptr<ExpressionStmt>& exprStmt);
    
    // Expression analysis
    TypePtr analyzeExpression(const ExprPtr& expr);
    TypePtr analyzeBinaryExpr(const std::shared_ptr<BinaryExpr>& binary);
    TypePtr analyzeUnaryExpr(const std::shared_ptr<UnaryExpr>& unary);
    TypePtr analyzeCallExpr(const std::shared_ptr<CallExpr>& call);
    TypePtr analyzeIdentifier(const std::shared_ptr<Identifier>& ident);
    TypePtr analyzeIntegerLiteral(const std::shared_ptr<IntegerLiteral>& lit);
    TypePtr analyzeFloatLiteral(const std::shared_ptr<FloatLiteral>& lit);
    TypePtr analyzeStringLiteral(const std::shared_ptr<StringLiteral>& lit);
    TypePtr analyzeBoolLiteral(const std::shared_ptr<BoolLiteral>& lit);
    TypePtr analyzeAssignmentExpr(const std::shared_ptr<BinaryExpr>& assign);
    
    // Type analysis
    TypePtr resolveType(const TypePtr& type);
};

// Implementation

inline void SemanticAnalyzer::reportError(const std::string& message, 
                                          const Location& loc) {
    errors_.push_back(SemanticError(message, loc));
}

inline bool SemanticAnalyzer::analyze(const std::shared_ptr<Program>& program) {
    // First pass: collect all declarations
    for (const auto& decl : program->declarations) {
        if (auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(decl)) {
            // Register function in symbol table
            std::vector<TypePtr> paramTypes;
            for (const auto& param : funcDecl->parameters) {
                paramTypes.push_back(param->type);
            }
            
            auto symbol = symbolTable_->createFunction(
                funcDecl->name, funcDecl->returnType, paramTypes, funcDecl->location);
            
            if (!symbolTable_->define(symbol)) {
                reportError("Function '" + funcDecl->name + "' is already defined", 
                           funcDecl->location);
            }
        } else if (auto structDecl = std::dynamic_pointer_cast<StructDecl>(decl)) {
            auto symbol = symbolTable_->createStruct(structDecl->name, structDecl->location);
            if (!symbolTable_->define(symbol)) {
                reportError("Struct '" + structDecl->name + "' is already defined",
                           structDecl->location);
            }
        }
    }
    
    // Second pass: analyze declarations
    for (const auto& decl : program->declarations) {
        analyzeDeclaration(decl);
    }
    
    return !hasErrors();
}

inline void SemanticAnalyzer::analyzeDeclaration(const DeclPtr& decl) {
    if (auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(decl)) {
        analyzeFunctionDecl(funcDecl);
    } else if (auto varDecl = std::dynamic_pointer_cast<VariableDecl>(decl)) {
        analyzeVariableDecl(varDecl);
    } else if (auto structDecl = std::dynamic_pointer_cast<StructDecl>(decl)) {
        analyzeStructDecl(structDecl);
    }
}

inline void SemanticAnalyzer::analyzeFunctionDecl(
    const std::shared_ptr<FunctionDecl>& func) {
    
    // Save the current return type
    auto previousReturnType = currentFunctionReturnType_;
    currentFunctionReturnType_ = func->returnType;
    
    // Enter function scope
    symbolTable_->pushScope();
    
    // Add parameters to scope
    for (const auto& param : func->parameters) {
        auto symbol = symbolTable_->createParameter(
            param->name, param->type, param->location);
        
        if (!symbolTable_->define(symbol)) {
            reportError("Parameter '" + param->name + "' is already defined",
                       param->location);
        }
    }
    
    // Analyze function body
    analyzeBlockStmt(std::dynamic_pointer_cast<BlockStmt>(func->body));
    
    // Exit function scope
    symbolTable_->popScope();
    
    // Restore previous return type
    currentFunctionReturnType_ = previousReturnType;
}

inline void SemanticAnalyzer::analyzeVariableDecl(
    const std::shared_ptr<VariableDecl>& var) {
    
    // Analyze initializer if present
    TypePtr initType = nullptr;
    if (var->initializer) {
        initType = analyzeExpression(var->initializer);
    }
    
    // Determine variable type
    TypePtr varType = var->type;
    if (!varType && initType) {
        // Type inference from initializer
        varType = initType;
    }
    
    // Check type consistency
    if (varType && initType) {
        if (!TypeChecker::isAssignable(varType, initType)) {
            reportError("Cannot initialize variable of type '" + 
                       TypeChecker::typeToString(varType) + "' with '" +
                       TypeChecker::typeToString(initType) + "'",
                       var->location);
        }
    }
    
    // Add to symbol table
    auto symbol = symbolTable_->createVariable(var->name, varType, 
                                               var->location, var->isConst);
    if (!symbolTable_->define(symbol)) {
        reportError("Variable '" + var->name + "' is already defined in this scope",
                   var->location);
    }
}

inline void SemanticAnalyzer::analyzeStructDecl(
    const std::shared_ptr<StructDecl>& struc) {
    // Struct analysis would involve checking field types, etc.
    // For now, we've already registered it in the first pass
    (void)struc; // Suppress unused parameter warning - TODO: implement full struct analysis
}

inline void SemanticAnalyzer::analyzeStatement(const StmtPtr& stmt) {
    if (!stmt) return;
    
    if (auto block = std::dynamic_pointer_cast<BlockStmt>(stmt)) {
        analyzeBlockStmt(block);
    } else if (auto ifStmt = std::dynamic_pointer_cast<IfStmt>(stmt)) {
        analyzeIfStmt(ifStmt);
    } else if (auto whileStmt = std::dynamic_pointer_cast<WhileStmt>(stmt)) {
        analyzeWhileStmt(whileStmt);
    } else if (auto forStmt = std::dynamic_pointer_cast<ForStmt>(stmt)) {
        analyzeForStmt(forStmt);
    } else if (auto retStmt = std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
        analyzeReturnStmt(retStmt);
    } else if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStmt>(stmt)) {
        analyzeExpressionStmt(exprStmt);
    }
}

inline void SemanticAnalyzer::analyzeBlockStmt(
    const std::shared_ptr<BlockStmt>& block) {
    
    symbolTable_->pushScope();
    
    for (const auto& stmt : block->statements) {
        analyzeStatement(stmt);
    }
    
    symbolTable_->popScope();
}

inline void SemanticAnalyzer::analyzeIfStmt(
    const std::shared_ptr<IfStmt>& ifStmt) {
    
    // Analyze condition
    auto condType = analyzeExpression(ifStmt->condition);
    if (condType && !TypeChecker::isBoolean(condType)) {
        reportError("If condition must be boolean, got '" + 
                   TypeChecker::typeToString(condType) + "'",
                   ifStmt->condition->location);
    }
    
    // Analyze branches
    analyzeStatement(ifStmt->thenBranch);
    if (ifStmt->elseBranch) {
        analyzeStatement(ifStmt->elseBranch);
    }
}

inline void SemanticAnalyzer::analyzeWhileStmt(
    const std::shared_ptr<WhileStmt>& whileStmt) {
    
    // Analyze condition
    auto condType = analyzeExpression(whileStmt->condition);
    if (condType && !TypeChecker::isBoolean(condType)) {
        reportError("While condition must be boolean, got '" +
                   TypeChecker::typeToString(condType) + "'",
                   whileStmt->condition->location);
    }
    
    // Analyze body
    bool wasInLoop = inLoop_;
    inLoop_ = true;
    analyzeStatement(whileStmt->body);
    inLoop_ = wasInLoop;
}

inline void SemanticAnalyzer::analyzeForStmt(
    const std::shared_ptr<ForStmt>& forStmt) {
    
    symbolTable_->pushScope();
    
    // Analyze initializer
    if (!std::holds_alternative<std::monostate>(forStmt->initializer)) {
        if (std::holds_alternative<DeclPtr>(forStmt->initializer)) {
            auto decl = std::get<DeclPtr>(forStmt->initializer);
            if (auto varDecl = std::dynamic_pointer_cast<VariableDecl>(decl)) {
                analyzeVariableDecl(varDecl);
            }
        } else if (std::holds_alternative<StmtPtr>(forStmt->initializer)) {
            analyzeStatement(std::get<StmtPtr>(forStmt->initializer));
        }
    }
    
    // Analyze condition
    if (forStmt->condition) {
        auto condType = analyzeExpression(forStmt->condition);
        if (condType && !TypeChecker::isBoolean(condType)) {
            reportError("For condition must be boolean, got '" +
                       TypeChecker::typeToString(condType) + "'",
                       forStmt->condition->location);
        }
    }
    
    // Analyze increment
    if (forStmt->increment) {
        analyzeExpression(forStmt->increment);
    }
    
    // Analyze body
    bool wasInLoop = inLoop_;
    inLoop_ = true;
    analyzeStatement(forStmt->body);
    inLoop_ = wasInLoop;
    
    symbolTable_->popScope();
}

inline void SemanticAnalyzer::analyzeReturnStmt(
    const std::shared_ptr<ReturnStmt>& ret) {
    
    if (!currentFunctionReturnType_) {
        reportError("Return statement outside of function", ret->location);
        return;
    }
    
    TypePtr retType = nullptr;
    if (ret->value) {
        retType = analyzeExpression(ret->value);
    }
    
    if (TypeChecker::isVoid(currentFunctionReturnType_)) {
        if (retType) {
            reportError("Void function cannot return a value", ret->location);
        }
    } else {
        if (!retType) {
            reportError("Non-void function must return a value", ret->location);
        } else if (!TypeChecker::isAssignable(currentFunctionReturnType_, retType)) {
            reportError("Return type mismatch: expected '" +
                       TypeChecker::typeToString(currentFunctionReturnType_) +
                       "', got '" + TypeChecker::typeToString(retType) + "'",
                       ret->location);
        }
    }
}

inline void SemanticAnalyzer::analyzeExpressionStmt(
    const std::shared_ptr<ExpressionStmt>& exprStmt) {
    
    if (exprStmt->expression) {
        analyzeExpression(exprStmt->expression);
    }
}

inline TypePtr SemanticAnalyzer::analyzeExpression(const ExprPtr& expr) {
    if (!expr) return nullptr;
    
    if (auto binary = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
        expr->inferredType = analyzeBinaryExpr(binary);
    } else if (auto unary = std::dynamic_pointer_cast<UnaryExpr>(expr)) {
        expr->inferredType = analyzeUnaryExpr(unary);
    } else if (auto call = std::dynamic_pointer_cast<CallExpr>(expr)) {
        expr->inferredType = analyzeCallExpr(call);
    } else if (auto ident = std::dynamic_pointer_cast<Identifier>(expr)) {
        expr->inferredType = analyzeIdentifier(ident);
    } else if (auto lit = std::dynamic_pointer_cast<IntegerLiteral>(expr)) {
        expr->inferredType = analyzeIntegerLiteral(lit);
    } else if (auto lit = std::dynamic_pointer_cast<FloatLiteral>(expr)) {
        expr->inferredType = analyzeFloatLiteral(lit);
    } else if (auto lit = std::dynamic_pointer_cast<StringLiteral>(expr)) {
        expr->inferredType = analyzeStringLiteral(lit);
    } else if (auto lit = std::dynamic_pointer_cast<BoolLiteral>(expr)) {
        expr->inferredType = analyzeBoolLiteral(lit);
    }
    
    return expr->inferredType;
}

inline TypePtr SemanticAnalyzer::analyzeBinaryExpr(
    const std::shared_ptr<BinaryExpr>& binary) {
    
    auto leftType = analyzeExpression(binary->left);
    auto rightType = analyzeExpression(binary->right);
    
    if (!leftType || !rightType) {
        return nullptr;
    }
    
    try {
        return TypeChecker::getBinaryOpResultType(binary->op, leftType, 
                                                  rightType, binary->location);
    } catch (const TypeError& e) {
        reportError(e.errorMessage, e.location);
        return nullptr;
    }
}

inline TypePtr SemanticAnalyzer::analyzeUnaryExpr(
    const std::shared_ptr<UnaryExpr>& unary) {
    
    auto operandType = analyzeExpression(unary->operand);
    
    if (!operandType) {
        return nullptr;
    }
    
    try {
        return TypeChecker::getUnaryOpResultType(unary->op, operandType, 
                                                 unary->location);
    } catch (const TypeError& e) {
        reportError(e.errorMessage, e.location);
        return nullptr;
    }
}

inline TypePtr SemanticAnalyzer::analyzeCallExpr(
    const std::shared_ptr<CallExpr>& call) {
    
    auto calleeType = analyzeExpression(call->callee);
    
    // Get function symbol
    if (auto ident = std::dynamic_pointer_cast<Identifier>(call->callee)) {
        auto symbolOpt = symbolTable_->lookup(ident->name);
        
        if (!symbolOpt) {
            reportError("Undefined function '" + ident->name + "'", ident->location);
            return nullptr;
        }
        
        auto symbol = symbolOpt.value();
        
        if (symbol->kind != SymbolKind::FUNCTION) {
            reportError("'" + ident->name + "' is not a function", ident->location);
            return nullptr;
        }
        
        // Check argument count
        if (call->arguments.size() != symbol->parameterTypes.size()) {
            reportError("Function '" + ident->name + "' expects " +
                       std::to_string(symbol->parameterTypes.size()) +
                       " arguments, but got " + 
                       std::to_string(call->arguments.size()),
                       call->location);
            return symbol->type;
        }
        
        // Check argument types
        for (size_t i = 0; i < call->arguments.size(); ++i) {
            auto argType = analyzeExpression(call->arguments[i]);
            if (argType && !TypeChecker::isAssignable(symbol->parameterTypes[i], 
                                                       argType)) {
                reportError("Argument " + std::to_string(i + 1) + 
                           " type mismatch: expected '" +
                           TypeChecker::typeToString(symbol->parameterTypes[i]) +
                           "', got '" + TypeChecker::typeToString(argType) + "'",
                           call->arguments[i]->location);
            }
        }
        
        return symbol->type;
    }
    
    return nullptr;
}

inline TypePtr SemanticAnalyzer::analyzeIdentifier(
    const std::shared_ptr<Identifier>& ident) {
    
    auto symbolOpt = symbolTable_->lookup(ident->name);
    
    if (!symbolOpt) {
        reportError("Undefined variable '" + ident->name + "'", ident->location);
        return nullptr;
    }
    
    return symbolOpt.value()->type;
}

inline TypePtr SemanticAnalyzer::analyzeIntegerLiteral(
    const std::shared_ptr<IntegerLiteral>& lit) {
    return TypeChecker::createPrimitiveType(PrimitiveTypeKind::INT, lit->location);
}

inline TypePtr SemanticAnalyzer::analyzeFloatLiteral(
    const std::shared_ptr<FloatLiteral>& lit) {
    return TypeChecker::createPrimitiveType(PrimitiveTypeKind::FLOAT, lit->location);
}

inline TypePtr SemanticAnalyzer::analyzeStringLiteral(
    const std::shared_ptr<StringLiteral>& lit) {
    return TypeChecker::createPrimitiveType(PrimitiveTypeKind::STRING, lit->location);
}

inline TypePtr SemanticAnalyzer::analyzeBoolLiteral(
    const std::shared_ptr<BoolLiteral>& lit) {
    return TypeChecker::createPrimitiveType(PrimitiveTypeKind::BOOL, lit->location);
}

inline TypePtr SemanticAnalyzer::resolveType(const TypePtr& type) {
    if (!type) return nullptr;
    
    if (type->nodeType == ASTNodeType::NAMED_TYPE) {
        auto named = std::dynamic_pointer_cast<NamedType>(type);
        auto symbolOpt = symbolTable_->lookup(named->name);
        
        if (!symbolOpt) {
            reportError("Undefined type '" + named->name + "'", named->location);
            return nullptr;
        }
        
        auto symbol = symbolOpt.value();
        if (symbol->kind != SymbolKind::STRUCT && 
            symbol->kind != SymbolKind::ENUM) {
            reportError("'" + named->name + "' is not a type", named->location);
            return nullptr;
        }
        
        return type;
    }
    
    return type;
}

} // namespace nova

#endif // NOVA_SEMANTIC_ANALYZER_H
