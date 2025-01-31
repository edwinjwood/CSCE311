#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <unordered_map>

/**
 * @brief Read the expression and values from the command line arguments.
 */
void readExpressionAndValues(int argc, char* argv[], std::string& expression, std::unordered_map<char, bool>& values);

#endif // UTIL_H