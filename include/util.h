#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <unordered_map>

void readExpressionAndValues(int argc, char* argv[], std::string& expression, std::unordered_map<char, bool>& values);
std::string removeWhitespace(const std::string& str);

#endif // UTIL_H