/**
 * @file catch.hpp - Minimal test framework for Nova Compiler
 */

#ifndef NOVA_CATCH_HPP
#define NOVA_CATCH_HPP

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cmath>
#include <exception>

namespace nova {
namespace test {

struct TestCase {
    std::string name;
    std::function<void()> func;
};

class TestRunner {
    static TestRunner& inst() { static TestRunner r; return r; }
public:
    static void add(const char* n, std::function<void()> f) { 
        inst().tests.push_back({n, f}); 
    }
    static int runAll() {
        auto& t = inst().tests;
        int ok = 0, fail = 0;
        for (auto& tc : t) {
            try { tc.func(); ok++; std::cout << "[PASS] " << tc.name << "\n"; }
            catch (std::exception& e) { fail++; std::cout << "[FAIL] " << tc.name << ": " << e.what() << "\n"; }
        }
        std::cout << "\nResults: " << ok << " passed, " << fail << " failed, " << t.size() << " total\n";
        return fail ? 1 : 0;
    }
private:
    TestRunner() {}
    std::vector<TestCase> tests;
};

class FailEx : public std::exception {
    std::string m;
public:
    FailEx(const std::string& s) : m(s) {}
    const char* what() const noexcept override { return m.c_str(); }
};

class Approx {
    double v; double eps;
public:
    Approx(double x) : v(x), eps(0.001) {}
    friend bool operator==(double l, const Approx& r) { return (l-r.v)<r.eps && (l-r.v)>-r.eps; }
    friend bool operator==(const Approx& l, double r) { return r==l; }
    Approx& setEps(double e) { eps=e; return *this; }
};

}
}

#define REQUIRE(x) do { if(!(x)) throw nova::test::FailEx(std::string("REQUIRE failed: ") + #x); } while(0)
#define CHECK(x) do { if(!(x)) std::cout << "  CHECK failed: " #x "\n"; } while(0)
#define REQUIRE_FALSE(x) REQUIRE(!(x))
#define CHECK_FALSE(x) CHECK(!(x))
#define REQUIRE_THROWS(x) do{bool _c=false; try{(x);}catch(...){_c=true;}if(!_c) throw nova::test::FailEx("REQUIRE_THROWS failed");}while(0)
#define REQUIRE_NOTHROW(x) try{(x);}catch(...){throw nova::test::FailEx("REQUIRE_NOTHROW failed");}
#define APPROX(x) nova::test::Approx(x)
#define REQUIRE_THROWS_AS(expr, exctype) \
    do { bool _caught = false; \
         try { (expr); } catch (const exctype&) { _caught = true; } catch (...) {} \
         if (!_caught) throw nova::test::FailEx(std::string("REQUIRE_THROWS_AS failed: expected ") + #exctype); } while(0)

// Each TEST_CASE gets unique name via __LINE__ combined in namespace
#define TC_CONCAT2(a,b) a##b
#define TC_CONCAT(a,b) TC_CONCAT2(a,b)

#define TEST_CASE(name, tags) \
    void TC_CONCAT(test_func_,__LINE__)(); \
    namespace { \
        struct TC_CONCAT(test_reg_,__LINE__) { \
            TC_CONCAT(test_reg_,__LINE__)() { \
                nova::test::TestRunner::add((name), TC_CONCAT(test_func_,__LINE__)); \
            } \
        } TC_CONCAT(test_inst_,__LINE__); \
    } \
    void TC_CONCAT(test_func_,__LINE__)()

#ifdef CATCH_CONFIG_MAIN
int main() { return nova::test::TestRunner::runAll(); }
#endif

#endif // NOVA_CATCH_HPP
