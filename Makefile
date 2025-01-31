# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Iinclude -Wall -Wextra -pedantic -g
	# use C++ 17, add include dir to include<>, all warnings, add debug info

# Directories
SRC_DIR := src
BIN_DIR := bin
BUILD_DIR := build
INCLUDE_DIR := include

# Executable name
TARGET := $(BIN_DIR)/evaluate-boolean

# Source files and corresponding object files
SRC_FILES := $(wildcard $(SRC_DIR)/*.cc)  # $(wildcard PATTERN)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cc, $(BUILD_DIR)/%.o, $(SRC_FILES))
		# $(patsubst PATTERN, REPLACEMENT, TEXT)

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o $@

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure the build & bin directories exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)


# Run some tests
test1: $(TARGET)
	./$(TARGET) "a + b" F T

test2: $(TARGET)
	./$(TARGET) "a * b + c" T F T

test3: $(TARGET)
	./$(TARGET) "a + b * c + d" F F T T

test4: $(TARGET)
	./$(TARGET) "a + b * c + d" F T T T

test5: $(TARGET)
	./$(TARGET) "a + b * c + d * e + f + g * h + i * j + k + l * m + n + o * p + q + r * s + t + u + v + w * x * y + z" F F F F F F F F F F F F F F F F F F F F F F T T T F

# Run all tests
test: test1 test2 test3 test4

# Clean up build directory
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean
