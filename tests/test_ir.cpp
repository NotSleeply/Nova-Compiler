/**
 * @file test_ir.cpp
 * @brief Unit tests for the IR Generator module
 * 
 * This file contains comprehensive tests for IR generation,
 * testing instruction generation, basic blocks, and control flow.
 */

#include "catch2/catch.hpp"
#include "ir/ir_generator.h"
#include "semantic/semantic_analyzer.h"
#include "parser/parser.h"
#include "lexer/lexer.h"

using namespace nova;

// Helper function to generate IR
IRModulePtr generateIR(const std::string& source) {
    Lexer lexer(source, "test.nv");
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    
    SemanticAnalyzer analyzer;
    analyzer.analyze(program);
    
    IRGenerator irGen;
    return irGen.generate(program);
}

// ============================================================================
// IR Module Tests
// ============================================================================

TEST_CASE("IR generator creates module", "[ir][module]") {
    auto module = generateIR("fn foo() -> void { }");
    
    REQUIRE(module != nullptr);
    CHECK_FALSE(module->name.empty());
}

TEST_CASE("IR module contains functions", "[ir][module]") {
    auto module = generateIR(
        "fn foo() -> void { }\n"
        "fn bar() -> void { }\n"
    );
    
    REQUIRE(module != nullptr);
    CHECK(module->functions.size() == 2);
}

TEST_CASE("IR module tracks global variables", "[ir][module]") {
    auto module = generateIR("let x: int = 42;");
    
    REQUIRE(module != nullptr);
    CHECK(module->globalVariables.size() == 1);
}

// ============================================================================
// IR Function Tests
// ============================================================================

TEST_CASE("IR generator creates function", "[ir][functions]") {
    auto module = generateIR("fn foo() -> void { }");
    
    REQUIRE(module->functions.size() == 1);
    auto& func = module->functions[0];
    
    CHECK(func->name == "foo");
    CHECK(func->returnType == "void");
}

TEST_CASE("IR function has parameters", "[ir][functions]") {
    auto module = generateIR("fn add(a: int, b: int) -> int { return a + b; }");
    
    REQUIRE(module->functions.size() == 1);
    auto& func = module->functions[0];
    
    CHECK(func->parameters.size() == 2);
    CHECK(func->parameters[0].first == "a");
    CHECK(func->parameters[0].second == "int");
    CHECK(func->parameters[1].first == "b");
    CHECK(func->parameters[1].second == "int");
}

TEST_CASE("IR function has basic blocks", "[ir][functions]") {
    auto module = generateIR("fn foo() -> void { return; }");
    
    REQUIRE(module->functions.size() == 1);
    auto& func = module->functions[0];
    
    CHECK_FALSE(func->basicBlocks.empty());
}

// ============================================================================
// IR Basic Block Tests
// ============================================================================

TEST_CASE("IR basic block has name", "[ir][basic-blocks]") {
    auto module = generateIR("fn foo() -> void { return; }");
    
    auto& func = module->functions[0];
    auto& block = func->basicBlocks[0];
    
    CHECK_FALSE(block->name.empty());
}

TEST_CASE("IR basic block contains instructions", "[ir][basic-blocks]") {
    auto module = generateIR("fn foo() -> void { return; }");
    
    auto& func = module->functions[0];
    auto& block = func->basicBlocks[0];
    
    CHECK_FALSE(block->instructions.empty());
}

// ============================================================================
// IR Instruction Tests - Arithmetic
// ============================================================================

TEST_CASE("IR generates addition instruction", "[ir][instructions][arithmetic]") {
    auto module = generateIR("fn add(a: int, b: int) -> int { return a + b; }");
    
    auto& func = module->functions[0];
    auto& block = func->basicBlocks[0];
    
    // Find ADD instruction
    bool hasAdd = false;
    for (const auto& inst : block->instructions) {
        if (inst->opcode == IROpcode::ADD) {
            hasAdd = true;
            break;
        }
    }
    
    CHECK(hasAdd);
}

TEST_CASE("IR generates subtraction instruction", "[ir][instructions][arithmetic]") {
    auto module = generateIR("fn sub(a: int, b: int) -> int { return a - b; }");
    
    auto& func = module->functions[0];
    auto& block = func->basicBlocks[0];
    
    bool hasSub = false;
    for (const auto& inst : block->instructions) {
        if (inst->opcode == IROpcode::SUB) {
            hasSub = true;
            break;
        }
    }
    
    CHECK(hasSub);
}

