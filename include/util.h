#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <unordered_map>
#include <vector>

void readExpressionAndValues(int argc, char* argv[], std::string& expression, std::unordered_map<char, bool>& values);
std::string removeWhitespace(const std::string& str);
std::vector<std::string> explode(const std::string& str, const std::string& delimiters);

#endif // UTIL_H