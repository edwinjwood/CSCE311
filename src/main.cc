#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "boolean_expression_parser.h"
#include "util.h"

/**
 * Read the expression and variable values from the command line arguments.
 * 
 */
int main(int argc, char* argv[]) {
    std::string expression;
    std::unordered_map<char, bool> values;

    // Read the expression and variable values
    readExpressionAndValues(argc, argv, expression, values);

    // Create and use the parser
    BooleanExpressionParser parser(expression, values);
    bool result = parser.parse();

    if (parser.hasError()) {
        return 1;
    }

    // Output the result
    std::cout << "Result of expression is: " << (result ? "True" : "False") << std::endl;
    return 0;
}