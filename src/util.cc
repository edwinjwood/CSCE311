#include "util.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <unordered_set>

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

    // Extract unique variables from the expression in order of appearance
    std::vector<char> variables;
    std::unordered_set<char> seen;
    for (char c : expression) {
        if (std::isalpha(c) && seen.find(c) == seen.end()) {
            variables.push_back(c);
            seen.insert(c);
        }
    }

    // Ensure the number of provided values matches the number of variables
    if (variables.size() != static_cast<size_t>(argc - 2)) {
        std::cerr << "Error: Number of provided values does not match the number of variables in the expression.\n";
        exit(1);
    }

    // Map values directly to variables in order of appearance
    for (size_t i = 0; i < variables.size(); ++i) {
        std::string valueStr = removeWhitespace(argv[i + 2]);
        values[variables[i]] = (valueStr[0] == 'T');
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