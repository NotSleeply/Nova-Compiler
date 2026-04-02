/**
 * @file ast.h
 * @brief Abstract Syntax Tree (AST) definitions for Nova Compiler
 * @author Nova Compiler Team
 * @version 0.1.0
 * @date 2026-04-02
 * 
 * The AST represents the hierarchical structure of the source code.
 * Each node in the tree represents a construct in the program.
 */

#ifndef NOVA_AST_H
#define NOVA_AST_H

#include "lexer/token.h"
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <iostream>
#include <sstream>

namespace nova {

// Forward declarations
class ASTNode;
class Expression;
class Statement;
class Declaration;
class Type;

// Smart pointer aliases
using ASTNodePtr = std::shared_ptr<ASTNode>;
using ExprPtr = std::shared_ptr<Expression>;
using StmtPtr = std::shared_ptr<Statement>;
using DeclPtr = std::shared_ptr<Declaration>;
using TypePtr = std::shared_ptr<Type>;

/**
 * @enum ASTNodeType
 * @brief Enumeration of all AST node types
 */
enum class ASTNodeType {
    // Expressions
    INTEGER_LITERAL_EXPR,
    FLOAT_LITERAL_EXPR,
    STRING_LITERAL_EXPR,
    CHAR_LITERAL_EXPR,
    BOOL_LITERAL_EXPR,
    NULL_LITERAL_EXPR,
    IDENTIFIER_EXPR,
    BINARY_EXPR,
    UNARY_EXPR,
    CALL_EXPR,
    INDEX_EXPR,
    MEMBER_EXPR,
    ASSIGNMENT_EXPR,
    TERNARY_EXPR,
    ARRAY_LITERAL_EXPR,
    LAMBDA_EXPR,
    
    // Statements
    EXPRESSION_STMT,
    BLOCK_STMT,
    IF_STMT,
    WHILE_STMT,
    FOR_STMT,
    RETURN_STMT,
    BREAK_STMT,
    CONTINUE_STMT,
    
    // Declarations
    VARIABLE_DECL,
    FUNCTION_DECL,
    PARAM_DECL,
    STRUCT_DECL,
    ENUM_DECL,
    ENUM_VARIANT_DECL,
    FIELD_DECL,
    
    // Types
    PRIMITIVE_TYPE,
    NAMED_TYPE,
    ARRAY_TYPE,
    OPTIONAL_TYPE,
    FUNCTION_TYPE,
    
    // Program
    PROGRAM
};

/**
 * @brief Convert ASTNodeType to string
 */
inline std::string astNodeTypeToString(ASTNodeType type) {
    switch (type) {
        case ASTNodeType::INTEGER_LITERAL_EXPR: return "IntegerLiteral";
        case ASTNodeType::FLOAT_LITERAL_EXPR: return "FloatLiteral";
        case ASTNodeType::STRING_LITERAL_EXPR: return "StringLiteral";
        case ASTNodeType::CHAR_LITERAL_EXPR: return "CharLiteral";
        case ASTNodeType::BOOL_LITERAL_EXPR: return "BoolLiteral";
        case ASTNodeType::NULL_LITERAL_EXPR: return "NullLiteral";
        case ASTNodeType::IDENTIFIER_EXPR: return "Identifier";
        case ASTNodeType::BINARY_EXPR: return "BinaryExpr";
        case ASTNodeType::UNARY_EXPR: return "UnaryExpr";
        case ASTNodeType::CALL_EXPR: return "CallExpr";
        case ASTNodeType::INDEX_EXPR: return "IndexExpr";
        case ASTNodeType::MEMBER_EXPR: return "MemberExpr";
        case ASTNodeType::ASSIGNMENT_EXPR: return "AssignmentExpr";
        case ASTNodeType::TERNARY_EXPR: return "TernaryExpr";
        case ASTNodeType::ARRAY_LITERAL_EXPR: return "ArrayLiteral";
        case ASTNodeType::LAMBDA_EXPR: return "LambdaExpr";
        case ASTNodeType::EXPRESSION_STMT: return "ExpressionStmt";
        case ASTNodeType::BLOCK_STMT: return "BlockStmt";
        case ASTNodeType::IF_STMT: return "IfStmt";
        case ASTNodeType::WHILE_STMT: return "WhileStmt";
        case ASTNodeType::FOR_STMT: return "ForStmt";
        case ASTNodeType::RETURN_STMT: return "ReturnStmt";
        case ASTNodeType::BREAK_STMT: return "BreakStmt";
        case ASTNodeType::CONTINUE_STMT: return "ContinueStmt";
        case ASTNodeType::VARIABLE_DECL: return "VariableDecl";
        case ASTNodeType::FUNCTION_DECL: return "FunctionDecl";
        case ASTNodeType::PARAM_DECL: return "ParamDecl";
        case ASTNodeType::STRUCT_DECL: return "StructDecl";
        case ASTNodeType::ENUM_DECL: return "EnumDecl";
        case ASTNodeType::ENUM_VARIANT_DECL: return "EnumVariantDecl";
        case ASTNodeType::FIELD_DECL: return "FieldDecl";
        case ASTNodeType::PRIMITIVE_TYPE: return "PrimitiveType";
        case ASTNodeType::NAMED_TYPE: return "NamedType";
        case ASTNodeType::ARRAY_TYPE: return "ArrayType";
        case ASTNodeType::OPTIONAL_TYPE: return "OptionalType";
        case ASTNodeType::FUNCTION_TYPE: return "FunctionType";
        case ASTNodeType::PROGRAM: return "Program";
        default: return "Unknown";
    }
}

/**
 * @class ASTNode
 * @brief Base class for all AST nodes
 */
class ASTNode {
public:
    ASTNodeType nodeType;
    Location location;
    
