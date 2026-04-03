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
 * @brief Abstract base class for code generators
 */
class CodeGenerator {
public:
    virtual ~CodeGenerator() = default;
    
    /**
     * @brief Generate target code from IR module
     * @param module IR module to translate
     * @return Generated code structure
     */
    virtual GeneratedCode generate(const IRModulePtr& module) = 0;
    
    /**
     * @brief Set output filename
     */
    virtual void setOutputFile(const std::string& filename) {
        outputFilename = filename;
    }
    
protected:
    std::string outputFilename;
};

/**
 * @class CCodeGenerator
 * @brief Generates C source code from IR
 */
class CCodeGenerator : public CodeGenerator {
public:
    CCodeGenerator() : indentLevel(0) {}
    
    GeneratedCode generate(const IRModulePtr& module) override;
    
    void setOutputFile(const std::string& filename) override {
        outputFilename = filename;
    }

private:
    // Output stream
    std::ostringstream output;
    std::ostringstream globalOutput;
    int indentLevel;
    
    // Maps for tracking variables
    std::unordered_map<std::string, std::string> tempVarTypes;  // temp var -> C type
    std::unordered_map<std::string, int> tempVarCounters;       // temp var counters
    std::unordered_map<std::string, std::string> labelMap;      // IR label -> C label
    
    // Code generation helpers
    void emitHeader();
    void emitGlobals(const IRModulePtr& module);
    void emitFunction(const IRFuncPtr& func);
    void emitBasicBlock(const IRBlockPtr& block, const std::string& funcName);
    void emitInstruction(const IRInstPtr& inst, const std::string& funcName);
    
    // Type translation
    std::string translateType(const std::string& novaType);
    
    // Operand translation
    std::string translateOperand(const IROperand& operand);
    
    // Instruction generation
    void generateArithmetic(const IRInstPtr& inst);
    void generateComparison(const IRInstPtr& inst);
    void generateLogical(const IRInstPtr& inst);
    void generateMemoryOp(const IRInstPtr& inst);
    void generateControlFlow(const IRInstPtr& inst, const std::string& funcName);
    void generateFunctionCall(const IRInstPtr& inst);
    
    // Utility functions
    void emit(const std::string& code);
    void emitLine(const std::string& code);
    void indent() { indentLevel++; }
    void dedent() { indentLevel--; }
    std::string getIndent() const { return std::string(indentLevel * 4, ' '); }
    
    // Label management
    std::string sanitizeLabel(const std::string& label);
    std::string sanitizeIdentifier(const std::string& name);
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
            default:
                throw std::runtime_error("Unsupported target platform");
        }
    }
};

// ============================================================================
// Implementation (inline for header-only library)
// ============================================================================

inline GeneratedCode CCodeGenerator::generate(const IRModulePtr& module) {
    // Clear previous state
    output.str("");
    globalOutput.str("");
    tempVarTypes.clear();
    tempVarCounters.clear();
    labelMap.clear();
    indentLevel = 0;
    
    // Generate C code
    emitHeader();
    emitGlobals(module);
    
    // Generate all functions
    for (const auto& func : module->functions) {
        emitFunction(func);
    }
    
    // Combine global and function code
    std::string fullCode = globalOutput.str() + "\n" + output.str();
    
    // Determine output filename
    std::string filename = outputFilename.empty() ? 
        module->name + ".c" : outputFilename;
    
    return GeneratedCode(fullCode, filename, "C");
}

inline void CCodeGenerator::emitHeader() {
    emitLine("/* Generated by Nova Compiler */");
    emitLine("#include <stdio.h>");
    emitLine("#include <stdlib.h>");
    emitLine("#include <string.h>");
    emitLine("#include <stdbool.h>");
    emitLine("");
}

inline void CCodeGenerator::emitGlobals(const IRModulePtr& module) {
    if (!module->globalVariables.empty()) {
        emitLine("/* Global Variables */");
        for (const auto& gv : module->globalVariables) {
            std::string type = translateType(gv.second);
            std::string name = sanitizeIdentifier(gv.first);
            globalOutput << type << " " << name << ";\n";
        }
        globalOutput << "\n";
    }
}

