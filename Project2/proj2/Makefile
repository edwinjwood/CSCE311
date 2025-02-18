# Compiler & Flags
CXX := g++
CXXFLAGS := -std=c++17  # C++ version
CXXFLAGS += -Wall -Wextra -pedantic  # generate all warnings
CXXFLAGS += -g  # add GDB instrumentation
CXXFLAGS += -I include -I ../ipc/include -I ../util/include 
CXXFLAGS += -MMD  # generate .d file with source and header dependencies
CXXFLAGS += -MP  # add phony targets to avoid errors if headers are deleted

# Build directories
BUILD_DIR := build

# Source files
CLIENT_SRC := src/bool_expr_client.cc
SERVER_SRC := src/bool_expr_server.cc
IPC_SRC := ../ipc/src/domain_socket.cc
SEMAPHORE_SRC := ../util/src/thread_sem_manager.cc
PARSER_SRC := ../util/src/bool_expr_parser.cc

# Object and dependency files in build/
CLIENT_OBJS := $(addprefix $(BUILD_DIR)/, $(notdir $(CLIENT_SRC:.cc=.o))) \
               $(addprefix $(BUILD_DIR)/, $(notdir $(IPC_SRC:.cc=.o))) \

SERVER_OBJS := $(addprefix $(BUILD_DIR)/, $(notdir $(SERVER_SRC:.cc=.o))) \
               $(addprefix $(BUILD_DIR)/, $(notdir $(IPC_SRC:.cc=.o))) \
               $(addprefix $(BUILD_DIR)/, $(notdir $(PARSER_SRC:.cc=.o))) \
               $(addprefix $(BUILD_DIR)/, $(notdir $(SEMAPHORE_SRC:.cc=.o)))

# Map .d dependency files to object files
DEPS := $(CLIENT_OBJS:.o=.d) $(SERVER_OBJS:.o=.d)

# Final executables
CLIENT_EXEC := bool-expr-client
SERVER_EXEC := bool-expr-server

# Default target
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Build executables
$(CLIENT_EXEC): $(CLIENT_OBJS)
	$(CXX) $(CLIENT_OBJS) -o $@

$(SERVER_EXEC): $(SERVER_OBJS)
	$(CXX) $(SERVER_OBJS) -o $@

# Build .o files inside build/
$(BUILD_DIR)/%.o: ../ipc/src/%.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: ../util/src/%.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: src/%.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(CLIENT_EXEC) $(SERVER_EXEC)

# Include dependency files (.d). Only available in GNU Make. The '-' makes this
# fail silently. Works just like #include from C/C++ in that it "copies" the
# dependency files' contents here in the makefile.
-include $(DEPS)

.PHONY: all clean