    ASTNode(ASTNodeType type, const Location& loc) 
        : nodeType(type), location(loc) {}
    
    virtual ~ASTNode() = default;
    
    /**
     * @brief Accept a visitor (for the visitor pattern)
     */
    // virtual void accept(ASTVisitor& visitor) = 0;
    
    /**
     * @brief Convert to string representation (for debugging)
     */
    virtual std::string toString(int indent = 0) const {
        return std::string(indent, ' ') + astNodeTypeToString(nodeType);
    }
    
    /**
     * @brief Pretty print the AST
     */
    void print(int indent = 0) const {
        std::cout << toString(indent) << std::endl;
    }
};

// ============================================================================
// Type System
// ============================================================================

/**
 * @enum PrimitiveTypeKind
 * @brief Built-in primitive types
 */
enum class PrimitiveTypeKind {
    VOID,
    INT,
    FLOAT,
    STRING,
    BOOL,
    CHAR
};

/**
 * @class Type
 * @brief Base class for type nodes
 */
class Type : public ASTNode {
public:
    Type(ASTNodeType type, const Location& loc) : ASTNode(type, loc) {}
};

/**
 * @class PrimitiveType
 * @brief Represents a primitive type (int, float, string, etc.)
 */
class PrimitiveType : public Type {
public:
    PrimitiveTypeKind kind;
    
    PrimitiveType(PrimitiveTypeKind k, const Location& loc)
        : Type(ASTNodeType::PRIMITIVE_TYPE, loc), kind(k) {}
    
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "PrimitiveType: ";
        switch (kind) {
            case PrimitiveTypeKind::VOID: result += "void"; break;
            case PrimitiveTypeKind::INT: result += "int"; break;
            case PrimitiveTypeKind::FLOAT: result += "float"; break;
            case PrimitiveTypeKind::STRING: result += "string"; break;
            case PrimitiveTypeKind::BOOL: result += "bool"; break;
            case PrimitiveTypeKind::CHAR: result += "char"; break;
        }
        return result;
    }
};

/**
 * @class NamedType
 * @brief Represents a user-defined type (struct, enum, etc.)
 */
class NamedType : public Type {
public:
    std::string name;
    
    NamedType(const std::string& n, const Location& loc)
        : Type(ASTNodeType::NAMED_TYPE, loc), name(n) {}
    
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "NamedType: " + name;
    }
};

/**
 * @class ArrayType
 * @brief Represents an array type
 */
class ArrayType : public Type {
public:
    TypePtr elementType;
    
    ArrayType(TypePtr elem, const Location& loc)
        : Type(ASTNodeType::ARRAY_TYPE, loc), elementType(elem) {}
    
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ArrayType of " + 
               (elementType ? elementType->toString(0) : "unknown");
    }
};

// ============================================================================
// Expressions
// ============================================================================

/**
 * @class Expression
 * @brief Base class for expression nodes
 */
class Expression : public ASTNode {
public:
    TypePtr inferredType;  // Type inference result
    
