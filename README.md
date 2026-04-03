# Nova 编译器

<div align="center">

![Nova Compiler Logo](docs/images/logo.png)

**一个从零构建的现代编译器前端框架**

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/NotSleeply/Nova-Compiler)
[![Version](https://img.shields.io/badge/version-0.2.0-orange.svg)](https://github.com/NotSleeply/Nova-Compiler/releases)

[特性](#特性) • [快速开始](#快速开始) • [文档](#文档) • [贡献指南](#贡献指南) • [路线图](#路线图)

</div>

---

## 概述

Nova 编译器是一个使用现代 C++ 从零构建的**编译器前端框架**。它演示了编译器构造的基本概念，包括词法分析、语法分析、抽象语法树和语义分析。

### 为什么选择 Nova？

- **教育性** - 清晰、文档完善的代码，非常适合学习编译器构造
- **现代化** - 使用 C++17 特性和现代软件工程实践
- **可扩展** - 模块化架构设计，便于扩展和修改
- **实用性** - 实现了真实世界的编译器模式和技术

---

## 特性

### 当前特性 (v0.1.0)

- ✅ **词法分析器** - 完整的词法分析与词元生成
  - 整型、浮点、字符串、字符和布尔字面量
  - 标识符和关键字
  - 运算符和分隔符
  - 注释（单行和多行）
  - 字符串中的转义序列
  - 源码位置跟踪

- ✅ **语法分析器** - 递归下降解析器
  - 正确优先级的表达式解析
  - 语句解析（if、while、for、return、代码块）
  - 声明解析（函数、变量、结构体）
  - 错误恢复和同步
  - 完整的 AST 生成

- ✅ **AST** - 全面的抽象语法树
  - 类型系统（基本类型、命名类型、数组）
  - 表达式节点（字面量、二元、一元、调用等）
  - 语句节点（控制流、代码块等）
  - 声明节点（函数、变量、结构体）
  - 访问者模式支持（计划中）

- ✅ **语义分析** - 完整的语义检查
  - 类型检查和类型推导
  - 作用域管理
  - 符号表构建
  - 错误检测和报告

- ✅ **IR 生成** - 中间表示生成
  - 三地址码表示
  - 基本块划分
  - 控制流图构建
  - 函数和全局变量处理

- ✅ **代码生成** - 目标代码生成
  - C代码生成（transpilation）
  - 类型转换（Nova类型 → C类型）
  - 操作码映射（IR操作码 → C操作符）
  - 支持算术、比较、逻辑、内存和控制流操作
  - 可扩展的多平台支持（LLVM IR、x86-64、WASM）

---

## 快速开始

### 前置要求

- **编译器**: GCC 9+ 或 Clang 10+（需要 C++17 支持）
- **Make**: GNU Make 或兼容工具
- **操作系统**: Linux、macOS 或 Windows（使用 MinGW/Cygwin）

### 构建

```bash
# 克隆仓库
git clone https://github.com/NotSleeply/Nova-Compiler.git
cd Nova-Compiler

# 构建编译器
make

# 运行示例
./bin/novac examples/hello.nv
```

### 使用方法

```bash
# 完整编译（词法→语法→语义→IR→代码生成）
./bin/novac source.nv

# 仅运行词法分析器（打印词元）
./bin/novac --lex source.nv

# 仅运行语法分析器（打印 AST）
./bin/novac --parse source.nv

# 运行语义分析
./bin/novac --semantic source.nv

# 生成并打印 IR
./bin/novac --ir source.nv

# 生成目标代码（C代码）
./bin/novac --codegen source.nv

# 生成目标代码到指定文件
./bin/novac --codegen -o output.c source.nv

# 编译并执行生成的C代码
./bin/novac source.nv
gcc -o program source.nv.c
./program

# 显示帮助
./bin/novac --help
```

---

## 示例

输入 (`hello.nv`):
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

词法分析器输出:
```
KW_FN 'fn' at hello.nv:1:1
IDENTIFIER 'greet' at hello.nv:1:4
LPAREN '(' at hello.nv:1:9
IDENTIFIER 'name' at hello.nv:1:10
COLON ':' at hello.nv:1:14
KW_STRING 'string' at hello.nv:1:16
...
```

语法分析器输出:
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

## 架构

```
Nova 编译器
├── Lexer (词法分析)
│   ├── 字符流 → 词元流
│   ├── 关键字识别
│   └── 错误报告
│
├── Parser (语法分析)
│   ├── 词元流 → AST
│   ├── 递归下降
│   └── 错误恢复
│
├── AST (抽象语法树)
│   ├── 表达式节点
│   ├── 语句节点
│   └── 声明节点
│
├── Semantic Analysis (语义分析 - 计划中)
│   ├── 类型检查
│   ├── 符号表
│   └── 作用域管理
│
└── Code Generation (代码生成 - 计划中)
    ├── IR 生成
    └── 目标代码
```

---

## 项目结构

```
Nova-Compiler/
├── include/           # 头文件
│   ├── lexer/        # 词法分析器头文件
│   ├── parser/       # 语法分析器头文件
│   ├── ast/          # AST 定义
│   └── semantic/     # 语义分析（计划中）
│
├── src/              # 源文件
│   ├── main.cpp      # 入口点
│   └── ...
│
├── tests/            # 单元测试
├── examples/         # Nova 程序示例
├── docs/             # 文档
├── Makefile          # 构建配置
└── README.md         # 本文件
```

---

## 语言规范

### 类型

| 类型 | 描述 | 示例 |
|------|------|------|
| `int` | 64 位整数 | `42`, `-17`, `0xFF` |
| `float` | 双精度浮点数 | `3.14`, `-0.5`, `1e10` |
| `bool` | 布尔值 | `true`, `false` |
| `string` | 字符串 | `"hello"`, `'world'` |
| `char` | 字符 | `'a'`, `'\n'` |
| `void` | 无类型 | - |

### 运算符

| 类别 | 运算符 |
|------|--------|
| 算术 | `+`, `-`, `*`, `/`, `%`, `**` |
| 比较 | `==`, `!=`, `<`, `<=`, `>`, `>=` |
| 逻辑 | `&&`, `\|\|`, `!` |
| 位运算 | `&`, `\|`, `^`, `~`, `<<`, `>>` |
| 赋值 | `=`, `+=`, `-=`, `*=`, `/=`, `%=` |
| 自增/自减 | `++`, `--` |

### 控制流

```nova
// If-else
if (condition) {
    // ...
} else {
    // ...
}

// While 循环
while (condition) {
    // ...
}

// For 循环
for (let i: int = 0; i < 10; i = i + 1) {
    // ...
}

// Return
return value;
```

### 函数

```nova
fn functionName(param: type) -> returnType {
    // 函数体
}
```

---

## 路线图

### v0.1.0 (当前) - 基础框架
- [x] 词法分析器实现
- [x] 语法分析器实现
- [x] AST 设计
- [x] 基础示例

### v0.2.0 - 语义分析
- [ ] 类型检查
- [ ] 符号表
- [ ] 作用域管理
- [ ] 错误检测

### v0.3.0 - IR 生成
- [ ] 中间表示
- [ ] SSA 形式
- [ ] 基础优化

### v0.4.0 - 代码生成
- [ ] LLVM IR 后端
- [ ] C 代码生成
- [ ] 字节码虚拟机

### v1.0.0 - 完整编译器
- [ ] 完整语言支持
- [ ] 标准库
- [ ] 包管理器
- [ ] IDE 集成

---

## 贡献指南

我们欢迎您的贡献！以下是参与方式：

### 贡献方式

- 🐛 **报告 Bug** - 使用 [GitHub Issues](https://github.com/NotSleeply/Nova-Compiler/issues)
- 💡 **建议功能** - 在 issues 中分享您的想法
- 📝 **改进文档** - 修复错别字、添加示例
- 💻 **编写代码** - 选择一个 issue 并提交 PR
- 🧪 **添加测试** - 提高测试覆盖率

### 开发环境设置

```bash
# Fork 并克隆
git clone https://github.com/YOUR_USERNAME/Nova-Compiler.git
cd Nova-Compiler

# 创建功能分支
git checkout -b feature/amazing-feature

# 进行修改并测试
make clean && make
./bin/novac --test

# 提交 Pull Request
git push origin feature/amazing-feature
```

### 代码风格

- 遵循现代 C++ 最佳实践
- 使用有意义的变量名
- 为复杂逻辑添加注释
- 为新功能编写测试

---

## 文档

- **[语言规范](docs/LANGUAGE.md)** - Nova 语言语法和语义
- **[架构指南](docs/ARCHITECTURE.md)** - 内部设计和实现
- **[贡献指南](docs/CONTRIBUTING.md)** - 如何贡献
- **[API 参考](docs/API.md)** - 公共 API 文档

---

## 许可证

本项目采用 **MIT 许可证** - 详情请见 [LICENSE](LICENSE) 文件。

---

## 致谢

Nova 编译器受到以下知识和项目的启发：

- **Crafting Interpreters** - Robert Nystrom 著
- **Engineering a Compiler** - Cooper & Torczon 著
- **Modern Compiler Implementation** - Andrew Appel 著
- **LLVM Project** - IR 设计的灵感来源

---

## 社区

- **GitHub**: [NotSleeply/Nova-Compiler](https://github.com/NotSleeply/Nova-Compiler)
- **Issues**: [Bug 报告和功能请求](https://github.com/NotSleeply/Nova-Compiler/issues)
- **Discussions**: [GitHub Discussions](https://github.com/NotSleeply/Nova-Compiler/discussions)

---

<div align="center">

**由开源社区用 ❤️ 构建**

[⬆ 返回顶部](#nova-编译器)

</div>
