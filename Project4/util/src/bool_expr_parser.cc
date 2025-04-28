#include <bool_expr_parser.h>

bool SatSolver(std::size_t total_variables, const std::string& expression) {
  BooleanExpressionParser parser(expression);
  for (int i = 0; i < 1 << total_variables; ++i) {
    ValueMap values;

    // For each bit position (variable), assign value
    for (std::size_t j = 0; j < total_variables; ++j) {
      char var = 'a' + static_cast<char>(j);
      bool val = (i >> j) & 1;  // e.g. 1 0 1&1 -> 1 0&1 1 -> 1&1 0 1 -> etc
      values[var] = val;
    }

    if (parser.Parse(values)) {
      return true;
    } else if (parser.HasError()) {
      std::cerr << "[SATSOLVER] " << parser.Error() << std::endl;
    }
  }
  return false;
}

bool BooleanExpressionParser::Parse() {
  bool result = ParseExpr();
  if (has_error_ || current_index_ != expression_.size()) {
    std::string parsed = expression_.substr(0, current_index_);
    std::string unexpected = expression_.substr(current_index_);
    ReportError("Unexpected tokens after parsing: \"" + parsed + "<->" + unexpected + "\"");
    return false; // Indicate parse error
  }
  return result;
}

bool BooleanExpressionParser::Parse(const ValueMap& values) {
  values_  = &values;
  // reset parser vars in case this is not first parse of expr
  current_index_ = 0;
  has_error_ = 0;
  err_msg_ = "";
  return Parse();
}

bool BooleanExpressionParser::HasError() const {
  return has_error_;
}

char BooleanExpressionParser::CurrentChar() const {
  if (current_index_ < expression_.size()) {
    return expression_[current_index_];
  }
  return '\0';
}

// Advance to the next character
void BooleanExpressionParser::Consume() {
  if (current_index_ < expression_.size()) {
    ++current_index_;
  }
}

// Parse OR ('+') expressions
bool BooleanExpressionParser::ParseExpr() {
  bool result = ParseTerm(); // Parse AND expressions first
  while (CurrentChar() == '+') {
    Consume(); // Consume the '+'
    result = ParseTerm() || result;
    if (has_error_) return false; // Halt on error
  }
  return result;
}

// Parse AND ('*') expressions
bool BooleanExpressionParser::ParseTerm() {
  bool result = ParseFactor(); // Parse primary values first
  while (CurrentChar() == '*') {
    Consume(); // Consume the '*'
    result = ParseFactor() && result;
    if (has_error_) return false; // Halt on error
  }
  return result;
}

// Parse primary values: variables (a-z), optionally negated with `'`
bool BooleanExpressionParser::ParseFactor() {
  char token = CurrentChar();

  if (std::isalpha(token)) {
    Consume(); // consume the variable
    bool negated = false;

    if (CurrentChar() == '\'') {
      Consume(); // consume the negation
      negated = true;
    }

    if (values_->find(token) == values_->end()) {
      ReportError("Undefined variable: " + std::string(1, token));
      return false;
    }

    bool value = values_->at(token);
    return negated ? !value : value;
  }

  ReportError("Unexpected token: \"" + std::string(1, token)
                                     + "\", expr: \""
                                     + expression_ + "\"");
  return false;
}


void BooleanExpressionParser::ReportError(const std::string& message) {
  if (!has_error_) {
    err_msg_ = "Error: " + message;
    has_error_ = true;
  }
}

const std::string BooleanExpressionParser::Error() const {
  return err_msg_;
}


const std::string Explode(const char input[], char delim) {
  std::string result;

  for (const char* c = input; *c != '\0'; ++c)
    if (*c != delim)
      result += *c;

  return result;
}


const std::unordered_map<char, bool> BuildMap(const std::string& b_vals) {
  // Read variable values
  std::unordered_map<char, bool> values;
  std::size_t varIndex = 0;
  for (char b_val : b_vals) {
    char var = 'a' + varIndex; // Variables are a, b, c, ...
    values[var] = (b_val == 'T');
    varIndex++;
  }
  return values;
}