    Expression(ASTNodeType type, const Location& loc) 
        : ASTNode(type, loc) {}
};

/**
 * @class IntegerLiteral
 * @brief Integer literal expression (e.g., 42, 0xFF)
 */
class IntegerLiteral : public Expression {
public:
    int64_t value;
    
    IntegerLiteral(int64_t val, const Location& loc)
        : Expression(ASTNodeType::INTEGER_LITERAL_EXPR, loc), value(val) {}
    
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "IntegerLiteral: " + 
               std::to_string(value);
    }
};

/**
 * @class FloatLiteral
 * @brief Float literal expression (e.g., 3.14)
 */
class FloatLiteral : public Expression {
public:
    double value;
    
    FloatLiteral(double val, const Location& loc)
        : Expression(ASTNodeType::FLOAT_LITERAL_EXPR, loc), value(val) {}
    
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "FloatLiteral: " + 
               std::to_string(value);
    }
};

/**
 * @class StringLiteral
 * @brief String literal expression (e.g., "hello")
 */
class StringLiteral : public Expression {
public:
    std::string value;
    
    StringLiteral(const std::string& val, const Location& loc)
        : Expression(ASTNodeType::STRING_LITERAL_EXPR, loc), value(val) {}
    
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "StringLiteral: \"" + value + "\"";
    }
};

/**
 * @class BoolLiteral
 * @brief Boolean literal expression (true/false)
 */
class BoolLiteral : public Expression {
public:
    bool value;
    
    BoolLiteral(bool val, const Location& loc)
        : Expression(ASTNodeType::BOOL_LITERAL_EXPR, loc), value(val) {}
    
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "BoolLiteral: " + 
               (value ? "true" : "false");
    }
};

/**
 * @class Identifier
 * @brief Identifier expression (variable name, function name, etc.)
 */
class Identifier : public Expression {
public:
    std::string name;
    
    Identifier(const std::string& n, const Location& loc)
        : Expression(ASTNodeType::IDENTIFIER_EXPR, loc), name(n) {}
    
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "Identifier: " + name;
    }
};

/**
 * @class BinaryExpr
 * @brief Binary expression (e.g., a + b, x == y)
 */
class BinaryExpr : public Expression {
public:
    ExprPtr left;
    TokenType op;
    ExprPtr right;
    
    BinaryExpr(ExprPtr l, TokenType o, ExprPtr r, const Location& loc)
        : Expression(ASTNodeType::BINARY_EXPR, loc), 
          left(l), op(o), right(r) {}
    
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "BinaryExpr:\n";
        result += left->toString(indent + 2) + "\n";
        result += std::string(indent + 2, ' ') + "Op: " + tokenTypeToString(op) + "\n";
        result += right->toString(indent + 2);
        return result;
    }
};

/**
 * @class UnaryExpr
 * @brief Unary expression (e.g., -x, !flag)
 */
class UnaryExpr : public Expression {
public:
    TokenType op;
    ExprPtr operand;
    bool isPrefix;  // true for prefix (++x), false for postfix (x++)
    
    UnaryExpr(TokenType o, ExprPtr opnd, bool prefix, const Location& loc)
        : Expression(ASTNodeType::UNARY_EXPR, loc), 
          op(o), operand(opnd), isPrefix(prefix) {}
    
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "UnaryExpr: " + 
               tokenTypeToString(op) + " (" + 
               (isPrefix ? "prefix" : "postfix") + ")";
    }
};

/**
 * @class CallExpr
 * @brief Function call expression (e.g., foo(a, b))
 */
class CallExpr : public Expression {
public:
    ExprPtr callee;
    std::vector<ExprPtr> arguments;
    
    CallExpr(ExprPtr c, std::vector<ExprPtr> args, const Location& loc)
        : Expression(ASTNodeType::CALL_EXPR, loc), 
          callee(c), arguments(std::move(args)) {}
    
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "CallExpr:\n";
        result += callee->toString(indent + 2) + "\n";
        result += std::string(indent + 2, ' ') + "Arguments:\n";
        for (const auto& arg : arguments) {
            result += arg->toString(indent + 4) + "\n";
        }
        return result;
    }
};

// ============================================================================
// Statements
// ============================================================================

/**
 * @class Statement
 * @brief Base class for statement nodes
 */
class Statement : public ASTNode {
public:
    Statement(ASTNodeType type, const Location& loc) 
        : ASTNode(type, loc) {}
};

