# Nova Compiler

<div align="center">

![Nova Compiler Logo](docs/images/logo.png)

**A modern compiler frontend framework built from scratch**

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/NotSleeply/Nova-Compiler)
[![Version](https://img.shields.io/badge/version-0.1.0-orange.svg)](https://github.com/NotSleeply/Nova-Compiler/releases)

[Features](#features) • [Quick Start](#quick-start) • [Documentation](#documentation) • [Contributing](#contributing) • [Roadmap](#roadmap)

</div>

---

## Overview

Nova Compiler is a **compiler frontend framework** built entirely from scratch in modern C++. It demonstrates the fundamental concepts of compiler construction, including lexical analysis, parsing, abstract syntax trees, and semantic analysis.

### Why Nova?

- **Educational** - Clean, well-documented code perfect for learning compiler construction
- **Modern** - Uses C++17 features and modern software engineering practices
- **Extensible** - Modular architecture designed for easy extension and modification
- **Practical** - Implements real-world compiler patterns and techniques

---

## Features

### Current Features (v0.1.0)

- ✅ **Lexer** - Full lexical analysis with token generation
  - Integer, float, string, char, and boolean literals
  - Identifiers and keywords
  - Operators and delimiters
  - Comments (single-line and multi-line)
  - Escape sequences in strings
  - Source location tracking

- ✅ **Parser** - Recursive descent parser
  - Expression parsing with proper precedence
  - Statement parsing (if, while, for, return, block)
  - Declaration parsing (functions, variables, structs)
  - Error recovery and synchronization
  - Complete AST generation

- ✅ **AST** - Comprehensive Abstract Syntax Tree
  - Type system (primitives, named types, arrays)
  - Expression nodes (literals, binary, unary, call, etc.)
  - Statement nodes (control flow, blocks, etc.)
  - Declaration nodes (functions, variables, structs)
  - Visitor pattern support (planned)

- 🚧 **Semantic Analysis** - In development
- 🚧 **IR Generation** - Planned
- 🚧 **Code Generation** - Planned

---

## Quick Start

### Prerequisites

- **Compiler**: GCC 9+ or Clang 10+ (C++17 support required)
- **Make**: GNU Make or compatible
- **OS**: Linux, macOS, or Windows (with MinGW/Cygwin)

### Build

```bash
# Clone the repository
git clone https://github.com/NotSleeply/Nova-Compiler.git
cd Nova-Compiler

# Build the compiler
make

# Run with an example
./bin/novac examples/hello.nv
```

### Usage

```bash
# Full compilation
./bin/novac source.nv

# Run lexer only (print tokens)
./bin/novac --lex source.nv

# Run parser only (print AST)
./bin/novac --parse source.nv

# Show help
./bin/novac --help
```

---

## Example

Input (`hello.nv`):
```nova
fn greet(name: string) -> string {
    return "Hello, " + name + "!";
}

fn main() -> void {
    let x: int = 42;
    if (x > 40) {
        println("x is large");
    }
}
```

Lexer Output:
```
KW_FN 'fn' at hello.nv:1:1
IDENTIFIER 'greet' at hello.nv:1:4
LPAREN '(' at hello.nv:1:9
IDENTIFIER 'name' at hello.nv:1:10
COLON ':' at hello.nv:1:14
KW_STRING 'string' at hello.nv:1:16
...
```

Parser Output:
```
Program:
  FunctionDecl: greet
    Parameters:
      ParamDecl: name : NamedType: string
    Return Type: NamedType: string
    Body:
      BlockStmt:
        ReturnStmt:
          BinaryExpr: ...
```

---

## Architecture

```
Nova Compiler
├── Lexer (Lexical Analysis)
│   ├── Character stream → Token stream
│   ├── Keyword recognition
│   └── Error reporting
│
├── Parser (Syntax Analysis)
│   ├── Token stream → AST
│   ├── Recursive descent
│   └── Error recovery
│
├── AST (Abstract Syntax Tree)
│   ├── Expression nodes
│   ├── Statement nodes
│   └── Declaration nodes
│
├── Semantic Analysis (planned)
│   ├── Type checking
│   ├── Symbol table
│   └── Scope management
│
└── Code Generation (planned)
    ├── IR generation
    └── Target code
```

---

## Project Structure

```
Nova-Compiler/
├── include/           # Header files
│   ├── lexer/        # Lexer headers
│   ├── parser/       # Parser headers
│   ├── ast/          # AST definitions
│   └── semantic/     # Semantic analysis (planned)
│
├── src/              # Source files
│   ├── main.cpp      # Entry point
│   └── ...
│
├── tests/            # Unit tests
├── examples/         # Example Nova programs
├── docs/             # Documentation
├── Makefile          # Build configuration
└── README.md         # This file
```

---

## Language Specification

### Types

| Type | Description | Example |
|------|-------------|---------|
| `int` | 64-bit integer | `42`, `-17`, `0xFF` |
| `float` | Double-precision float | `3.14`, `-0.5`, `1e10` |
| `bool` | Boolean | `true`, `false` |
| `string` | String | `"hello"`, `'world'` |
| `char` | Character | `'a'`, `'\n'` |
| `void` | No type | - |

### Operators

| Category | Operators |
|----------|-----------|
| Arithmetic | `+`, `-`, `*`, `/`, `%`, `**` |
| Comparison | `==`, `!=`, `<`, `<=`, `>`, `>=` |
| Logical | `&&`, `\|\|`, `!` |
| Bitwise | `&`, `\|`, `^`, `~`, `<<`, `>>` |
| Assignment | `=`, `+=`, `-=`, `*=`, `/=`, `%=` |
| Increment | `++`, `--` |

### Control Flow

```nova
// If-else
if (condition) {
    // ...
} else {
    // ...
}

// While loop
while (condition) {
    // ...
}

// For loop
for (let i: int = 0; i < 10; i = i + 1) {
    // ...
}

// Return
return value;
```

### Functions

```nova
fn functionName(param: type) -> returnType {
    // body
}
```

---

## Roadmap

### v0.1.0 (Current) - Foundation
- [x] Lexer implementation
- [x] Parser implementation
- [x] AST design
- [x] Basic examples

### v0.2.0 - Semantic Analysis
- [ ] Type checking
- [ ] Symbol table
- [ ] Scope management
- [ ] Error detection

### v0.3.0 - IR Generation
- [ ] Intermediate representation
- [ ] SSA form
- [ ] Basic optimizations

### v0.4.0 - Code Generation
- [ ] LLVM IR backend
- [ ] C code generation
- [ ] Bytecode VM

### v1.0.0 - Complete Compiler
- [ ] Full language support
- [ ] Standard library
- [ ] Package manager
- [ ] IDE integration

---

## Contributing

We welcome contributions! Here's how you can help:

### Ways to Contribute

- 🐛 **Report bugs** - Use [GitHub Issues](https://github.com/NotSleeply/Nova-Compiler/issues)
- 💡 **Suggest features** - Share your ideas in issues
- 📝 **Improve documentation** - Fix typos, add examples
- 💻 **Write code** - Pick an issue and submit a PR
- 🧪 **Add tests** - Improve test coverage

### Development Setup

```bash
# Fork and clone
git clone https://github.com/YOUR_USERNAME/Nova-Compiler.git
cd Nova-Compiler

# Create feature branch
git checkout -b feature/amazing-feature

# Make changes and test
make clean && make
./bin/novac --test

# Submit pull request
git push origin feature/amazing-feature
```

### Code Style

- Follow modern C++ best practices
- Use meaningful variable names
- Add comments for complex logic
- Write tests for new features

---

## Documentation

- **[Language Specification](docs/LANGUAGE.md)** - Nova language syntax and semantics
- **[Architecture Guide](docs/ARCHITECTURE.md)** - Internal design and implementation
- **[Contributing Guide](docs/CONTRIBUTING.md)** - How to contribute
- **[API Reference](docs/API.md)** - Public API documentation

---

## License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

---

## Acknowledgments

Nova Compiler is inspired by and built upon knowledge from:

- **Crafting Interpreters** by Robert Nystrom
- **Engineering a Compiler** by Cooper & Torczon
- **Modern Compiler Implementation** by Andrew Appel
- **LLVM Project** - For inspiration on IR design

---

## Community

- **GitHub**: [NotSleeply/Nova-Compiler](https://github.com/NotSleeply/Nova-Compiler)
- **Issues**: [Bug reports and feature requests](https://github.com/NotSleeply/Nova-Compiler/issues)
- **Discussions**: [GitHub Discussions](https://github.com/NotSleeply/Nova-Compiler/discussions)

---

<div align="center">

**Built with ❤️ by the open-source community**

[⬆ Back to Top](#nova-compiler)

</div>
