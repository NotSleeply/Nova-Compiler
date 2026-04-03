# Nova 语言规范

**版本**: v0.2.0  
**最后更新**: 2025-01-03

---

## 目录

1. [概述](#概述)
2. [词法结构](#词法结构)
3. [类型系统](#类型系统)
4. [表达式](#表达式)
5. [语句](#语句)
6. [声明](#声明)
7. [函数](#函数)
8. [控制流](#控制流)
9. [错误处理](#错误处理)
10. [示例代码](#示例代码)

---

## 概述

Nova 是一门现代、简洁的系统编程语言，设计目标：

- **简洁性**: 语法清晰，易于学习和使用
- **类型安全**: 强类型系统，编译时类型检查
- **性能**: 编译为高效的 C 代码
- **可读性**: 强调代码可读性和可维护性

### 设计哲学

- 明确优于隐式
- 编译时错误优于运行时错误
- 简单的设计优于复杂的功能
- 可读性优于简洁性

---

## 词法结构

### 关键字

Nova 保留以下关键字：

```
fn          let         const       if          else
while       for         return      struct      enum
true        false       void        int         float
string      bool        char        break       continue
null        import      export      as          from
```

### 标识符

标识符规则：
- 以字母或下划线开头
- 后跟字母、数字或下划线
- 区分大小写
- 不能是关键字

```
identifier ::= [a-zA-Z_][a-zA-Z0-9_]*
```

示例：
```
myVar
_privateVar
CamelCase
snake_case
var123
```

### 字面量

#### 整数字面量
```
42          // 十进制
0b1010      // 二进制
0o755       // 八进制
0xFF        // 十六进制
```

#### 浮点字面量
```
3.14
2.71828
1.0e10
6.626e-34
```

#### 字符串字面量
```
"hello, world"
"line 1\nline 2"
"tab\there"
"quote: \""
```

支持转义序列：
- `\n` - 换行
- `\t` - 制表符
- `\\` - 反斜杠
- `\"` - 双引号
- `\0` - 空字符

#### 字符字面量
```
'a'
'\n'
'\\'
'\"'
```

#### 布尔字面量
```
true
false
```

### 运算符

#### 算术运算符
```
+   加法
-   减法
*   乘法
/   除法
%   取模
```

#### 比较运算符
```
==  等于
!=  不等于
<   小于
<=  小于等于
>   大于
>=  大于等于
```

#### 逻辑运算符
```
&&  逻辑与
||  逻辑或
!   逻辑非
```

#### 位运算符
```
&   按位与
|   按位或
^   按位异或
~   按位取反
<<  左移
>>  右移
```

#### 赋值运算符
```
=   简单赋值
+=  加法赋值
-=  减法赋值
*=  乘法赋值
/=  除法赋值
%=  取模赋值
```

#### 其他运算符
```
++  自增
--  自减
?:  条件运算符
```

### 分隔符

```
(   )   [   ]   {   }
,   ;   :   .   ->
```

### 注释

#### 单行注释
```nova
// 这是单行注释
let x = 10; // 行尾注释
```

#### 多行注释
```nova
/* 
   这是多行注释
   可以跨越多行
*/
```

---

## 类型系统

### 基本类型

#### 整数类型
```
int     有符号整数（平台相关，通常 32 或 64 位）
int8    有符号 8 位整数
int16   有符号 16 位整数
int32   有符号 32 位整数
int64   有符号 64 位整数
uint    无符号整数
uint8   无符号 8 位整数
uint16  无符号 16 位整数
uint32  无符号 32 位整数
uint64  无符号 64 位整数
```

#### 浮点类型
```
float   浮点数（通常 64 位双精度）
f32     32 位单精度浮点
f64     64 位双精度浮点
```

#### 字符和字符串
```
char    单个字符
string  字符串
```

#### 布尔类型
```
bool    布尔值（true 或 false）
```

#### 空类型
```
void    无类型（用于函数返回值）
```

### 复合类型

#### 数组
```nova
int[10]      // 固定大小数组
float[]      // 动态数组（未来支持）
```

#### 结构体
```nova
struct Point {
    x: float,
    y: float
}
```

#### 枚举
```nova
enum Status {
    Ok,
    Error,
    Pending
}
```

#### 指针（未来支持）
```nova
int*         // 指向整数的指针
void*        // 通用指针
```

### 类型别名

```nova
type Age = int;
type Name = string;
type Point3D = struct { x: float, y: float, z: float };
```

### 类型推断

Nova 支持类型推断：

```nova
let x = 42;          // 推断为 int
let y = 3.14;        // 推断为 float
let z = "hello";     // 推断为 string
let flag = true;     // 推断为 bool
```

---

## 表达式

### 字面量表达式

```nova
42
3.14
"hello"
'a'
true
```

### 标识符表达式

```nova
x
myVar
structure.field
```

### 算术表达式

```nova
a + b
a - b
a * b
a / b
a % b
-a
```

### 比较表达式

```nova
a == b
a != b
a < b
a <= b
a > b
a >= b
```

### 逻辑表达式

```nova
a && b
a || b
!a
```

### 位运算表达式

```nova
a & b
a | b
a ^ b
~a
a << b
a >> b
```

### 赋值表达式

```nova
a = 10
a += 5
a -= 3
a *= 2
a /= 2
a %= 3
```

### 自增/自减表达式

```nova
a++    // 后缀自增
++a    // 前缀自增
a--    // 后缀自减
--a    // 前缀自减
```

### 条件表达式

```nova
condition ? value_if_true : value_if_false
```

示例：
```nova
let max = a > b ? a : b;
```

### 函数调用表达式

```nova
functionName(arg1, arg2, arg3)
```

### 数组访问表达式

```nova
array[index]
```

### 成员访问表达式

```nova
structure.field
pointer->field    // 未来支持
```

### 括号表达式

```nova
(a + b) * c
```

### 运算符优先级

从高到低：

1. `()` `[]` `.` `->`
2. `!` `~` `++` `--` (一元运算符)
3. `*` `/` `%`
4. `+` `-`
5. `<<` `>>`
6. `<` `<=` `>` `>=`
7. `==` `!=`
8. `&`
9. `^`
10. `|`
11. `&&`
12. `||`
13. `?:` (条件运算符)
14. `=` `+=` `-=` `*=` `/=` `%=` (赋值运算符)

---

## 语句

### 表达式语句

```nova
x = 10;
foo();
```

### 复合语句（代码块）

```nova
{
    let x = 10;
    let y = 20;
    x + y;
}
```

### 空语句

```nova
;
```

### 声明语句

```nova
let x: int = 10;
const PI: float = 3.14159;
```

---

## 声明

### 变量声明

#### let 声明（可变变量）
```nova
let x: int = 10;
let y = 20;           // 类型推断
let z: float;         // 延迟初始化
z = 3.14;
```

#### const 声明（常量）
```nova
const PI: float = 3.14159;
const MESSAGE = "Hello";  // 类型推断
```

### 函数声明

```nova
fn functionName(param1: type1, param2: type2) -> returnType {
    // 函数体
}
```

示例：
```nova
fn add(a: int, b: int) -> int {
    return a + b;
}

fn greet(name: string) -> void {
    print("Hello, " + name);
}

fn getPi() -> float {
    3.14159  // 隐式返回
}
```

### 结构体声明

```nova
struct Point {
    x: float,
    y: float
}

fn main() {
    let p = Point { x: 1.0, y: 2.0 };
    print(p.x, p.y);
}
```

### 枚举声明

```nova
enum Status {
    Ok,
    Error,
    Pending
}

fn checkStatus(s: Status) -> void {
    if s == Status.Ok {
        print("OK");
    }
}
```

---

## 函数

### 函数定义

```nova
fn functionName(parameters) -> returnType {
    // 函数体
}
```

### 参数

#### 位置参数
```nova
fn add(a: int, b: int) -> int {
    return a + b;
}
```

#### 默认参数（未来支持）
```nova
fn greet(name: string, greeting: string = "Hello") -> void {
    print(greeting + ", " + name);
}
```

### 返回值

#### 显式返回
```nova
fn max(a: int, b: int) -> int {
    if a > b {
        return a;
    }
    return b;
}
```

#### 隐式返回
```nova
fn add(a: int, b: int) -> int {
    a + b  // 最后一个表达式的值作为返回值
}
```

#### 无返回值
```nova
fn printMessage(msg: string) -> void {
    print(msg);
}
```

### 函数调用

```nova
let result = add(10, 20);
greet("Alice");
```

### 递归

```nova
fn factorial(n: int) -> int {
    if n <= 1 {
        return 1;
    }
    return n * factorial(n - 1);
}
```

---

## 控制流

### 条件语句

#### if 语句
```nova
if condition {
    // 执行代码
}
```

#### if-else 语句
```nova
if condition {
    // 条件为真
} else {
    // 条件为假
}
```

#### if-else if-else 链
```nova
if condition1 {
    // 条件1为真
} else if condition2 {
    // 条件2为真
} else {
    // 所有条件都为假
}
```

示例：
```nova
let score = 85;
let grade: string;

if score >= 90 {
    grade = "A";
} else if score >= 80 {
    grade = "B";
} else if score >= 70 {
    grade = "C";
} else if score >= 60 {
    grade = "D";
} else {
    grade = "F";
}
```

### 循环语句

#### while 循环
```nova
while condition {
    // 循环体
}
```

示例：
```nova
let i = 0;
while i < 10 {
    print(i);
    i++;
}
```

#### for 循环
```nova
for init; condition; update {
    // 循环体
}
```

示例：
```nova
for let i = 0; i < 10; i++ {
    print(i);
}
```

#### 循环控制

**break**: 跳出循环
```nova
while true {
    let input = getInput();
    if input == "quit" {
        break;
    }
}
```

**continue**: 跳到下一次迭代
```nova
for let i = 0; i < 10; i++ {
    if i % 2 == 0 {
        continue;
    }
    print(i);  // 只打印奇数
}
```

### 匹配语句（未来支持）

```nova
match value {
    pattern1 => action1,
    pattern2 => action2,
    _ => defaultAction
}
```

---

## 错误处理

### 编译时错误

编译器在以下情况会报告错误：

- **词法错误**: 非法字符、未闭合的字符串等
- **语法错误**: 语法结构错误
- **语义错误**: 类型不匹配、未定义的标识符等

示例：
```nova
let x: int = "string";  // 类型错误
```

### 运行时错误（未来支持）

计划支持错误处理机制：

```nova
fn divide(a: float, b: float) -> float | Error {
    if b == 0 {
        return Error("Division by zero");
    }
    return a / b;
}
```

---

## 示例代码

### Hello World

```nova
fn main() -> void {
    print("Hello, World!");
}
```

### 阶乘计算

```nova
fn factorial(n: int) -> int {
    if n <= 1 {
        return 1;
    }
    return n * factorial(n - 1);
}

fn main() -> void {
    let result = factorial(5);
    print("5! = " + result);
}
```

### 斐波那契数列

```nova
fn fibonacci(n: int) -> int {
    if n <= 1 {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

fn main() -> void {
    for let i = 0; i < 10; i++ {
        print(fibonacci(i));
    }
}
```

### 结构体使用

```nova
struct Point {
    x: float,
    y: float
}

fn distance(p1: Point, p2: Point) -> float {
    let dx = p2.x - p1.x;
    let dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}

fn main() -> void {
    let p1 = Point { x: 0.0, y: 0.0 };
    let p2 = Point { x: 3.0, y: 4.0 };
    let dist = distance(p1, p2);
    print("Distance: " + dist);
}
```

### 枚举使用

```nova
enum Direction {
    North,
    South,
    East,
    West
}

fn move(dir: Direction) -> void {
    if dir == Direction.North {
        print("Moving north");
    } else if dir == Direction.South {
        print("Moving south");
    } else if dir == Direction.East {
        print("Moving east");
    } else {
        print("Moving west");
    }
}
```

---

## 附录

### 关键字完整列表

| 关键字 | 用途 |
|--------|------|
| `fn` | 函数声明 |
| `let` | 可变变量声明 |
| `const` | 常量声明 |
| `if` | 条件语句 |
| `else` | 条件分支 |
| `while` | while 循环 |
| `for` | for 循环 |
| `return` | 函数返回 |
| `struct` | 结构体声明 |
| `enum` | 枚举声明 |
| `true` | 布尔真值 |
| `false` | 布尔假值 |
| `void` | 空类型 |
| `int` | 整数类型 |
| `float` | 浮点类型 |
| `string` | 字符串类型 |
| `bool` | 布尔类型 |
| `char` | 字符类型 |
| `break` | 跳出循环 |
| `continue` | 继续下一次迭代 |
| `null` | 空值（未来支持） |
| `import` | 导入模块（未来支持） |
| `export` | 导出符号（未来支持） |
| `as` | 类型别名（未来支持） |
| `from` | 模块导入源（未来支持） |

### 运算符优先级表

| 优先级 | 运算符 | 结合性 |
|--------|--------|--------|
| 1 (最高) | `()` `[]` `.` `->` | 左到右 |
| 2 | `!` `~` `++` `--` `+` `-` (一元) | 右到左 |
| 3 | `*` `/` `%` | 左到右 |
| 4 | `+` `-` (二元) | 左到右 |
| 5 | `<<` `>>` | 左到右 |
| 6 | `<` `<=` `>` `>=` | 左到右 |
| 7 | `==` `!=` | 左到右 |
| 8 | `&` | 左到右 |
| 9 | `^` | 左到右 |
| 10 | `\|` | 左到右 |
| 11 | `&&` | 左到右 |
| 12 | `\|\|` | 左到右 |
| 13 | `?:` | 右到左 |
| 14 (最低) | `=` `+=` `-=` `*=` `/=` `%=` | 右到左 |

---

## 未来特性

以下特性计划在未来版本中实现：

- [ ] 泛型
- [ ] 模式匹配
- [ ] 错误处理机制
- [ ] 模块系统
- [ ] 并发原语
- [ ] 内存安全特性
- [ ] 宏系统
- [ ] 反射

---

## 版本历史

- **v0.2.0** (2025-01-03): 添加代码生成、完善类型系统
- **v0.1.0** (2024-12-20): 初始版本，支持词法和语法分析

---

**文档维护者**: Nova 编译器团队  
**许可协议**: MIT License
