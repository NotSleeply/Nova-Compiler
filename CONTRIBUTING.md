# 贡献指南

感谢您对 Nova 编译器项目的关注！本文档将帮助您了解如何为项目做出贡献。

---

## 目录

1. [行为准则](#行为准则)
2. [如何贡献](#如何贡献)
3. [开发环境设置](#开发环境设置)
4. [代码规范](#代码规范)
5. [提交规范](#提交规范)
6. [Pull Request 流程](#pull-request-流程)
7. [问题报告](#问题报告)
8. [功能请求](#功能请求)
9. [文档贡献](#文档贡献)

---

## 行为准则

### 我们的承诺

为了营造开放和友好的环境，我们承诺：

- 尊重不同的观点和经验
- 优雅地接受建设性批评
- 关注对社区最有利的事情
- 对其他社区成员表示同理心

### 不可接受的行为

- 使用性化的语言或图像
- 捣乱、侮辱/贬损评论以及人身或政治攻击
- 公开或私下的骚扰
- 未经明确许可，发布他人的私人信息
- 其他在专业环境中可能被合理认为不适当的行为

---

## 如何贡献

### 贡献方式

您可以通过以下方式为 Nova 编译器做出贡献：

1. **报告 Bug**: 发现问题后提交 Issue
2. **功能请求**: 提出新功能建议
3. **代码贡献**: 提交 Pull Request
4. **文档改进**: 完善、翻译文档
5. **测试贡献**: 编写测试用例
6. **社区支持**: 回答问题、参与讨论

### 贡献流程

```
1. Fork 仓库
   ↓
2. 创建功能分支
   ↓
3. 进行开发
   ↓
4. 编写测试
   ↓
5. 提交 Pull Request
   ↓
6. 代码审查
   ↓
7. 合并到主分支
```

---

## 开发环境设置

### 系统要求

- **操作系统**: Linux / macOS / Windows
- **编译器**: GCC 9+ / Clang 10+ / MSVC 2019+
- **构建工具**: Make
- **C++ 标准**: C++17

### 安装步骤

#### Linux (Ubuntu/Debian)

```bash
# 安装依赖
sudo apt-get update
sudo apt-get install -y build-essential git

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
# 安装 Visual Studio 2019+ (包含 C++ 工具)

# 克隆仓库
git clone https://github.com/NotSleeply/Nova-Compiler.git
cd Nova-Compiler

# 构建项目
make

# 运行测试
make test
```

### 项目结构

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

---

## 代码规范

### 命名约定

#### 类和结构体
```cpp
// PascalCase
class Lexer { };
struct Token { };
```

#### 函数和方法
```cpp
// camelCase
void parseExpression();
int getLineNumber();
```

#### 变量
```cpp
// camelCase for local variables
int lineNumber;

// snake_case_ with trailing underscore for member variables
class Lexer {
private:
    std::string source_;
    int currentPos_;
};
```

#### 常量
```cpp
// UPPER_SNAKE_CASE
const int MAX_TOKEN_LENGTH = 1024;
constexpr double PI = 3.14159;
```

#### 枚举
```cpp
// PascalCase for enum name and values
enum class TokenType {
    IntLiteral,
    FloatLiteral,
    StringLiteral,
    Identifier
};
```

### 代码格式化

#### 缩进
- 使用 4 个空格缩进（不使用制表符）

#### 大括号
```cpp
// 函数和类定义：新行
void function()
{
    // ...
}

class MyClass
{
public:
    // ...
};

// 控制流：同行
if (condition) {
    // ...
} else {
    // ...
}

while (condition) {
    // ...
}
```

#### 行长度
- 最大行长度：100 字符
- 长行应适当换行

#### 空行
- 类/结构体成员分组之间加空行
- 函数之间加空行
- 逻辑块之间可加空行

### 注释规范

#### 文件头注释
```cpp
/**
 * @file lexer.h
 * @brief 词法分析器定义
 * @author Your Name
 * @date 2025-01-03
 */
```

#### 函数注释
```cpp
/**
 * @brief 将源代码转换为词元流
 * @param source 源代码字符串
 * @param filename 文件名（用于错误报告）
 * @return 词元列表
 * @throws LexicalError 如果遇到非法字符
 */
std::vector<Token> tokenize(const std::string& source, const std::string& filename);
```

#### 行内注释
```cpp
// 检查是否是数字字面量
if (isdigit(currentChar)) {
    return scanNumber();
}
```

#### TODO 注释
```cpp
// TODO(author): 添加对 Unicode 标识符的支持
// FIXME(author): 修复多行注释中的错误恢复
```

### C++ 特性使用

#### 智能指针
```cpp
// 优先使用智能指针
using ASTNodePtr = std::shared_ptr<ASTNode>;

// 避免使用裸指针（除非必要）
ASTNode* node = nullptr;  // ❌ 避免
auto node = std::make_shared<ASTNode>();  // ✅ 推荐
```

#### RAII
```cpp
// 使用 RAII 管理资源
class FileReader {
public:
    FileReader(const std::string& filename) : file_(filename) { }
    ~FileReader() { if (file_.is_open()) file_.close(); }
    
private:
    std::ifstream file_;
};
```

#### 范围 for 循环
```cpp
// 使用范围 for 循环
for (const auto& token : tokens) {
    processToken(token);
}

// 避免传统 for 循环（除非需要索引）
for (size_t i = 0; i < tokens.size(); ++i) {
    processToken(tokens[i]);
}
```

#### auto 使用
```cpp
// 类型明显时使用 auto
auto it = map.find(key);  // ✅ 好
auto length = str.length();  // ✅ 好

// 类型不明显时显式声明类型
int count = calculateCount();  // ✅ 好（明确类型）
```

---

## 提交规范

### 提交消息格式

```
<type>(<scope>): <subject>

<body>

<footer>
```

#### Type（必需）

- `feat`: 新功能
- `fix`: Bug 修复
- `docs`: 文档更新
- `style`: 代码格式化（不影响功能）
- `refactor`: 重构（既不是新功能也不是 Bug 修复）
- `test`: 添加或修改测试
- `chore`: 构建过程或辅助工具的变动
- `perf`: 性能优化
- `ci`: CI 配置文件和脚本的变动

#### Scope（可选）

表示提交影响的范围，例如：

- `lexer`: 词法分析器
- `parser`: 语法分析器
- `semantic`: 语义分析器
- `ir`: IR 生成器
- `codegen`: 代码生成器
- `docs`: 文档
- `test`: 测试

#### Subject（必需）

提交的简要描述：

- 使用动词开头，第一人称现在时："change" 而非 "changed" 或 "changes"
- 首字母小写
- 结尾不加句号

#### Body（可选）

提交的详细描述：

- 使用第一人称现在时
- 应该说明代码变动的动机，并与之前的行为对比

#### Footer（可选）

用于关闭 Issue 或标注破坏性变更：

```
Closes #123
BREAKING CHANGE: 不再支持 Nova v0.1.0 的旧语法
```

### 提交示例

```
feat(lexer): 添加对 Unicode 标识符的支持

- 支持 UTF-8 编码的 Unicode 标识符
- 更新词法分析器测试用例
- 更新语言规范文档

Closes #45
```

```
fix(parser): 修复嵌套函数声明的解析错误

之前在解析嵌套函数声明时会错误地将其解析为函数调用。
现在正确处理了函数声明的嵌套情况。

修复了 issue #67 中报告的问题。
```

---

## Pull Request 流程

### 创建 Pull Request

1. **Fork 仓库**: 点击右上角 "Fork" 按钮

2. **克隆您的 Fork**:
   ```bash
   git clone https://github.com/YOUR_USERNAME/Nova-Compiler.git
   cd Nova-Compiler
   ```

3. **添加上游仓库**:
   ```bash
   git remote add upstream https://github.com/NotSleeply/Nova-Compiler.git
   ```

4. **创建功能分支**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

5. **进行开发**:
   - 编写代码
   - 添加测试
   - 更新文档

6. **提交更改**:
   ```bash
   git add .
   git commit -m "feat: your feature description"
   ```

7. **推送到您的 Fork**:
   ```bash
   git push origin feature/your-feature-name
   ```

8. **创建 Pull Request**:
   - 访问您的 Fork 页面
   - 点击 "Compare & pull request"
   - 填写 PR 描述
   - 提交 PR

### Pull Request 要求

#### 必需项

- [ ] 代码通过所有测试
- [ ] 新功能有对应的测试用例
- [ ] 遵循代码规范
- [ ] 提交消息符合规范
- [ ] 更新相关文档

#### 可选项

- [ ] 添加示例代码
- [ ] 更新 CHANGELOG.md
- [ ] 性能测试（如果涉及性能优化）

### 代码审查

所有 Pull Request 都需要至少一位维护者的审查才能合并。

#### 审查重点

1. **正确性**: 代码是否正确实现了功能
2. **测试**: 测试覆盖率是否足够
3. **性能**: 是否有性能问题
4. **可维护性**: 代码是否易于理解和维护
5. **文档**: 文档是否完整

#### 审查流程

```
提交 PR → 自动测试 → 代码审查 → 修改反馈 → 最终批准 → 合并
```

---

## 问题报告

### 提交 Issue 之前

1. 搜索现有 Issue，确保不是重复问题
2. 阅读文档，确认不是使用方法问题
3. 尝试使用最新版本，确认问题是否已修复

### Issue 模板

```markdown
**问题描述**
简要描述遇到的问题。

**复现步骤**
1. 创建文件 `test.nv`，内容如下：
   ```nova
   // 您的代码
   ```
2. 运行命令：
   ```bash
   ./bin/novac test.nv
   ```
3. 查看错误信息

**预期行为**
描述您期望发生什么。

**实际行为**
描述实际发生了什么。

**环境信息**
- 操作系统: [例如 Ubuntu 20.04]
- 编译器版本: [例如 GCC 10.2.0]
- Nova 版本: [例如 v0.2.0]

**附加信息**
- 错误日志
- 截图
- 其他相关信息
```

---

## 功能请求

### 提交功能请求之前

1. 搜索现有 Issue，确保不是重复请求
2. 阅读文档，确认功能不存在
3. 考虑功能是否符合项目目标

### 功能请求模板

```markdown
**功能描述**
清晰描述您希望添加的功能。

**使用场景**
描述这个功能可以解决什么问题。

**建议的实现方式**
如果您有想法，可以建议如何实现。

**替代方案**
描述您考虑过的其他替代方案。

**附加信息**
- 相关的编译器实现
- 参考文献
- 其他相关信息
```

---

## 文档贡献

### 文档类型

1. **语言规范**: `docs/LANGUAGE.md`
2. **架构设计**: `docs/ARCHITECTURE.md`
3. **API 文档**: 代码中的注释
4. **教程**: `docs/tutorials/`
5. **示例代码**: `examples/`

### 文档风格

- 使用简洁、清晰的语言
- 提供代码示例
- 包含必要的图表
- 保持文档更新

### 文档更新流程

1. 修改文档文件
2. 提交 Pull Request
3. 等待审查
4. 合并更新

---

## 获取帮助

- **GitHub Issues**: 提交问题或功能请求
- **GitHub Discussions**: 参与讨论
- **Email**: maintainer@example.com

---

## 许可协议

通过贡献代码，您同意您的贡献将按照 MIT 许可协议授权。

---

**感谢您为 Nova 编译器做出的贡献！**
