/**
 * @file symbol_table.h
 * @brief Symbol table implementation for Nova Compiler
 * @author Nova Compiler Team
 * @version 0.2.0
 * @date 2026-04-02
 * 
 * The symbol table manages all symbols (variables, functions, types)
 * and their scopes during semantic analysis.
 */

#ifndef NOVA_SYMBOL_TABLE_H
#define NOVA_SYMBOL_TABLE_H

#include "ast/ast.h"
#include "lexer/token.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>

namespace nova {

/**
 * @enum SymbolKind
 * @brief Types of symbols in the symbol table
 */
enum class SymbolKind {
    VARIABLE,      // Variable (let)
    CONSTANT,      // Constant (const)
    FUNCTION,      // Function
    PARAMETER,     // Function parameter
    STRUCT,        // Struct type
    ENUM,          // Enum type
    FIELD          // Struct field
};

/**
 * @brief Convert SymbolKind to string
 */
inline std::string symbolKindToString(SymbolKind kind) {
    switch (kind) {
        case SymbolKind::VARIABLE: return "variable";
        case SymbolKind::CONSTANT: return "constant";
        case SymbolKind::FUNCTION: return "function";
        case SymbolKind::PARAMETER: return "parameter";
        case SymbolKind::STRUCT: return "struct";
        case SymbolKind::ENUM: return "enum";
        case SymbolKind::FIELD: return "field";
        default: return "unknown";
    }
}

/**
 * @struct Symbol
 * @brief Represents a symbol in the symbol table
 */
struct Symbol {
    std::string name;           // Symbol name
    SymbolKind kind;            // Symbol kind
    TypePtr type;               // Symbol type
    Location location;          // Location of declaration
    bool isDefined;             // Is the symbol fully defined?
    int scopeLevel;             // Scope nesting level
    
    // For functions
    std::vector<TypePtr> parameterTypes;
    
    Symbol(const std::string& n, SymbolKind k, TypePtr t, 
           const Location& loc, int level)
        : name(n), kind(k), type(t), location(loc), 
          isDefined(true), scopeLevel(level) {}
    
    std::string toString() const {
        return symbolKindToString(kind) + " " + name + 
               (type ? " : " + type->toString(0) : "");
    }
};

using SymbolPtr = std::shared_ptr<Symbol>;

/**
 * @class Scope
 * @brief Represents a single scope in the scope stack
 */
class Scope {
public:
    explicit Scope(int level, Scope* parent = nullptr)
        : level_(level), parent_(parent) {}
    
    /**
     * @brief Define a new symbol in this scope
     * @return true if successful, false if symbol already exists
     */
    bool define(const SymbolPtr& symbol) {
        if (symbols_.find(symbol->name) != symbols_.end()) {
            return false;  // Symbol already defined
        }
        symbols_[symbol->name] = symbol;
        return true;
    }
    
    /**
     * @brief Look up a symbol in this scope only
     */
    std::optional<SymbolPtr> lookupLocal(const std::string& name) const {
        auto it = symbols_.find(name);
        if (it != symbols_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * @brief Look up a symbol in this scope or parent scopes
     */
    std::optional<SymbolPtr> lookup(const std::string& name) const {
        auto local = lookupLocal(name);
        if (local) return local;
        
        if (parent_) {
            return parent_->lookup(name);
        }
        return std::nullopt;
    }
    
    int getLevel() const { return level_; }
    Scope* getParent() const { return parent_; }
    
private:
    int level_;
    Scope* parent_;
    std::unordered_map<std::string, SymbolPtr> symbols_;
};

/**
 * @class SymbolTable
 * @brief Manages the scope stack and symbol lookup
 */
class SymbolTable {
public:
    SymbolTable() {
        // Create global scope
        pushScope();
        defineBuiltins();
    }
    
    /**
     * @brief Enter a new scope
     */
    void pushScope() {
        Scope* parent = scopes_.empty() ? nullptr : scopes_.back().get();
        scopes_.push_back(std::make_unique<Scope>(currentLevel_ + 1, parent));
        currentLevel_++;
    }
    
    /**
     * @brief Exit the current scope
     */
    void popScope() {
        if (currentLevel_ > 0) {
            scopes_.pop_back();
            currentLevel_--;
        }
    }
    
    /**
     * @brief Define a new symbol in the current scope
     * @return true if successful, false if symbol already exists
     */
    bool define(const SymbolPtr& symbol) {
        return currentScope()->define(symbol);
    }
    
    /**
     * @brief Look up a symbol (searches all visible scopes)
     */
    std::optional<SymbolPtr> lookup(const std::string& name) const {
        return currentScope()->lookup(name);
    }
    
    /**
     * @brief Look up a symbol in the current scope only
     */
    std::optional<SymbolPtr> lookupLocal(const std::string& name) const {
        return currentScope()->lookupLocal(name);
    }
    
    /**
     * @brief Check if a symbol exists in any visible scope
     */
    bool exists(const std::string& name) const {
        return lookup(name).has_value();
    }
    
    /**
     * @brief Check if a symbol exists in the current scope
     */
    bool existsLocal(const std::string& name) const {
        return lookupLocal(name).has_value();
    }
    
    /**
     * @brief Get the current scope level
     */
    int getCurrentLevel() const { return currentLevel_; }
    
    /**
     * @brief Get the current scope
     */
    Scope* currentScope() const {
        return scopes_.back().get();
    }
    
    /**
     * @brief Create a symbol for a variable
     */
    SymbolPtr createVariable(const std::string& name, TypePtr type, 
                            const Location& loc, bool isConst = false) {
        auto kind = isConst ? SymbolKind::CONSTANT : SymbolKind::VARIABLE;
        return std::make_shared<Symbol>(name, kind, type, loc, currentLevel_);
    }
    
    /**
     * @brief Create a symbol for a function
     */
    SymbolPtr createFunction(const std::string& name, TypePtr returnType,
                            const std::vector<TypePtr>& paramTypes,
                            const Location& loc) {
        auto symbol = std::make_shared<Symbol>(name, SymbolKind::FUNCTION, 
                                               returnType, loc, currentLevel_);
        symbol->parameterTypes = paramTypes;
        return symbol;
    }
    
    /**
     * @brief Create a symbol for a parameter
     */
    SymbolPtr createParameter(const std::string& name, TypePtr type,
                             const Location& loc) {
        return std::make_shared<Symbol>(name, SymbolKind::PARAMETER, 
                                       type, loc, currentLevel_);
    }
    
    /**
     * @brief Create a symbol for a struct
     */
    SymbolPtr createStruct(const std::string& name, const Location& loc) {
        return std::make_shared<Symbol>(name, SymbolKind::STRUCT, 
                                       nullptr, loc, currentLevel_);
    }

private:
    std::vector<std::unique_ptr<Scope>> scopes_;
    int currentLevel_ = -1;
    
    /**
     * @brief Define built-in types and functions
     */
    void defineBuiltins() {
        // Built-in types are implicitly available
        // We could add built-in functions like println here
    }
};

} // namespace nova

#endif // NOVA_SYMBOL_TABLE_H