/**
 * @class ExpressionStmt
 * @brief Expression statement (expression followed by semicolon)
 */
class ExpressionStmt : public Statement {
public:
    ExprPtr expression;
    
    ExpressionStmt(ExprPtr expr, const Location& loc)
        : Statement(ASTNodeType::EXPRESSION_STMT, loc), expression(expr) {}
    
    std::string toString(int indent = 0) const override {
        return expression->toString(indent);
    }
};

/**
 * @class BlockStmt
 * @brief Block statement (a sequence of statements in braces)
 */
class BlockStmt : public Statement {
public:
    std::vector<StmtPtr> statements;
    
    BlockStmt(std::vector<StmtPtr> stmts, const Location& loc)
        : Statement(ASTNodeType::BLOCK_STMT, loc), 
          statements(std::move(stmts)) {}
    
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "BlockStmt:\n";
        for (const auto& stmt : statements) {
            result += stmt->toString(indent + 2) + "\n";
        }
        return result;
    }
};

/**
 * @class IfStmt
 * @brief If statement with optional else clause
 */
class IfStmt : public Statement {
public:
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;  // Can be nullptr
    
    IfStmt(ExprPtr cond, StmtPtr then, StmtPtr els, const Location& loc)
        : Statement(ASTNodeType::IF_STMT, loc), 
          condition(cond), thenBranch(then), elseBranch(els) {}
    
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "IfStmt:\n";
        result += std::string(indent + 2, ' ') + "Condition:\n";
        result += condition->toString(indent + 4) + "\n";
        result += std::string(indent + 2, ' ') + "Then:\n";
        result += thenBranch->toString(indent + 4);
        if (elseBranch) {
            result += "\n" + std::string(indent + 2, ' ') + "Else:\n";
            result += elseBranch->toString(indent + 4);
        }
        return result;
    }
};

/**
 * @class WhileStmt
 * @brief While loop statement
 */
class WhileStmt : public Statement {
public:
    ExprPtr condition;
    StmtPtr body;
    
    WhileStmt(ExprPtr cond, StmtPtr b, const Location& loc)
        : Statement(ASTNodeType::WHILE_STMT, loc), 
          condition(cond), body(b) {}
    
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "WhileStmt:\n";
        result += std::string(indent + 2, ' ') + "Condition:\n";
        result += condition->toString(indent + 4) + "\n";
        result += std::string(indent + 2, ' ') + "Body:\n";
        result += body->toString(indent + 4);
        return result;
    }
};

/**
 * @class ForStmt
 * @brief For loop statement
 */
class ForStmt : public Statement {
public:
    StmtPtr initializer;   // Can be nullptr
    ExprPtr condition;     // Can be nullptr
    ExprPtr increment;     // Can be nullptr
    StmtPtr body;
    
    ForStmt(StmtPtr init, ExprPtr cond, ExprPtr incr, StmtPtr b, 
            const Location& loc)
        : Statement(ASTNodeType::FOR_STMT, loc), 
          initializer(init), condition(cond), increment(incr), body(b) {}
    
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "ForStmt:\n";
        if (initializer) {
            result += std::string(indent + 2, ' ') + "Initializer:\n";
            result += initializer->toString(indent + 4) + "\n";
        }
        if (condition) {
            result += std::string(indent + 2, ' ') + "Condition:\n";
            result += condition->toString(indent + 4) + "\n";
        }
        if (increment) {
            result += std::string(indent + 2, ' ') + "Increment:\n";
            result += increment->toString(indent + 4) + "\n";
        }
        result += std::string(indent + 2, ' ') + "Body:\n";
        result += body->toString(indent + 4);
        return result;
    }
};

/**
 * @class ReturnStmt
 * @brief Return statement
 */
class ReturnStmt : public Statement {
public:
    ExprPtr value;  // Can be nullptr for void returns
    
    ReturnStmt(ExprPtr val, const Location& loc)
        : Statement(ASTNodeType::RETURN_STMT, loc), value(val) {}
    
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "ReturnStmt";
        if (value) {
            result += ":\n" + value->toString(indent + 2);
        }
        return result;
    }
};

// ============================================================================
// Declarations
// ============================================================================

/**
 * @class Declaration
 * @brief Base class for declaration nodes
 */
class Declaration : public ASTNode {
public:
    std::string name;
    
