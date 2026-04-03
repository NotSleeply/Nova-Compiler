# Nova Compiler 单元测试

## 概述

Nova Compiler 包含全面的单元测试套件，覆盖编译器的所有核心模块。测试框架采用简化的 Catch2 风格实现，无需外部依赖。

## 测试框架

### 特性

- ✅ **简单易用** - 类 Catch2 的测试宏，学习成本低
- ✅ **无依赖** - 单头文件实现，无需外部库
- ✅ **自动注册** - 测试用例自动注册，无需手动维护列表
- ✅ **详细报告** - 清晰的测试结果输出
- ✅ **异常支持** - 支持异常测试和验证

### 使用方法

```cpp
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

TEST_CASE("测试名称", "[标签]") {
    REQUIRE(expression);  // 失败时抛出异常
    CHECK(expression);    // 失败时继续执行
}
```

## 测试套件

### 1. 词法分析器测试 (`test_lexer.cpp`)

**测试数量**: 100+ 测试用例

**覆盖范围**:
- 关键字识别（17个关键字）
- 标识符解析（变量名、函数名）
- 字面量解析（整数、浮点、字符串、字符、布尔）
- 运算符识别（算术、比较、逻辑、赋值、位运算）
- 分隔符识别（括号、花括号、方括号、分号等）
- 注释处理（单行、多行、嵌套）
- 转义序列处理
- 位置跟踪（行号、列号）
- 错误处理（未终止字符串、非法字符等）

**示例测试**:
```cpp
TEST_CASE("Lexer recognizes integer literals", "[lexer][literals]") {
    Lexer lexer("42 0 123456789", "integers.nv");
    std::vector<Token> tokens = lexer.tokenize();
    
    CHECK(tokens[0].type == TokenType::INTEGER_LITERAL);
    CHECK(std::get<int64_t>(tokens[0].value) == 42);
}
```

### 2. 语法分析器测试 (`test_parser.cpp`)

**测试数量**: 80+ 测试用例

**覆盖范围**:
- 函数声明解析（参数、返回类型、函数体）
- 变量声明解析（let、const）
- 表达式解析（字面量、二元、一元、函数调用）
- 运算符优先级测试
- 语句解析（return、if、while、for、代码块）
- 类型解析（基本类型、数组类型、命名类型）
- 错误处理和恢复

**示例测试**:
```cpp
TEST_CASE("Parser parses function with parameters", "[parser][functions]") {
    auto program = parseSource("fn add(a: int, b: int) -> int { return a + b; }");
    
    auto funcDecl = std::dynamic_pointer_cast<FunctionDecl>(program->declarations[0]);
    CHECK(funcDecl->name == "add");
    CHECK(funcDecl->params.size() == 2);
}
```

### 3. 语义分析测试 (`test_semantic.cpp`)

**测试数量**: 70+ 测试用例

**覆盖范围**:
- 类型检查（基本类型、类型推导）
- 二元表达式类型检查
- 函数声明验证
- 函数调用验证（参数数量、类型匹配）
- 变量声明和使用验证
- 作用域管理（嵌套作用域、变量遮蔽）
- 控制流验证（if、while、for 条件类型）
- 错误检测和报告

**示例测试**:
```cpp
TEST_CASE("Semantic analyzer detects type mismatch", "[semantic][types][errors]") {
    std::vector<CompilationError> errors;
    bool result = analyzeSource("let x: int = \"string\";", errors);
    
    CHECK_FALSE(result);
    CHECK_FALSE(errors.empty());
}
```

### 4. IR 生成测试 (`test_ir.cpp`)

**测试数量**: 60+ 测试用例

**覆盖范围**:
- IR 模块生成（函数、全局变量）
- IR 函数生成（参数、返回类型、基本块）
- IR 基本块生成（指令序列、标签）
- 算术指令生成（ADD、SUB、MUL、DIV、MOD）
- 比较指令生成（EQ、NE、LT、LE、GT、GE）
- 逻辑指令生成（AND、OR、NOT）
- 控制流指令生成（JMP、JZ、JNZ、RET）
- 函数调用指令生成（CALL、PARAM）
- IR 操作数测试

**示例测试**:
```cpp
TEST_CASE("IR generates addition instruction", "[ir][instructions][arithmetic]") {
    auto module = generateIR("fn add(a: int, b: int) -> int { return a + b; }");
    
    auto& block = module->functions[0]->basicBlocks[0];
    bool hasAdd = false;
    for (const auto& inst : block->instructions) {
        if (inst->opcode == IROpcode::ADD) {
            hasAdd = true;
            break;
        }
    }
    CHECK(hasAdd);
}
```

### 5. 代码生成测试 (`test_codegen.cpp`)

**测试数量**: 50+ 测试用例

**覆盖范围**:
- C 代码结构验证（头文件、主函数）
- 函数定义生成（参数、返回类型）
- 类型转换（Nova 类型 → C 类型）
- 算术运算生成（+、-、*、/、%）
- 比较运算生成（==、!=、<、>、<=、>=）
- 逻辑运算生成（&&、||、!）
- 控制流生成（return、if、while）
- 函数调用生成
- 变量声明生成（临时变量、全局变量）
- 代码质量检查（格式化、注释）

**示例测试**:
```cpp
TEST_CASE("Generated code handles addition", "[codegen][arithmetic]") {
    auto code = generateCCode("fn add(a: int, b: int) -> int { return a + b; }");
    
    CHECK(code.code.find("int add(int a, int b)") != std::string::npos);
    CHECK(code.code.find("+") != std::string::npos);
}
```

## 构建和运行测试

### 构建所有测试

```bash
make -f Makefile.test build-tests
```

### 运行所有测试

```bash
make -f Makefile.test test
```

### 运行特定测试

```bash
make -f Makefile.test test-lexer      # 运行词法分析器测试
make -f Makefile.test test-parser     # 运行语法分析器测试
make -f Makefile.test test-semantic   # 运行语义分析测试
make -f Makefile.test test-ir         # 运行 IR 生成测试
make -f Makefile.test test-codegen    # 运行代码生成测试
```

### 详细输出

```bash
make -f Makefile.test test-verbose
```

### 清理测试文件

```bash
make -f Makefile.test clean
```

## 测试统计

| 模块 | 测试用例数 | 覆盖范围 |
|------|-----------|---------|
| 词法分析器 | 100+ | 关键字、标识符、字面量、运算符、注释、错误处理 |
| 语法分析器 | 80+ | 声明、表达式、语句、类型、错误处理 |
| 语义分析 | 70+ | 类型检查、作用域、函数调用、控制流 |
| IR 生成 | 60+ | 指令生成、基本块、控制流图 |
| 代码生成 | 50+ | C代码输出、类型映射、指令映射 |
| **总计** | **360+** | **编译器前端全流程** |

## 测试原则

1. **单元测试优先** - 每个模块独立测试，确保正确性
2. **边界情况覆盖** - 测试空输入、极限值、非法输入
3. **错误处理验证** - 确保错误被正确检测和报告
4. **集成测试补充** - 测试模块间的协作
5. **可读性优先** - 测试代码清晰易懂

## 未来改进

- [ ] 添加性能测试
- [ ] 添加模糊测试
- [ ] 集成代码覆盖率工具
- [ ] 添加持续集成测试
- [ ] 扩展测试用例（更复杂的场景）

## 贡献指南

添加新测试时：
1. 遵循现有测试的命名规范
2. 为测试添加适当的标签
3. 测试边界情况和错误处理
4. 确保测试独立且可重复运行

## 联系方式

如有问题或建议，请提交 Issue 或 Pull Request。
