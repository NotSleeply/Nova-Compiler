# Changelog

All notable changes to Nova Compiler will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Semantic analysis module (in progress)
- Type checking infrastructure
- Symbol table implementation

## [0.1.0] - 2026-04-02

### Added
- **Lexer (Lexical Analysis)**
  - Complete tokenization of source code
  - Support for all literal types (int, float, string, char, bool)
  - Keyword and identifier recognition
  - Operator and delimiter handling
  - Single-line (`//`) and multi-line (`/* */`) comment support
  - Escape sequences in strings
  - Source location tracking (file, line, column, offset)
  - Comprehensive error reporting

- **Parser (Syntax Analysis)**
  - Recursive descent parser implementation
  - Expression parsing with proper operator precedence
  - Statement parsing (if, while, for, return, block)
  - Declaration parsing (functions, variables, structs)
  - Type annotation parsing
  - Error recovery and synchronization
  - Complete AST generation

- **AST (Abstract Syntax Tree)**
  - Type system (primitives, named types, arrays)
  - Expression nodes (literals, binary, unary, call, member access)
  - Statement nodes (control flow, blocks, return)
  - Declaration nodes (functions, variables, structs, fields)
  - Pretty printing for debugging

- **Build System**
  - Makefile with multiple build targets
  - Debug and release configurations
  - Test target
  - Installation targets

- **Examples**
  - Hello World example (`hello.nv`)
  - Demonstration of language features

- **Documentation**
  - Comprehensive README
  - Inline code documentation
  - Architecture overview

### Project Infrastructure
- MIT License
- .gitignore for build artifacts
- Changelog for version tracking

## [0.0.1] - 2026-04-02

### Added
- Initial project structure
- GitHub repository creation
- Basic directory layout

---

## Version History

| Version | Date | Description |
|---------|------|-------------|
| 0.1.0 | 2026-04-02 | Initial release with lexer and parser |
| 0.0.1 | 2026-04-02 | Project initialization |

---

[Unreleased]: https://github.com/NotSleeply/Nova-Compiler/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/NotSleeply/Nova-Compiler/releases/tag/v0.1.0
[0.0.1]: https://github.com/NotSleeply/Nova-Compiler/releases/tag/v0.0.1