    Declaration(ASTNodeType type, const std::string& n, const Location& loc)
        : ASTNode(type, loc), name(n) {}
};

/**
 * @class ParamDecl
 * @brief Function parameter declaration
 */
class ParamDecl : public Declaration {
public:
    TypePtr type;
    
    ParamDecl(const std::string& n, TypePtr t, const Location& loc)
        : Declaration(ASTNodeType::PARAM_DECL, n, loc), type(t) {}
    
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "ParamDecl: " + name + 
               (type ? " : " + type->toString(0) : "");
    }
};

/**
 * @class VariableDecl
 * @brief Variable declaration (let or const)
 */
class VariableDecl : public Declaration {
public:
    TypePtr type;         // Can be nullptr for type inference
    ExprPtr initializer;  // Can be nullptr
    bool isConst;
    
    VariableDecl(const std::string& n, TypePtr t, ExprPtr init, 
                 bool constant, const Location& loc)
        : Declaration(ASTNodeType::VARIABLE_DECL, n, loc), 
          type(t), initializer(init), isConst(constant) {}
    
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + 
                            (isConst ? "ConstDecl: " : "VarDecl: ") + name;
        if (type) {
            result += " : " + type->toString(0);
        }
        if (initializer) {
            result += "\n" + std::string(indent + 2, ' ') + "Initializer:\n";
            result += initializer->toString(indent + 4);
        }
        return result;
    }
};

/**
 * @class FunctionDecl
 * @brief Function declaration
 */
class FunctionDecl : public Declaration {
public:
    std::vector<std::shared_ptr<ParamDecl>> parameters;
    TypePtr returnType;  // Can be nullptr for void or inferred
    StmtPtr body;        // BlockStmt
    
    FunctionDecl(const std::string& n, 
                 std::vector<std::shared_ptr<ParamDecl>> params,
                 TypePtr ret, StmtPtr b, const Location& loc)
        : Declaration(ASTNodeType::FUNCTION_DECL, n, loc), 
          parameters(std::move(params)), returnType(ret), body(b) {}
    
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "FunctionDecl: " + name + "\n";
        result += std::string(indent + 2, ' ') + "Parameters:\n";
        for (const auto& param : parameters) {
            result += param->toString(indent + 4) + "\n";
        }
        if (returnType) {
            result += std::string(indent + 2, ' ') + "Return Type: " + 
                     returnType->toString(0) + "\n";
        }
        result += std::string(indent + 2, ' ') + "Body:\n";
        result += body->toString(indent + 4);
        return result;
    }
};

/**
 * @class FieldDecl
 * @brief Struct field declaration
 */
class FieldDecl : public Declaration {
public:
    TypePtr type;
    
    FieldDecl(const std::string& n, TypePtr t, const Location& loc)
        : Declaration(ASTNodeType::FIELD_DECL, n, loc), type(t) {}
    
    std::string toString(int indent = 0) const override {
        return std::string(indent, ' ') + "FieldDecl: " + name + 
               " : " + (type ? type->toString(0) : "unknown");
    }
};

/**
 * @class StructDecl
 * @brief Struct declaration
 */
class StructDecl : public Declaration {
public:
    std::vector<std::shared_ptr<FieldDecl>> fields;
    
    StructDecl(const std::string& n, 
               std::vector<std::shared_ptr<FieldDecl>> f,
               const Location& loc)
        : Declaration(ASTNodeType::STRUCT_DECL, n, loc), fields(std::move(f)) {}
    
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "StructDecl: " + name + "\n";
        result += std::string(indent + 2, ' ') + "Fields:\n";
        for (const auto& field : fields) {
            result += field->toString(indent + 4) + "\n";
        }
        return result;
    }
};

/**
 * @class Program
 * @brief Root node representing the entire program
 */
class Program : public ASTNode {
public:
    std::vector<DeclPtr> declarations;
    
    Program(const Location& loc)
        : ASTNode(ASTNodeType::PROGRAM, loc) {}
    
    std::string toString(int indent = 0) const override {
        std::string result = std::string(indent, ' ') + "Program:\n";
        for (const auto& decl : declarations) {
            result += decl->toString(indent + 2) + "\n";
        }
        return result;
    }
};

// Helper functions for creating AST nodes
namespace ast {

template<typename T, typename... Args>
std::shared_ptr<T> make(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

} // namespace ast

} // namespace nova

#endif // NOVA_AST_H
