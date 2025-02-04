#ifndef BOOLEAN_EXPRESSION_PARSER_H
#define BOOLEAN_EXPRESSION_PARSER_H

#include <string>
#include <unordered_map>
#include <vector>

class BooleanExpressionParser {
public:
    BooleanExpressionParser(const std::string& expression,
                            const std::unordered_map<char, bool>& values);

    bool parse();
    bool hasError() const;

private:
    std::vector<std::string> tokens;
    const std::unordered_map<char, bool>& values;
    size_t tokenIndex;
    bool error;

    std::string currentToken() const;
    void consume();
    bool parseExpr();
    bool parseTerm();
    bool parseFactor();
    void reportError(const std::string& message);
};

#endif // BOOLEAN_EXPRESSION_PARSER_H