TEST_CASE("IR generates multiplication instruction", "[ir][instructions][arithmetic]") {
    auto module = generateIR("fn mul(a: int, b: int) -> int { return a * b; }");
    
    auto& func = module->functions[0];
    auto& block = func->basicBlocks[0];
    
    bool hasMul = false;
    for (const auto& inst : block->instructions) {
        if (inst->opcode == IROpcode::MUL) {
            hasMul = true;
            break;
        }
    }
    
    CHECK(hasMul);
}

TEST_CASE("IR generates division instruction", "[ir][instructions][arithmetic]") {
    auto module = generateIR("fn div(a: int, b: int) -> int { return a / b; }");
    
    auto& func = module->functions[0];
    auto& block = func->basicBlocks[0];
    
    bool hasDiv = false;
    for (const auto& inst : block->instructions) {
        if (inst->opcode == IROpcode::DIV) {
            hasDiv = true;
            break;
        }
    }
    
    CHECK(hasDiv);
}

TEST_CASE("IR generates modulo instruction", "[ir][instructions][arithmetic]") {
    auto module = generateIR("fn mod(a: int, b: int) -> int { return a % b; }");
    
    auto& func = module->functions[0];
    auto& block = func->basicBlocks[0];
    
    bool hasMod = false;
    for (const auto& inst : block->instructions) {
        if (inst->opcode == IROpcode::MOD) {
            hasMod = true;
            break;
        }
    }
    
    CHECK(hasMod);
}

// ============================================================================
// IR Instruction Tests - Comparison
// ============================================================================

TEST_CASE("IR generates equality comparison", "[ir][instructions][comparison]") {
    auto module = generateIR("fn eq(a: int, b: int) -> bool { return a == b; }");
    
    auto& func = module->functions[0];
    auto& block = func->basicBlocks[0];
    
    bool hasEq = false;
    for (const auto& inst : block->instructions) {
        if (inst->opcode == IROpcode::EQ) {
            hasEq = true;
            break;
        }
    }
    
    CHECK(hasEq);
}

TEST_CASE("IR generates less-than comparison", "[ir][instructions][comparison]") {
    auto module = generateIR("fn lt(a: int, b: int) -> bool { return a < b; }");
    
    auto& func = module->functions[0];
    auto& block = func->basicBlocks[0];
    
    bool hasLt = false;
    for (const auto& inst : block->instructions) {
        if (inst->opcode == IROpcode::LT) {
            hasLt = true;
            break;
        }
    }
    
    CHECK(hasLt);
}

TEST_CASE("IR generates greater-than comparison", "[ir][instructions][comparison]") {
    auto module = generateIR("fn gt(a: int, b: int) -> bool { return a > b; }");
    
    auto& func = module->functions[0];
    auto& block = func->basicBlocks[0];
    
    bool hasGt = false;
    for (const auto& inst : block->instructions) {
        if (inst->opcode == IROpcode::GT) {
            hasGt = true;
            break;
        }
    }
    
    CHECK(hasGt);
}

// ============================================================================
// IR Instruction Tests - Control Flow
// ============================================================================

TEST_CASE("IR generates return instruction", "[ir][instructions][control-flow]") {
    auto module = generateIR("fn foo() -> int { return 42; }");
    
    auto& func = module->functions[0];
    auto& block = func->basicBlocks[0];
    
    bool hasRet = false;
    for (const auto& inst : block->instructions) {
        if (inst->opcode == IROpcode::RET) {
            hasRet = true;
            break;
        }
    }
    
    CHECK(hasRet);
}

TEST_CASE("IR generates jump instructions for if", "[ir][instructions][control-flow]") {
    auto module = generateIR(
        "fn foo(x: int) -> int {\n"
        "    if (x > 0) {\n"
        "        return x;\n"
        "    }\n"
        "    return 0;\n"
        "}\n"
    );
    
    auto& func = module->functions[0];
    
    // Should have multiple basic blocks for if-else
    CHECK(func->basicBlocks.size() >= 2);
    
    // Should have jump instructions
    bool hasJmp = false;
    for (const auto& block : func->basicBlocks) {
        for (const auto& inst : block->instructions) {
            if (inst->opcode == IROpcode::JMP || 
                inst->opcode == IROpcode::JZ || 
                inst->opcode == IROpcode::JNZ) {
                hasJmp = true;
                break;
            }
        }
    }
    
    CHECK(hasJmp);
}

TEST_CASE("IR generates jump instructions for while", "[ir][instructions][control-flow]") {
    auto module = generateIR(
        "fn countdown(n: int) -> int {\n"
        "    while (n > 0) {\n"
        "        n = n - 1;\n"
        "    }\n"
        "    return n;\n"
        "}\n"
    );
    
    auto& func = module->functions[0];
    
    // Should have multiple basic blocks for while loop
    CHECK(func->basicBlocks.size() >= 3);
}

// ============================================================================
// IR Instruction Tests - Function Calls
// ============================================================================

