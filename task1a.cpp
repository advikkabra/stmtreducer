#include <climits>
#include <iostream>
#include <stack>
#include <string>
#include <string_view>
#include <utility>

// Types of tokens
enum TokenType { Operand, Operator, Variable, Dummy };

class StatementReducer {
public:
  static void parse_line(std::string &&expression) {

    // Only 2 types of expressions supported
    // print <variable_name>
    // <variable_name> = <arithemtic_expression>

    int equals_position = expression.find("=");

    if (equals_position == std::string::npos) {
      std::cout << expression << std::endl;
    } else {
      expression.push_back(
          '$'); // Adding a dummy operator at the end to empty the stack
      expression.push_back(
          ' '); // Adding a dummy whitespace at the end for the token to end

      std::string output = parse_arthematic_expression(std::string_view(
          {expression.begin() + equals_position, expression.end()}));
      std::cout << std::string_view({expression.begin(),
                                     expression.begin() + equals_position + 1})
                << " " << output << std::endl;
    }
  }

private:
  StatementReducer() {}

  static std::string parse_arthematic_expression(std::string_view expression) {
    // parse the expression and return the result
    // add dummy char to the end of the string (for easier implementation,
    // optional) otherwise resolve the elements left on the stacks in the end
    std::stack<char> operatorStack;
    std::stack<std::string> valueStack;

    // tokenize the elements of expression string.
    // ignore white spaces, write a function to construct number from digits

    TokenType current_type = Dummy; // Default value
    std::string current_token = "";

    for (char character : expression) {
      // Continuation of current token
      if (current_type == Operand && is_digit(character)) {
        current_token.push_back(character);
        continue;
      } else if (current_type == Variable &&
                 (is_digit(character) || is_alpha(character))) {
        current_token.push_back(character);
        continue;
      }

      // Current token has terminated.
      if (!current_token.empty() && current_type != Dummy) {
        // push number onto operandStack
        if (current_type == Operand || current_type == Variable)
          valueStack.push(current_token);
        // if token is an operation, check the top element of operatorStack
        // and call perform_one_operation if needed. Finally, push operator
        // onto operatorStack
        else if (current_type == Operator) {
          // Keep performing the operations till you empty the operator stack
          // Or you find an operation with lower precedence
          while (!operatorStack.empty() &&
                 precedence(current_token[0]) >=
                     precedence(operatorStack.top())) {
            perform_one_operation(operatorStack, valueStack);
          }
          operatorStack.push(current_token[0]);
        }
      }

      // Flush the token and reset the type to the next character's type
      current_token.clear();

      // No need to include whitespace in the token
      if (!is_white_space(character))
        current_token.push_back(character);

      if (is_digit(character))
        current_type = Operand;
      else if (character == '/' || character == '*' || character == '+' ||
               character == '-' || character == '$')
        current_type = Operator;
      else if (is_alpha(character))
        current_type = Variable;
    }

    // After iterating through all elements of input string,
    return valueStack.top();
  }

  static void perform_one_operation(std::stack<char> &operatorStack,
                                    std::stack<std::string> &valueStack) {
    static int variable_count = 1;

    std::string &value2 = valueStack.top();
    valueStack.pop();
    std::string &value1 = valueStack.top();
    valueStack.pop();
    char op = operatorStack.top();
    operatorStack.pop();

    std::cout << "t" << variable_count << " = " << value1 << " " << op << " "
              << value2 << std::endl;

    std::string value = "t" + std::to_string(variable_count);
    valueStack.push(std::move(value));

    variable_count++;
  }

  static bool is_digit(char i) { return '0' <= i && i <= '9'; }
  static bool is_white_space(char i) { return i == ' '; }
  static bool is_alpha(char i) {
    return ('a' <= i && i <= 'z') || ('A' <= i && i <= 'Z');
  }

  static int precedence(char i) {
    // assigns numbers or 'priorities' to operations based on which they are
    // compared
    int precedence = 0;
    switch (i) {
    case '+':
    case '-':
      precedence++;
    case '/':
    case '*':
      precedence++;
      break;
    default:
      precedence = INT_MAX;
    }
    return precedence;
  }
};

int main() {
  std::string line;
  while (std::getline(std::cin, line)) {
    StatementReducer::parse_line(std::move(line));
  }
}
