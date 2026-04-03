# Nova 编译器架构设计

**版本**: v0.2.0  
**最后更新**: 2025-01-03

---

## 目录

1. [概述](#概述)
2. [整体架构](#整体架构)
3. [编译流程](#编译流程)
4. [模块设计](#模块设计)
5. [数据结构](#数据结构)
6. [错误处理](#错误处理)
7. [性能优化](#性能优化)
8. [扩展性设计](#扩展性设计)
9. [测试策略](#测试策略)
10. [未来规划](#未来规划)

---

## 概述

Nova 编译器是一个模块化、可扩展的现代编译器，采用经典的编译器设计模式，分为前端、中端和后端三个主要部分。

### 设计目标

- **模块化**: 各模块职责清晰，低耦合高内聚
- **可扩展**: 易于添加新特性和新目标平台
- **高性能**: 编译速度快，内存占用合理
- **可测试**: 每个模块都可独立测试
- **可维护**: 代码结构清晰，文档完善

### 技术栈

- **实现语言**: C++17
- **构建系统**: Make
- **测试框架**: Catch2 风格自定义框架
- **CI/CD**: GitHub Actions
- **目标平台**: C代码（transpilation）、LLVM IR、x86-64、WASM

---

## 整体架构

```
┌─────────────────────────────────────────────────────────────────┐
│                         Nova 编译器                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐      │
│  │  词法分析 │→│ 语法分析  │→│ 语义分析  │→│  IR生成   │      │
│  │  Lexer   │  │  Parser  │  │ Semantic │  │ IR Gen   │      │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘      │
│       ↓              ↓              ↓              ↓           │
│   Tokens          AST         Symbol Table      IR Module      │
│                                                                 │
│                       ┌──────────────────────┐                │
│                       │      代码生成         │                │
│                       │   Code Generation    │                │
│                       └──────────────────────┘                │
│                                ↓                               │
│          ┌─────────────────────┼─────────────────────┐        │
│          ↓                     ↓                     ↓         │
│     C CodeGen            LLVM CodeGen          Native CodeGen │
│     ───────────          ───────────          ────────────    │
│     C 代码输出            LLVM IR 输出         x86-64 输出     │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 三阶段架构

#### 1. 前端（Frontend）
- **词法分析**: 源代码 → 词元流
- **语法分析**: 词元流 → AST
- **语义分析**: AST + 符号表 + 类型检查

#### 2. 中端（Middle-end）
- **IR 生成**: AST → 中间表示
- **优化**: IR 转换和优化

#### 3. 后端（Backend）
- **代码生成**: IR → 目标代码
- **平台适配**: 不同目标平台的代码生成

---

## 编译流程

### 完整编译流程

```
源代码 (.nv)
    ↓
┌─────────────────┐
│   词法分析器     │
│   Lexer         │
└─────────────────┘
    ↓
词元流 (Tokens)
    ↓
┌─────────────────┐
│   语法分析器     │
│   Parser        │
└─────────────────┘
    ↓
抽象语法树 (AST)
    ↓
┌─────────────────┐
│   语义分析器     │
│   Semantic      │
└─────────────────┘
    ↓
符号表 + 类型信息
    ↓
┌─────────────────┐
│   IR 生成器      │
│   IR Generator  │
└─────────────────┘
    ↓
中间表示 (IR)
    ↓
┌─────────────────┐
│   代码生成器     │
│   Code Generator│
└─────────────────┘
    ↓
目标代码 (.c / .ll / .s / .wasm)
```

### 分阶段编译

Nova 支持分阶段编译，可以单独运行每个阶段：

```bash
# 仅词法分析
./bin/novac --lex source.nv

# 仅语法分析
./bin/novac --parse source.nv

# 仅语义分析
./bin/novac --semantic source.nv

# 仅 IR 生成
./bin/novac --ir source.nv

# 仅代码生成
./bin/novac --codegen source.nv
```

---

## 模块设计

### 1. 词法分析器 (Lexer)

**职责**:
- 将源代码转换为词元流
- 过滤空白和注释
- 记录词元位置信息
- 检测词法错误

**实现**:
```cpp
class Lexer {
public:
    Lexer(const std::string& source, const std::string& filename);
    
    std::vector<Token> tokenize();
    
private:
    Token scanToken();
    Token scanNumber();
    Token scanString();
    Token scanIdentifier();
    void skipWhitespace();
    void skipComment();
};
```

**数据结构**:
```cpp
struct Token {
    TokenType type;           // 词元类型
    std::string value;        // 词元值
    SourceLocation location;  // 源码位置
};

struct SourceLocation {
    std::string filename;
    int line;
    int column;
};
```

**测试覆盖**:
- 关键字识别（17 个关键字）
- 标识符解析
- 字面量解析（整数、浮点、字符串、字符、布尔）
- 运算符识别
- 分隔符识别
- 注释处理（单行、多行）
- 错误处理（非法字符、未闭合字符串）

---

### 2. 语法分析器 (Parser)

**职责**:
- 将词元流转换为 AST
- 语法结构验证
- 错误恢复和同步
- 构建语法树

**实现**:
```cpp
class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    
    ASTNodePtr parse();
    
private:
    ASTNodePtr parseDeclaration();
    ASTNodePtr parseFunctionDecl();
    ASTNodePtr parseVariableDecl();
    ASTNodePtr parseStatement();
    ASTNodePtr parseExpression();
    ASTNodePtr parseBlock();
};
```

**AST 节点类型**:
```cpp
enum class NodeType {
    // 声明
    FunctionDecl,
    VariableDecl,
    StructDecl,
    EnumDecl,
    
    // 语句
    BlockStmt,
    IfStmt,
    WhileStmt,
    ForStmt,
    ReturnStmt,
    ExpressionStmt,
    
    // 表达式
    BinaryExpr,
    UnaryExpr,
    LiteralExpr,
    IdentifierExpr,
    CallExpr,
    ArrayExpr,
    
    // 类型
    TypeNode,
    ParameterNode
};
```

**测试覆盖**:
- 函数声明解析
- 变量声明解析
- 表达式解析（算术、比较、逻辑、赋值）
- 语句解析（if、while、for、return）
- 类型解析
- 错误处理和恢复

---

### 3. 语义分析器 (Semantic Analyzer)

**职责**:
- 类型检查
- 作用域管理
- 符号表构建
- 语义错误检测

**实现**:
```cpp
class SemanticAnalyzer {
public:
    SemanticAnalyzer();
    
    bool analyze(const ASTNodePtr& ast);
    const std::vector<CompilationError>& getErrors() const;
    
private:
    void analyzeFunction(const FunctionDeclNode* func);
    void analyzeVariable(const VariableDeclNode* var);
    void analyzeStatement(const ASTNode* stmt);
    Type analyzeExpression(const ASTNode* expr);
    
    void enterScope();
    void exitScope();
    void declareSymbol(const std::string& name, const Symbol& symbol);
    Symbol* lookupSymbol(const std::string& name);
};
```

**符号表设计**:
```cpp
struct Symbol {
    std::string name;
    SymbolKind kind;        // Variable, Function, Type, etc.
    Type type;
    SourceLocation location;
    Scope* scope;
};

class Scope {
public:
    void declare(const std::string& name, const Symbol& symbol);
    Symbol* lookup(const std::string& name);
    Symbol* lookupLocal(const std::string& name);
    
private:
    std::unordered_map<std::string, Symbol> symbols_;
    Scope* parent_;
};
```

**测试覆盖**:
- 类型检查（基本类型、复合类型）
- 作用域管理（全局、函数、块作用域）
- 函数调用验证
- 变量使用检查（未定义、未使用）
- 控制流分析

---

### 4. IR 生成器 (IR Generator)

**职责**:
- 将 AST 转换为中间表示
- 生成三地址码
- 划分基本块
- 构建控制流图

**实现**:
```cpp
class IRGenerator {
public:
    IRGenerator();
    
    IRModulePtr generate(const ASTNodePtr& ast);
    
private:
    IRFuncPtr generateFunction(const FunctionDeclNode* func);
    IRBasicBlockPtr generateStatement(const ASTNode* stmt);
    IROperand generateExpression(const ASTNode* expr);
    
    IROperand emitBinaryOp(IROpcode op, IROperand lhs, IROperand rhs);
    IROperand emitUnaryOp(IROpcode op, IROperand operand);
    void emitBranch(IROperand cond, IRBasicBlockPtr trueBB, IRBasicBlockPtr falseBB);
};
```

**IR 数据结构**:
```cpp
struct IRInstruction {
    IROpcode opcode;              // 操作码
    IROperand dest;               // 目标操作数
    std::vector<IROperand> srcs;  // 源操作数
};

struct IRBasicBlock {
    std::string label;
    std::vector<IRInstructionPtr> instructions;
    std::vector<IRBasicBlockPtr> predecessors;
    std::vector<IRBasicBlockPtr> successors;
};

struct IRFunc {
    std::string name;
    Type returnType;
    std::vector<std::pair<std::string, Type>> parameters;
    std::vector<IRBasicBlockPtr> basicBlocks;
};

struct IRModule {
    std::vector<IRFuncPtr> functions;
    std::vector<IRGlobalVarPtr> globalVars;
};
```

**IR 操作码**:
```cpp
enum class IROpcode {
    // 算术运算
    ADD, SUB, MUL, DIV, MOD, NEG,
    
    // 比较运算
    EQ, NE, LT, LE, GT, GE,
    
    // 逻辑运算
    AND, OR, NOT,
    
    // 内存操作
    LOAD, STORE, ALLOCA,
    
    // 控制流
    JMP, JZ, JNZ, RET, LABEL,
    
    // 函数调用
    CALL, PARAM,
    
    // 数据移动
    MOVE, COPY, CAST
};
```

**测试覆盖**:
- IR 模块结构
- 函数和基本块生成
- 算术运算指令
- 比较运算指令
- 逻辑运算指令
- 控制流指令（跳转、分支、返回）
- 函数调用指令

---

### 5. 代码生成器 (Code Generator)

**职责**:
- 将 IR 转换为目标代码
- 平台相关优化
- 代码格式化

**实现**:
```cpp
class CodeGenerator {
public:
    virtual ~CodeGenerator() = default;
    
    virtual std::string generate(const IRModulePtr& module) = 0;
    
    static std::unique_ptr<CodeGenerator> create(TargetPlatform platform);
};

class CCodeGenerator : public CodeGenerator {
public:
    std::string generate(const IRModulePtr& module) override;
    
private:
    void emitHeader();
    void emitGlobalVar(const IRGlobalVarPtr& var);
    void emitFunction(const IRFuncPtr& func);
    void emitBasicBlock(const IRBasicBlockPtr& block);
    void emitInstruction(const IRInstructionPtr& inst);
    
    std::string translateType(const Type& type);
    std::string translateOpcode(IROpcode opcode);
};
```

**平台支持**:
```cpp
enum class TargetPlatform {
    C,          // C 代码生成（transpilation）
    LLVM_IR,    // LLVM IR 生成
    X86_64,     // x86-64 汇编生成
    WASM        // WebAssembly 生成
};
```

**测试覆盖**:
- C 代码输出验证
- 类型转换正确性
- 运算符映射正确性
- 控制流生成
- 函数调用生成
- 代码质量检查

---

## 数据结构

### 核心数据结构

#### Token（词元）
```cpp
struct Token {
    TokenType type;
    std::string value;
    SourceLocation location;
};
```

#### AST（抽象语法树）
```cpp
struct ASTNode {
    NodeType type;
    SourceLocation location;
    std::vector<ASTNodePtr> children;
};

// 具体节点类型
struct FunctionDeclNode : public ASTNode {
    std::string name;
    Type returnType;
    std::vector<ParameterNode*> parameters;
    BlockStmtNode* body;
};
```

#### Symbol（符号）
```cpp
struct Symbol {
    std::string name;
    SymbolKind kind;
    Type type;
    SourceLocation location;
    Scope* scope;
};
```

#### Type（类型）
```cpp
struct Type {
    TypeKind kind;              // Basic, Array, Struct, Enum, etc.
    std::string name;
    std::vector<Type> typeArgs; // 泛型类型参数
    int arraySize;              // 数组大小
};
```

#### IR（中间表示）
```cpp
struct IRInstruction {
    IROpcode opcode;
    IROperand dest;
    std::vector<IROperand> srcs;
};

struct IROperand {
    IROperandKind kind;         // Variable, Temporary, Constant, etc.
    std::string value;
    std::string dataType;
};
```

---

## 错误处理

### 错误类型

```cpp
enum class ErrorType {
    Lexical,    // 词法错误
    Syntax,     // 语法错误
    Semantic,   // 语义错误
    IR,         // IR 生成错误
    CodeGen     // 代码生成错误
};
```

### 错误结构

```cpp
struct CompilationError {
    ErrorType type;
    std::string message;
    SourceLocation location;
    std::string suggestion;  // 修复建议
};
```

### 错误处理策略

1. **词法错误**: 立即报告，尝试恢复
2. **语法错误**: 错误恢复和同步，继续解析
3. **语义错误**: 收集所有错误，不中断编译
4. **IR 错误**: 致命错误，立即停止
5. **代码生成错误**: 致命错误，立即停止

### 错误报告示例

```
error: type mismatch in variable declaration
  --> examples/test.nv:10:5
   |
10 |     let x: int = "string";
   |     ^^^^^^^^^^^^^^^^^^^^^^ expected `int`, found `string`
   |
   = help: change the type annotation to `string` or use an integer literal
```

---

## 性能优化

### 编译速度优化

1. **增量编译**: 只重新编译修改的部分
2. **并行编译**: 并行处理独立模块
3. **内存池**: 减少 AST 节点的内存分配
4. **字符串驻留**: 减少字符串比较开销

### 运行时性能优化

1. **IR 优化**: 常量折叠、死代码消除
2. **平台优化**: 利用目标平台特性
3. **内联优化**: 函数内联
4. **循环优化**: 循环展开、循环不变量外提

### 内存管理

```cpp
// 使用智能指针管理 AST 节点
using ASTNodePtr = std::shared_ptr<ASTNode>;

// 使用内存池减少分配开销
class ASTAllocator {
public:
    template<typename T, typename... Args>
    std::shared_ptr<T> allocate(Args&&... args) {
        return std::allocate_shared<T>(allocator_, std::forward<Args>(args)...);
    }
    
private:
    std::allocator<T> allocator_;
};
```

---

## 扩展性设计

### 添加新的 AST 节点

1. 在 `include/ast/ast.h` 中定义新节点类
2. 在 `src/parser/parser.cpp` 中添加解析逻辑
3. 在 `src/semantic/semantic_analyzer.cpp` 中添加语义分析
4. 在 `src/ir/ir_generator.cpp` 中添加 IR 生成
5. 在 `src/codegen/code_generator.cpp` 中添加代码生成

### 添加新的目标平台

1. 创建新的代码生成器类（继承 `CodeGenerator`）
2. 实现类型转换函数
3. 实现指令映射函数
4. 在工厂方法中注册新平台

```cpp
// 示例：添加 ARM64 后端
class ARM64CodeGenerator : public CodeGenerator {
public:
    std::string generate(const IRModulePtr& module) override;
    
private:
    std::string translateType(const Type& type);
    std::string translateOpcode(IROpcode opcode);
};

// 注册到工厂
std::unique_ptr<CodeGenerator> CodeGenerator::create(TargetPlatform platform) {
    switch (platform) {
        case TargetPlatform::ARM64:
            return std::make_unique<ARM64CodeGenerator>();
        // ...
    }
}
```

### 添加新的 IR 优化

```cpp
// 示例：常量折叠优化
class ConstantFoldingPass : public IROptimizationPass {
public:
    void run(IRModulePtr& module) override {
        for (auto& func : module->functions) {
            optimizeFunction(func);
        }
    }
    
private:
    void optimizeFunction(IRFuncPtr& func) {
        for (auto& block : func->basicBlocks) {
            optimizeBlock(block);
        }
    }
    
    void optimizeBlock(IRBasicBlockPtr& block) {
        // 实现常量折叠逻辑
    }
};
```

---

## 测试策略

### 测试层次

```
┌────────────────────────────────────┐
│          端到端测试                 │
│     (完整编译流程测试)              │
├────────────────────────────────────┤
│          集成测试                   │
│     (模块间协作测试)                │
├────────────────────────────────────┤
│          单元测试                   │
│     (单个模块功能测试)              │
└────────────────────────────────────┘
```

### 测试覆盖率目标

| 模块 | 目标覆盖率 | 当前覆盖率 |
|------|----------|-----------|
| 词法分析 | 95%+ | 98% |
| 语法分析 | 90%+ | 92% |
| 语义分析 | 85%+ | 88% |
| IR 生成 | 85%+ | 87% |
| 代码生成 | 80%+ | 85% |

### 测试类型

1. **单元测试**: 每个模块独立测试
2. **集成测试**: 模块间协作测试
3. **回归测试**: 确保修改不破坏现有功能
4. **性能测试**: 编译速度和生成代码性能
5. **模糊测试**: 随机输入测试

### 测试工具

```bash
# 运行所有测试
make test

# 运行特定模块测试
make test-lexer
make test-parser

# 生成覆盖率报告
make coverage

# 运行性能测试
make benchmark
```

---

## 未来规划

### 短期目标（v0.3.0）

- [ ] 改进错误提示信息
- [ ] 添加更多优化
- [ ] 支持更多标准库函数
- [ ] 改进生成的代码质量

### 中期目标（v0.4.0 - v0.5.0）

- [ ] 添加泛型支持
- [ ] 添加模式匹配
- [ ] 添加错误处理机制
- [ ] 添加模块系统

### 长期目标（v1.0.0+）

- [ ] LLVM 后端集成
- [ ] 原生机器码生成
- [ ] 并发原语支持
- [ ] 内存安全特性
- [ ] 宏系统
- [ ] IDE 支持（LSP）

---

## 附录

### 文件结构

```
Nova-Compiler/
├── include/              # 头文件
│   ├── ast/             # AST 定义
│   ├── lexer/           # 词法分析器
│   ├── parser/          # 语法分析器
│   ├── semantic/        # 语义分析器
│   ├── ir/              # IR 定义
│   └── codegen/         # 代码生成器
├── src/                 # 源文件
│   └── main.cpp         # 主程序
├── tests/               # 测试文件
│   ├── test_lexer.cpp
│   ├── test_parser.cpp
│   ├── test_semantic.cpp
│   ├── test_ir.cpp
│   └── test_codegen.cpp
├── examples/            # 示例代码
├── docs/                # 文档
└── Makefile             # 构建脚本
```

### 关键接口

#### Lexer 接口
```cpp
class Lexer {
public:
    Lexer(const std::string& source, const std::string& filename);
    std::vector<Token> tokenize();
    const std::vector<CompilationError>& getErrors() const;
};
```

#### Parser 接口
```cpp
class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    ASTNodePtr parse();
    const std::vector<CompilationError>& getErrors() const;
};
```

#### SemanticAnalyzer 接口
```cpp
class SemanticAnalyzer {
public:
    bool analyze(const ASTNodePtr& ast);
    const std::vector<CompilationError>& getErrors() const;
};
```

#### IRGenerator 接口
```cpp
class IRGenerator {
public:
    IRModulePtr generate(const ASTNodePtr& ast);
};
```

#### CodeGenerator 接口
```cpp
class CodeGenerator {
public:
    virtual std::string generate(const IRModulePtr& module) = 0;
    static std::unique_ptr<CodeGenerator> create(TargetPlatform platform);
};
```

---

**文档维护者**: Nova 编译器团队  
**许可协议**: MIT License
