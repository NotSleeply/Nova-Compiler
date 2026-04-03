# Nova Language Specification

**Version**: 0.2.0  
**Last Updated**: 2026-04-03

## Table of Contents

1. [Introduction](#introduction)
2. [Lexical Structure](#lexical-structure)
3. [Types](#types)
4. [Declarations](#declarations)
5. [Expressions](#expressions)
6. [Statements](#statements)
7. [Functions](#functions)
8. [Memory Management](#memory-management)
9. [Standard Library](#standard-library)

---

## Introduction

Nova is a modern, statically-typed programming language designed for systems programming with a focus on safety, performance, and developer productivity. It features:

- **Static typing** with type inference
- **Memory safety** through ownership semantics
- **Zero-cost abstractions**
- **Clean, readable syntax** inspired by Rust, Swift, and Kotlin
- **Ahead-of-time compilation** to native code via C transpilation

### Design Goals

1. **Safety First**: Catch errors at compile time, not runtime
2. **Performance**: Zero-cost abstractions, no garbage collector
3. **Readability**: Clear, unambiguous syntax
4. **Simplicity**: Small core language, powerful standard library
5. **Interoperability**: Seamless integration with C code

---

## Lexical Structure

### Keywords

```
fn          let         const       if          else
while       for         return      struct      enum
true        false       void        int         float
string      bool        char        break       continue
match       case        default     pub         priv
static      mut         ref         as          in
```

### Identifiers

```
identifier ::= [a-zA-Z_][a-zA-Z0-9_]*
```

**Examples**: `foo`, `_bar`, `myVar123`, `MyType`

### Literals

#### Integer Literals

```
decimal     ::= [0-9]+
hexadecimal ::= 0x[0-9a-fA-F]+
octal       ::= 0o[0-7]+
binary      ::= 0b[01]+
```

**Examples**: `42`, `0xFF`, `0o755`, `0b1010`

#### Floating-Point Literals

```
float ::= [0-9]+\.[0-9]+([eE][+-]?[0-9]+)?
```

**Examples**: `3.14`, `2.5e10`, `1.0e-5`

#### String Literals

```
string ::= "([^"\\]|\\.)*"
```

**Escape Sequences**:
- `\n` - Newline
- `\t` - Tab
- `\\` - Backslash
- `\"` - Double quote
- `\0` - Null character
- `\xHH` - Hexadecimal byte

**Examples**: `"Hello, World!"`, `"Line 1\nLine 2"`

#### Character Literals

```
char ::= '([^'\\]|\\.)'
```

**Examples**: `'A'`, `'\n'`, `'\x41'`

#### Boolean Literals

```
boolean ::= true | false
```

### Operators

```
Arithmetic: + - * / % ** (power)
Comparison: == != < <= > >=
Logical:    && || !
Bitwise:    & | ^ ~ << >>
Assignment: = += -= *= /= %= **= &= |= ^= <<= >>=
Other:      . -> :: ?:
```

### Delimiters

```
( ) [ ] { } , ; : ::
```

### Comments

```
// Single-line comment

/*
 * Multi-line comment
 */
```

---

## Types

### Primitive Types

| Type | Description | Size | Range |
|------|-------------|------|-------|
| `void` | No value | 0 | N/A |
| `int` | Signed integer | 64-bit | -2^63 to 2^63-1 |
| `float` | Floating-point | 64-bit | IEEE 754 double |
| `bool` | Boolean | 8-bit | `true`/`false` |
| `char` | Unicode character | 32-bit | UTF-32 code point |
| `string` | UTF-8 string | ptr | Variable length |

### Type Modifiers

- `mut` - Mutable
- `const` - Immutable (default)
- `ref` - Reference
- `static` - Static storage

**Examples**:

```nova
let x: int = 42;              // Immutable integer
let mut y: int = 10;          // Mutable integer
let ref z: int = &x;          // Reference to integer
let static PI: float = 3.14;  // Static constant
```

### Composite Types

#### Structs

```nova
struct Point {
    x: float,
    y: float,
}

struct Person {
    name: string,
    age: int,
    email: string,
}
```

#### Enums

```nova
enum Option<T> {
    Some(T),
    None,
}

enum Result<T, E> {
    Ok(T),
    Err(E),
}

enum Color {
    Red,
    Green,
    Blue,
    RGB(int, int, int),
}
```

### Arrays

```nova
let arr: int[5] = [1, 2, 3, 4, 5];
let matrix: float[3][3] = [
    [1.0, 0.0, 0.0],
    [0.0, 1.0, 0.0],
    [0.0, 0.0, 1.0],
];
```

### Pointers

```nova
let ptr: *int = &x;      // Raw pointer
let ref ref_val: int = &x;  // Reference (safer)
```

### Type Aliases

```nova
type UserID = int;
type Matrix = float[3][3];
type Callback = fn(int, int) -> int;
```

---

## Declarations

### Variable Declarations

```nova
// Immutable variable (type inferred)
let x = 42;

// Immutable variable (explicit type)
let y: float = 3.14;

// Mutable variable
let mut counter = 0;

// Constant (compile-time)
const MAX_SIZE: int = 100;

// Static variable (program lifetime)
static instance_count: int = 0;
```

### Function Declarations

```nova
// Basic function
fn add(a: int, b: int) -> int {
    return a + b;
}

// Function with no return value
fn greet(name: string) -> void {
    print("Hello, " + name + "!");
}

// Generic function
fn max<T>(a: T, b: T) -> T where T: Comparable {
    if a > b {
        return a;
    }
    return b;
}

// Function with default parameters
fn power(base: int, exp: int = 2) -> int {
    // ...
}
```

### Struct Declarations

```nova
struct Point {
    x: float,
    y: float,
}

// Methods
impl Point {
    fn new(x: float, y: float) -> Point {
        return Point { x: x, y: y };
    }

    fn distance(&self, other: &Point) -> float {
        let dx = self.x - other.x;
        let dy = self.y - other.y;
        return sqrt(dx * dx + dy * dy);
    }
}
```

### Enum Declarations

```nova
enum Option<T> {
    Some(T),
    None,
}

impl<T> Option<T> {
    fn is_some(&self) -> bool {
        match self {
            Option::Some(_) => true,
            Option::None => false,
        }
    }
}
```

---

## Expressions

### Arithmetic Expressions

```nova
let sum = a + b;
let diff = a - b;
let product = a * b;
let quotient = a / b;
let remainder = a % b;
let power = a ** 2;
```

### Comparison Expressions

```nova
let eq = a == b;
let ne = a != b;
let lt = a < b;
let le = a <= b;
let gt = a > b;
let ge = a >= b;
```

### Logical Expressions

```nova
let and_result = a && b;
let or_result = a || b;
let not_result = !a;
```

### Bitwise Expressions

```nova
let and = a & b;
let or = a | b;
let xor = a ^ b;
let not = ~a;
let left_shift = a << 2;
let right_shift = a >> 2;
```

### Assignment Expressions

```nova
x = 42;
x += 10;   // x = x + 10
x -= 5;    // x = x - 5
x *= 2;    // x = x * 2
x /= 3;    // x = x / 3
```

### Function Calls

```nova
let result = add(10, 20);
let greeting = greet("Alice");
let value = compute(x, y, z);
```

### Member Access

```nova
let point = Point { x: 1.0, y: 2.0 };
let x_coord = point.x;
let distance = point.distance(&other);
```

### Array Access

```nova
let arr = [1, 2, 3, 4, 5];
let first = arr[0];
let last = arr[4];
arr[2] = 10;
```

### Conditional Expression

```nova
let max = if a > b { a } else { b };
let result = if x > 0 {
    "positive"
} else if x < 0 {
    "negative"
} else {
    "zero"
};
```

### Cast Expression

```nova
let int_val = 42;
let float_val = int_val as float;
let char_code = 'A' as int;
```

---

## Statements

### Expression Statements

```nova
x = 10;
add(5, 3);
```

### Block Statements

```nova
{
    let x = 10;
    let y = 20;
    print(x + y);
}
```

### If Statements

```nova
if x > 0 {
    print("positive");
} else if x < 0 {
    print("negative");
} else {
    print("zero");
}
```

### While Statements

```nova
let i = 0;
while i < 10 {
    print(i);
    i += 1;
}
```

### For Statements

```nova
// C-style for loop
for let i = 0; i < 10; i += 1 {
    print(i);
}

// Range-based for loop
for item in collection {
    print(item);
}

// For loop with step
for i in 0..10 step 2 {
    print(i);
}
```

### Match Statements

```nova
match value {
    Option::Some(x) => print("Got: " + x),
    Option::None => print("Nothing"),
}

match color {
    Color::Red => print("Red"),
    Color::Green => print("Green"),
    Color::Blue => print("Blue"),
    Color::RGB(r, g, b) => print("RGB(" + r + ", " + g + ", " + b + ")"),
    _ => print("Unknown color"),
}
```

### Return Statements

```nova
fn add(a: int, b: int) -> int {
    return a + b;
}

fn early_exit(x: int) -> void {
    if x < 0 {
        return;
    }
    print(x);
}
```

### Break and Continue

```nova
while true {
    let x = get_input();
    if x == -1 {
        break;
    }
    if x == 0 {
        continue;
    }
    process(x);
}
```

---

## Functions

### Function Parameters

```nova
// Multiple parameters
fn add(a: int, b: int) -> int {
    return a + b;
}

// Default parameters
fn greet(name: string = "World") -> string {
    return "Hello, " + name + "!";
}

// Variadic functions
fn sum(count: int, ...) -> int {
    // ...
}
```

### Return Types

```nova
// Single return type
fn get_value() -> int {
    return 42;
}

// Multiple return types (tuple)
fn divide(a: int, b: int) -> (int, bool) {
    if b == 0 {
        return (0, false);
    }
    return (a / b, true);
}

// No return value
fn do_something() -> void {
    // ...
}
```

### Closures

```nova
let add = |a: int, b: int| -> int {
    return a + b;
};

let multiplier = |factor: int| {
    return |x: int| -> int {
        return x * factor;
    };
};
```

### Higher-Order Functions

```nova
fn apply_twice(f: fn(int) -> int, x: int) -> int {
    return f(f(x));
}

fn map<T, U>(arr: T[], f: fn(T) -> U) -> U[] {
    // ...
}
```

---

## Memory Management

### Ownership

Nova uses ownership semantics inspired by Rust:

1. **Each value has exactly one owner**
2. **When owner goes out of scope, value is dropped**
3. **Ownership can be transferred or borrowed**

```nova
fn take_ownership(s: string) -> void {
    // s is dropped when function exits
}

let text = "Hello".to_string();
take_ownership(text);
// text is no longer valid here
```

### Borrowing

```nova
fn borrow_value(s: &string) -> void {
    // Can read but not modify
}

fn borrow_mutably(s: &mut string) -> void {
    // Can read and modify
    s += " World";
}

let mut text = "Hello".to_string();
borrow_value(&text);        // Immutable borrow
borrow_mutably(&mut text);  // Mutable borrow
```

### Lifetimes

```nova
fn longest<'a>(x: &'a string, y: &'a string) -> &'a string {
    if x.len() > y.len() {
        return x;
    }
    return y;
}
```

---

## Standard Library

### Core Types

```nova
// Option type for nullable values
enum Option<T> {
    Some(T),
    None,
}

// Result type for error handling
enum Result<T, E> {
    Ok(T),
    Err(E),
}
```

### Collections

```nova
// Dynamic array
let vec = Vec<int>::new();
vec.push(1);
vec.push(2);

// Hash map
let map = HashMap<string, int>::new();
map.insert("one", 1);
map.insert("two", 2);
```

### I/O

```nova
// Print to stdout
print("Hello, World!");
println("With newline");

// Read from stdin
let input = read_line();

// File I/O
let content = read_file("input.txt");
write_file("output.txt", content);
```

### String Operations

```nova
let s = "Hello, World!";

let length = s.len();
let upper = s.to_upper();
let lower = s.to_lower();
let parts = s.split(", ");
let joined = parts.join(" - ");
let replaced = s.replace("World", "Nova");
```

### Math Functions

```nova
let x = abs(-42);           // 42
let y = sqrt(16.0);         // 4.0
let z = pow(2.0, 10.0);     // 1024.0
let max_val = max(10, 20);  // 20
let min_val = min(10, 20);  // 10
```

---

## Example Programs

### Hello World

```nova
fn main() -> void {
    println("Hello, World!");
}
```

### Factorial

```nova
fn factorial(n: int) -> int {
    if n <= 1 {
        return 1;
    }
    return n * factorial(n - 1);
}

fn main() -> void {
    let result = factorial(5);
    println("5! = " + result);
}
```

### Fibonacci

```nova
fn fibonacci(n: int) -> int {
    if n <= 1 {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

fn main() -> void {
    for i in 0..10 {
        println("fib(" + i + ") = " + fibonacci(i));
    }
}
```

### Struct Usage

```nova
struct Point {
    x: float,
    y: float,
}

impl Point {
    fn new(x: float, y: float) -> Point {
        return Point { x: x, y: y };
    }

    fn distance(&self, other: &Point) -> float {
        let dx = self.x - other.x;
        let dy = self.y - other.y;
        return sqrt(dx * dx + dy * dy);
    }
}

fn main() -> void {
    let p1 = Point::new(0.0, 0.0);
    let p2 = Point::new(3.0, 4.0);
    let dist = p1.distance(&p2);
    println("Distance: " + dist);  // 5.0
}
```

---

## Future Features

The following features are planned for future releases:

1. **Pattern Matching** - Advanced pattern matching with guards
2. **Traits** - Rust-like trait system for polymorphism
3. **Generics** - Full generic type system
4. **Error Handling** - Built-in Result type and error propagation
5. **Modules** - Module system for code organization
6. **Concurrency** - Safe concurrency primitives
7. **Metaprogramming** - Compile-time reflection and macros

---

## Version History

### v0.2.0 (Current)
- Complete compiler frontend
- IR generation
- C code generation
- Comprehensive test suite

### v0.1.0
- Initial release
- Lexer and parser
- Basic semantic analysis

---

## References

- [Rust Language Reference](https://doc.rust-lang.org/reference/)
- [Swift Programming Language](https://swift.org/documentation/)
- [Kotlin Language Specification](https://kotlinlang.org/docs/reference/)
- [C11 Standard](https://en.cppreference.com/w/c/11)

---

**Note**: This specification is a work in progress. Features marked as "Future" are not yet implemented. The language syntax and semantics may evolve based on implementation experience and user feedback.
