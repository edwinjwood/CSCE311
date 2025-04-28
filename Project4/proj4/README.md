# N-SAT Solver Implementation

## Purpose

This project implements a parallel N-SAT solver that evaluates boolean expressions to determine their satisfiability. The implementation uses memory-mapped file I/O for efficient expression loading, multi-threaded processing with pthreads for parallel evaluation, and thread synchronization with mutexes to ensure proper resource sharing.

## Project Directory Structure

```bash
proj4/
+-- include/
| +-- n_sat_solver.h
|
+-- src/
| +-- n_sat_solver.cc
|
+-- README.md
```
## Files

### Header Files

- `include/bankers_resource_manager.h`:
  - **Purpose**: Declares the `BankersResourceManager` class.
  - **Details**: Defines the interface for resource allocation and deallocation using the Banker's Algorithm. Includes methods for requesting resources, releasing resources, and checking if the system is in a safe state.

### Source Files

- `src/bankers_resource_manager.cc`:
  - **Purpose**: Implements the Banker's Algorithm.
  - **Details**: Contains the implementation of resource allocation, deallocation, and safety checking. Uses thread synchronization to ensure thread safety during resource manipulation.

## Understanding the Banker's Algorithm

The Banker's Algorithm prevents deadlocks by keeping track of:

1. **Available Resources**: The number of available resources of each type
2. **Maximum Need**: The maximum demand of each process
3. **Current Allocation**: Resources currently allocated to each process
4. **Remaining Need**: Resources still needed by each process (Maximum - Allocation)

When a process requests resources, the algorithm:

1. Checks if the request exceeds the process's declared maximum need
2. Checks if enough resources are available
3. Tentatively grants the request
4. Checks if the resulting state is safe
5. If safe, confirms the allocation; if unsafe, reverts and denies the request

A state is considered safe if there exists a sequence in which all processes can complete execution.

## How to Compile and Run

To build the project, you can use the provided `Makefile`. Here are the steps:

1. Navigate to the project directory.
2. Run the following commands to build the project:
   `make`
3. Run the program:

- **Format**: `./n-sat-solver <number_of_threads> <input_file> <number_of_variables>`

- **Example**: `./n-sat-solver 4 dat/dnf_exprs_16_10.txt 16`

Usage Details:

- `number_of_threads`: Number of threads to use for parallel processing
- `input_file`: Path to the file containing boolean expressions
- `number_of_variables`: Number of variables in the boolean expressions

This example runs the solver with 4 threads, evaluates the expressions in the specified file, and assumes 16 variables in each expression.

### Example Output

```bash
Thread  Sat  Unsat
     0    1     2
     1    1     1
     2    1     1
     3    2     1
 Total    5     5
```

## Implementation Details

The implementation demonstrates several key concepts:

1. **Memory-Mapped Files**:
   - Uses `mmap` for efficient file I/O instead of traditional stream reading
   - Properly handles file opening, mapping, and unmapping with required commenting
   - Processes file content line-by-line to extract expressions

2. **Multi-threading with Pthreads**:
   - Creates and manages threads using the pthread library
   - Properly commented thread creation and joining points as required
   - Implements a thread-safe approach to parallel expression evaluation

3. **Thread Synchronization**:
   - Uses mutexes to protect shared resources and prevent race conditions
   - Implements RAII-style mutex guards for exception safety
   - Contains properly commented critical sections

4. **Expression Distribution**:
   - Distributes expressions among threads using a round-robin approach
   - Ensures balanced workload regardless of expression complexity
   - Collects and aggregates results from all threads

5. **Boolean Expression Evaluation**:
   - Uses the SatSolver function to determine expression satisfiability
   - Tracks SAT/UNSAT counts per thread and overall
   - Efficiently processes expressions with the specified number of variables

## Error Handling

The implementation includes robust error handling:

1. Validates command-line arguments (number of threads, input file, number of variables)
2. Handles file opening and memory mapping failures with proper error messages
3. Manages thread creation and joining errors to prevent resource leaks
4. Ensures proper cleanup of resources (unmapping files, closing file descriptors)
5. Uses synchronization to prevent race conditions during thread execution