inline void CCodeGenerator::emitFunction(const IRFuncPtr& func) {
    // Collect all temporary variables
    std::unordered_map<std::string, std::string> tempVars;
    for (const auto& block : func->basicBlocks) {
        for (const auto& inst : block->instructions) {
            if (inst->dest.isTemporary()) {
                std::string tempName = inst->dest.value;
                std::string type = translateType(inst->dest.dataType.empty() ? "int" : inst->dest.dataType);
                tempVars[tempName] = type;
            }
        }
    }
    
    // Function signature
    std::string returnType = translateType(func->returnType);
    std::string funcName = sanitizeIdentifier(func->name);
    
    output << returnType << " " << funcName << "(";
    
    // Parameters
    for (size_t i = 0; i < func->parameters.size(); ++i) {
        if (i > 0) output << ", ";
        std::string paramType = translateType(func->parameters[i].second);
        std::string paramName = sanitizeIdentifier(func->parameters[i].first);
        output << paramType << " " << paramName;
    }
    
    output << ") {\n";
    indent();
    
    // Declare temporary variables
    if (!tempVars.empty()) {
        emitLine("/* Temporary variables */");
        for (const auto& [tempName, tempType] : tempVars) {
            emitLine(tempType + " _t_" + tempName + ";");
        }
        emitLine("");
    }
    
    // Generate basic blocks
    for (const auto& block : func->basicBlocks) {
        emitBasicBlock(block, func->name);
    }
    
    dedent();
    emitLine("}");
    emitLine("");
}

inline void CCodeGenerator::emitBasicBlock(const IRBlockPtr& block, const std::string& funcName) {
    // Emit block label
    std::string label = sanitizeLabel(block->name);
    emitLine("");
    emitLine("/* Block: " + block->name + " */");
    emit(label + ":");
    indent();
    
    // Emit instructions
    for (const auto& inst : block->instructions) {
        emitInstruction(inst, funcName);
    }
    
    dedent();
}

inline void CCodeGenerator::emitInstruction(const IRInstPtr& inst, const std::string& funcName) {
    switch (inst->opcode) {
        case IROpcode::ADD:
        case IROpcode::SUB:
        case IROpcode::MUL:
        case IROpcode::DIV:
        case IROpcode::MOD:
        case IROpcode::NEG:
            generateArithmetic(inst);
            break;
            
        case IROpcode::EQ:
        case IROpcode::NE:
        case IROpcode::LT:
        case IROpcode::LE:
        case IROpcode::GT:
        case IROpcode::GE:
            generateComparison(inst);
            break;
            
        case IROpcode::AND:
        case IROpcode::OR:
        case IROpcode::NOT:
            generateLogical(inst);
            break;
            
        case IROpcode::LOAD:
        case IROpcode::STORE:
        case IROpcode::ALLOCA:
            generateMemoryOp(inst);
            break;
            
        case IROpcode::JMP:
        case IROpcode::JZ:
        case IROpcode::JNZ:
        case IROpcode::LABEL:
        case IROpcode::RET:
            generateControlFlow(inst, funcName);
            break;
            
        case IROpcode::CALL:
        case IROpcode::PARAM:
            generateFunctionCall(inst);
            break;
            
        case IROpcode::MOVE:
        case IROpcode::COPY:
            if (!inst->dest.isNone() && !inst->operands.empty()) {
                std::string dest = translateOperand(inst->dest);
                std::string src = translateOperand(inst->operands[0]);
                emitLine(dest + " = " + src + ";");
            }
            break;
            
        case IROpcode::CAST:
            if (!inst->dest.isNone() && !inst->operands.empty()) {
                std::string dest = translateOperand(inst->dest);
                std::string src = translateOperand(inst->operands[0]);
                std::string targetType = translateType(inst->dest.dataType);
                emitLine(dest + " = (" + targetType + ")" + src + ";");
            }
            break;
            
        case IROpcode::NOP:
            // No operation
            break;
            
        default:
            emitLine("/* Unhandled opcode: " + opcodeToString(inst->opcode) + " */");
            break;
    }
}

