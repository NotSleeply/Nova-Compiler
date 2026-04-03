/**
 * @file code_generator.h
 * @brief Code Generator - Translates IR to target code
 * 
 * This module transforms the Intermediate Representation (IR) into
 * executable target code. Initially targeting C code generation,
 * with potential future support for LLVM IR, x86-64 assembly, etc.
 * 
 * Copyright (c) 2024 Nova Compiler Team
 * Licensed under MIT License
 */

#ifndef NOVA_CODEGEN_CODE_GENERATOR_H
#define NOVA_CODEGEN_CODE_GENERATOR_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include "../ir/ir.h"

namespace nova {
namespace codegen {

/**
 * @enum TargetPlatform
 * @brief Supported target platforms for code generation
 */
enum class TargetPlatform {
    C_CODE,         // Generate C source code (transpilation)
    LLVM_IR,        // Generate LLVM IR (future)
    X86_64_ASM,     // Generate x86-64 assembly (future)
    WASM,           // Generate WebAssembly (future)
};

/**
 * @struct GeneratedCode
 * @brief Container for generated target code
 */
struct GeneratedCode {
    std::string code;           // Generated code content
    std::string filename;       // Suggested output filename
    std::string language;       // Target language (e.g., "C", "LLVM IR")
    std::vector<std::string> dependencies;  // Required libraries/files
    
    GeneratedCode() = default;
    GeneratedCode(const std::string& c, const std::string& fn, const std::string& lang)
        : code(c), filename(fn), language(lang) {}
};

/**
 * @class CodeGenerator
 * @brief Base class for code generators
 */
class CodeGenerator {
public:
    virtual ~CodeGenerator() = default;
    
    virtual GeneratedCode generate(const std::shared_ptr<IRModule>& module) = 0;
    virtual TargetPlatform getPlatform() const = 0;
    
    void setOutputFile(const std::string& filename) {
        outputFile_ = filename;
    }
    
    const std::string& getOutputFile() const {
        return outputFile_;
    }
    
    void setOptimization(bool enable) {
        optimization_ = enable;
    }
    
    bool isOptimizationEnabled() const {
        return optimization_;
    }
    
protected:
    std::string outputFile_;
    bool optimization_ = false;
    int indentLevel_ = 0;
    
    std::string indent(int level = -1) const {
        if (level == -1) level = indentLevel_;
        return std::string(level * 4, ' ');
    }
    
    std::string currentIndent() const {
        return indent(indentLevel_);
    }
};

/**
 * @class CCodeGenerator
 * @brief Generates C source code from IR
 */
class CCodeGenerator : public CodeGenerator {
public:
    CCodeGenerator() : CodeGenerator() {}
    
    GeneratedCode generate(const std::shared_ptr<IRModule>& module) override;
    TargetPlatform getPlatform() const override { return TargetPlatform::C_CODE; }
    
private:
    std::stringstream code_;
    std::stringstream header_;
    std::stringstream functions_;
    
    std::unordered_map<std::string, std::string> typeMap_;
    std::vector<std::string> includes_;
    std::vector<std::string> globals_;
    
    void generateHeader(const std::shared_ptr<IRModule>& module);
    void generateGlobals(const std::shared_ptr<IRModule>& module);
    void generateFunction(const std::shared_ptr<IRFunction>& func);
    void generateBasicBlock(const std::shared_ptr<IRBasicBlock>& block);
    void generateInstruction(const std::shared_ptr<IRInstruction>& instr);
    
