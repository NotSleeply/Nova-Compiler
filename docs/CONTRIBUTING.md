# Contributing to Nova Compiler

感谢你考虑为 Nova 编译器做出贡献！🎉

## 目录

1. [行为准则](#行为准则)
2. [如何贡献](#如何贡献)
3. [开发环境设置](#开发环境设置)
4. [代码风格指南](#代码风格指南)
5. [提交代码](#提交代码)
6. [测试指南](#测试指南)
7. [文档指南](#文档指南)
8. [问题报告](#问题报告)
9. [功能请求](#功能请求)

---

## 行为准则

### 我们的承诺

为了营造一个开放和友好的环境，我们作为贡献者和维护者承诺：无论年龄、体型、残疾、种族、性别认同和表达、经验水平、教育程度、社会经济地位、国籍、外貌、种族、宗教或性取向如何，参与我们的项目和社区都将为每个人提供无骚扰的体验。

### 我们的标准

积极行为的例子包括：

- 使用友好和包容的语言
- 尊重不同的观点和经验
- 优雅地接受建设性批评
- 关注对社区最有利的事情
- 对其他社区成员表示同理心

---

## 如何贡献

### 贡献类型

我们欢迎以下类型的贡献：

1. **错误修复** - 修复编译器中的bug
2. **新功能** - 添加新的语言特性或编译器功能
3. **文档改进** - 完善文档、添加示例
4. **测试** - 添加单元测试、集成测试
5. **性能优化** - 提高编译器性能
6. **代码重构** - 改善代码结构和可读性

### 贡献流程

1. **Fork 仓库**
   ```bash
   git clone https://github.com/your-username/Nova-Compiler.git
   cd Nova-Compiler
   ```

2. **创建功能分支**
   ```bash
   git checkout -b feature/your-feature-name
   # 或
   git checkout -b fix/your-bug-fix
   ```

3. **进行更改**
   - 编写代码
   - 添加测试
   - 更新文档

4. **运行测试**
   ```bash
   make test
   ```

5. **提交更改**
   ```bash
   git add .
   git commit -m "描述你的更改"
   ```

6. **推送到 Fork**
   ```bash
   git push origin feature/your-feature-name
   ```

7. **创建 Pull Request**
   - 前往原仓库
   - 点击 "New Pull Request"
   - 填写 PR 模板
   - 等待审核

---

## 开发环境设置

### 系统要求

- **操作系统**: Linux, macOS, 或 Windows
- **编译器**: GCC 9+ 或 Clang 10+
- **构建工具**: GNU Make
- **C++ 标准**: C++17

### 安装步骤

#### Linux (Ubuntu/Debian)

```bash
# 安装编译器和构建工具
sudo apt-get update
sudo apt-get install -y build-essential g++ make

# 克隆仓库
git clone https://github.com/NotSleeply/Nova-Compiler.git
cd Nova-Compiler

# 构建项目
make

# 运行测试
make test
```

#### macOS

```bash
# 安装 Xcode Command Line Tools
xcode-select --install

# 安装 Homebrew (如果还没有)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装依赖
brew install gcc make

# 克隆仓库
git clone https://github.com/NotSleeply/Nova-Compiler.git
cd Nova-Compiler

# 构建项目
make

# 运行测试
make test
```

#### Windows

```powershell
# 安装 MinGW-w64 或使用 Visual Studio
# 推荐使用 MSYS2: https://www.msys2.org/

# 安装依赖
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make

# 克隆仓库
git clone https://github.com/NotSleeply/Nova-Compiler.git
cd Nova-Compiler

# 构建项目
mingw32-make

# 运行测试
mingw32-make test
```

### IDE 设置

#### VS Code

推荐扩展：
- C/C++ Extension Pack
- C++ Intellisense
- CodeLLDB (调试)

配置文件 (`.vscode/settings.json`):
```json
{
    "C_Cpp.default.cppStandard": "c++17",
    "C_Cpp.default.compilerPath": "/usr/bin/g++",
    "files.associations": {
        "*.h": "cpp",
        "*.cpp": "cpp"
    }
}
```

#### CLion

- 打开项目根目录
- IDE 会自动检测 `Makefile`
- 配置 C++17 标准

---

## 代码风格指南

### C++ 代码风格

#### 命名约定

```cpp
// 类名: PascalCase
class SemanticAnalyzer {
    // ...
};

// 函数名: camelCase
void analyzeFunction(const FunctionDeclPtr& func);

// 变量名: camelCase
int localVar = 42;

// 常量: UPPER_CASE
const int MAX_TOKENS = 1000;

// 成员变量: 带前缀 m_
class Lexer {
private:
    std::string m_source;
    size_t m_position;
};

// 或使用后缀 _
class Lexer {
private:
    std::string source_;
    size_t position_;
};
```

#### 格式化规则

```cpp
// 大括号风格 (K&R)
if (condition) {
    // ...
} else {
    // ...
}

// 函数定义
ReturnType ClassName::methodName(Type param1, Type param2) {
    // ...
}

// 缩进: 4 空格
void function() {
    if (true) {
        doSomething();
    }
}

// 行宽: 100 字符
// 长行拆分
auto result = someVeryLongFunction(
    argument1,
    argument2,
    argument3
);
```

#### 包含头文件顺序

```cpp
// 1. 相关头文件
#include "my_class.h"

// 2. C 系统头文件
#include <cstdio>
#include <cstdlib>

// 3. C++ 标准库
#include <string>
#include <vector>
#include <memory>

// 4. 第三方库
#include <gtest/gtest.h>

// 5. 项目内其他头文件
#include "other_class.h"
```

#### 智能指针使用

```cpp
// 优先使用智能指针
auto ptr = std::make_shared<MyClass>();
auto uniquePtr = std::make_unique<MyClass>();

// 避免裸指针 (除了函数参数)
void process(const MyClass* obj);  // OK: 不涉及所有权
MyClass* create();                  // 避免: 使用智能指针

// 使用引用传递对象
void process(const MyClass& obj);   // 好
void process(MyClass* obj);         // 避免 (除非可以为 nullptr)
```

#### 异常安全

```cpp
// 使用 RAII 管理资源
{
    std::ifstream file("data.txt");  // 自动关闭
    std::vector<int> data;           // 自动释放
}  // 自动清理

// 避免裸 new/delete
// 错误:
int* ptr = new int[100];
// ... 如果这里抛出异常，内存泄漏
delete[] ptr;

// 正确:
auto ptr = std::make_unique<int[]>(100);
// 自动清理，即使抛出异常
```

### 注释风格

```cpp
// 单行注释: 使用 //

/*
 * 多行注释: 使用这种风格
 * 每行开头加星号
 */

/**
 * 文档注释: 用于函数和类说明
 * @param name 参数说明
 * @return 返回值说明
 */
int calculate(int value);

// TODO 注释
// TODO(username): 描述待完成的工作

// FIXME 注释
// FIXME: 描述需要修复的问题
```

---

## 提交代码

### Commit 消息格式

遵循 [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <subject>

<body>

<footer>
```

#### Type 类型

- `feat`: 新功能
- `fix`: Bug 修复
- `docs`: 文档更新
- `style`: 代码格式 (不影响功能)
- `refactor`: 重构
- `test`: 添加测试
- `chore`: 构建/工具相关
- `perf`: 性能优化

#### 示例

```
feat(lexer): 添加对 Unicode 标识符的支持

- 支持 UTF-8 编码的标识符
- 添加 Unicode 字符分类
- 更新词法分析器测试用例

Closes #123
```

```
fix(parser): 修复嵌套 if 语句的解析错误

修复了当 if 语句嵌套时，else 子句关联错误的问题。

Fixes #456
```

```
docs: 更新 README 中的构建说明

添加 Windows 平台的构建指南。
```

### Pull Request 指南

#### PR 标题

使用与 commit 消息相同的格式：

```
feat(semantic): 添加类型推导支持
```

#### PR 描述模板

```markdown
## 描述

简要描述此 PR 的目的和实现方式。

## 更改类型

- [ ] Bug 修复
- [ ] 新功能
- [ ] 重构
- [ ] 文档更新
- [ ] 测试

## 如何测试

描述如何测试这些更改：

1. 运行 `make test`
2. 运行 `./bin/novac examples/test.nv`
3. ...

## 检查清单

- [ ] 代码遵循项目的代码风格
- [ ] 已添加测试用例
- [ ] 所有测试通过
- [ ] 已更新相关文档
- [ ] Commit 消息符合规范

## 相关 Issue

Closes #123
```

---

## 测试指南

### 测试结构

```
tests/
├── test_lexer.cpp      # 词法分析器测试
├── test_parser.cpp     # 语法分析器测试
├── test_semantic.cpp   # 语义分析测试
├── test_ir.cpp         # IR 生成测试
├── test_codegen.cpp    # 代码生成测试
└── catch2/
    └── catch.hpp       # 测试框架
```

### 编写测试

#### 单元测试示例

```cpp
#include "catch.hpp"
#include "lexer/lexer.h"

TEST_CASE("Lexer recognizes integer literals", "[lexer][literals]") {
    SECTION("Decimal integers") {
        Lexer lexer("42 123 9999", "test.nv");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 4);  // 3 integers + EOF
        CHECK(tokens[0].type == TokenType::INT_LITERAL);
        CHECK(tokens[0].value == "42");
        CHECK(tokens[1].value == "123");
        CHECK(tokens[2].value == "9999");
    }
    
    SECTION("Hexadecimal integers") {
        Lexer lexer("0xFF 0xABCD", "test.nv");
        auto tokens = lexer.tokenize();
        
        REQUIRE(tokens.size() == 3);
        CHECK(tokens[0].value == "0xFF");
        CHECK(tokens[1].value == "0xABCD");
    }
}

TEST_CASE("Lexer handles errors gracefully", "[lexer][errors]") {
    SECTION("Invalid characters") {
        Lexer lexer("@#$", "test.nv");
        auto tokens = lexer.tokenize();
        
        // 应该跳过无效字符并继续
        CHECK_FALSE(tokens.empty());
        CHECK(tokens.back().type == TokenType::EOF_TOKEN);
    }
}
```

#### 集成测试示例

```cpp
TEST_CASE("Full compilation pipeline", "[integration]") {
    std::string source = R"(
        fn add(a: int, b: int) -> int {
            return a + b;
        }
    )";
    
    // 词法分析
    Lexer lexer(source, "test.nv");
    auto tokens = lexer.tokenize();
    REQUIRE_FALSE(tokens.empty());
    
    // 语法分析
    Parser parser(tokens, "test.nv");
    auto ast = parser.parse();
    REQUIRE(ast != nullptr);
    
    // 语义分析
    SemanticAnalyzer analyzer;
    bool success = analyzer.analyze(ast);
    REQUIRE(success);
    
    // IR 生成
    IRGenerator irGen;
    auto ir = irGen.generate(ast);
    REQUIRE(ir != nullptr);
    
    // 代码生成
    CCodeGenerator codegen;
    std::string code = codegen.generate(ir);
    CHECK_FALSE(code.empty());
}
```

### 运行测试

```bash
# 运行所有测试
make test

# 运行特定测试
make test-lexer
make test-parser

# 详细输出
make test-verbose

# 使用测试框架直接运行
./bin/test_lexer "[lexer]"
./bin/test_parser "[parser]"
```

### 测试覆盖率

我们鼓励为所有新代码添加测试：

- **单元测试**: 每个公开的函数/方法
- **边界测试**: 空输入、极限值
- **错误测试**: 异常情况、错误处理
- **集成测试**: 多个组件协作

---

## 文档指南

### 文档类型

1. **API 文档**: 代码中的注释
2. **用户文档**: README, 使用指南
3. **设计文档**: ARCHITECTURE.md
4. **语言规范**: LANGUAGE.md
5. **变更日志**: CHANGELOG.md

### 文档风格

#### 代码注释

```cpp
/**
 * @brief 分析函数的语义信息
 * 
 * 执行类型检查、变量解析和作用域管理。
 * 如果发现语义错误，会将其添加到错误列表中。
 * 
 * @param func 要分析的函数声明节点
 * @return 如果分析成功返回 true，否则返回 false
 * 
 * @example
 * SemanticAnalyzer analyzer;
 * auto func = parseFunction("fn add(a: int, b: int) -> int { return a + b; }");
 * bool success = analyzer.analyzeFunction(func);
 */
bool analyzeFunction(const FunctionDeclPtr& func);
```

#### README 文档

```markdown
# 功能名称

简要描述功能。

## 用法

```nova
// 代码示例
```

## 参数

- `param1`: 参数说明

## 返回值

返回值说明

## 示例

更多示例...

## 注意事项

需要注意的事项...
```

### 更新文档

当添加新功能时，请确保更新：

1. **LANGUAGE.md**: 如果添加了新的语言特性
2. **ARCHITECTURE.md**: 如果改变了架构
3. **README.md**: 如果改变了使用方式
4. **CHANGELOG.md**: 记录所有重要变更

---

## 问题报告

### 报告 Bug

如果发现 bug，请创建 [Issue](https://github.com/NotSleeply/Nova-Compiler/issues)，包含：

1. **标题**: 简要描述问题
2. **环境**: 操作系统、编译器版本
3. **复现步骤**: 详细步骤
4. **期望行为**: 应该发生什么
5. **实际行为**: 实际发生了什么
6. **日志/截图**: 相关输出

#### Bug 报告模板

```markdown
## Bug 描述

简要描述 bug。

## 环境信息

- 操作系统: Ubuntu 22.04
- 编译器: GCC 11.2
- Nova 版本: v0.2.0

## 复现步骤

1. 创建文件 `test.nv`:
   ```nova
   fn main() {
       // ...
   }
   ```
2. 运行 `./bin/novac test.nv`
3. 看到错误

## 期望行为

应该成功编译。

## 实际行为

编译器崩溃，输出:
```
Error: segmentation fault
```

## 日志

完整的错误输出...

## 其他信息

其他相关信息...
```

---

## 功能请求

### 提交功能请求

如果你想添加新功能：

1. **检查现有 Issues**: 确认没有被请求
2. **创建 Issue**: 描述功能
3. **讨论**: 与维护者讨论实现方案
4. **实现**: 在达成一致后开始实现

#### 功能请求模板

```markdown
## 功能描述

简要描述你希望添加的功能。

## 使用场景

描述什么情况下需要这个功能。

## 建议的实现方案

如果有想法，简要描述如何实现。

## 替代方案

是否有其他方式可以实现相同目标？

## 其他信息

其他相关信息、示例等。
```

---

## 代码审核

### 审核标准

所有 PR 都需要通过代码审核，我们会检查：

1. **代码质量**: 清晰、可维护、遵循风格指南
2. **正确性**: 功能实现正确
3. **测试**: 有充分的测试覆盖
4. **文档**: 文档更新完整
5. **性能**: 没有性能退化

### 审核流程

1. 提交 PR
2. 自动测试运行 (CI)
3. 代码审核 (至少一位维护者)
4. 修改建议 (如果有)
5. 最终批准
6. 合并到主分支

---

## 联系方式

- **GitHub Issues**: 用于 bug 报告和功能请求
- **Pull Requests**: 用于代码贡献
- **Email**: notsleeply@example.com (仅用于敏感问题)

---

## 许可证

通过贡献代码，你同意你的代码将在项目的 MIT 许可证下发布。

---

**感谢你的贡献！** 🎉

每一份贡献，无论大小，都让 Nova 编译器变得更好！
