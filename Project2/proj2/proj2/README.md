# Boolean Expression Client-Server Application

## Purpose

This project implements a client-server system that evaluates Boolean expressions using domain sockets for IPC (Inter-Process Communication). The server loads Boolean expressions from a file, receives truth values from clients, evaluates the expressions, and returns the results. The client sends truth values to the server and displays the evaluation results. This demonstrates practical IPC communication through UNIX domain sockets.

## Project Directory Structure

```bash
Project2/
├── Makefile                    # Build configuration file
|
├── proj2/                      # Client-server implementation
│   ├── src/
│   │   ├── bool_expr_client.cc # Client implementation
│   │   ├── bool_expr_server.cc # Server implementation
│   │
│   ├── include/
│   │   ├── bool_expr_client.h  # Client header
│   │   ├── bool_expr_server.h  # Server header
│   │
│   └── bin/                    # Build directory (generated during build)
│       ├── bool-expr-client    # Client executable
│       ├── bool-expr-server    # Server executable
|
├── util/                       # Boolean expression parser utilities
│   ├── src/
│   │   ├── bool_expr_parser.cc # Parser implementation
│   │
│   ├── include/
│   │   ├── bool_expr_parser.h  # Parser header
|
├── ipc/                        # IPC utilities
│   ├── src/
│   │   ├── domain_socket.cc    # Domain socket implementation
│   │
│   ├── include/
│   │   ├── domain_socket.h     # Domain socket header
|
└── README.md                   # This file
```

## Files

### Header Files

- `include/bool_expr_client.h`:
  - **Purpose**: Declares the `BooleanExpressionClient` class.
  - **Details**: Defines the interface for connecting to the server, sending truth values, and receiving evaluation results. Inherits from `DomainSocketClient` to handle communication over domain sockets.

- `include/bool_expr_server.h`:
  - **Purpose**: Declares the `BooleanExpressionServer` class.
  - **Details**: Defines the interface for loading expressions, handling client connections, and evaluating expressions. Inherits from `DomainSocketServer` to handle incoming client connections.

- `util/include/bool_expr_parser.h`:
  - **Purpose**: Declares the `BooleanExpressionParser` class and utility functions.
  - **Details**: Contains declarations for parsing and evaluating Boolean expressions, including `Parse()`, `HasError()`, and `Error()` methods. Also includes utility functions like `Explode()` and `BuildMap()` for processing expressions and truth values.

- `ipc/include/domain_socket.h`:
  - **Purpose**: Declares base classes for domain socket communication.
  - **Details**: Defines `DomainSocketServer` and `DomainSocketClient` classes that handle the low-level socket operations, including connection establishment, data transmission, and connection teardown.

### Source Files

- `proj2/src/bool_expr_client.cc`:
  - **Purpose**: Implements the client application.
  - **Details**: Connects to the server, formats and sends truth values, receives and parses evaluation results, and displays the output to the user. Handles errors gracefully and provides a clean shutdown mechanism.

- `proj2/src/bool_expr_server.cc`:
  - **Purpose**: Implements the server application.
  - **Details**: Loads and pre-processes expressions from a file, accepts client connections, extracts truth values from client messages, evaluates expressions using the parser, and returns formatted results to clients.

- `util/src/bool_expr_parser.cc`:
  - **Purpose**: Implements the Boolean expression parser.
  - **Details**: Contains the logic for parsing and evaluating Boolean expressions using a recursive descent parser. Implements utility functions like `Explode()` for processing strings and `BuildMap()` for creating truth value mappings.

- `ipc/src/domain_socket.cc`:
  - **Purpose**: Implements the domain socket communication.
  - **Details**: Handles socket creation, binding, listening, accepting connections, reading, writing, and cleanup operations. Provides robust error handling for network operations.

## How to Compile and Run

To build the project, you can use the provided `Makefile`. Here are the steps:

1. Navigate to the project directory.
2. Run the following commands to build the project:

   ```sh
   make
   ```

3. Start the server:
   - **Argument Format**: `./bin/bool-expr-server <expressions_file> <socket_name> <unit_separator> <eot>`

   ```sh
   ./bin/bool-expr-server dat/expr_25k.txt bool_expr_sock ":" "."
   ```

4. In a separate terminal, run the client:
   - **Argument Format**: `./bin/bool-expr-client <socket_name> <truth_values>`

   ```sh
   ./bin/bool-expr-client bool_expr_sock T F T F
   ```

### Example Output

**Server Output:**

   ```sh
Client connected
   14B sent, 7B received
   ```

**Client Output:**

   ```sh
BoolExprClient connecting...
Finished with 14B received, 7B sent.
Results
True Evaluations: 3
False Evaluations: 5
Could Not Evaluate: 0
   ```
