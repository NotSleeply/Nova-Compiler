/**
 * @file ir_generator.h
 * @brief IR生成器 - 将AST转换为中间表示
 * @author Nova Compiler Team
 * @version 0.1.0
 * @date 2026-04-02
 * 
 * 采用递归下降的方式遍历AST，生成三地址码形式的IR。
 */

#ifndef NOVA_IR_GENERATOR_H
#define NOVA_IR_GENERATOR_H

#include "ast/ast.h"
#include "ir/ir.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <stack>
#include <sstream>

namespace nova {

/**
 * @class IRGenerator
 * @brief IR生成器主类
 */
class IRGenerator {
public:
    IRGenerator();
    
    /**
     * @brief 从AST程序生成IR模块
     */
    IRModulePtr generate(const std::shared_ptr<Program>& program);
    
private:
    IRModulePtr currentModule_;
    IRFuncPtr currentFunction_;
    IRBlockPtr currentBlock_;
    
    // 临时变量计数器
    int tempCounter_;
    
    // 标签计数器
    int labelCounter_;
    
    // 符号表：变量名 -> IR操作数
    std::unordered_map<std::string, IROperand> variables_;
    
    // 循环栈（用于break和continue）
    struct LoopInfo {
        std::string continueLabel;
        std::string breakLabel;
    };
    std::stack<LoopInfo> loopStack_;
    
    // 辅助方法
    
    /**
     * @brief 生成新的临时变量
     */
    std::string newTemp(const std::string& type = "");
    
    /**
     * @brief 生成新的标签
     */
    std::string newLabel(const std::string& prefix = "L");
    
    /**
     * @brief 获取当前基本块，如果不存在则创建
     */
    IRBlockPtr getCurrentBlock();
    
    /**
     * @brief 创建新的基本块
     */
    IRBlockPtr createBlock(const std::string& name = "");
    
    /**
     * @brief 添加指令到当前基本块
     */
    void emit(IROpcode opcode, IROperand dest = IROperand::none(),
              std::vector<IROperand> operands = {},
              const std::string& comment = "", int line = 0);
    
    // 表达式处理
    
    /**
     * @brief 处理表达式，返回结果操作数
     */
    IROperand processExpression(const ExprPtr& expr);
    
    IROperand processIntegerLiteral(const std::shared_ptr<IntegerLiteral>& lit);
    IROperand processFloatLiteral(const std::shared_ptr<FloatLiteral>& lit);
    IROperand processStringLiteral(const std::shared_ptr<StringLiteral>& lit);
    IROperand processBoolLiteral(const std::shared_ptr<BoolLiteral>& lit);
    IROperand processIdentifier(const std::shared_ptr<Identifier>& ident);
    IROperand processBinaryExpr(const std::shared_ptr<BinaryExpr>& binExpr);
    IROperand processUnaryExpr(const std::shared_ptr<UnaryExpr>& unaryExpr);
    IROperand processCallExpr(const std::shared_ptr<CallExpr>& callExpr);
    IROperand processAssignmentExpr(const std::shared_ptr<AssignmentExpr>& assignExpr);
    IROperand processTernaryExpr(const std::shared_ptr<TernaryExpr>& ternExpr);
    IROperand processArrayLiteral(const std::shared_ptr<ArrayLiteral>& arrayLit);
    IROperand processIndexExpr(const std::shared_ptr<IndexExpr>& indexExpr);
    IROperand processMemberExpr(const std::shared_ptr<MemberExpr>& memberExpr);
    
    // 语句处理
    
    /**
     * @brief 处理语句
     */
    void processStatement(const StmtPtr& stmt);
    