inline std::string CCodeGenerator::translateType(const std::string& novaType) {
    if (novaType.empty() || novaType == "void") return "void";
    if (novaType == "int" || novaType == "integer") return "int";
    if (novaType == "float" || novaType == "double") return "double";
    if (novaType == "bool" || novaType == "boolean") return "bool";
    if (novaType == "string") return "char*";
    if (novaType == "char") return "char";
    return "int";  // Default to int
}

inline std::string CCodeGenerator::translateOperand(const IROperand& operand) {
    switch (operand.type) {
        case IROperand::Type::VARIABLE:
            return sanitizeIdentifier(operand.value);
        case IROperand::Type::CONSTANT:
            return operand.value;
        case IROperand::Type::TEMPORARY:
            return "_t_" + sanitizeIdentifier(operand.value);
        case IROperand::Type::LABEL:
            return sanitizeLabel(operand.value);
        case IROperand::Type::FUNCTION:
            return sanitizeIdentifier(operand.value);
        case IROperand::Type::NONE:
            return "";
        default:
            return operand.value;
    }
}

inline void CCodeGenerator::generateArithmetic(const IRInstPtr& inst) {
    if (inst->dest.isNone()) return;
    
    std::string dest = translateOperand(inst->dest);
    std::string op;
    
    switch (inst->opcode) {
        case IROpcode::ADD: op = "+"; break;
        case IROpcode::SUB: op = "-"; break;
        case IROpcode::MUL: op = "*"; break;
        case IROpcode::DIV: op = "/"; break;
        case IROpcode::MOD: op = "%"; break;
        case IROpcode::NEG:
            emitLine(dest + " = -" + translateOperand(inst->operands[0]) + ";");
            return;
        default: return;
    }
    
    if (inst->operands.size() >= 2) {
        std::string left = translateOperand(inst->operands[0]);
        std::string right = translateOperand(inst->operands[1]);
        emitLine(dest + " = " + left + " " + op + " " + right + ";");
    }
}

inline void CCodeGenerator::generateComparison(const IRInstPtr& inst) {
    if (inst->dest.isNone() || inst->operands.size() < 2) return;
    
    std::string dest = translateOperand(inst->dest);
    std::string left = translateOperand(inst->operands[0]);
    std::string right = translateOperand(inst->operands[1]);
    std::string op;
    
    switch (inst->opcode) {
        case IROpcode::EQ: op = "=="; break;
        case IROpcode::NE: op = "!="; break;
        case IROpcode::LT: op = "<"; break;
        case IROpcode::LE: op = "<="; break;
        case IROpcode::GT: op = ">"; break;
        case IROpcode::GE: op = ">="; break;
        default: return;
    }
    
    emitLine(dest + " = " + left + " " + op + " " + right + ";");
}

inline void CCodeGenerator::generateLogical(const IRInstPtr& inst) {
    if (inst->dest.isNone()) return;
    
    std::string dest = translateOperand(inst->dest);
    
    if (inst->opcode == IROpcode::NOT && !inst->operands.empty()) {
        std::string operand = translateOperand(inst->operands[0]);
        emitLine(dest + " = !" + operand + ";");
    } else if (inst->operands.size() >= 2) {
        std::string left = translateOperand(inst->operands[0]);
        std::string right = translateOperand(inst->operands[1]);
        std::string op = (inst->opcode == IROpcode::AND) ? "&&" : "||";
        emitLine(dest + " = " + left + " " + op + " " + right + ";");
    }
}

