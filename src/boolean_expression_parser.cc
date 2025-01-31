#include "boolean_expression_parser.h"
#include <iostream>

/**
 * @brief Construct a new Boolean Expression Parser:: Boolean Expression Parser object
 * 
 */
BooleanExpressionParser::BooleanExpressionParser(const std::string& expression,
                                                 const std::unordered_map<char, bool>& values)
    : expression(expression), values(values), currentIndex(0), error(0) {
    // empty
}

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

/**
 * @brief Get the current character
 * @return char the current character
 */
char BooleanExpressionParser::currentChar() const {
    if (currentIndex < expression.size()) {
        return expression[currentIndex];
    }
    return '\0';
}

/**
 * @brief Consume the current character
 */
void BooleanExpressionParser::consume() {
    if (currentIndex < expression.size()) {
        currentIndex++;
    }
}

/**
 * @brief Parse the expression
 * @return true if the expression is valid
 * @return false if the expression is invalid
 */
bool BooleanExpressionParser::parseExpr() {
    bool result = parseTerm(); // Parse AND expressions first
    while (currentChar() == '+') {
        consume(); // Consume the '+'
        result = result || parseTerm();
        if (error) return false; // Halt on error
    }
    return result;
}

/**
 * @brief Parse the term
 * @return true if the term is valid
 * @return false if the term is invalid
 */
bool BooleanExpressionParser::parseTerm() {
    bool result = parseFactor(); // Parse primary values first
    while (currentChar() == '*') {
        consume(); // Consume the '*'
        result = result && parseFactor();
        if (error) return false; // Halt on error
    }
    return result;
}

/**
 * @brief Parse the factor
 * @return true if the factor is valid
 * @return false if the factor is invalid
 */
bool BooleanExpressionParser::parseFactor() {
    char token = currentChar();
    if (std::isalpha(token)) {
        consume();
        if (values.find(token) != values.end()) {
            return values.at(token);
        } else {
            reportError("Undefined variable: " + std::string(1, token));
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