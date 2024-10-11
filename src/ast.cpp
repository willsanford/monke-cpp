//
// Created by William Sanford on 7/14/2023.
//

#include "ast.h"
#include "token.h"
#include "utils.h"
#include <format>
#include <iostream>

std::optional<precedence> get_precedence(token_t t) {
  switch (t) {
    case ::EQ:
    case ::NotEq:
      return precedence::equals;
    case ::LT:
    case ::GT:
      return precedence::lessgreater;
    case ::PLUS:
    case ::MINUS:
      return precedence::sum;
    case ::SLASH:
    case ::ASTERISK:
      return precedence::product;
    case ::LPAREN:
      return precedence::call;
    default:
      return std::nullopt;
  }
}

std::string LetStatement::string() {
  return str_join({this->token_literal(),
                   " ",
                   ::token_literal(name),
                   " = ",
                   ::token_literal(value),
                   ";"});
}

std::string ExpressionStatement::string() {
  return ::string(e);
}

std::string CallExpression::string() {
  std::vector<std::string> args;
  std::transform(arguments.begin(), arguments.end(), std::back_inserter(args), [](const auto e){return ::string(*e); });
  std::string args_joined = str_join(args, ", ");

  return str_join({
    ::string(*function),
    "(",
    args_joined,
    ")"
  });
}

std::string BlockStatement::string() {
  std::string out = "";
  for (const auto &statement: get_statements()) {
    out += ::string(statement);
  }
  return out;
}

std::string IfExpression::string() {
  auto out = str_join({"if ", ::string(*condition), " ",::string(*consequence)});
  if (alternative.has_value()) {
    out += " else " + ::string(*alternative.value());
  }
  return out;
}
std::string ReturnStatement::string() {
  return str_join({this->token_literal(),
                   " ",
                   ::token_literal(return_value),
                   ";"});
}

std::string FunctionLiteral::string() {
  std::string params = "";
  if (parameters.size() > 0) {
    for (size_t i = 0; i < parameters.size() - 1; i++) {
      params += ::string(*parameters[0]) + ",";
    }
    params += ::string(*parameters[parameters.size()-1]);
  }
  return str_join({
    token_literal(),
    "(",
    params,
    ")",
    ::string(*body)
  });
}


std::string PrefixExpression::string() {
  return std::format("({}{})", op, ::string(*right));
}

std::string InfixExpression::string() {
  return std::format("({} {} {})", ::string(*left), op, ::string(*right));
}

std::string Identifier::string() {
  return token_literal();
}

template<typename T, std::enable_if_t<!is_variant_v<T>, int>>
auto token_literal(T node) -> std::string {
  return node.token_literal();
}

template<typename T, std::enable_if_t<is_variant_v<T>, int>>
auto token_literal(T node) -> std::string {
  return std::visit(overloads{
                            [](auto &n) -> std::string { return token_literal(n); },
                    },
                    node);
}

template<typename T, std::enable_if_t<!is_variant_v<T>, int>>
auto string(T node) -> std::string {
  return node.string();
}

template<typename T, std::enable_if_t<is_variant_v<T>, int>>
auto string(T node) -> std::string {
  return std::visit(overloads{
                            [](auto &n) -> std::string {
                              return string(n);
                            },
                    },
                    node);
}

std::string Program::string() {
  std::string out;
  for (const auto &statement: statements) {
    out += ::string(statement);
  }
  return out;
}

std::string Program::token_literal() {
  std::string out;
  for (const auto &statement: statements) {
    out += ::token_literal(statement);
  }
  return out;
}

template<typename T>
bool is_statement_type(Node &n) {
  return std::holds_alternative<Statement>(n) &&
         std::holds_alternative<T>(std::get<Statement>(n));
}

template<>
bool is_statement_type<ReturnStatement>(Node &n) {
  return std::holds_alternative<Statement>(n) &&
         std::holds_alternative<ReturnStatement>(std::get<Statement>(n));
}
template<>
bool is_statement_type<LetStatement>(Node &n) {
  return std::holds_alternative<Statement>(n) &&
         std::holds_alternative<LetStatement>(std::get<Statement>(n));
}

template<typename T>
bool is_expression_type(Node &n) {
  return std::holds_alternative<Expression>(n) &&
         std::holds_alternative<T>(std::get<Expression>(n));
}

template<typename T, typename... Ts>
std::ostream &operator<<(std::ostream &os, const std::variant<T, Ts...> &v) {
  std::visit([&os](auto &&arg) {
    os << arg;
  },
             v);
  return os;
}

// TODO : Figure out we need the template specialization here. Not sure why
template<>
std::ostream &operator<<(std::ostream &os, const Expression &v) {
  std::visit([&os](auto &&arg) {
    os << arg;
  },
             v);
  return os;
}
template<>
std::ostream &operator<<(std::ostream &os, const Statement &v) {
  std::visit([&os](auto &&arg) {
    os << arg;
  },
             v);
  return os;
}

