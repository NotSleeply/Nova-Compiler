# Nova Compiler Makefile
# ======================
# Build system for the Nova Compiler frontend

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -g
LDFLAGS = 
INCLUDES = -Iinclude

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# Source files
SOURCES = $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# Target executable
TARGET = $(BIN_DIR)/novac

# Test files
TEST_DIR = tests
TEST_SOURCES = $(wildcard $(TEST_DIR)/**/*.cpp)
TEST_TARGET = $(BIN_DIR)/nova_tests

# Example files
EXAMPLE_DIR = examples

# Default target
.PHONY: all
all: $(TARGET)

# Create directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/lexer
	@mkdir -p $(BUILD_DIR)/parser
	@mkdir -p $(BUILD_DIR)/ast
	@mkdir -p $(BUILD_DIR)/semantic

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Build main executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "✓ Build complete: $@"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "✓ Clean complete"

# Run tests
.PHONY: test
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Build tests
$(TEST_TARGET): $(TEST_SOURCES) $(filter-out $(BUILD_DIR)/main.o,$(OBJECTS)) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)

# Run an example
.PHONY: run-example
run-example: $(TARGET)
	./$(TARGET) $(EXAMPLE_DIR)/hello.nv

# Format code (requires clang-format)
.PHONY: format
format:
	@find $(SRC_DIR) $(INCLUDE_DIR) -name '*.cpp' -o -name '*.h' | xargs clang-format -i
	@echo "✓ Code formatted"

# Static analysis (requires cppcheck)
.PHONY: check
check:
	cppcheck --enable=all --std=c++17 -I $(INCLUDE_DIR) $(SRC_DIR)

# Generate documentation (requires doxygen)
.PHONY: docs
docs:
	doxygen Doxyfile
	@echo "✓ Documentation generated in docs/"

# Install to system (requires root)
.PHONY: install
install: $(TARGET)
	@cp $(TARGET) /usr/local/bin/novac
	@echo "✓ Installed to /usr/local/bin/novac"

# Uninstall from system
.PHONY: uninstall
uninstall:
	@rm -f /usr/local/bin/novac
	@echo "✓ Uninstalled novac"

# Debug build
.PHONY: debug
debug: CXXFLAGS += -DDEBUG -O0
debug: clean $(TARGET)

# Release build
.PHONY: release
release: CXXFLAGS += -O3 -DNDEBUG
release: clean $(TARGET)

# Watch for changes and rebuild (requires inotifywait on Linux)
.PHONY: watch
watch:
	@while true; do \
		inotifywait -q -e modify -r $(SRC_DIR) $(INCLUDE_DIR); \
		$(MAKE) clean && $(MAKE); \
	done

# Show help
.PHONY: help
help:
	@echo "Nova Compiler Build System"
	@echo "=========================="
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all          Build the compiler (default)"
	@echo "  clean        Remove build files"
	@echo "  test         Run tests"
	@echo "  run-example  Run with example file"
	@echo "  format       Format code with clang-format"
	@echo "  check        Run static analysis"
	@echo "  docs         Generate documentation"
	@echo "  install      Install to system"
	@echo "  uninstall    Uninstall from system"
	@echo "  debug        Build with debug flags"
	@echo "  release      Build with optimization"
	@echo "  watch        Watch and rebuild on changes"
	@echo "  help         Show this help message"

# Dependencies
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp \
                     $(INCLUDE_DIR)/lexer/token.h \
                     $(INCLUDE_DIR)/lexer/lexer.h \
                     $(INCLUDE_DIR)/parser/parser.h \
                     $(INCLUDE_DIR)/ast/ast.h \
                     $(INCLUDE_DIR)/semantic/semantic_analyzer.h \
                     $(INCLUDE_DIR)/ir/ir.h \
                     $(INCLUDE_DIR)/ir/ir_generator.h