    void processExpressionStmt(const std::shared_ptr<ExpressionStmt>& exprStmt);
    void processBlockStmt(const std::shared_ptr<BlockStmt>& blockStmt);
    void processIfStmt(const std::shared_ptr<IfStmt>& ifStmt);
    void processWhileStmt(const std::shared_ptr<WhileStmt>& whileStmt);
    void processForStmt(const std::shared_ptr<ForStmt>& forStmt);
    void processReturnStmt(const std::shared_ptr<ReturnStmt>& returnStmt);
    void processBreakStmt(const std::shared_ptr<BreakStmt>& breakStmt);
    void processContinueStmt(const std::shared_ptr<ContinueStmt>& continueStmt);
    
    // 声明处理
    
    /**
     * @brief 处理声明
     */
    void processDeclaration(const DeclPtr& decl);
    
    void processVariableDecl(const std::shared_ptr<VariableDecl>& varDecl);
    void processFunctionDecl(const std::shared_ptr<FunctionDecl>& funcDecl);
    void processParamDecl(const std::shared_ptr<ParamDecl>& paramDecl);
    void processStructDecl(const std::shared_ptr<StructDecl>& structDecl);
    void processEnumDecl(const std::shared_ptr<EnumDecl>& enumDecl);
    
    // 类型转换
    
    /**
     * @brief 将AST类型转换为IR类型字符串
     */
    std::string typeToString(const TypePtr& type);
    
    /**
     * @brief 根据运算符和操作数类型确定结果类型
     */
    std::string getResultType(IROpcode opcode, const IROperand& left, const IROperand& right);
    
