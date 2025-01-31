# Boolean Expression Parser

## Purpose

The purpose of this project is to provide a Boolean expression parser that can evaluate expressions with variables and logical operators. The parser reads an expression and a set of variable values from the command line, parses the expression, and evaluates it based on the provided values. This can be useful for applications that require dynamic evaluation of logical expressions.

### Project Directory Structure

proj1/
├── Makefile                    # Build configuration file for make program
|
├── src/
│   ├── boolean_expression_parser.cc  # Source file for the parser logic
│   ├── main.cc                       # Main entry point to program
│   ├── util.cc                       # Source file for utility functions
|
├── include/
│   ├── boolean_expression_parser.h   # Header file for the parser logic
│   ├── util.h                        # Header file for utility declarations
|
├── bin/                        # Build directory (generated during build process)
│   ├── evaluate-boolean              # Main entry point of the program
|
├── build/                      # Compile directory (generated during build process)
│   ├── boolean_expression_parser.o   # object for the parser logic
│   ├── main.o                        # object for the+ main entry point to program
│   ├── util.o                        # object for utility functions
|
└── README.md                   # This file

### Files

- `include/boolean_expression_parser.h`: Header file for the `BooleanExpressionParser` class. It contains the class declaration and public interface for parsing and evaluating boolean expressions.
- `include/util.h`: Header file for utility functions. It contains declarations for functions that assist in reading expressions and variable values from command line arguments.
- `src/boolean_expression_parser.cc`: Implementation of the `BooleanExpressionParser` class. It contains the definitions of the methods declared in the header file, including the logic for parsing and evaluating boolean expressions.
- `src/util.cc`: Implementation of utility functions. It contains the definitions of the functions declared in the header file, including the logic for reading expressions and variable values from command line arguments.
- `src/main.cc`: Main program that uses the `BooleanExpressionParser` class and utility functions. It contains the `main` function, which is the entry point of the program. It reads the command line arguments, creates a `BooleanExpressionParser` object, and outputs the result of the parsed expression.

### How to Compile and Run

To build the project, you can use the provided `Makefile`. Here are the steps:

1. Navigate to the project directory.
2. Run the following commands to build and run the project:

   ```sh
   make
    
   ./bin/evaluate-boolean "<expression>" <values>

   ./bin/evaluate-boolean "a + b * c + a * d * e" F T T T F

### Example Output

    ```sh
    Result of expression is: True