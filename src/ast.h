//
// Created by William Sanford on 7/14/2023.
//

#ifndef MONKE_CPP_AST_H
#define MONKE_CPP_AST_H

#include "token.h"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

// Operator Precedence
enum struct precedence {
  lowest = 0,
  equals = 1,
  lessgreater = 2,
  sum = 3,
  product = 4,
  prefix = 5,
  call = 6
};

std::optional<precedence> get_precedence(token_t t);


// Primary template for non-variant types
template<typename T, typename = void>
struct is_variant : std::false_type {};

// Specialization for std::variant
template<typename... Args>
struct is_variant<std::variant<Args...>> : std::true_type {};
template<typename T>
inline constexpr bool is_variant_v = is_variant<T>::value;

template<typename T, std::enable_if_t<is_variant_v<T>, int> = 0>
bool operator==(const T &lhs, const T &rhs);
template<class... Ts>
struct overloads : Ts... {
  using Ts::operator()...;
};
template<class... Ts>
overloads(Ts...) -> overloads<Ts...>;

// Forward declarations
class Identifier;
class IntegerLiteral;
class PrefixExpression;
class InfixExpression;
typedef std::variant<Identifier, IntegerLiteral, PrefixExpression, InfixExpression> Expression;
class LetStatement;
class ReturnStatement;
class ExpressionStatement;
typedef std::variant<LetStatement, ReturnStatement, ExpressionStatement> Statement;
typedef std::variant<Statement, Expression> Node;


class Identifier {
  public:
  Identifier() : token(Token(token_t::IDENT, "DEFAULT")){};
  Identifier(Token t) : token(t){};
  Token token;
  std::string token_literal() { return token.literal; }
  std::string string();
  bool operator==(const Identifier &) const { std::unreachable(); }
};

class IntegerLiteral {
  public:
  IntegerLiteral(Token t, uint64_t val) : t(t), val(val){};
  Token t;
  uint64_t val;
  std::string token_literal() { return std::to_string(val); }
  std::string string() { return token_literal(); };
  bool operator==(const IntegerLiteral &other) const {
    return other.val == val;
  }
};

class PrefixExpression {
  public:
  PrefixExpression(){};
  PrefixExpression(std::string prefix, Expression *right_) {
    op = prefix;
    right = right_;
  };
  Token t;
  std::string op;
  Expression *right;
  std::string token_literal() { return t.literal; }
  std::string string();
  bool operator==(const PrefixExpression &other) const;
};

class InfixExpression {
  public:
  InfixExpression(){};
  InfixExpression(Token t, Expression *left, std::string op, Expression *right) {
    this->t = t;
    this->left = left;
    this->op = op;
    this->right = right;
  };
  Token t;
  std::string op;
  Expression *left;
  Expression *right;
  std::string token_literal() { return t.literal; }
  std::string string();
  bool operator==(const InfixExpression &other) const;
};
class LetStatement {
  public:
  LetStatement() : token(Token(token_t::LET, "let")){};
  Token token;
  Identifier name;
  Expression value;
  std::string token_literal() { return token.literal; }
  std::string string();
  bool operator==(const LetStatement &) const { std::unreachable(); }
};

class ReturnStatement {
  public:
  ReturnStatement(Expression return_value) : token(Token(token_t::RETURN, "Return")), return_value(return_value){};
  ReturnStatement() : token(Token(token_t::RETURN, "Return")), return_value(Expression()){};
  Token token;
  Expression return_value;
  std::string token_literal() { return token.literal; }
  std::string string();
  bool operator==(const ReturnStatement &) const { std::unreachable(); }
};

class ExpressionStatement {
  public:
  // TODO : Not sure what to do for the token here
  ExpressionStatement() : token(Token(token_t::ILLEGAL, "")), e(Expression()){};
  ExpressionStatement(Expression e) : token(Token(token_t::ILLEGAL, "")), e(e){};
  std::string token_literal() { std::unreachable(); }
  std::string string() { std::unreachable(); }
  Token token;
  Expression e;
  bool operator==(const ExpressionStatement &other) const { return e == other.e; };
};

class Program {
  public:
  Program() {
    statements = {};
  };
  Program(std::vector<Node> statements) : statements(statements){};
  std::string token_literal();
  std::string string();
  std::vector<Node> statements;
};


// Helper variable template


// One for branches in the variant tree and another for the leaves
template<typename T, std::enable_if_t<is_variant_v<T>, int> = 0>
auto token_literal(T node) -> std::string;

template<typename T, std::enable_if_t<!is_variant_v<T>, int> = 0>
auto token_literal(T node) -> std::string;

// One for branches in the variant tree and another for the leaves
template<typename T, std::enable_if_t<is_variant_v<T>, int> = 0>
auto string(T node) -> std::string;

template<typename T, std::enable_if_t<!is_variant_v<T>, int> = 0>
auto string(T node) -> std::string;

template<typename T>
bool is_statement_type(Node &n);

template<typename T>
bool is_expression_type(Node &n);

template<typename T, typename... Ts>
std::ostream &operator<<(std::ostream &os, const std::variant<T, Ts...> &v);
std::ostream &operator<<(std::ostream &os, const ExpressionStatement &obj);
std::ostream &operator<<(std::ostream &os, const ReturnStatement &obj);
std::ostream &operator<<(std::ostream &os, const LetStatement &obj);
std::ostream &operator<<(std::ostream &os, const PrefixExpression &obj);
std::ostream &operator<<(std::ostream &os, const InfixExpression &obj);
std::ostream &operator<<(std::ostream &os, const Identifier &obj);
std::ostream &operator<<(std::ostream &os, const IntegerLiteral &obj);
std::ostream &operator<<(std::ostream &os, const Program &obj);

#endif// MONKE_CPP_AST_H