    std::string translateTypeToC(const std::string& type) const;
    std::string translateOperand(const IROperand& op) const;
    void emit(const std::string& line);
    void emitInclude(const std::string& header);
    void emitGlobal(const std::string& decl);
    std::string opcodeToOperator(IROpcode op) const;
};

/**
 * @class CodeGeneratorFactory
 * @brief Factory for creating code generators
 */
class CodeGeneratorFactory {
public:
    static std::unique_ptr<CodeGenerator> create(TargetPlatform platform) {
        switch (platform) {
            case TargetPlatform::C_CODE:
                return std::make_unique<CCodeGenerator>();
            case TargetPlatform::LLVM_IR:
                throw std::runtime_error("LLVM IR generation not yet implemented");
            case TargetPlatform::X86_64_ASM:
                throw std::runtime_error("x86-64 assembly generation not yet implemented");
            case TargetPlatform::WASM:
                throw std::runtime_error("WebAssembly generation not yet implemented");
            default:
                throw std::runtime_error("Unknown target platform");
        }
    }
};

// ============================================================================
// Implementation
// ============================================================================

inline GeneratedCode CCodeGenerator::generate(const std::shared_ptr<IRModule>& module) {
    code_.str("");
    header_.str("");
    functions_.str("");
    typeMap_.clear();
    includes_.clear();
    globals_.clear();
    
    generateHeader(module);
    generateGlobals(module);
    
    for (const auto& func : module->functions) {
        generateFunction(func);
    }
    
    code_ << header_.str() << "\n";
    
    if (!globals_.empty()) {
        code_ << "/* Global Variables */\n";
        for (const auto& global : globals_) {
            code_ << global << "\n";
        }
        code_ << "\n";
    }
    
    code_ << functions_.str();
    
    std::string outFile = outputFile_.empty() ? 
        (module->name.empty() ? "output.c" : module->name + ".c") : 
        outputFile_;
    
    return GeneratedCode(code_.str(), outFile, "C");
}

inline void CCodeGenerator::generateHeader(const std::shared_ptr<IRModule>& module) {
    emitInclude("stdio.h");
    emitInclude("stdlib.h");
    emitInclude("string.h");
    emitInclude("stdbool.h");
    emitInclude("stdint.h");
    
    header_ << "/*\n";
    header_ << " * Generated by Nova Compiler\n";
    header_ << " * Source: " << module->name << "\n";
    header_ << " * Target: C Code\n";
    header_ << " */\n\n";
    
    for (const auto& inc : includes_) {
        header_ << inc << "\n";
    }
}

inline void CCodeGenerator::generateGlobals(const std::shared_ptr<IRModule>& module) {
    for (const auto& global : module->globalVariables) {
        std::string name = global.first;
        std::string type = global.second;
        emitGlobal(translateTypeToC(type) + " " + name + ";");
    }
}

inline void CCodeGenerator::generateFunction(const std::shared_ptr<IRFunction>& func) {
    std::string returnType = translateTypeToC(func->returnType);
    std::string params;
    
    for (size_t i = 0; i < func->parameters.size(); ++i) {
        if (i > 0) params += ", ";
        params += translateTypeToC(func->paramTypes[i]) + " " + func->parameters[i];
        typeMap_[func->parameters[i]] = func->paramTypes[i];
    }
    
    if (params.empty()) params = "void";
    
    functions_ << returnType << " " << func->name << "(" << params << ") {\n";
    indentLevel_ = 1;
    
    for (size_t i = 0; i < func->basicBlocks.size(); ++i) {
        const auto& block = func->basicBlocks[i];
        
        if (i > 0) {
            functions_ << currentIndent() << func->name << "_" << block->label << ":;\n";
        }
        
        for (const auto& instr : block->instructions) {
            generateInstruction(instr);
        }
    }
    
    indentLevel_ = 0;
    functions_ << "}\n\n";
}

inline void CCodeGenerator::generateBasicBlock(const std::shared_ptr<IRBasicBlock>& block) {
    for (const auto& instr : block->instructions) {
        generateInstruction(instr);
    }
}

inline void CCodeGenerator::generateInstruction(const std::shared_ptr<IRInstruction>& instr) {
    switch (instr->opcode) {
        case IROpcode::ADD:
        case IROpcode::SUB:
        case IROpcode::MUL:
        case IROpcode::DIV:
        case IROpcode::MOD:
        case IROpcode::NEG:
        case IROpcode::EQ:
        case IROpcode::NE:
        case IROpcode::LT:
        case IROpcode::LE:
        case IROpcode::GT:
        case IROpcode::GE:
        case IROpcode::AND:
        case IROpcode::OR:
        case IROpcode::NOT:
        case IROpcode::BIT_AND:
        case IROpcode::BIT_OR:
        case IROpcode::BIT_XOR:
        case IROpcode::BIT_NOT:
        case IROpcode::SHL:
        case IROpcode::SHR: {
            std::string op = opcodeToOperator(instr->opcode);
            std::string dest = translateOperand(instr->dest);
            
            if (instr->operands.size() == 1) {
                std::string src = translateOperand(instr->operands[0]);
                std::string type = instr->dest.type.empty() ? "int" : translateTypeToC(instr->dest.type);
                emit(currentIndent() + type + " " + dest + " = " + op + src + ";");
                typeMap_[dest] = instr->dest.type;
            } else if (instr->operands.size() == 2) {
                std::string left = translateOperand(instr->operands[0]);
                std::string right = translateOperand(instr->operands[1]);
                std::string type = instr->dest.type.empty() ? "int" : translateTypeToC(instr->dest.type);
                
                bool needsDeclaration = (typeMap_.find(dest) == typeMap_.end());
                
                if (needsDeclaration) {
                    emit(currentIndent() + type + " " + dest + " = " + left + " " + op + " " + right + ";");
                    typeMap_[dest] = instr->dest.type;
                } else {
                    emit(currentIndent() + dest + " = " + left + " " + op + " " + right + ";");
                }
            }
            break;
        }
            
        case IROpcode::MOVE:
        case IROpcode::COPY: {
            std::string dest = translateOperand(instr->dest);
            std::string src = translateOperand(instr->operands[0]);
            
            bool needsDeclaration = (typeMap_.find(dest) == typeMap_.end());
            
            if (needsDeclaration && !instr->dest.type.empty()) {
                std::string type = translateTypeToC(instr->dest.type);
                emit(currentIndent() + type + " " + dest + " = " + src + ";");
                typeMap_[dest] = instr->dest.type;
            } else {
                emit(currentIndent() + dest + " = " + src + ";");
            }
            break;
        }
            
        case IROpcode::LOAD: {
            std::string dest = translateOperand(instr->dest);
            std::string ptr = translateOperand(instr->operands[0]);
            std::string type = translateTypeToC(instr->dest.type);
            
            bool needsDeclaration = (typeMap_.find(dest) == typeMap_.end());
            if (needsDeclaration) {
                emit(currentIndent() + type + " " + dest + " = *" + ptr + ";");
                typeMap_[dest] = instr->dest.type;
            } else {
                emit(currentIndent() + dest + " = *" + ptr + ";");
            }
            break;
        }
            
        case IROpcode::STORE: {
            std::string ptr = translateOperand(instr->dest);
            std::string val = translateOperand(instr->operands[0]);
            emit(currentIndent() + "*" + ptr + " = " + val + ";");
            break;
        }
            
        case IROpcode::ALLOCA: {
            std::string dest = translateOperand(instr->dest);
            std::string type = translateTypeToC(instr->dest.type);
            emit(currentIndent() + type + " " + dest + ";");
            typeMap_[dest] = instr->dest.type;
            break;
        }
            
        case IROpcode::GEP: {
            std::string dest = translateOperand(instr->dest);
            std::string ptr = translateOperand(instr->operands[0]);
            std::string idx = translateOperand(instr->operands[1]);
            std::string type = translateTypeToC(instr->dest.type);
            
            bool needsDeclaration = (typeMap_.find(dest) == typeMap_.end());
            if (needsDeclaration) {
                emit(currentIndent() + type + " " + dest + " = " + ptr + "[" + idx + "];");
                typeMap_[dest] = instr->dest.type;
            } else {
                emit(currentIndent() + dest + " = " + ptr + "[" + idx + "];");
            }
            break;
        }
            
        case IROpcode::LABEL: {
            std::string label = translateOperand(instr->dest);
            functions_ << currentIndent() << label << ":;\n";
            break;
        }
        
        case IROpcode::JMP: {
            std::string target = translateOperand(instr->dest);
            functions_ << currentIndent() << "goto " << target << ";\n";
            break;
        }
        
        case IROpcode::JZ: {
            std::string cond = translateOperand(instr->operands[0]);
            std::string target = translateOperand(instr->dest);
            functions_ << currentIndent() << "if (!" << cond << ") goto " << target << ";\n";
            break;
        }
        
        case IROpcode::JNZ: {
            std::string cond = translateOperand(instr->operands[0]);
            std::string target = translateOperand(instr->dest);
            functions_ << currentIndent() << "if (" << cond << ") goto " << target << ";\n";
            break;
        }
            
        case IROpcode::CALL: {
            std::string dest = translateOperand(instr->dest);
            std::string func = translateOperand(instr->operands[0]);
            
            std::string args;
            for (size_t i = 1; i < instr->operands.size(); ++i) {
                if (i > 1) args += ", ";
                args += translateOperand(instr->operands[i]);
            }
            
            if (instr->dest.type != IROperand::Type::NONE) {
                std::string type = translateTypeToC(instr->dest.type);
                bool needsDeclaration = (typeMap_.find(dest) == typeMap_.end());
                
                if (needsDeclaration) {
                    emit(currentIndent() + type + " " + dest + " = " + func + "(" + args + ");");
                    typeMap_[dest] = instr->dest.type;
                } else {
                    emit(currentIndent() + dest + " = " + func + "(" + args + ");");
                }
            } else {
                emit(currentIndent() + func + "(" + args + ");");
            }
            break;
        }
            
        case IROpcode::RET:
            if (!instr->operands.empty()) {
                std::string value = translateOperand(instr->operands[0]);
                emit(currentIndent() + "return " + value + ";");
            } else {
                emit(currentIndent() + "return;");
            }
            break;
            
        case IROpcode::CAST:
            if (instr->dest.type != IROperand::Type::NONE) {
                emit(currentIndent() + translateTypeToC(instr->dest.type) + " " + 
                     translateOperand(instr->dest) + " = (" + 
                     translateTypeToC(instr->dest.type) + ")" + 
                     translateOperand(instr->operands[0]) + ";");
            }
            break;
            
        case IROpcode::PARAM:
            break;
            
        case IROpcode::NOP:
            break;
            
        case IROpcode::PHI:
            break;
            
        default:
            emit(currentIndent() + "/* Unknown opcode */");
            break;
    }
}

inline std::string CCodeGenerator::translateTypeToC(const std::string& type) const {
    if (type.empty()) return "int";
    
    static const std::unordered_map<std::string, std::string> typeMap = {
        {"int", "int"},
        {"int32", "int32_t"},
        {"int64", "int64_t"},
        {"float", "float"},
        {"float32", "float"},
        {"float64", "double"},
        {"double", "double"},
        {"bool", "bool"},
        {"boolean", "bool"},
        {"char", "char"},
        {"string", "char*"},
        {"void", "void"},
        {"auto", "auto"},
    };
    
    auto it = typeMap.find(type);
    if (it != typeMap.end()) {
        return it->second;
    }
    
    if (type.back() == '*') {
        return translateTypeToC(type.substr(0, type.size() - 1)) + "*";
    }
    
    size_t bracketPos = type.find('[');
    if (bracketPos != std::string::npos) {
        return translateTypeToC(type.substr(0, bracketPos)) + type.substr(bracketPos);
    }
    
    return type;
}

inline std::string CCodeGenerator::translateOperand(const IROperand& op) const {
    switch (op.type) {
        case IROperand::Type::VARIABLE:
            return op.value;
            
        case IROperand::Type::TEMP:
            return op.value;
            
        case IROperand::Type::CONSTANT:
            return op.value;
            
        case IROperand::Type::LABEL: {
            std::string label = op.value;
            if (label[0] == '@') {
                label = label.substr(1);
            }
            return label;
        }
            
        case IROperand::Type::FUNCTION:
            return op.value;
            
        case IROperand::Type::NONE:
        default:
            return "";
    }
}

inline void CCodeGenerator::emit(const std::string& line) {
    functions_ << line << "\n";
}

inline void CCodeGenerator::emitInclude(const std::string& header) {
    includes_.push_back("#include <" + header + ">");
}

inline void CCodeGenerator::emitGlobal(const std::string& decl) {
    globals_.push_back(decl);
}

inline std::string CCodeGenerator::opcodeToOperator(IROpcode op) const {
    static const std::unordered_map<IROpcode, std::string> opMap = {
        {IROpcode::ADD, "+"},
        {IROpcode::SUB, "-"},
        {IROpcode::MUL, "*"},
        {IROpcode::DIV, "/"},
        {IROpcode::MOD, "%"},
        {IROpcode::NEG, "-"},
        {IROpcode::EQ, "=="},
        {IROpcode::NE, "!="},
        {IROpcode::LT, "<"},
        {IROpcode::LE, "<="},
        {IROpcode::GT, ">"},
        {IROpcode::GE, ">="},
        {IROpcode::AND, "&&"},
        {IROpcode::OR, "||"},
        {IROpcode::NOT, "!"},
        {IROpcode::BIT_AND, "&"},
        {IROpcode::BIT_OR, "|"},
        {IROpcode::BIT_XOR, "^"},
        {IROpcode::BIT_NOT, "~"},
        {IROpcode::SHL, "<<"},
        {IROpcode::SHR, ">>"},
    };
    
    auto it = opMap.find(op);
    return it != opMap.end() ? it->second : "?";
}

} // namespace codegen
} // namespace nova

#endif // NOVA_CODEGEN_CODE_GENERATOR_H
