#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// Types of tokens
enum TokenType { Number, Operator, Variable, Dummy };
enum StatementType { Print, Assignment, Empty };

struct Token {
  std::string value;
  TokenType type;

  bool operator==(const Token &other) { return value == other.value; }
};

class StatementParser {
public:
  StatementParser() { variable_table = {}; }
  void parse_line(std::string &&line) {
    line += " ";
    TokenType current_type = Dummy;
    StatementType statement_type = Empty;
    int token_start = 0;
    int token_end = 0;
    std::vector<Token> tokens;
    std::string_view assigned_variable;
    // Only 3 types of statements supported
    // print <variable_name>
    // <variable_name> = <arithemtic_expression>
    // <variable_name> = <value>

    for (char character : line) {
      if (current_type == Number && is_digit(character)) {
        token_end++;
        continue;
      } else if (current_type == Variable &&
                 (is_digit(character) || is_alpha(character))) {
        token_end++;
        continue;
      }

      // Current token has terminated

      std::string_view value = std::string_view(line.begin() + token_start,
                                                line.begin() + token_end);

      if (value == "print") {
        if (tokens.empty() && statement_type == Empty)
          statement_type = Print;
        else
          throw std::runtime_error("Invalid Statement: l55");

      }

      else if (current_type == Variable) {
        if (statement_type == Empty) {
          statement_type = Assignment;
          assigned_variable = value;
          token_start = token_end;
        }
      }

      if (token_start != token_end && current_type != Dummy)
        tokens.push_back({std::string(value), current_type});

      token_start = token_end;

      token_end++;
      if (is_white_space(character) || character == '=') {
        token_start++;
        current_type = Dummy;
      } else if (is_alpha(character))
        current_type = Variable;
      else if (is_digit(character))
        current_type = Number;
      else if (character == '+' || character == '-' || character == '*' ||
               character == '/')
        current_type = Operator;
    }

    switch (statement_type) {
    case Assignment:
      variable_table[std::string(assigned_variable)] = tokens;
      break;
    case Print:
      if (variable_table.find(std::string(tokens[1].value)) !=
          variable_table.end()) {
        std::vector<Token> &expression =
            variable_table.at(std::string(tokens[1].value));
        std::vector<std::string> to_print = {};

        if (expression.size() == 1) {
          to_print.push_back(output_reduced_expression(expression[0]));
        } else if (expression[0].type == Number &&
                   expression[2].type == Number) {
          to_print.push_back(return_number(expression));
        } else {
          for (Token &token : expression) {
            if (token.type == Variable)
              to_print.push_back(output_reduced(token).value);
            else
              to_print.push_back(std::string(token.value));
          }
        }

        std::cout << tokens[1].value << " = ";
        for (std::string &token : to_print)
          std::cout << token << " ";
        std::cout << std::endl;
      }
      std::cout << "print " << tokens[1].value << std::endl;
      break;
    case Empty:
      throw std::runtime_error("Invalid statement");
    }
  }

private:
  Token output_reduced(Token variable) {
    static int variable_count = 1;
    if (variable_table.find(variable.value) == variable_table.end())
      return {std::string(variable.value), Variable};
    std::vector<Token> &expression = variable_table.at(variable.value);

    if (expression.size() == 1) {
      if (expression[0].type == Number)
        return {expression[0].value, Number};
      else if (expression[0].type == Variable)
        return output_reduced(expression[0]);
      else
        throw std::runtime_error("Invalid statement");
    }

    if (expression.size() != 3)
      throw std::runtime_error("Invalid statement");

    if (expression[0].type == Number && expression[2].type == Number)
      return {return_number(expression), Number};

    std::vector<Token> output;

    if (expression[0].type == Variable)
      output.push_back(output_reduced(expression[0]));
    else
      output.push_back(expression[0]);
    output.push_back(expression[1]);
    if (expression[2].type == Variable)
      output.push_back(output_reduced(expression[2]));
    else
      output.push_back(expression[2]);

    if (output[0].type == Number && output[2].type == Number)
      return {return_number(output), Number};

    std::string assigned = "p" + std::to_string(variable_count);
    variable_count++;

    std::cout << assigned << " = " << output[0].value << " " << output[1].value
              << " " << output[2].value << std::endl;

    return {assigned, Variable};
  }

  std::string output_reduced_expression(Token variable) {
    if (variable.type == Number)
      return std::string(variable.value);
    if (variable_table.find(variable.value) == variable_table.end())
      return std::string(variable.value);
    std::vector<Token> &expression = variable_table.at(variable.value);

    if (expression.size() == 1) {
      if (expression[0].type == Number)
        return std::string(expression[0].value);
      else if (expression[0].type == Variable)
        return output_reduced_expression(expression[0]);
      else
        throw std::runtime_error("Invalid statement");
    }

    if (expression.size() != 3)
      throw std::runtime_error("Invalid statement");

    if (expression[0].type == Number && expression[2].type == Number)
      return return_number(expression);

    std::vector<Token> new_expression(3);
    if (expression[0].type == Variable) {
      new_expression[0] = output_reduced(expression[0]);
    } else {
      new_expression[0] = expression[0];
    }
    new_expression[1] = expression[1];

    if (expression[2].type == Variable) {
      new_expression[2] = output_reduced(expression[2]);
    } else {
      new_expression[2] = expression[2];
    }

    if (new_expression[0].type == Number && new_expression[2].type == Number)
      return return_number(new_expression);

    return new_expression[0].value + " " + new_expression[1].value + " " +
           new_expression[2].value;
  }

  std::string return_number(const std::vector<Token> &expression) {
    int a = std::stoi(expression[0].value);
    int b = std::stoi(expression[2].value);
    int res;

    switch (expression[1].value[0]) {
    case '/':
      res = a / b;
      break;
    case '*':
      res = a * b;
      break;
    case '+':
      res = a + b;
      break;
    case '-':
      res = a - b;
      break;
    default:
      throw std::runtime_error("Unrecognized operator");
    }

    return std::to_string(res);
  }

  std::unordered_map<std::string, std::vector<Token>> variable_table;

  bool is_digit(char i) { return '0' <= i && i <= '9'; }
  bool is_white_space(char i) { return i == ' '; }
  bool is_alpha(char i) {
    return ('a' <= i && i <= 'z') || ('A' <= i && i <= 'Z');
  }
};

int main() {
  std::string line;
  StatementParser parser = StatementParser();
  while (std::getline(std::cin, line)) {
    parser.parse_line(std::move(line));
  }
}
