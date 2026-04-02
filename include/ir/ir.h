/**
 * @file ir.h
 * @brief 中间表示（Intermediate Representation）定义
 * @author Nova Compiler Team
 * @version 0.1.0
 * @date 2026-04-02
 * 
 * 采用三地址码作为中间表示形式。
 * 每条指令最多包含三个操作数：一个目标操作数和最多两个源操作数。
 */

#ifndef NOVA_IR_H
#define NOVA_IR_H

#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <iostream>
#include <sstream>

namespace nova {

// 前向声明
class IRInstruction;
class IRBasicBlock;
class IRFunction;
class IRModule;

// 智能指针别名
using IRInstPtr = std::shared_ptr<IRInstruction>;
using IRBlockPtr = std::shared_ptr<IRBasicBlock>;
using IRFuncPtr = std::shared_ptr<IRFunction>;
using IRModulePtr = std::shared_ptr<IRModule>;

/**
 * @enum IROpcode
 * @brief IR指令操作码
 */
enum class IROpcode {
    // 算术运算
    ADD,        // 加法
    SUB,        // 减法
    MUL,        // 乘法
    DIV,        // 除法
    MOD,        // 取模
    NEG,        // 负号
    
    // 比较运算
    EQ,         // 等于
    NE,         // 不等于
    LT,         // 小于
    LE,         // 小于等于
    GT,         // 大于
    GE,         // 大于等于
    
    // 逻辑运算
    AND,        // 逻辑与
    OR,         // 逻辑或
    NOT,        // 逻辑非
    
    // 位运算
    BIT_AND,    // 按位与
    BIT_OR,     // 按位或
    BIT_XOR,    // 按位异或
    BIT_NOT,    // 按位取反
    SHL,        // 左移
    SHR,        // 右移
    
    // 内存操作
    LOAD,       // 加载
    STORE,      // 存储
    ALLOCA,     // 分配内存
    GEP,        // 获取元素指针（GetElementPtr）
    
    // 控制流
    JMP,        // 无条件跳转
    JZ,         // 为零跳转
    JNZ,        // 非零跳转
    LABEL,      // 标签
    
    // 函数调用
    CALL,       // 函数调用
    RET,        // 返回
    PARAM,      // 参数传递
    
    // 其他
    MOVE,       // 赋值
    COPY,       // 复制
    CAST,       // 类型转换
    NOP,        // 空操作
    PHI,        // PHI节点（SSA形式）
};

/**
 * @brief 将操作码转换为字符串
 */
inline std::string opcodeToString(IROpcode opcode) {
    switch (opcode) {
        case IROpcode::ADD: return "add";
        case IROpcode::SUB: return "sub";
        case IROpcode::MUL: return "mul";
        case IROpcode::DIV: return "div";
        case IROpcode::MOD: return "mod";
        case IROpcode::NEG: return "neg";
        
        case IROpcode::EQ: return "eq";
        case IROpcode::NE: return "ne";
        case IROpcode::LT: return "lt";
        case IROpcode::LE: return "le";
        case IROpcode::GT: return "gt";
        case IROpcode::GE: return "ge";
        
        case IROpcode::AND: return "and";
        case IROpcode::OR: return "or";
        case IROpcode::NOT: return "not";
        
        case IROpcode::BIT_AND: return "bit_and";
        case IROpcode::BIT_OR: return "bit_or";
        case IROpcode::BIT_XOR: return "bit_xor";
        case IROpcode::BIT_NOT: return "bit_not";
        case IROpcode::SHL: return "shl";
        case IROpcode::SHR: return "shr";
        
        case IROpcode::LOAD: return "load";
        case IROpcode::STORE: return "store";
        case IROpcode::ALLOCA: return "alloca";
        case IROpcode::GEP: return "gep";
        
        case IROpcode::JMP: return "jmp";
        case IROpcode::JZ: return "jz";
        case IROpcode::JNZ: return "jnz";
        case IROpcode::LABEL: return "label";
        
        case IROpcode::CALL: return "call";
        case IROpcode::RET: return "ret";
        case IROpcode::PARAM: return "param";
        
        case IROpcode::MOVE: return "move";
        case IROpcode::COPY: return "copy";
        case IROpcode::CAST: return "cast";
        case IROpcode::NOP: return "nop";
        case IROpcode::PHI: return "phi";
        
        default: return "unknown";
    }
}

/**
 * @class IROperand
 * @brief IR操作数
 */
class IROperand {
public:
    enum class Type {
        VARIABLE,   // 变量
        CONSTANT,   // 常量
        TEMPORARY,  // 临时变量
        LABEL,      // 标签
        FUNCTION,   // 函数名
        NONE        // 无操作数
    };
    
    Type type;
    std::string value;
    std::string dataType;  // 数据类型（int, float, string等）
    
    IROperand(Type t = Type::NONE, const std::string& v = "", const std::string& dt = "")
        : type(t), value(v), dataType(dt) {}
    
    // 创建不同类型的操作数
    static IROperand variable(const std::string& name, const std::string& dataType = "") {
        return IROperand(Type::VARIABLE, name, dataType);
    }
    
    static IROperand constant(const std::string& value, const std::string& dataType = "") {
        return IROperand(Type::CONSTANT, value, dataType);
    }
    
