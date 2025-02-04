#include "util.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

/**
 * @brief Read the expression and values from the command line arguments.
 */
void readExpressionAndValues(int argc, char* argv[], std::string& expression, std::unordered_map<char, bool>& values) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <expression> <values>\n";
        std::cerr << "Example: " << argv[0] << " \"a + b * c + a * d * e\" F T T T F\n";
        exit(1);
    }

    // Read the expression and remove whitespace
    expression = removeWhitespace(argv[1]);

    // Read variable values
    size_t varIndex = 0;
    for (int i = 2; i < argc; ++i) {
        std::string valueStr = removeWhitespace(argv[i]);
        char var = 'a' + varIndex; // Variables are a, b, c, ...
        values[var] = (valueStr[0] == 'T');
        varIndex++;
    }
}

/**
 * @brief Remove all whitespace characters from a string.
 * @param str The input string.
 * @return A new string with all whitespace characters removed.
 */
std::string removeWhitespace(const std::string& str) {
    std::string result;
    std::remove_copy_if(str.begin(), str.end(), std::back_inserter(result), [](unsigned char c) { return std::isspace(c); });
    return result;
}

/**
 * @brief Explode a string into a vector of strings based on multiple delimiters.
 * @param str The input string.
 * @param delimiters The characters to split the string by.
 * @return A vector of strings split by the delimiters.
 */
std::vector<std::string> explode(const std::string& str, const std::string& delimiters) {
    std::vector<std::string> tokens;
    std::string token;
    for (char c : str) {
        if (delimiters.find(c) != std::string::npos) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(std::string(1, c));
        } else {
            token += c;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}