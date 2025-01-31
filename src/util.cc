#include "util.h"
#include <iostream>
#include <sstream>
#include <vector>

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
    std::vector<std::string> tokens = explode(argv[2], ' ');
    size_t varIndex = 0;
    for (const auto& token : tokens) {
        char var = 'a' + varIndex; // Variables are a, b, c, ...
        values[var] = (token == "T");
        varIndex++;
    }
}

/**
 * @brief Explode a string into a vector of strings based on a delimiter.
 */
std::vector<std::string> explode(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}