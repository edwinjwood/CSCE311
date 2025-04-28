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

- `include/n_sat_solver.h`:
  - **Purpose**: Declares the `NSatSolver` class and supporting structures.
  - **Details**: Defines the interface for loading expressions, solving them in parallel, and managing thread synchronization. Includes the ThreadData and ThreadStats structures for thread management.

### Source Files

- `src/n_sat_solver.cc`:
  - **Purpose**: Implements the N-SAT solver with memory-mapped file I/O and multi-threading.
  - **Details**: Contains the implementation for loading expressions using memory mapping, distributing work among threads, solving expressions in parallel, and collecting results. Includes proper thread synchronization and memory management.

## Understanding the N-SAT Problem

The N-SAT problem deals with determining whether a boolean expression with N variables is satisfiable (has at least one assignment that makes it true). This implementation:

1. Loads expressions from a file using memory-mapped I/O
2. Distributes these expressions among multiple threads
3. Evaluates each expression to determine if it's satisfiable (SAT) or unsatisfiable (UNSAT)
4. Collects the results and presents statistics per thread and overall

## How to Compile and Run

To build the project, you can use the provided `Makefile`. Here are the steps:

1. Navigate to the project directory.
2. Run the following commands to build the project: `make`
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
