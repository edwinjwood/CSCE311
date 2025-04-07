# Banker's Algorithm Resource Management Implementation

## Purpose

This project implements the Banker's Algorithm for deadlock avoidance in resource allocation systems. The algorithm ensures that resources are allocated in a way that keeps the system in a safe state, where all processes can eventually complete without deadlock. This implementation demonstrates thread synchronization using mutexes, resource management, and the application of the Banker's Algorithm in a multi-threaded environment.

## Project Directory Structure

```bash
Project3/
├── Makefile                           # Build configuration file
├── proj3/                             # Implementation directory
│   ├── src/
│   │   ├── bankers_resource_manager.cc  # Banker's Algorithm implementation
│   │   ├── bankers_thread.cc            # Thread implementation for testing
│   │   └── thread_mutex.cc              # Thread synchronization implementation
│   ├── include/
│   │   ├── bankers_resource_manager.h   # Resource manager header
│   │   └── thread_mutex.h               # Thread synchronization header
│   └── bin/                           # Build directory (generated during build)
│       └── bankers_test                # Test executable
└── README.md                          # This file
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

- **Format**: `./bankers-threads <thread_count> "<initial_resources>" "<max_demands_1>" "<max_demands_2>" ...`

- **Example**: `./bankers-threads 7 "5 5 5" "2 3 4" "1 5 5" "2 3 3" "5 5 1"`

Where:

- `thread_count`: Number of threads/processes to create in the simulation
- `initial_resources`: Available resources at program start (e.g., "5 5 5" means 5 units of each of the three resource types)
- `max_demands_X`: Maximum resource demands for each process (each set in quotes)

This example creates 7 threads with the specified initial resources (5 units of each type) and various maximum resource demands for different processes.

### Example Output

```bash
Thread 0 requested: {0 0 3}
   Need: {2 3 4}
   Available: {5 5 5}
   Safe. Request allocated. Order: {P0 P1 P2 P3}
   Available: {5 5 2}
   Need: {2 3 1}
Thread 1 requested: {3 1 0}
   Need: {5 1 1}
   Available: {5 5 2}
   Safe. Request allocated. Order: {P0 P1 P2 P3}
   Thread 0 releasing all resources
   Updated Available: {5 5 5}
```

## Implementation Details

The implementation demonstrates several key concepts:

1. **Thread Safety**: All operations on shared data are protected by mutexes.
2. **Resource Allocation**: The Banker's Algorithm is used to safely allocate resources.
3. **Deadlock Avoidance**: The implementation prevents deadlock by ensuring the system stays in a safe state.
4. **Resource Tracking**: The system tracks available resources, maximum needs, current allocations, and remaining needs.
5. **Safe State Checking**: The safety algorithm simulates resource allocation to determine if a sequence exists where all processes can complete.

## Error Handling

The implementation includes robust error handling:

1. Validates input parameters for all public methods
2. Rejects requests that exceed a process's maximum need
3. Denies requests when insufficient resources are available
4. Prevents unsafe allocations that could lead to deadlock
5. Ensures processes cannot release more resources than they hold
