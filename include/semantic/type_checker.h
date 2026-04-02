/**
 * @file type_checker.h
 * @brief Type checking utilities for Nova Compiler
 * @author Nova Compiler Team
 * @version 0.2.0
 * @date 2026-04-02
 * 
 * Provides type checking and type compatibility functions.
 */

#ifndef NOVA_TYPE_CHECKER_H
#define NOVA_TYPE_CHECKER_H

#include "ast/ast.h"
#include "lexer/token.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace nova {

/**
 * @class TypeError
 * @brief Exception thrown when type checking fails
 */
class TypeError : public std::runtime_error {
public:
    TypeError(const std::string& message, const Location& loc)
        : std::runtime_error(message + " at " + loc.toString()),
          location(loc), errorMessage(message) {}
    
    Location location;
    std::string errorMessage;
};

/**
 * @class TypeChecker
 * @brief Utility class for type operations and compatibility checking
 */
class TypeChecker {
public:
    /**
     * @brief Check if two types are equal
     */
    static bool typesEqual(const TypePtr& t1, const TypePtr& t2);
    
    /**
     * @brief Check if source type can be assigned to target type
     */
    static bool isAssignable(const TypePtr& target, const TypePtr& source);
    
    /**
     * @brief Check if a type is a numeric type (int or float)
     */
    static bool isNumeric(const TypePtr& type);
    
    /**
     * @brief Check if a type is a boolean type
     */
    static bool isBoolean(const TypePtr& type);
    
    /**
     * @brief Check if a type is a string type
     */
    static bool isString(const TypePtr& type);
    
    /**
     * @brief Check if a type is void
     */
    static bool isVoid(const TypePtr& type);
    
    /**
     * @brief Get the common type for binary operations
     */
    static TypePtr getCommonType(const TypePtr& t1, const TypePtr& t2);
    
    /**
     * @brief Get the result type for a binary operator
     */
    static TypePtr getBinaryOpResultType(TokenType op, const TypePtr& left, 
                                         const TypePtr& right, const Location& loc);
    
    /**
     * @brief Get the result type for a unary operator
     */
    static TypePtr getUnaryOpResultType(TokenType op, const TypePtr& operand,
                                        const Location& loc);
    
    /**
     * @brief Create a primitive type
     */
    static TypePtr createPrimitiveType(PrimitiveTypeKind kind, const Location& loc);
    
    /**
     * @brief Create a named type
     */
    static TypePtr createNamedType(const std::string& name, const Location& loc);
    
    /**
     * @brief Get the string representation of a type
     */
    static std::string typeToString(const TypePtr& type);
    
    /**
     * @brief Get the type kind for binary comparison operators
     */
    static TypePtr getComparisonResultType(const TypePtr& left, const TypePtr& right);
};

// Implementation

inline bool TypeChecker::typesEqual(const TypePtr& t1, const TypePtr& t2) {
    if (!t1 || !t2) return false;
    
    // Both are primitive types
    if (t1->nodeType == ASTNodeType::PRIMITIVE_TYPE && 
        t2->nodeType == ASTNodeType::PRIMITIVE_TYPE) {
        auto p1 = std::dynamic_pointer_cast<PrimitiveType>(t1);
        auto p2 = std::dynamic_pointer_cast<PrimitiveType>(t2);
        return p1->kind == p2->kind;
    }
    
    // Both are named types
    if (t1->nodeType == ASTNodeType::NAMED_TYPE && 
        t2->nodeType == ASTNodeType::NAMED_TYPE) {
        auto n1 = std::dynamic_pointer_cast<NamedType>(t1);
        auto n2 = std::dynamic_pointer_cast<NamedType>(t2);
        return n1->name == n2->name;
    }
    
    // Both are array types
    if (t1->nodeType == ASTNodeType::ARRAY_TYPE && 
        t2->nodeType == ASTNodeType::ARRAY_TYPE) {
        auto a1 = std::dynamic_pointer_cast<ArrayType>(t1);
        auto a2 = std::dynamic_pointer_cast<ArrayType>(t2);
        return typesEqual(a1->elementType, a2->elementType);
    }
    
    return false;
}

inline bool TypeChecker::isAssignable(const TypePtr& target, const TypePtr& source) {
    if (!target || !source) return false;
    
    // Same types are always assignable
    if (typesEqual(target, source)) return true;
    
    // int can be assigned to float (implicit conversion)
    if (target->nodeType == ASTNodeType::PRIMITIVE_TYPE &&
        source->nodeType == ASTNodeType::PRIMITIVE_TYPE) {
        auto t = std::dynamic_pointer_cast<PrimitiveType>(target);
        auto s = std::dynamic_pointer_cast<PrimitiveType>(source);
        
        if (t->kind == PrimitiveTypeKind::FLOAT && 
            s->kind == PrimitiveTypeKind::INT) {
            return true;
        }
    }
    
    return false;
}