TEST_CASE("IR generates call instruction", "[ir][instructions][calls]") {
    auto module = generateIR(
        "fn square(x: int) -> int {\n"
        "    return x * x;\n"
        "}\n"
        "fn test() -> int {\n"
        "    return square(5);\n"
        "}\n"
    );
    
    REQUIRE(module->functions.size() == 2);
    
    // Find test function
    IRFuncPtr testFunc;
    for (const auto& func : module->functions) {
        if (func->name == "test") {
            testFunc = func;
            break;
        }
    }
    
    REQUIRE(testFunc != nullptr);
    
    // Find CALL instruction
    bool hasCall = false;
    for (const auto& block : testFunc->basicBlocks) {
        for (const auto& inst : block->instructions) {
            if (inst->opcode == IROpcode::CALL) {
                hasCall = true;
                break;
            }
        }
    }
    
    CHECK(hasCall);
}

// ============================================================================
// IR Operand Tests
// ============================================================================

TEST_CASE("IR operand can be variable", "[ir][operands]") {
    IROperand op = IROperand::variable("x", "int");
    
    CHECK(op.type == IROperand::Type::VARIABLE);
    CHECK(op.value == "x");
    CHECK(op.dataType == "int");
}

TEST_CASE("IR operand can be constant", "[ir][operands]") {
    IROperand op = IROperand::constant("42", "int");
    
    CHECK(op.type == IROperand::Type::CONSTANT);
    CHECK(op.value == "42");
}

TEST_CASE("IR operand can be temporary", "[ir][operands]") {
    IROperand op = IROperand::temp("t0", "int");
    
    CHECK(op.type == IROperand::Type::TEMPORARY);
    CHECK(op.value == "t0");
}

TEST_CASE("IR operand can be label", "[ir][operands]") {
    IROperand op = IROperand::label("entry");
    
    CHECK(op.type == IROperand::Type::LABEL);
    CHECK(op.value == "entry");
}

TEST_CASE("IR operand can be function", "[ir][operands]") {
    IROperand op = IROperand::function("foo");
    
    CHECK(op.type == IROperand::Type::FUNCTION);
    CHECK(op.value == "foo");
}

// ============================================================================
// IR Instruction Tests
// ============================================================================

TEST_CASE("IR instruction has opcode", "[ir][instructions]") {
    IRInstruction inst(IROpcode::ADD);
    
    CHECK(inst.opcode == IROpcode::ADD);
}

TEST_CASE("IR instruction has destination", "[ir][instructions]") {
    auto dest = IROperand::temp("t0", "int");
    IRInstruction inst(IROpcode::ADD, dest);
    
    CHECK(inst.dest.type == IROperand::Type::TEMPORARY);
    CHECK(inst.dest.value == "t0");
}

TEST_CASE("IR instruction has operands", "[ir][instructions]") {
    auto dest = IROperand::temp("t0", "int");
    auto left = IROperand::variable("x", "int");
    auto right = IROperand::constant("1", "int");
    
    IRInstruction inst(IROpcode::ADD, dest, {left, right});
    
    CHECK(inst.operands.size() == 2);
    CHECK(inst.operands[0].value == "x");
    CHECK(inst.operands[1].value == "1");
}

TEST_CASE("IR instruction can be converted to string", "[ir][instructions]") {
    auto dest = IROperand::temp("t0", "int");
    auto left = IROperand::variable("x", "int");
    auto right = IROperand::constant("1", "int");
    
    IRInstruction inst(IROpcode::ADD, dest, {left, right});
    
    std::string str = inst.toString();
    CHECK_FALSE(str.empty());
    CHECK(str.find("add") != std::string::npos);
}

// ============================================================================
// IR Module Tests - Output
// ============================================================================

TEST_CASE("IR module can be converted to string", "[ir][module][output]") {
    auto module = generateIR("fn foo() -> void { return; }");
    
    std::string str = module->toString();
    CHECK_FALSE(str.empty());
    CHECK(str.find("function") != std::string::npos);
    CHECK(str.find("foo") != std::string::npos);
}

TEST_CASE("IR module string contains all functions", "[ir][module][output]") {
    auto module = generateIR(
        "fn foo() -> void { }\n"
        "fn bar() -> void { }\n"
    );
    
    std::string str = module->toString();
    CHECK(str.find("foo") != std::string::npos);
    CHECK(str.find("bar") != std::string::npos);
}

TEST_CASE("IR module string contains global variables", "[ir][module][output]") {
    auto module = generateIR("let x: int = 42;");
    
    std::string str = module->toString();
    CHECK(str.find("global") != std::string::npos);
    CHECK(str.find("x") != std::string::npos);
}
