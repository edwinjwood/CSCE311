#include "boolean_expression_parser.h"
#include "util.h"
#include <iostream>

/**
 * @brief Construct a new Boolean Expression Parser:: Boolean Expression Parser object
 * 
 */
BooleanExpressionParser::BooleanExpressionParser(const std::string& expression,
                                                 const std::unordered_map<char, bool>& values)
    : values(values), tokenIndex(0), error(false) {
    tokens = explode(expression, "+*()");
}

/**
 * @brief Parse the boolean expression
 * @return true if the expression is valid
 * @return false if the expression is invalid
 */
bool BooleanExpressionParser::parse() {
    bool result = parseExpr();
    if (error || tokenIndex != tokens.size()) {
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

std::string BooleanExpressionParser::currentToken() const {
    if (tokenIndex < tokens.size()) {
        return tokens[tokenIndex];
    }
    return "";
}

void BooleanExpressionParser::consume() {
    if (tokenIndex < tokens.size()) {
        tokenIndex++;
    }
}

bool BooleanExpressionParser::parseExpr() {
    bool result = parseTerm(); // Parse AND expressions first
    while (true) {
        std::string token = currentToken();
        if (token == "+") {
            consume(); // Consume the '+'
            result = result || parseTerm();
            if (error) return false; // Halt on error
        } else {
            break;
        }
    }
    return result;
}

bool BooleanExpressionParser::parseTerm() {
    bool result = parseFactor(); // Parse primary values first
    while (true) {
        std::string token = currentToken();
        if (token == "*") {
            consume(); // Consume the '*'
            result = result && parseFactor();
            if (error) return false; // Halt on error
        } else {
            break;
        }
    }
    return result;
}

bool BooleanExpressionParser::parseFactor() {
    std::string token = currentToken();

    if (token.empty()) {
        reportError("Unexpected end of expression");
        return false;
    }

    if (std::isalpha(token[0])) {
        consume();
        if (values.find(token[0]) != values.end()) {
            return values.at(token[0]);
        } else {
            reportError("Undefined variable: " + token);
            return false;
        }
    } else if (token == "T" || token == "F") {
        consume();
        return token == "T";
    } else if (token == "(") {
        consume(); // Consume the '('
        bool result = parseExpr();
        if (currentToken() == ")") {
            consume(); // Consume the ')'
            return result;
        } else {
            reportError("Expected closing parenthesis");
            return false;
        }
    } else {
        reportError("Unexpected token: " + token);
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