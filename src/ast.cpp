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

std::string ReturnStatement::string() {
  return str_join({this->token_literal(),
                   " ",
                   ::token_literal(return_value),
                   ";"});
}

std::string PrefixExpression::string() {
  return "(" + op + ::string(*right) + ")";
}

std::string InfixExpression::string() {
  return "(" + ::string(*left) + op + ::string(*right) + ")";
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
                            [](auto &n) -> std::string { return string(n); },
                    },
                    node);
}

std::string Program::string() {
  std::string out;
  for (const auto &statement: statements) {
    out += ::string(statement) + '\n';
  }
  return out;
}

std::string Program::token_literal() {
  std::string out;
  for (const auto &statement: statements) {
    out += ::token_literal(statement) + '\n';
  }
  return out;
}

template<typename T, typename U>
bool compareHelper(const U &lhs, const T &rhs) {
  if (const U *rhsPtr = std::get_if<U>(&rhs)) {
    return lhs == *rhsPtr;
  }
  return false;
}

template<typename T, std::enable_if_t<is_variant_v<T>, int>>
bool operator==(const T &lhs, const T &rhs) {
  return std::visit(overloads{
                            [&rhs](const auto &l) -> bool {
                              return compareHelper<T>(l, rhs);
                            },
                    },
                    lhs);
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

std::ostream &operator<<(std::ostream &os, const ExpressionStatement &obj) {
  os << "ExpressionStatement(exp=" << obj.e << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const ReturnStatement &obj) {
  os << "ReturnStatement(ret=" << obj.return_value << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const LetStatement &obj) {
  os << "LetStatement(name=" << obj.name << ", value=" << obj.name << ")";
  return os;
}
std::ostream &operator<<(std::ostream &os, const PrefixExpression &obj) {
  os << "PrefixExpression(op=" << obj.op << ", right=" << *obj.right << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const InfixExpression &obj) {
  os << "PrefixExpression(left=" << *obj.left << ", op=" << obj.op << ", right=" << *obj.right << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const Identifier &obj) {
  os << "Identifier(val=" << obj.token.literal << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const IntegerLiteral &obj) {
  os << "IntegerLiteral(val=" << obj.val << ")";
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
