/**
 * @file catch.hpp
 * @brief Minimal test framework for Nova Compiler unit tests
 * 
 * This is a simplified test framework inspired by Catch2, designed for
 * educational purposes and to avoid external dependencies.
 * 
 * Usage:
 *   #define CATCH_CONFIG_MAIN
 *   #include "catch2/catch.hpp"
 *   
 *   TEST_CASE("Test name", "[tags]") {
 *       REQUIRE(expression);
 *       CHECK(expression);
 *   }
 */

#ifndef NOVA_CATCH_HPP
#define NOVA_CATCH_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <sstream>
#include <cmath>
#include <exception>

// ============================================================================
// Test Framework Configuration
// ============================================================================

namespace nova {
namespace test {

// Test result structure
struct TestResult {
    std::string name;
    std::string section;
    bool passed;
    std::string message;
    std::string file;
    int line;
};

// Test case structure
struct TestCase {
    std::string name;
    std::vector<std::string> tags;
    std::function<void()> function;
};

// Test runner singleton
class TestRunner {
public:
    static TestRunner& instance() {
        static TestRunner runner;
        return runner;
    }
    
    void addTest(const std::string& name, const std::vector<std::string>& tags, 
                 std::function<void()> function) {
        tests.push_back({name, tags, function});
    }
    
    int runTests() {
        int passed = 0;
        int failed = 0;
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "Running " << tests.size() << " test cases..." << std::endl;
        std::cout << "========================================\n" << std::endl;
        
        for (const auto& test : tests) {
            currentTest = test.name;
            currentSection = "";
            
            try {
                test.function();
                std::cout << "[PASS] " << test.name << std::endl;
                passed++;
            } catch (const TestFailureException& e) {
                std::cout << "[FAIL] " << test.name << std::endl;
                std::cout << "  " << e.what() << std::endl;
                failed++;
            } catch (const std::exception& e) {
                std::cout << "[ERROR] " << test.name << std::endl;
                std::cout << "  Unexpected exception: " << e.what() << std::endl;
                failed++;
            } catch (...) {
                std::cout << "[ERROR] " << test.name << std::endl;
                std::cout << "  Unknown exception" << std::endl;
                failed++;
            }
        }
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "Test Results:" << std::endl;
        std::cout << "  Passed: " << passed << std::endl;
        std::cout << "  Failed: " << failed << std::endl;
        std::cout << "  Total:  " << tests.size() << std::endl;
        std::cout << "========================================\n" << std::endl;
        
        return failed > 0 ? 1 : 0;
    }
    
    void setCurrentSection(const std::string& section) {
        currentSection = section;
    }
    
    void addResult(const TestResult& result) {
        results.push_back(result);
    }
    
private:
    TestRunner() = default;
    
    std::vector<TestCase> tests;
    std::vector<TestResult> results;
    std::string currentTest;
    std::string currentSection;
};

// Test failure exception
class TestFailureException : public std::exception {
public:
    TestFailureException(const std::string& msg, const std::string& file, int line)
        : message(msg), filename(file), lineNumber(line) {
        fullMessage = filename + ":" + std::to_string(lineNumber) + ": " + message;
    }
    
    const char* what() const noexcept override {
        return fullMessage.c_str();
    }
    
private:
    std::string message;
    std::string filename;
    int lineNumber;
    std::string fullMessage;
};

// Approximate comparison for floating-point numbers
class Approx {
public:
    explicit Approx(double value) : value(value), epsilon(0.001) {}
    
    friend bool operator==(double lhs, const Approx& rhs) {
        return std::abs(lhs - rhs.value) < rhs.epsilon;
    }
    
    friend bool operator==(const Approx& lhs, double rhs) {
        return rhs == lhs;
    }
    
    Approx& epsilon(double eps) {
        epsilon = eps;
        return *this;
    }
    
private:
    double value;
    double epsilon;
};

// Assertion macros
#define REQUIRE(expr) \
    do { \
        if (!(expr)) { \
            throw nova::test::TestFailureException( \
                "REQUIRE failed: " #expr, __FILE__, __LINE__); \
        } \
    } while (false)

#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            std::cout << "  CHECK failed: " << #expr << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
        } \
    } while (false)

#define REQUIRE_FALSE(expr) REQUIRE(!(expr))
#define CHECK_FALSE(expr) CHECK(!(expr))

#define REQUIRE_THROWS(expr) \
    do { \
        bool caught = false; \
        try { expr; } catch (...) { caught = true; } \
        if (!caught) { \
            throw nova::test::TestFailureException( \
                "REQUIRE_THROWS failed: " #expr " did not throw", __FILE__, __LINE__); \
        } \
    } while (false)

#define REQUIRE_THROWS_AS(expr, exception_type) \
    do { \
        bool caught = false; \
        try { expr; } catch (const exception_type&) { caught = true; } catch (...) {} \
        if (!caught) { \
            throw nova::test::TestFailureException( \
                "REQUIRE_THROWS_AS failed: " #expr " did not throw " #exception_type, \
                __FILE__, __LINE__); \
        } \
    } while (false)

#define REQUIRE_NOTHROW(expr) \
    do { \
        try { expr; } catch (...) { \
            throw nova::test::TestFailureException( \
                "REQUIRE_NOTHROW failed: " #expr " threw an exception", \
                __FILE__, __LINE__); \
        } \
    } while (false)

// Test case registration
#define TEST_CASE(name, tags) \
    void test_function_##__LINE__(); \
    namespace { \
        struct TestRegistrar_##__LINE__ { \
            TestRegistrar_##__LINE__() { \
                nova::test::TestRunner::instance().addTest( \
                    name, {tags}, test_function_##__LINE__); \
            } \
        } test_registrar_##__LINE__; \
    } \
    void test_function_##__LINE__()

// Section macro
#define SECTION(name) \
    nova::test::TestRunner::instance().setCurrentSection(name);

} // namespace test
} // namespace nova

// Main function
#ifdef CATCH_CONFIG_MAIN
int main() {
    return nova::test::TestRunner::instance().runTests();
}
#endif

#endif // NOVA_CATCH_HPP
