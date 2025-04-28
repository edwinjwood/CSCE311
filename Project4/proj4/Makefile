# Compiler & Flags
CXX := g++
CXXFLAGS := -std=c++17  # C++ version
CXXFLAGS += -Wall -Wextra -pedantic  # generate all warnings
CXXFLAGS += -g  # add GDB instrumentation
CXXFLAGS += -I include -I ../sync/include -I ../util/include 
CXXFLAGS += -MMD  # generate .d file with source and header dependencies
CXXFLAGS += -MP  # add phony targets to avoid errors if headers are deleted

# Build directories
BUILD_DIR := build

# Source files
SRC := src/n_sat_solver.cc
SYNC_SRC := ../sync/src/thread_mutex.cc
PARSER_SRC := ../util/src/bool_expr_parser.cc

# Object and dependency files in build/
OBJS := $(addprefix $(BUILD_DIR)/, $(notdir $(SRC:.cc=.o))) \
        $(addprefix $(BUILD_DIR)/, $(notdir $(SYNC_SRC:.cc=.o))) \
        $(addprefix $(BUILD_DIR)/, $(notdir $(PARSER_SRC:.cc=.o)))

# Map .d dependency files to object files
DEPS := $(OBJS:.o=.d) $(SERVER_OBJS:.o=.d)

# Final executables
EXEC := n-sat-solver

# Default target
all: $(EXEC) $(SERVER_EXEC)

# Build executables
$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@

# Build .o files inside build/
$(BUILD_DIR)/%.o: ../sync/src/%.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: ../util/src/%.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: src/%.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(EXEC) $(SERVER_EXEC)

# Include dependency files (.d). Only available in GNU Make. The '-' makes this
# fail silently. Works just like #include from C/C++ in that it "copies" the
# dependency files' contents here in the makefile.
-include $(DEPS)

.PHONY: all clean
