#include "boolean_expression_parser.h"
#include <iostream>

/**
 * @brief Construct a new Boolean Expression Parser:: Boolean Expression Parser object
 * 
 */
BooleanExpressionParser::BooleanExpressionParser(const std::string& expression,
                                                 const std::unordered_map<char, bool>& values)
    : expression(expression), values(values), currentIndex(0), error(false) {}

/**
 * @brief Parse the boolean expression
 * @return true if the expression is valid
 * @return false if the expression is invalid
 */
bool BooleanExpressionParser::parse() {
    bool result = parseExpr();
    if (error || currentIndex != expression.size()) {
        reportError("Unexpected tokens after parsing.");
        return false; // Indicate parse error
    }
    return result;
}

/**
 * @brief Check if there is an error
 * @return true if there is an error
 * @return false if there is no error
 */
bool BooleanExpressionParser::hasError() const {
    return error;
}

char BooleanExpressionParser::currentChar() const {
    if (currentIndex < expression.size()) {
        return expression[currentIndex];
    }
    return '\0';
}

void BooleanExpressionParser::consume() {
    if (currentIndex < expression.size()) {
        currentIndex++;
    }
}

bool BooleanExpressionParser::parseExpr() {
    bool result = parseTerm(); // Parse AND expressions first
    while (currentChar() == '+') {
        consume(); // Consume the '+'
        result = result || parseTerm();
        if (error) return false; // Halt on error
    }
    return result;
}

bool BooleanExpressionParser::parseTerm() {
    bool result = parseFactor(); // Parse primary values first
    while (currentChar() == '*') {
        consume(); // Consume the '*'
        result = result && parseFactor();
        if (error) return false; // Halt on error
    }
    return result;
}

bool BooleanExpressionParser::parseFactor() {
    char token = currentChar();
    std::cout << "Parsing factor: " << token << std::endl; // Debug output

    if (std::isalpha(token)) {
        consume();
        if (values.find(token) != values.end()) {
            return values.at(token);
        } else {
            reportError("Undefined variable: " + std::string(1, token));
            return false;
        }
    } else if (token == 'T' || token == 'F') {
        consume();
        return token == 'T';
    } else if (token == '(') {
        consume(); // Consume the '('
        bool result = parseExpr();
        if (currentChar() == ')') {
            consume(); // Consume the ')'
            return result;
        } else {
            reportError("Expected closing parenthesis");
            return false;
        }
    } else {
        reportError("Unexpected token: " + std::string(1, token));
        return false;
    }
}

/**
 * @brief Report an error
 * @param message the error message
 */
void BooleanExpressionParser::reportError(const std::string& message) {
    if (!error) {
        std::cerr << "Error: " << message << std::endl;
        error = true;
    }
}