inline bool TypeChecker::isNumeric(const TypePtr& type) {
    if (!type || type->nodeType != ASTNodeType::PRIMITIVE_TYPE) {
        return false;
    }
    auto prim = std::dynamic_pointer_cast<PrimitiveType>(type);
    return prim->kind == PrimitiveTypeKind::INT || 
           prim->kind == PrimitiveTypeKind::FLOAT;
}

inline bool TypeChecker::isBoolean(const TypePtr& type) {
    if (!type || type->nodeType != ASTNodeType::PRIMITIVE_TYPE) {
        return false;
    }
    auto prim = std::dynamic_pointer_cast<PrimitiveType>(type);
    return prim->kind == PrimitiveTypeKind::BOOL;
}

inline bool TypeChecker::isString(const TypePtr& type) {
    if (!type || type->nodeType != ASTNodeType::PRIMITIVE_TYPE) {
        return false;
    }
    auto prim = std::dynamic_pointer_cast<PrimitiveType>(type);
    return prim->kind == PrimitiveTypeKind::STRING;
}

inline bool TypeChecker::isVoid(const TypePtr& type) {
    if (!type || type->nodeType != ASTNodeType::PRIMITIVE_TYPE) {
        return false;
    }
    auto prim = std::dynamic_pointer_cast<PrimitiveType>(type);
    return prim->kind == PrimitiveTypeKind::VOID;
}

inline TypePtr TypeChecker::getCommonType(const TypePtr& t1, const TypePtr& t2) {
    if (typesEqual(t1, t2)) return t1;
    
    // int + float = float
    if (isNumeric(t1) && isNumeric(t2)) {
        if (isNumeric(t1) && isNumeric(t2)) {
            auto p1 = std::dynamic_pointer_cast<PrimitiveType>(t1);
            auto p2 = std::dynamic_pointer_cast<PrimitiveType>(t2);
            
            if (p1->kind == PrimitiveTypeKind::FLOAT || 
                p2->kind == PrimitiveTypeKind::FLOAT) {
                return createPrimitiveType(PrimitiveTypeKind::FLOAT, t1->location);
            }
        }
    }
    
    return nullptr;
}

inline TypePtr TypeChecker::getBinaryOpResultType(TokenType op, 
                                                   const TypePtr& left, 
                                                   const TypePtr& right,
                                                   const Location& loc) {
    using tk = TokenType;
    
    // Arithmetic operators (+, -, *, /, %, **)
    if (op == tk::OP_PLUS || op == tk::OP_MINUS || op == tk::OP_STAR || 
        op == tk::OP_SLASH || op == tk::OP_PERCENT || op == tk::OP_STAR_STAR) {
        
        // String concatenation
        if (op == tk::OP_PLUS && (isString(left) || isString(right))) {
            return createPrimitiveType(PrimitiveTypeKind::STRING, loc);
        }
        
        // Numeric operations
        if (isNumeric(left) && isNumeric(right)) {
            // int op int = int
            // float op float = float
            // int op float = float
            return getCommonType(left, right);
        }
        
        throw TypeError("Invalid operand types for operator '" + 
                       tokenTypeToString(op) + "'", loc);
    }
    
    // Comparison operators (==, !=, <, <=, >, >=)
    if (op == tk::OP_EQUAL_EQUAL || op == tk::OP_BANG_EQUAL ||
        op == tk::OP_LESS || op == tk::OP_LESS_EQUAL ||
        op == tk::OP_GREATER || op == tk::OP_GREATER_EQUAL) {
        
        // Can compare same types
        if (typesEqual(left, right) || 
            (isNumeric(left) && isNumeric(right))) {
            return createPrimitiveType(PrimitiveTypeKind::BOOL, loc);
        }
        
        throw TypeError("Cannot compare different types", loc);
    }
    
    // Logical operators (&&, ||)
    if (op == tk::OP_AND || op == tk::OP_OR) {
        if (!isBoolean(left) || !isBoolean(right)) {
            throw TypeError("Logical operators require boolean operands", loc);
        }
        return createPrimitiveType(PrimitiveTypeKind::BOOL, loc);
    }
    
    // Bitwise operators (&, |, ^, <<, >>)
    if (op == tk::OP_AMPERSAND || op == tk::OP_PIPE || op == tk::OP_CARET ||
        op == tk::OP_LESS_LESS || op == tk::OP_GREATER_GREATER) {
        
        auto leftInt = std::dynamic_pointer_cast<PrimitiveType>(left);
        auto rightInt = std::dynamic_pointer_cast<PrimitiveType>(right);
        
        if (leftInt && leftInt->kind == PrimitiveTypeKind::INT &&
            rightInt && rightInt->kind == PrimitiveTypeKind::INT) {
            return left;
        }
        
        throw TypeError("Bitwise operators require integer operands", loc);
    }
    
    // Assignment operators (=, +=, -=, etc.)
    if (op == tk::OP_EQUAL || op == tk::OP_PLUS_EQUAL || 
        op == tk::OP_MINUS_EQUAL || op == tk::OP_STAR_EQUAL || 
        op == tk::OP_SLASH_EQUAL || op == tk::OP_PERCENT_EQUAL) {
        
        if (!isAssignable(left, right)) {
            throw TypeError("Cannot assign '" + typeToString(right) + 
                          "' to '" + typeToString(left) + "'", loc);
        }
        return left;
    }
    
    throw TypeError("Unknown binary operator: " + tokenTypeToString(op), loc);
}