    static IROperand temp(const std::string& name, const std::string& dataType = "") {
        return IROperand(Type::TEMPORARY, name, dataType);
    }
    
    static IROperand label(const std::string& name) {
        return IROperand(Type::LABEL, name);
    }
    
    static IROperand function(const std::string& name) {
        return IROperand(Type::FUNCTION, name);
    }
    
    static IROperand none() {
        return IROperand(Type::NONE);
    }
    
    std::string toString() const {
        switch (type) {
            case Type::VARIABLE: return "%" + value;
            case Type::CONSTANT: return value;
            case Type::TEMPORARY: return "$" + value;
            case Type::LABEL: return "@" + value;
            case Type::FUNCTION: return "&" + value;
            case Type::NONE: return "_";
            default: return "?";
        }
    }
    
    bool isNone() const { return type == Type::NONE; }
    bool isVariable() const { return type == Type::VARIABLE; }
    bool isConstant() const { return type == Type::CONSTANT; }
    bool isTemporary() const { return type == Type::TEMPORARY; }
    bool isLabel() const { return type == Type::LABEL; }
    bool isFunction() const { return type == Type::FUNCTION; }
};

/**
 * @class IRInstruction
 * @brief IR指令
 */
class IRInstruction {
public:
    IROpcode opcode;
    IROperand dest;                         // 目标操作数
    std::vector<IROperand> operands;        // 源操作数
    std::string comment;                    // 注释（用于调试）
    int lineNumber;                         // 源代码行号
    
    IRInstruction(IROpcode op, IROperand d = IROperand::none(), 
                  std::vector<IROperand> ops = {}, 
                  const std::string& cmt = "", int line = 0)
        : opcode(op), dest(d), operands(ops), comment(cmt), lineNumber(line) {}
    
    // 便捷构造函数
    static IRInstPtr create(IROpcode op, IROperand dest = IROperand::none(),
                           std::vector<IROperand> operands = {},
                           const std::string& comment = "", int line = 0) {
        return std::make_shared<IRInstruction>(op, dest, operands, comment, line);
    }
    
    std::string toString() const {
        std::ostringstream oss;
        
        // 标签指令特殊处理
        if (opcode == IROpcode::LABEL) {
            oss << dest.toString() << ":";
            if (!comment.empty()) {
                oss << "  # " << comment;
            }
            return oss.str();
        }
        
        // 其他指令
        oss << opcodeToString(opcode);
        
        if (!dest.isNone()) {
            oss << " " << dest.toString();
        }
        
        if (!operands.empty()) {
            oss << ", ";
            for (size_t i = 0; i < operands.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << operands[i].toString();
            }
        }
        
        if (!comment.empty()) {
            oss << "  # " << comment;
        }
        
        return oss.str();
    }
};

/**
 * @class IRBasicBlock
 * @brief IR基本块
 */
class IRBasicBlock {
public:
    std::string name;
    std::vector<IRInstPtr> instructions;
    std::vector<IRBlockPtr> predecessors;
    std::vector<IRBlockPtr> successors;
    
    explicit IRBasicBlock(const std::string& n) : name(n) {}
    
    void addInstruction(IRInstPtr inst) {
        instructions.push_back(inst);
    }
    
    void addPredecessor(IRBlockPtr block) {
        predecessors.push_back(block);
    }
    
    void addSuccessor(IRBlockPtr block) {
        successors.push_back(block);
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << name << ":\n";
        for (const auto& inst : instructions) {
            oss << "  " << inst->toString() << "\n";
        }
        return oss.str();
    }
};

/**
 * @class IRFunction
 * @brief IR函数
 */
class IRFunction {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> parameters;  // 参数名和类型
    std::string returnType;
    std::vector<IRBlockPtr> basicBlocks;
    
    IRFunction(const std::string& n, const std::string& retType = "void")
        : name(n), returnType(retType) {}
    
    void addParameter(const std::string& paramName, const std::string& paramType) {
        parameters.push_back({paramName, paramType});
    }
    
    void addBasicBlock(IRBlockPtr block) {
        basicBlocks.push_back(block);
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "function " << name << "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << parameters[i].second << " " << parameters[i].first;
        }
        oss << ") -> " << returnType << " {\n";
        
        for (const auto& block : basicBlocks) {
            oss << block->toString();
        }
        
        oss << "}\n";
        return oss.str();
    }
};

/**
 * @class IRModule
 * @brief IR模块（整个程序）
 */
class IRModule {
public:
    std::string name;
    std::vector<IRFuncPtr> functions;
    std::vector<std::pair<std::string, std::string>> globalVariables;  // 全局变量
    
    explicit IRModule(const std::string& n) : name(n) {}
    
    void addFunction(IRFuncPtr func) {
        functions.push_back(func);
    }
    
    void addGlobalVariable(const std::string& varName, const std::string& varType) {
        globalVariables.push_back({varName, varType});
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "# IR Module: " << name << "\n\n";
        
        // 全局变量
        if (!globalVariables.empty()) {
            oss << "# Global Variables\n";
            for (const auto& gv : globalVariables) {
                oss << "@global " << gv.second << " " << gv.first << "\n";
            }
            oss << "\n";
        }
        
        // 函数
        for (const auto& func : functions) {
            oss << func->toString() << "\n";
        }
        
        return oss.str();
    }
};

} // namespace nova

#endif // NOVA_IR_H