inline void CCodeGenerator::generateMemoryOp(const IRInstPtr& inst) {
    switch (inst->opcode) {
        case IROpcode::ALLOCA:
            // Memory allocation
            if (!inst->dest.isNone()) {
                std::string dest = translateOperand(inst->dest);
                std::string type = translateType(inst->dest.dataType);
                emitLine(dest + " = (" + type + "*)malloc(sizeof(" + type + "));");
            }
            break;
            
        case IROpcode::LOAD:
            // Load from memory
            if (!inst->dest.isNone() && !inst->operands.empty()) {
                std::string dest = translateOperand(inst->dest);
                std::string ptr = translateOperand(inst->operands[0]);
                emitLine(dest + " = *" + ptr + ";");
            }
            break;
            
        case IROpcode::STORE:
            // Store to memory
            if (inst->operands.size() >= 2) {
                std::string value = translateOperand(inst->operands[0]);
                std::string ptr = translateOperand(inst->operands[1]);
                emitLine("*" + ptr + " = " + value + ";");
            }
            break;
            
        default:
            break;
    }
}

inline void CCodeGenerator::generateControlFlow(const IRInstPtr& inst, const std::string& funcName) {
    switch (inst->opcode) {
        case IROpcode::LABEL:
            // Labels are handled at block level
            break;
            
        case IROpcode::JMP:
            if (!inst->operands.empty()) {
                std::string target = translateOperand(inst->operands[0]);
                emitLine("goto " + target + ";");
            }
            break;
            
        case IROpcode::JZ:
            if (inst->operands.size() >= 2) {
                std::string cond = translateOperand(inst->operands[0]);
                std::string target = translateOperand(inst->operands[1]);
                emitLine("if (!" + cond + ") goto " + target + ";");
            }
            break;
            
        case IROpcode::JNZ:
            if (inst->operands.size() >= 2) {
                std::string cond = translateOperand(inst->operands[0]);
                std::string target = translateOperand(inst->operands[1]);
                emitLine("if (" + cond + ") goto " + target + ";");
            }
            break;
            
        case IROpcode::RET:
            if (!inst->operands.empty()) {
                std::string value = translateOperand(inst->operands[0]);
                emitLine("return " + value + ";");
            } else {
                emitLine("return;");
            }
            break;
            
        default:
            break;
    }
}

inline void CCodeGenerator::generateFunctionCall(const IRInstPtr& inst) {
    switch (inst->opcode) {
        case IROpcode::CALL:
            if (!inst->operands.empty()) {
                std::string funcName = translateOperand(inst->operands[0]);
                
                // Build argument list
                std::string args;
                for (size_t i = 1; i < inst->operands.size(); ++i) {
                    if (i > 1) args += ", ";
                    args += translateOperand(inst->operands[i]);
                }
                
                if (!inst->dest.isNone()) {
                    std::string result = translateOperand(inst->dest);
                    emitLine(result + " = " + funcName + "(" + args + ");");
                } else {
                    emitLine(funcName + "(" + args + ");");
                }
            }
            break;
            
        case IROpcode::PARAM:
            // Parameters are handled in function signature
            break;
            
        default:
            break;
    }
}

inline void CCodeGenerator::emit(const std::string& code) {
    output << getIndent() << code << "\n";
}

inline void CCodeGenerator::emitLine(const std::string& code) {
    if (!code.empty()) {
        output << getIndent() << code << "\n";
    } else {
        output << "\n";
    }
}

inline std::string CCodeGenerator::sanitizeLabel(const std::string& label) {
    std::string result = label;
    // Replace special characters with underscores
    for (char& c : result) {
        if (!isalnum(c) && c != '_') {
            c = '_';
        }
    }
    // Ensure it starts with a letter
    if (!result.empty() && isdigit(result[0])) {
        result = "L_" + result;
    }
    return result;
}

inline std::string CCodeGenerator::sanitizeIdentifier(const std::string& name) {
    std::string result = name;
    // Replace special characters with underscores
    for (char& c : result) {
        if (!isalnum(c) && c != '_') {
            c = '_';
        }
    }
    // Ensure it doesn't start with a digit
    if (!result.empty() && isdigit(result[0])) {
        result = "_" + result;
    }
    return result;
}

} // namespace codegen
} // namespace nova

#endif // NOVA_CODEGEN_CODE_GENERATOR_H