std::ostream &operator<<(std::ostream &os, const ExpressionStatement &obj) {
  os << "ExpressionStatement(exp=" << obj.e << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const ReturnStatement &obj) {
  os << "ReturnStatement(ret=" << obj.return_value << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const LetStatement &obj) {
  os << "LetStatement(name=" << obj.name << ", value=" << obj.value << ")";
  return os;
}
std::ostream &operator<<(std::ostream &os, const PrefixExpression &obj) {
  os << "PrefixExpression(op=" << obj.op << ", right=" << *obj.right << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const IfExpression &obj) {
  os << "IfExpression(cond=" << *obj.condition << ", consequence=" << *obj.consequence;
  if (obj.alternative.has_value()) {
    os <<  ", alternative=" << *obj.alternative.value();
  }
  os << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const BlockStatement &obj) {
  os << "BlockStatement(statements=[\n";
  for (const auto &s: obj.get_statements()) {
    os << '\t' << s << ",\n";
  }
  os << "])";
  return os;
}

std::ostream &operator<<(std::ostream &os, const InfixExpression &obj) {
  os << "InfixExpression(left=" << *obj.left << ", op=" << obj.op << ", right=" << *obj.right << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const Identifier &obj) {
  os << "Identifier(val=" << obj.token.literal << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const BooleanLiteral &obj) {
  os << "BooleanLiteral(val=" << obj.value << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const IntegerLiteral &obj) {
  os << "IntegerLiteral(val=" << obj.val << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const StringLiteral &obj) {
  os << "StringLiteral(val=" << obj.value << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const CharLiteral &obj) {
  os << "CharLiteral(val=" << obj.value << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const FloatLiteral &obj) {
  os << "(FloatLiteral(val=" << std::to_string(obj.val) << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const FunctionLiteral &obj) {
  os << "FunctionLiteral(params=";
  if (obj.parameters.size() > 0) {
    for (size_t i = 0; i < obj.parameters.size() - 1; i++) {
      os << *obj.parameters[i] << ",";
    }
    os << *obj.parameters[obj.parameters.size() - 1];
  }
  os << ", body=" << *obj.body << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const CallExpression &obj) {
  os << "CallExpression(name=" << *obj.function << ", params=[";
  for (const auto p: obj.arguments) {
    os << *p;
  }
  os << "])";
  return os;
}


std::ostream &operator<<(std::ostream &os, const Program &obj) {
  os << "Program(statements=[\n";
  for (auto s: obj.statements) {
    os << '\t' << s << ",\n";
  }
  os << "])";
  return os;
}

bool PrefixExpression::operator==(const PrefixExpression &other) const {
  return (op == other.op) && (*(other.right) == *right);
}

bool InfixExpression::operator==(const InfixExpression &other) const {
  return (op == other.op) && (*(other.right) == *right) && (*(other.left) == *left);
}
bool LetStatement::operator==(const LetStatement &other) const {
  return (name == other.name) && (value == other.value);
}
bool ReturnStatement::operator==(const ReturnStatement &other) const {
  return return_value== other.return_value;
}
bool ExpressionStatement::operator==(const ExpressionStatement &other) const { return e == other.e; };

bool IfExpression::operator==(const IfExpression &other) const {
  bool eq = true;
  eq &= *other.condition ==  *condition;
  eq &= *other.consequence ==  *consequence;
  if (alternative.has_value()) {
    eq &= other.alternative.has_value();
    eq &= *other.alternative.value() == *alternative.value();
  }
  return eq;
}

bool BlockStatement::operator==(const BlockStatement &other) const {
  bool eq = true;
  auto stmts1 = get_statements();
  auto stmts2 = other.get_statements();
  eq &= stmts1.size() == stmts2.size();
  if (!eq) return eq;
  for (size_t i = 0; i < stmts1.size(); i++) {
    eq &= stmts1[i] == stmts2[i];
  };
  return eq;
}
bool Program::operator==(const Program &other) const {
  for (size_t i = 0; i < statements.size(); i++) {
    if (other.statements[i] != statements[i]) {
      return false;
    }
  }
  return true;
};

bool FunctionLiteral::operator==(const FunctionLiteral &other) const {
  bool eq = true;
  eq &= parameters.size() == other.parameters.size();
  if (!eq) return eq;
  for (size_t i = 0; i < other.parameters.size(); i++) {
    eq &= *parameters[i] == *other.parameters[i];
  }
  eq &= *body == *other.body;

  return eq;
};

bool CallExpression::operator==(const CallExpression &other) const {
  bool eq = true;
  eq &= *other.function == *function;
  if (!eq) return eq;
  eq &= other.arguments.size() == arguments.size();
  if (!eq) return eq;
  for (size_t i = 0; i < arguments.size(); i++) eq &= *arguments[i] == *other.arguments[i];
  return eq;
}

const std::vector<Statement> BlockStatement::get_statements() const {
  std::vector<Statement> out;
  for (const auto &[type, s_ptr]: statements) {
    switch (type) {
      case StatementType::LS:
        out.push_back(*reinterpret_cast<LetStatement*>(s_ptr));
      break;
      case StatementType::BS:
        out.push_back(*reinterpret_cast<BlockStatement*>(s_ptr));
      break;
      case StatementType::RS:
        out.push_back(*reinterpret_cast<ReturnStatement*>(s_ptr));
      break;
      case StatementType::ES:
        out.push_back(*reinterpret_cast<ExpressionStatement*>(s_ptr));
      break;
      default:
        unimplemented();
    }
  }
  return out;
}
