#include "util.h"
#include <iostream>

/**
 * @brief Read the expression and values from the command line arguments.
 */
void readExpressionAndValues(int argc, char* argv[], std::string& expression, std::unordered_map<char, bool>& values) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <expression> <values>\n";
        std::cerr << "Example: " << argv[0] << " \"a + b * c + a * d * e\" F T T T F\n";
        exit(1);
    }

    // Read the expression
    expression = argv[1];

    // Read variable values
    size_t varIndex = 0;
    for (size_t i = 2; i < argc; ++i) {
        char var = 'a' + varIndex; // Variables are a, b, c, ...
        values[var] = (argv[i][0] == 'T');
        varIndex++;
    }
}