    /**
     * @brief 添加类型转换指令（如果需要）
     */
    IROperand addCastIfNeeded(const IROperand& operand, const std::string& targetType);
};

// ==================== 内联实现 ====================

inline IRGenerator::IRGenerator() 
    : tempCounter_(0), labelCounter_(0) {}

inline IRModulePtr IRGenerator::generate(const std::shared_ptr<Program>& program) {
    currentModule_ = std::make_shared<IRModule>(program->fileName);
    
    // 第一遍：收集所有函数声明
    for (const auto& decl : program->declarations) {
        if (auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(decl)) {
            // 创建函数框架
            std::string returnType = typeToString(funcDecl->returnType);
            auto irFunc = std::make_shared<IRFunction>(funcDecl->name, returnType);
            
            // 添加参数
            for (const auto& param : funcDecl->parameters) {
                irFunc->addParameter(param->name, typeToString(param->type));
            }
            
            currentModule_->addFunction(irFunc);
        }
    }
    
    // 第二遍：生成函数体IR
    for (const auto& decl : program->declarations) {
        processDeclaration(decl);
    }
    
    return currentModule_;
}

inline std::string IRGenerator::newTemp(const std::string& type) {
    return "t" + std::to_string(tempCounter_++);
}

inline std::string IRGenerator::newLabel(const std::string& prefix) {
    return prefix + std::to_string(labelCounter_++);
}

inline IRBlockPtr IRGenerator::getCurrentBlock() {
    if (!currentBlock_) {
        currentBlock_ = createBlock("entry");
        if (currentFunction_) {
            currentFunction_->addBasicBlock(currentBlock_);
        }
    }
    return currentBlock_;
}

inline IRBlockPtr IRGenerator::createBlock(const std::string& name) {
    std::string blockName = name.empty() ? newLabel("BB") : name;
    return std::make_shared<IRBasicBlock>(blockName);
}

inline void IRGenerator::emit(IROpcode opcode, IROperand dest,
                              std::vector<IROperand> operands,
                              const std::string& comment, int line) {
    auto inst = IRInstruction::create(opcode, dest, operands, comment, line);
    getCurrentBlock()->addInstruction(inst);
}

inline IROperand IRGenerator::processExpression(const ExprPtr& expr) {
    if (!expr) return IROperand::none();
    
    switch (expr->nodeType) {
        case ASTNodeType::INTEGER_LITERAL_EXPR:
            return processIntegerLiteral(std::static_pointer_cast<IntegerLiteral>(expr));
        case ASTNodeType::FLOAT_LITERAL_EXPR:
            return processFloatLiteral(std::static_pointer_cast<FloatLiteral>(expr));
        case ASTNodeType::STRING_LITERAL_EXPR:
            return processStringLiteral(std::static_pointer_cast<StringLiteral>(expr));
        case ASTNodeType::BOOL_LITERAL_EXPR:
            return processBoolLiteral(std::static_pointer_cast<BoolLiteral>(expr));
        case ASTNodeType::IDENTIFIER_EXPR:
            return processIdentifier(std::static_pointer_cast<Identifier>(expr));
        case ASTNodeType::BINARY_EXPR:
            return processBinaryExpr(std::static_pointer_cast<BinaryExpr>(expr));
        case ASTNodeType::UNARY_EXPR:
            return processUnaryExpr(std::static_pointer_cast<UnaryExpr>(expr));
        case ASTNodeType::CALL_EXPR:
            return processCallExpr(std::static_pointer_cast<CallExpr>(expr));
        case ASTNodeType::ASSIGNMENT_EXPR:
            return processAssignmentExpr(std::static_pointer_cast<AssignmentExpr>(expr));
        case ASTNodeType::TERNARY_EXPR:
            return processTernaryExpr(std::static_pointer_cast<TernaryExpr>(expr));
        case ASTNodeType::ARRAY_LITERAL_EXPR:
            return processArrayLiteral(std::static_pointer_cast<ArrayLiteral>(expr));
        case ASTNodeType::INDEX_EXPR:
            return processIndexExpr(std::static_pointer_cast<IndexExpr>(expr));
        case ASTNodeType::MEMBER_EXPR:
            return processMemberExpr(std::static_pointer_cast<MemberExpr>(expr));
        default:
            return IROperand::none();
    }
}

inline IROperand IRGenerator::processIntegerLiteral(const std::shared_ptr<IntegerLiteral>& lit) {
    return IROperand::constant(std::to_string(lit->value), "int");
}

inline IROperand IRGenerator::processFloatLiteral(const std::shared_ptr<FloatLiteral>& lit) {
    return IROperand::constant(std::to_string(lit->value), "float");
}

inline IROperand IRGenerator::processStringLiteral(const std::shared_ptr<StringLiteral>& lit) {
    return IROperand::constant(lit->value, "string");
}

inline IROperand IRGenerator::processBoolLiteral(const std::shared_ptr<BoolLiteral>& lit) {
    return IROperand::constant(lit->value ? "true" : "false", "bool");
}

inline IROperand IRGenerator::processIdentifier(const std::shared_ptr<Identifier>& ident) {
    auto it = variables_.find(ident->name);
    if (it != variables_.end()) {
        return it->second;
    }
    // 如果未找到，创建一个变量操作数
    return IROperand::variable(ident->name);
}

inline IROperand IRGenerator::processBinaryExpr(const std::shared_ptr<BinaryExpr>& binExpr) {
    // 处理左右操作数
    IROperand left = processExpression(binExpr->left);
    IROperand right = processExpression(binExpr->right);
    
    // 根据运算符确定操作码
    IROpcode opcode;
    switch (binExpr->op) {
        case TokenType::OP_PLUS: opcode = IROpcode::ADD; break;
        case TokenType::OP_MINUS: opcode = IROpcode::SUB; break;
        case TokenType::OP_STAR: opcode = IROpcode::MUL; break;
        case TokenType::OP_SLASH: opcode = IROpcode::DIV; break;
        case TokenType::OP_PERCENT: opcode = IROpcode::MOD; break;
        
        case TokenType::OP_EQUAL_EQUAL: opcode = IROpcode::EQ; break;
        case TokenType::OP_BANG_EQUAL: opcode = IROpcode::NE; break;
        case TokenType::OP_LESS: opcode = IROpcode::LT; break;
        case TokenType::OP_LESS_EQUAL: opcode = IROpcode::LE; break;
        case TokenType::OP_GREATER: opcode = IROpcode::GT; break;
        case TokenType::OP_GREATER_EQUAL: opcode = IROpcode::GE; break;
        
        case TokenType::OP_AND: opcode = IROpcode::AND; break;
        case TokenType::OP_OR: opcode = IROpcode::OR; break;
        
        case TokenType::OP_AMPERSAND: opcode = IROpcode::BIT_AND; break;
        case TokenType::OP_PIPE: opcode = IROpcode::BIT_OR; break;
        case TokenType::OP_CARET: opcode = IROpcode::BIT_XOR; break;
        case TokenType::OP_LESS_LESS: opcode = IROpcode::SHL; break;
        case TokenType::OP_GREATER_GREATER: opcode = IROpcode::SHR; break;
        
        default:
            return IROperand::none();
    }
    
    // 确定结果类型
    std::string resultType = getResultType(opcode, left, right);
    
    // 创建临时变量存储结果
    std::string tempName = newTemp(resultType);
    IROperand result = IROperand::temp(tempName, resultType);
    
    // 生成指令
    emit(opcode, result, {left, right}, "Binary expression", binExpr->location.line);
    
    return result;
}

inline IROperand IRGenerator::processUnaryExpr(const std::shared_ptr<UnaryExpr>& unaryExpr) {
    IROperand operand = processExpression(unaryExpr->operand);
    
    IROpcode opcode;
    switch (unaryExpr->op) {
        case TokenType::OP_MINUS: opcode = IROpcode::NEG; break;
        case TokenType::OP_BANG: opcode = IROpcode::NOT; break;
        case TokenType::OP_TILDE: opcode = IROpcode::BIT_NOT; break;
        default:
            return IROperand::none();
    }
    
    std::string resultType = operand.dataType;
    std::string tempName = newTemp(resultType);
    IROperand result = IROperand::temp(tempName, resultType);
    
    emit(opcode, result, {operand}, "Unary expression", unaryExpr->location.line);
    
    return result;
}

inline IROperand IRGenerator::processCallExpr(const std::shared_ptr<CallExpr>& callExpr) {
    // 处理参数
    std::vector<IROperand> argOperands;
    for (const auto& arg : callExpr->arguments) {
        argOperands.push_back(processExpression(arg));
    }
    
    // 获取函数名
    std::string funcName;
    if (auto ident = std::dynamic_pointer_cast<Identifier>(callExpr->callee)) {
        funcName = ident->name;
    }
    
    // 参数指令
    for (const auto& arg : argOperands) {
        emit(IROpcode::PARAM, IROperand::none(), {arg}, "Parameter", callExpr->location.line);
    }
    
    // 调用指令
    IROperand funcOp = IROperand::function(funcName);
    std::string tempName = newTemp("unknown");
    IROperand result = IROperand::temp(tempName);
    
    emit(IROpcode::CALL, result, {funcOp}, 
         "Call " + funcName, callExpr->location.line);
    
    return result;
}

inline IROperand IRGenerator::processAssignmentExpr(const std::shared_ptr<AssignmentExpr>& assignExpr) {
    IROperand value = processExpression(assignExpr->value);
    
    // 处理左值
    IROperand target;
    if (auto ident = std::dynamic_pointer_cast<Identifier>(assignExpr->target)) {
        target = IROperand::variable(ident->name, value.dataType);
        variables_[ident->name] = target;
    }
    
    // 生成存储指令
    emit(IROpcode::MOVE, target, {value}, "Assignment", assignExpr->location.line);
    
    return target;
}

inline IROperand IRGenerator::processTernaryExpr(const std::shared_ptr<TernaryExpr>& ternExpr) {
    std::string elseLabel = newLabel("ternary_else");
    std::string endLabel = newLabel("ternary_end");
    
    IROperand condition = processExpression(ternExpr->condition);
    
    // 如果条件为假，跳转到else分支
    emit(IROpcode::JZ, IROperand::none(), {condition, IROperand::label(elseLabel)},
         "Ternary condition", ternExpr->location.line);
    
    // Then分支
    IROperand thenValue = processExpression(ternExpr->thenExpr);
    std::string tempName = newTemp(thenValue.dataType);
    IROperand result = IROperand::temp(tempName, thenValue.dataType);
    emit(IROpcode::MOVE, result, {thenValue}, "Ternary then value");
    emit(IROpcode::JMP, IROperand::none(), {IROperand::label(endLabel)}, "Jump to end");
    
    // Else分支
    emit(IROpcode::LABEL, IROperand::label(elseLabel), {}, "Else branch");
    IROperand elseValue = processExpression(ternExpr->elseExpr);
    emit(IROpcode::MOVE, result, {elseValue}, "Ternary else value");
    
    // 结束
    emit(IROpcode::LABEL, IROperand::label(endLabel), {}, "End ternary");
    
    return result;
}

inline IROperand IRGenerator::processArrayLiteral(const std::shared_ptr<ArrayLiteral>& arrayLit) {
    // 分配数组空间
    std::string tempName = newTemp("array");
    IROperand array = IROperand::temp(tempName, "array");
    
    // 计算数组大小
    int size = arrayLit->elements.size();
    emit(IROpcode::ALLOCA, array, 
         {IROperand::constant(std::to_string(size))},
         "Allocate array", arrayLit->location.line);
    
    // 初始化每个元素
    for (int i = 0; i < size; ++i) {
        IROperand elem = processExpression(arrayLit->elements[i]);
        emit(IROpcode::STORE, IROperand::none(),
             {array, IROperand::constant(std::to_string(i)), elem},
             "Store array element");
    }
    
    return array;
}

inline IROperand IRGenerator::processIndexExpr(const std::shared_ptr<IndexExpr>& indexExpr) {
    IROperand array = processExpression(indexExpr->object);
    IROperand index = processExpression(indexExpr->index);
    
    std::string tempName = newTemp("element");
    IROperand result = IROperand::temp(tempName);
    
    emit(IROpcode::LOAD, result, {array, index},
         "Array access", indexExpr->location.line);
    
    return result;
}

inline IROperand IRGenerator::processMemberExpr(const std::shared_ptr<MemberExpr>& memberExpr) {
    IROperand object = processExpression(memberExpr->object);
    
    // GEP指令：获取成员指针
    std::string tempName = newTemp("member");
    IROperand result = IROperand::temp(tempName);
    
    emit(IROpcode::GEP, result, {object, IROperand::constant(memberExpr->member)},
         "Member access", memberExpr->location.line);
    
    return result;
}

inline void IRGenerator::processStatement(const StmtPtr& stmt) {
    if (!stmt) return;
    
    switch (stmt->nodeType) {
        case ASTNodeType::EXPRESSION_STMT:
            processExpressionStmt(std::static_pointer_cast<ExpressionStmt>(stmt));
            break;
        case ASTNodeType::BLOCK_STMT:
            processBlockStmt(std::static_pointer_cast<BlockStmt>(stmt));
            break;
        case ASTNodeType::IF_STMT:
            processIfStmt(std::static_pointer_cast<IfStmt>(stmt));
            break;
        case ASTNodeType::WHILE_STMT:
            processWhileStmt(std::static_pointer_cast<WhileStmt>(stmt));
            break;
        case ASTNodeType::FOR_STMT:
            processForStmt(std::static_pointer_cast<ForStmt>(stmt));
            break;
        case ASTNodeType::RETURN_STMT:
            processReturnStmt(std::static_pointer_cast<ReturnStmt>(stmt));
            break;
        case ASTNodeType::BREAK_STMT:
            processBreakStmt(std::static_pointer_cast<BreakStmt>(stmt));
            break;
        case ASTNodeType::CONTINUE_STMT:
            processContinueStmt(std::static_pointer_cast<ContinueStmt>(stmt));
            break;
        default:
            break;
    }
}

inline void IRGenerator::processExpressionStmt(const std::shared_ptr<ExpressionStmt>& exprStmt) {
    processExpression(exprStmt->expression);
}

inline void IRGenerator::processBlockStmt(const std::shared_ptr<BlockStmt>& blockStmt) {
    for (const auto& stmt : blockStmt->statements) {
        processStatement(stmt);
    }
}

inline void IRGenerator::processIfStmt(const std::shared_ptr<IfStmt>& ifStmt) {
    std::string elseLabel = newLabel("if_else");
    std::string endLabel = newLabel("if_end");
    
    // 处理条件
    IROperand condition = processExpression(ifStmt->condition);
    
    // 条件为假时跳转
    if (ifStmt->elseBranch) {
        emit(IROpcode::JZ, IROperand::none(), 
             {condition, IROperand::label(elseLabel)},
             "If condition", ifStmt->location.line);
    } else {
        emit(IROpcode::JZ, IROperand::none(),
             {condition, IROperand::label(endLabel)},
             "If condition", ifStmt->location.line);
    }
    
    // Then分支
    processStatement(ifStmt->thenBranch);
    if (ifStmt->elseBranch) {
        emit(IROpcode::JMP, IROperand::none(), {IROperand::label(endLabel)}, "Jump to end");
        
        // Else分支
        emit(IROpcode::LABEL, IROperand::label(elseLabel), {}, "Else branch");
        processStatement(ifStmt->elseBranch);
    }
    
    // 结束
    emit(IROpcode::LABEL, IROperand::label(endLabel), {}, "End if");
}

inline void IRGenerator::processWhileStmt(const std::shared_ptr<WhileStmt>& whileStmt) {
    std::string startLabel = newLabel("while_start");
    std::string bodyLabel = newLabel("while_body");
    std::string endLabel = newLabel("while_end");
    
    // 记录循环信息
    loopStack_.push({startLabel, endLabel});
    
    // 开始标签
    emit(IROpcode::LABEL, IROperand::label(startLabel), {}, "While start", whileStmt->location.line);
    
    // 条件检查
    IROperand condition = processExpression(whileStmt->condition);
    emit(IROpcode::JZ, IROperand::none(), {condition, IROperand::label(endLabel)}, "While condition");
    
    // 循环体
    emit(IROpcode::LABEL, IROperand::label(bodyLabel), {}, "While body");
    processStatement(whileStmt->body);
    emit(IROpcode::JMP, IROperand::none(), {IROperand::label(startLabel)}, "Jump to start");
    
    // 结束标签
    emit(IROpcode::LABEL, IROperand::label(endLabel), {}, "While end");
    
    loopStack_.pop();
}

inline void IRGenerator::processForStmt(const std::shared_ptr<ForStmt>& forStmt) {
    std::string startLabel = newLabel("for_start");
    std::string bodyLabel = newLabel("for_body");
    std::string updateLabel = newLabel("for_update");
    std::string endLabel = newLabel("for_end");
    
    // 记录循环信息
    loopStack_.push({updateLabel, endLabel});
    
    // 初始化
    if (!std::holds_alternative<std::monostate>(forStmt->initializer)) {
        if (std::holds_alternative<DeclPtr>(forStmt->initializer)) {
            processDeclaration(std::get<DeclPtr>(forStmt->initializer));
        } else if (std::holds_alternative<StmtPtr>(forStmt->initializer)) {
            processStatement(std::get<StmtPtr>(forStmt->initializer));
        }
    }
    
    // 开始标签
    emit(IROpcode::LABEL, IROperand::label(startLabel), {}, "For start", forStmt->location.line);
    
    // 条件检查
    if (forStmt->condition) {
        IROperand condition = processExpression(forStmt->condition);
        emit(IROpcode::JZ, IROperand::none(), {condition, IROperand::label(endLabel)}, "For condition");
    }
    
    // 循环体
    emit(IROpcode::LABEL, IROperand::label(bodyLabel), {}, "For body");
    processStatement(forStmt->body);
    
    // 更新部分
    emit(IROpcode::LABEL, IROperand::label(updateLabel), {}, "For update");
    if (forStmt->increment) {
        processExpression(forStmt->increment);
    }
    emit(IROpcode::JMP, IROperand::none(), {IROperand::label(startLabel)}, "Jump to start");
    
    // 结束标签
    emit(IROpcode::LABEL, IROperand::label(endLabel), {}, "For end");
    
    loopStack_.pop();
}

inline void IRGenerator::processReturnStmt(const std::shared_ptr<ReturnStmt>& returnStmt) {
    if (returnStmt->value) {
        IROperand value = processExpression(returnStmt->value);
        emit(IROpcode::RET, IROperand::none(), {value}, "Return", returnStmt->location.line);
    } else {
        emit(IROpcode::RET, IROperand::none(), {}, "Return void", returnStmt->location.line);
    }
}

inline void IRGenerator::processBreakStmt(const std::shared_ptr<BreakStmt>& breakStmt) {
    if (!loopStack_.empty()) {
        emit(IROpcode::JMP, IROperand::none(),
             {IROperand::label(loopStack_.top().breakLabel)},
             "Break", breakStmt->location.line);
    }
}

inline void IRGenerator::processContinueStmt(const std::shared_ptr<ContinueStmt>& continueStmt) {
    if (!loopStack_.empty()) {
        emit(IROpcode::JMP, IROperand::none(),
             {IROperand::label(loopStack_.top().continueLabel)},
             "Continue", continueStmt->location.line);
    }
}

inline void IRGenerator::processDeclaration(const DeclPtr& decl) {
    if (!decl) return;
    
    switch (decl->nodeType) {
        case ASTNodeType::VARIABLE_DECL:
            processVariableDecl(std::static_pointer_cast<VariableDecl>(decl));
            break;
        case ASTNodeType::FUNCTION_DECL:
            processFunctionDecl(std::static_pointer_cast<FunctionDecl>(decl));
            break;
        case ASTNodeType::STRUCT_DECL:
            processStructDecl(std::static_pointer_cast<StructDecl>(decl));
            break;
        case ASTNodeType::ENUM_DECL:
            processEnumDecl(std::static_pointer_cast<EnumDecl>(decl));
            break;
        default:
            break;
    }
}

inline void IRGenerator::processVariableDecl(const std::shared_ptr<VariableDecl>& varDecl) {
    std::string varName = varDecl->name;
    std::string varType = typeToString(varDecl->type);
    
    // 分配空间
    IROperand var = IROperand::variable(varName, varType);
    variables_[varName] = var;
    
    emit(IROpcode::ALLOCA, var, {IROperand::constant("1")},
         "Variable " + varName + " : " + varType, varDecl->location.line);
    
    // 初始化
    if (varDecl->initializer) {
        IROperand initValue = processExpression(varDecl->initializer);
        emit(IROpcode::STORE, IROperand::none(), {var, initValue},
             "Initialize " + varName);
    }
}

inline void IRGenerator::processFunctionDecl(const std::shared_ptr<FunctionDecl>& funcDecl) {
    // 查找已创建的函数框架
    for (auto& func : currentModule_->functions) {
        if (func->name == funcDecl->name) {
            currentFunction_ = func;
            break;
        }
    }
    
    if (!currentFunction_) {
        currentFunction_ = std::make_shared<IRFunction>(funcDecl->name);
        currentModule_->addFunction(currentFunction_);
    }
    
    // 重置状态
    tempCounter_ = 0;
    labelCounter_ = 0;
    variables_.clear();
    currentBlock_ = nullptr;
    
    // 添加参数到符号表
    for (const auto& param : funcDecl->parameters) {
        IROperand paramOp = IROperand::variable(param->name, typeToString(param->type));
        variables_[param->name] = paramOp;
    }
    
    // 处理函数体
    if (funcDecl->body) {
        processStatement(funcDecl->body);
    }
    
    // 如果函数没有返回语句，添加隐式返回
    if (currentBlock_ && !currentBlock_->instructions.empty()) {
        auto lastInst = currentBlock_->instructions.back();
        if (lastInst->opcode != IROpcode::RET) {
            emit(IROpcode::RET, IROperand::none(), {}, "Implicit return");
        }
    }
    
    currentFunction_ = nullptr;
    currentBlock_ = nullptr;
}

inline void IRGenerator::processStructDecl(const std::shared_ptr<StructDecl>& structDecl) {
    // 结构体声明主要用于类型信息，IR层面暂不处理
    // 在后续优化中可以添加结构体布局信息
}

inline void IRGenerator::processEnumDecl(const std::shared_ptr<EnumDecl>& enumDecl) {
    // 枚举声明主要用于类型信息，IR层面暂不处理
}

inline std::string IRGenerator::typeToString(const TypePtr& type) {
    if (!type) return "void";
    
    if (type->nodeType == ASTNodeType::PRIMITIVE_TYPE) {
        auto primType = std::static_pointer_cast<PrimitiveType>(type);
        switch (primType->kind) {
            case PrimitiveTypeKind::INT: return "int";
            case PrimitiveTypeKind::FLOAT: return "float";
            case PrimitiveTypeKind::STRING: return "string";
            case PrimitiveTypeKind::BOOL: return "bool";
            case PrimitiveTypeKind::VOID: return "void";
            case PrimitiveTypeKind::CHAR: return "char";
            default: return "unknown";
        }
    } else if (type->nodeType == ASTNodeType::NAMED_TYPE) {
        auto namedType = std::static_pointer_cast<NamedType>(type);
        return namedType->name;
    } else if (type->nodeType == ASTNodeType::ARRAY_TYPE) {
        auto arrayType = std::static_pointer_cast<ArrayType>(type);
        return typeToString(arrayType->elementType) + "[]";
    }
    
    return "unknown";
}

inline std::string IRGenerator::getResultType(IROpcode opcode, const IROperand& left, const IROperand& right) {
    // 算术运算：如果任一操作数是float，结果是float
    if (opcode == IROpcode::ADD || opcode == IROpcode::SUB ||
        opcode == IROpcode::MUL || opcode == IROpcode::DIV ||
        opcode == IROpcode::MOD) {
        if (left.dataType == "float" || right.dataType == "float") {
            return "float";
        }
        return "int";
    }
    
    // 比较运算：结果是bool
    if (opcode == IROpcode::EQ || opcode == IROpcode::NE ||
        opcode == IROpcode::LT || opcode == IROpcode::LE ||
        opcode == IROpcode::GT || opcode == IROpcode::GE) {
        return "bool";
    }
    
    // 逻辑运算：结果是bool
    if (opcode == IROpcode::AND || opcode == IROpcode::OR ||
        opcode == IROpcode::NOT) {
        return "bool";
    }
    
    // 默认返回左操作数类型
    return left.dataType;
}

inline IROperand IRGenerator::addCastIfNeeded(const IROperand& operand, const std::string& targetType) {
    if (operand.dataType == targetType) {
        return operand;
    }
    
    std::string tempName = newTemp(targetType);
    IROperand result = IROperand::temp(tempName, targetType);
    
    emit(IROpcode::CAST, result, {operand}, 
         "Cast from " + operand.dataType + " to " + targetType);
    
    return result;
}

} // namespace nova

#endif // NOVA_IR_GENERATOR_H
