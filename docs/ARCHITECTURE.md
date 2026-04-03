# Nova Compiler Architecture

**Version**: 0.2.0  
**Last Updated**: 2026-04-03

## Table of Contents

1. [Overview](#overview)
2. [Design Philosophy](#design-philosophy)
3. [Architecture Layers](#architecture-layers)
4. [Core Components](#core-components)
5. [Data Flow](#data-flow)
6. [Compiler Pipeline](#compiler-pipeline)
7. [Intermediate Representations](#intermediate-representations)
8. [Code Generation](#code-generation)
9. [Error Handling](#error-handling)
10. [Testing Strategy](#testing-strategy)
11. [Future Extensions](#future-extensions)

---

## Overview

Nova is a modern, multi-stage compiler designed for clarity, modularity, and extensibility. It follows traditional compiler architecture principles while incorporating modern C++ practices.

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      Source Code (.nv)                       │
└───────────────────────────┬─────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                      Lexer (Scanner)                         │
│  - Tokenization                                              │
│  - Lexical Analysis                                          │
│  - Source Position Tracking                                  │
└───────────────────────────┬─────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                      Parser                                  │
│  - Recursive Descent Parsing                                 │
│  - AST Construction                                          │
│  - Syntax Error Recovery                                     │
└───────────────────────────┬─────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    Semantic Analyzer                         │
│  - Type Checking                                             │
│  - Symbol Resolution                                         │
│  - Scope Management                                          │
│  - Semantic Validation                                       │
└───────────────────────────┬─────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    IR Generator                              │
│  - Three-Address Code Generation                             │
│  - Control Flow Graph Construction                           │
│  - Basic Block Formation                                     │
└───────────────────────────┬─────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    Code Generator                            │
│  - Target Selection (C/LLVM/x86-64/WASM)                     │
│  - Instruction Mapping                                       │
│  - Code Emission                                             │
└───────────────────────────┬─────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    Target Code                               │
│  - C Source (.c)                                             │
│  - LLVM IR (.ll)                                             │
│  - Assembly (.s)                                             │
│  - WebAssembly (.wasm)                                       │
└─────────────────────────────────────────────────────────────┘
```

---

## Design Philosophy

### 1. **Modularity**
Each compiler phase is a separate module with well-defined interfaces. This allows:
- Independent testing and development
- Easy replacement or extension of components
- Clear separation of concerns

### 2. **Clarity Over Cleverness**
Code is written to be readable and maintainable:
- Self-documenting code with clear naming
- Comprehensive comments and documentation
- Simple, straightforward algorithms

### 3. **Modern C++ Practices**
- Smart pointers for memory management
- RAII for resource handling
- Standard library containers and algorithms
- Template metaprogramming where appropriate

### 4. **Error Resilience**
The compiler continues processing even after errors:
- Graceful error recovery
- Multiple errors reported per compilation
- Clear, actionable error messages

### 5. **Extensibility**
Design allows for future enhancements:
- Plugin architecture for analysis passes
- Support for multiple backends
- Generic IR for optimization passes

---

## Architecture Layers

### Layer 1: Frontend (Analysis)

**Responsibility**: Transform source code into structured intermediate representation

**Components**:
- Lexer
- Parser
- Semantic Analyzer
- IR Generator

**Output**: Type-checked AST → Three-address IR

### Layer 2: Middle-end (Optimization)

**Responsibility**: Optimize IR without changing semantics

**Components** (Planned):
- Constant Folding
- Dead Code Elimination
- Common Subexpression Elimination
- Loop Optimizations
- Inlining

**Output**: Optimized IR

### Layer 3: Backend (Synthesis)

**Responsibility**: Generate target-specific code

**Components**:
- Code Generator
- Register Allocator (Future)
- Instruction Scheduler (Future)

**Output**: Target code (C, LLVM IR, Assembly)

---

## Core Components

### 1. Lexer (`include/lexer/`)

**Files**:
- `lexer.h` - Lexer class definition
- `token.h` - Token types and structures

**Responsibilities**:
- Convert character stream into tokens
- Handle lexical errors (invalid characters, malformed literals)
- Track source positions for error reporting
- Filter comments and whitespace

**Key Data Structures**:

```cpp
struct Token {
    TokenType type;        // Token category
    std::string value;     // Lexeme
    SourcePosition pos;    // Location in source
};

struct SourcePosition {
    std::string filename;
    size_t line;
    size_t column;
};
```

**Token Types**:
- Keywords: `fn`, `let`, `if`, `while`, etc.
- Literals: Integer, Float, String, Char, Bool
- Operators: `+`, `-`, `*`, `/`, `==`, etc.
- Delimiters: `(`, `)`, `{`, `}`, `[`, `]`
- Identifiers: User-defined names
- Special: EOF, ERROR

**Design Patterns**:
- **Iterator Pattern**: Lexer provides token-by-token iteration
- **State Machine**: Handles multi-character operators and strings

---

### 2. Parser (`include/parser/`)

**Files**:
- `parser.h` - Parser class definition
- `ast.h` - AST node definitions
- `ast_visitor.h` - Visitor pattern for AST traversal

**Responsibilities**:
- Build Abstract Syntax Tree from token stream
- Enforce grammar rules and operator precedence
- Perform syntax error recovery
- Construct semantic-rich AST nodes

**Key Data Structures**:

```cpp
struct ASTNode {
    NodeType type;
    SourcePosition pos;
    std::vector<ASTNodePtr> children;
};

struct FunctionDecl : ASTNode {
    std::string name;
    std::vector<Parameter> params;
    TypePtr returnType;
    BlockStmtPtr body;
};

struct BinaryExpr : ASTNode {
    ASTNodePtr left;
    ASTNodePtr right;
    TokenType op;
};
```

**AST Node Hierarchy**:

```
ASTNode (Base)
├── Declaration
│   ├── FunctionDecl
│   ├── VarDecl
│   ├── StructDecl
│   └── EnumDecl
├── Statement
│   ├── BlockStmt
│   ├── IfStmt
│   ├── WhileStmt
│   ├── ForStmt
│   ├── ReturnStmt
│   ├── BreakStmt
│   └── ContinueStmt
└── Expression
    ├── BinaryExpr
    ├── UnaryExpr
    ├── LiteralExpr
    ├── IdentifierExpr
    ├── CallExpr
    ├── MemberExpr
    ├── IndexExpr
    └── CastExpr
```

**Parsing Strategy**:
- **Recursive Descent**: Hand-written parser, one function per grammar rule
- **Pratt Parsing**: Expression parsing with operator precedence
- **Error Recovery**: Synchronization on statement boundaries

**Operator Precedence** (lowest to highest):

| Level | Operators | Associativity |
|-------|-----------|---------------|
| 1     | `=` | Right |
| 2     | `+=`, `-=`, `*=`, `/=` | Right |
| 3     | `\|\|` | Left |
| 4     | `&&` | Left |
| 5     | `\|` | Left |
| 6     | `^` | Left |
| 7     | `&` | Left |
| 8     | `==`, `!=` | Left |
| 9     | `<`, `<=`, `>`, `>=` | Left |
| 10    | `<<`, `>>` | Left |
| 11    | `+`, `-` | Left |
| 12    | `*`, `/`, `%` | Left |
| 13    | Unary: `!`, `-`, `~` | Right |
| 14    | `**` | Right |
| 15    | `(`, `[`, `.`, `->` | Left |

---

### 3. Semantic Analyzer (`include/semantic/`)

**Files**:
- `semantic.h` - Semantic analyzer class
- `symbol_table.h` - Symbol table implementation
- `type_checker.h` - Type checking logic

**Responsibilities**:
- Type checking and type inference
- Symbol resolution and scope management
- Detect semantic errors (undeclared variables, type mismatches)
- Build symbol table

**Key Data Structures**:

```cpp
struct Symbol {
    std::string name;
    SymbolKind kind;        // Variable, Function, Type, etc.
    TypePtr type;
    SourcePosition declared;
    bool is_mutable;
    bool is_used;
};

struct Scope {
    std::map<std::string, SymbolPtr> symbols;
    ScopePtr parent;
    ScopeKind kind;         // Global, Function, Block, etc.
};

struct Type {
    TypeKind kind;          // Int, Float, String, Struct, etc.
    std::string name;
    std::vector<TypePtr> type_params;  // For generics
};
```

**Semantic Checks**:
1. **Variable Usage**: All variables declared before use
2. **Type Compatibility**: Assignment and operation types match
3. **Function Calls**: Correct number and types of arguments
4. **Control Flow**: Functions return values, break/continue in loops
5. **Mutability**: Cannot assign to immutable variables
6. **Scope Rules**: Proper variable shadowing and resolution

**Type System**:

```cpp
enum class TypeKind {
    Void,
    Int,
    Float,
    Bool,
    Char,
    String,
    Array,
    Pointer,
    Reference,
    Function,
    Struct,
    Enum,
    Generic,
};
```

---

### 4. IR Generator (`include/ir/`)

**Files**:
- `ir.h` - IR structures and generator
- `ir_printer.h` - IR visualization

**Responsibilities**:
- Generate three-address code from AST
- Build control flow graph
- Create basic blocks
- Allocate temporaries

**Key Data Structures**:

```cpp
struct IRModule {
    std::vector<IRFuncPtr> functions;
    std::vector<IRGlobalPtr> globals;
};

struct IRFunc {
    std::string name;
    TypePtr returnType;
    std::vector<Parameter> parameters;
    std::vector<BasicBlockPtr> basicBlocks;
};

struct BasicBlock {
    std::string label;
    std::vector<IRInstPtr> instructions;
    std::vector<BasicBlockPtr> predecessors;
    std::vector<BasicBlockPtr> successors;
};

struct IRInstruction {
    Opcode op;
    Operand dest;
    Operand src1;
    Operand src2;
};

struct Operand {
    OperandKind kind;       // Variable, Temporary, Constant, Label
    std::string value;
    std::string dataType;
};
```

**Instruction Set**:

| Category | Opcodes |
|----------|---------|
| Arithmetic | `ADD`, `SUB`, `MUL`, `DIV`, `MOD`, `NEG` |
| Comparison | `EQ`, `NE`, `LT`, `LE`, `GT`, `GE` |
| Logical | `AND`, `OR`, `NOT` |
| Memory | `LOAD`, `STORE`, `ALLOCA`, `GEP` |
| Control | `JMP`, `JZ`, `JNZ`, `RET`, `CALL`, `LABEL` |
| Data | `MOVE`, `COPY`, `CAST`, `PARAM` |

**IR Example**:

```
function: add
params: a(int), b(int)
return: int

entry:
    %t0 = ADD a, b
    RET %t0
```

---

### 5. Code Generator (`include/codegen/`)

**Files**:
- `code_generator.h` - Code generator interface
- `c_codegen.h` - C code generator

**Responsibilities**:
- Translate IR to target code
- Map types and operations to target constructs
- Generate runtime support code
- Handle platform-specific details

**Key Components**:

```cpp
class CodeGenerator {
public:
    virtual ~CodeGenerator() = default;
    virtual std::string generate(IRModulePtr module) = 0;
    virtual std::string getTargetName() const = 0;
};

class CCodeGenerator : public CodeGenerator {
public:
    std::string generate(IRModulePtr module) override;
    
private:
    void emitFunction(const IRFuncPtr& func);
    void emitBasicBlock(const BasicBlockPtr& block, const std::string& funcName);
    void emitInstruction(const IRInstPtr& inst);
    std::string translateType(const std::string& novaType);
    std::string translateOpcode(Opcode op);
};
```

**Type Mapping** (Nova → C):

| Nova Type | C Type |
|-----------|--------|
| `int` | `long long` |
| `float` | `double` |
| `bool` | `int` |
| `char` | `int` |
| `string` | `const char*` |
| `void` | `void` |

**Operation Mapping** (IR → C):

| IR Opcode | C Operator |
|-----------|------------|
| `ADD` | `+` |
| `SUB` | `-` |
| `MUL` | `*` |
| `DIV` | `/` |
| `MOD` | `%` |
| `EQ` | `==` |
| `NE` | `!=` |
| `LT` | `<` |
| `LE` | `<=` |
| `GT` | `>` |
| `GE` | `>=` |
| `AND` | `&&` |
| `OR` | `\|\|` |
| `NOT` | `!` |

**Code Generation Process**:

1. **Header Generation**: Include standard headers
2. **Global Variables**: Emit global variable declarations
3. **Function Generation**:
   - Function signature
   - Temporary variable declarations
   - Basic blocks with labels
   - Instruction translation
4. **Footer**: Any finalization code

---

## Data Flow

```
Source Code (string)
     │
     ▼
┌─────────┐
│  Lexer  │ Tokenize
└────┬────┘
     │
     ▼
Token Stream (vector<Token>)
     │
     ▼
┌─────────┐
│ Parser  │ Parse
└────┬────┘
     │
     ▼
AST (ASTNodePtr)
     │
     ▼
┌─────────────┐
│ Semantic    │ Analyze
│ Analyzer    │
└──────┬──────┘
       │
       ▼
Typed AST + Symbol Table
       │
       ▼
┌─────────────┐
│ IR          │ Generate
│ Generator   │
└──────┬──────┘
       │
       ▼
IR Module (IRModulePtr)
       │
       ▼
┌─────────────┐
│ Code        │ Emit
│ Generator   │
└──────┬──────┘
       │
       ▼
Target Code (string)
```

---

## Compiler Pipeline

### Main Pipeline (`src/main.cpp`)

```cpp
int main(int argc, char* argv[]) {
    // 1. Parse command-line arguments
    Config config = parseArgs(argc, argv);
    
    // 2. Read source file
    std::string source = readFile(config.inputFile);
    
    // 3. Lexical analysis
    Lexer lexer(source, config.inputFile);
    std::vector<Token> tokens = lexer.tokenize();
    if (config.lexOnly) {
        printTokens(tokens);
        return 0;
    }
    
    // 4. Syntax analysis
    Parser parser(tokens, config.inputFile);
    ASTNodePtr ast = parser.parse();
    if (config.parseOnly) {
        printAST(ast);
        return 0;
    }
    
    // 5. Semantic analysis
    SemanticAnalyzer analyzer;
    bool success = analyzer.analyze(ast);
    if (!success || config.semanticOnly) {
        printErrors(analyzer.getErrors());
        return success ? 0 : 1;
    }
    
    // 6. IR generation
    IRGenerator irGen;
    IRModulePtr ir = irGen.generate(ast);
    if (config.irOnly) {
        printIR(ir);
        return 0;
    }
    
    // 7. Code generation
    CodeGenerator* codegen = createCodeGenerator(config.target);
    std::string code = codegen->generate(ir);
    
    // 8. Write output
    writeFile(config.outputFile, code);
    
    return 0;
}
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `--lex` | Run lexer only, print tokens |
| `--parse` | Run parser only, print AST |
| `--semantic` | Run semantic analysis only |
| `--ir` | Generate and print IR |
| `--codegen` | Generate target code |
| `-o <file>` | Specify output file |
| `--target <lang>` | Target language (c, llvm, x86) |
| `--help` | Show help message |

---

## Intermediate Representations

### Why Three-Address Code?

**Advantages**:
1. **Simplicity**: Easy to generate and transform
2. **Machine-Independent**: Abstracts target details
3. **Optimization-Friendly**: Clear data flow
4. **Readability**: Human-readable format

**Example Transformation**:

**Source**:
```nova
let result = (a + b) * (c - d);
```

**AST**:
```
BinaryExpr(*)
├── BinaryExpr(+)
│   ├── Identifier(a)
│   └── Identifier(b)
└── BinaryExpr(-)
    ├── Identifier(c)
    └── Identifier(d)
```

**IR**:
```
%t0 = ADD a, b
%t1 = SUB c, d
%t2 = MUL %t0, %t1
result = MOVE %t2
```

### Control Flow Graph

**Structure**:
- Nodes: Basic blocks
- Edges: Control flow transfers

**Example**:

```
[entry] → [if_cond] → [if_true] → [merge]
                ↓
           [if_false] → [merge]
```

**IR Representation**:

```
entry:
    // ... setup code
    JMP if_cond

if_cond:
    %cond = GT x, 0
    JZ %cond, if_false

if_true:
    // ... true branch
    JMP merge

if_false:
    // ... false branch
    JMP merge

merge:
    // ... continue
```

---

## Code Generation

### Target Platforms

#### 1. C Transpilation (Current)

**Advantages**:
- Portability: C compilers available everywhere
- Debugging: Easy to read generated code
- Compatibility: Seamless C interop
- Bootstrap: No need for runtime library

**Process**:
1. Map Nova types to C types
2. Convert IR instructions to C statements
3. Generate function signatures
4. Add runtime support (standard headers)

**Generated Code Example**:

```c
/* Generated by Nova Compiler */
#include <stdio.h>
#include <stdlib.h>

long long add(long long a, long long b) {
    long long _t_t0;
    long long _t_t1;
    
    _t_t0 = a + b;
    _t_t1 = _t_t0;
    return _t_t1;
}
```

#### 2. LLVM IR (Planned)

**Advantages**:
- Optimization: Leverage LLVM optimization passes
- Native Code: Direct compilation to machine code
- Tooling: Rich ecosystem (debuggers, profilers)

**Example**:

```llvm
define i64 @add(i64 %a, i64 %b) {
entry:
    %t0 = add i64 %a, %b
    ret i64 %t0
}
```

#### 3. x86-64 Assembly (Planned)

**Advantages**:
- Performance: Direct control over generated code
- Learning: Educational for understanding assembly

**Example**:

```asm
add:
    mov rax, rdi    ; a
    add rax, rsi    ; + b
    ret
```

#### 4. WebAssembly (Planned)

**Advantages**:
- Portability: Run in any browser
- Safety: Sandboxed execution
- Size: Compact binary format

**Example**:

```wat
(func $add (param $a i64) (param $b i64) (result i64)
    local.get $a
    local.get $b
    i64.add)
```

---

## Error Handling

### Error Categories

#### 1. Lexical Errors

**Examples**:
- Invalid characters: `@`, `#`, `$`
- Malformed literals: `3.14.15`, `"unclosed string`
- Invalid escape sequences: `\z`

**Recovery**: Skip invalid character, continue tokenizing

#### 2. Syntax Errors

**Examples**:
- Missing semicolons
- Unmatched parentheses
- Invalid statement structure

**Recovery**: Synchronize to statement boundary (`;`, `}`)

#### 3. Semantic Errors

**Examples**:
- Undeclared variables
- Type mismatches
- Wrong number of arguments

**Recovery**: Continue analysis, collect multiple errors

### Error Reporting

**Structure**:

```cpp
struct CompilationError {
    ErrorSeverity severity;   // Error, Warning, Note
    ErrorCode code;
    std::string message;
    SourcePosition location;
    std::string sourceLine;
    std::string hint;
};
```

**Example Output**:

```
error[E0001]: undeclared variable
  --> examples/test.nv:5:8
   |
5  |     let y = x + 1;
   |            ^ undeclared here
   |
   = note: `x` has not been declared in this scope
   = help: did you mean to declare `let x`?
```

### Error Recovery Strategy

1. **Lexer**: Skip to next valid token
2. **Parser**: Synchronize at statement/declaration boundaries
3. **Semantic**: Continue analysis with error nodes
4. **IR**: Skip invalid constructs
5. **Codegen**: Report error, don't generate code

---

## Testing Strategy

### Test Pyramid

```
        /\
       /  \  End-to-End Tests
      /----\ (Examples)
     /      \
    /--------\ Integration Tests
   /          \ (Multi-module)
  /------------\
 /              \ Unit Tests
/________________\ (Per-function)
```

### Test Coverage

| Component | Unit Tests | Integration Tests | E2E Tests |
|-----------|------------|-------------------|-----------|
| Lexer | 100+ | 20+ | 10+ |
| Parser | 80+ | 15+ | 10+ |
| Semantic | 70+ | 10+ | 5+ |
| IR Gen | 60+ | 10+ | 5+ |
| Code Gen | 50+ | 10+ | 10+ |
| **Total** | **360+** | **65+** | **40+** |

### Test Categories

#### 1. Unit Tests

**Lexer Tests**:
- Keyword recognition
- Identifier parsing
- Literal parsing (int, float, string, char, bool)
- Operator and delimiter recognition
- Comment handling
- Error cases

**Parser Tests**:
- Expression parsing (precedence, associativity)
- Statement parsing
- Declaration parsing
- Type parsing
- Error recovery

**Semantic Tests**:
- Type checking
- Scope resolution
- Function call validation
- Error detection

#### 2. Integration Tests

- Full compilation pipeline
- Multiple files
- Complex expressions
- Real-world programs

#### 3. End-to-End Tests

- Example programs
- Standard library usage
- Generated code execution

### Test Framework

**Custom Catch2-style Framework**:

```cpp
TEST_CASE("Lexer recognizes all keywords", "[lexer]") {
    Lexer lexer("fn let const if else", "test.nv");
    auto tokens = lexer.tokenize();
    
    REQUIRE(tokens.size() == 6);
    CHECK(tokens[0].type == TokenType::FN);
    CHECK(tokens[1].type == TokenType::LET);
}

TEST_CASE("Parser handles operator precedence", "[parser]") {
    auto ast = parse("1 + 2 * 3");
    REQUIRE(ast != nullptr);
    
    // Check that multiplication binds tighter
    CHECK(ast->isBinaryExpr());
    CHECK(ast->op == TokenType::PLUS);
}
```

---

## Future Extensions

### 1. Optimizations

**Planned Passes**:
- Constant Folding
- Dead Code Elimination
- Common Subexpression Elimination
- Loop Invariant Code Motion
- Function Inlining
- Tail Call Optimization

**Implementation**:

```cpp
class OptimizationPass {
public:
    virtual ~OptimizationPass() = default;
    virtual bool run(IRModulePtr module) = 0;
    virtual std::string getName() const = 0;
};

class ConstantFoldingPass : public OptimizationPass {
    bool run(IRModulePtr module) override;
    std::string getName() const override { return "ConstantFolding"; }
};
```

### 2. Generic Types

**Syntax**:

```nova
fn max<T>(a: T, b: T) -> T where T: Comparable {
    if a > b { return a; }
    return b;
}
```

**Implementation**:
- Monomorphization: Generate specialized versions
- Type erasure: Use runtime type information

### 3. Traits

**Syntax**:

```nova
trait Drawable {
    fn draw(&self) -> void;
}

impl Drawable for Circle {
    fn draw(&self) -> void {
        // ...
    }
}
```

### 4. Modules

**Syntax**:

```nova
// math.nova
pub module math;

pub fn sqrt(x: float) -> float {
    // ...
}

// main.nova
use math::*;

fn main() {
    let result = sqrt(16.0);
}
```

### 5. Concurrency

**Syntax**:

```nova
async fn fetch_data(url: string) -> Result<string, Error> {
    // ...
}

fn main() {
    let result = await fetch_data("https://example.com");
}
```

### 6. Metaprogramming

**Syntax**:

```nova
macro generate_getters($struct_name:ident, $($field:ident),*) {
    // Generate getter methods
}

struct Person {
    name: string,
    age: int,
}

generate_getters!(Person, name, age);
```

---

## Build System

### Makefile Targets

```makefile
# Build compiler
make

# Run tests
make test

# Clean build artifacts
make clean

# Generate documentation
make docs

# Install to /usr/local
make install

# Create distribution package
make dist
```

### Dependencies

- **C++17**: Modern C++ features
- **GCC/Clang**: C++ compiler
- **Make**: Build system

---

## Project Statistics

| Metric | Value |
|--------|-------|
| Source Files | 20+ |
| Lines of Code | 5000+ |
| Header Files | 15+ |
| Test Files | 5+ |
| Test Cases | 360+ |
| Supported Types | 10+ |
| IR Instructions | 25+ |
| Target Platforms | 4 (1 implemented) |

---

## References

### Compiler Design Books
- *Compilers: Principles, Techniques, and Tools* (Dragon Book)
- *Engineering a Compiler* by Cooper and Torczon
- *Modern Compiler Implementation in C++* by Appel

### Language References
- [Rust Compiler](https://github.com/rust-lang/rust)
- [Clang](https://clang.llvm.org/)
- [Swift Compiler](https://github.com/apple/swift)

### Tools
- [LLVM Infrastructure](https://llvm.org/)
- [ANTLR Parser Generator](https://www.antlr.org/)

---

**Note**: This architecture document reflects the current state of the Nova compiler (v0.2.0). Future features and optimizations are planned but not yet implemented. The architecture may evolve based on implementation experience and performance requirements.