inline TypePtr TypeChecker::getUnaryOpResultType(TokenType op, 
                                                  const TypePtr& operand,
                                                  const Location& loc) {
    using tk = TokenType;
    
    // Arithmetic negation
    if (op == tk::OP_MINUS) {
        if (!isNumeric(operand)) {
            throw TypeError("Unary minus requires numeric operand", loc);
        }
        return operand;
    }
    
    // Logical not
    if (op == tk::OP_BANG) {
        if (!isBoolean(operand)) {
            throw TypeError("Logical not requires boolean operand", loc);
        }
        return createPrimitiveType(PrimitiveTypeKind::BOOL, loc);
    }
    
    // Increment/decrement
    if (op == tk::OP_PLUS_PLUS || op == tk::OP_MINUS_MINUS) {
        auto prim = std::dynamic_pointer_cast<PrimitiveType>(operand);
        if (prim && prim->kind == PrimitiveTypeKind::INT) {
            return operand;
        }
        throw TypeError("Increment/decrement operators require integer operand", loc);
    }
    
    // Bitwise not
    if (op == tk::OP_TILDE) {
        auto prim = std::dynamic_pointer_cast<PrimitiveType>(operand);
        if (prim && prim->kind == PrimitiveTypeKind::INT) {
            return operand;
        }
        throw TypeError("Bitwise not requires integer operand", loc);
    }
    
    throw TypeError("Unknown unary operator: " + tokenTypeToString(op), loc);
}

inline TypePtr TypeChecker::createPrimitiveType(PrimitiveTypeKind kind, 
                                                 const Location& loc) {
    return std::make_shared<PrimitiveType>(kind, loc);
}

inline TypePtr TypeChecker::createNamedType(const std::string& name, 
                                            const Location& loc) {
    return std::make_shared<NamedType>(name, loc);
}

inline std::string TypeChecker::typeToString(const TypePtr& type) {
    if (!type) return "unknown";
    
    if (type->nodeType == ASTNodeType::PRIMITIVE_TYPE) {
        auto prim = std::dynamic_pointer_cast<PrimitiveType>(type);
        switch (prim->kind) {
            case PrimitiveTypeKind::VOID: return "void";
            case PrimitiveTypeKind::INT: return "int";
            case PrimitiveTypeKind::FLOAT: return "float";
            case PrimitiveTypeKind::STRING: return "string";
            case PrimitiveTypeKind::BOOL: return "bool";
            case PrimitiveTypeKind::CHAR: return "char";
        }
    }
    
    if (type->nodeType == ASTNodeType::NAMED_TYPE) {
        auto named = std::dynamic_pointer_cast<NamedType>(type);
        return named->name;
    }
    
    if (type->nodeType == ASTNodeType::ARRAY_TYPE) {
        auto arr = std::dynamic_pointer_cast<ArrayType>(type);
        return typeToString(arr->elementType) + "[]";
    }
    
    return "unknown";
}

inline TypePtr TypeChecker::getComparisonResultType(const TypePtr& left, 
                                                     const TypePtr& right) {
    return createPrimitiveType(PrimitiveTypeKind::BOOL, left->location);
}

} // namespace nova

#endif // NOVA_TYPE_CHECKER_H
