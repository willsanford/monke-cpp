//
// Created by William Sanford on 7/14/2023.
//

#ifndef MONKE_CPP_AST_H
#define MONKE_CPP_AST_H

#include "token.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <numeric>
#include <optional>
#include <ranges>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "lexer.h"
#include "utils.h"

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

template<typename T, std::enable_if_t<is_variant_v<T>, int> = 0>
bool operator==(const T &lhs, const T &rhs);

// Forward declarations
class Identifier;
class IntegerLiteral;
class FloatLiteral;
class StringLiteral;
class CharLiteral;
class ArrayLiteral;
class PrefixExpression;
class InfixExpression;
class BooleanLiteral;
class CallExpression;
class IfExpression;
class FunctionLiteral;
class LetStatement;
class ReturnStatement;
class ExpressionStatement;
class BlockStatement;

typedef  std::variant<StringLiteral,
             CharLiteral,
             BooleanLiteral,
             ArrayLiteral,
             FloatLiteral,
             Identifier,
             IntegerLiteral,
             PrefixExpression,
             InfixExpression,
             IfExpression,
             FunctionLiteral,
             CallExpression>  Expression;

enum class ExpressionType {
  SL,
  CL,
  BL,
  AL,
  FL,
  ID,
  Int,
  Pre,
  Inf,
  If,
  FuncL,
  CE
};

typedef std::pair<ExpressionType, void *> IndirectExpression;

class ArrayLiteral {
  public:
      ArrayLiteral(){}
      std::vector<IndirectExpression> values;
      std::vector<Expression> get_values();
}

// See block statement for why we need this
enum class StatementType {
  LS,
  RS,
  ES,
  BS
};
typedef std::variant<LetStatement, ReturnStatement, ExpressionStatement, BlockStatement> Statement;
typedef std::variant<Statement, Expression> Node;
/*
 * TODO : we need to hold the statements as this for now as I can't figure out a better way to do this
 * In the future we can change this.
 */
class BlockStatement {
  public:
  BlockStatement(std::vector<std::pair<StatementType, void *>> statements) : statements(statements){};
  BlockStatement(Token t, std::vector<std::pair<StatementType, void *>> statements) : t(t), statements(statements){};
  Token t;
  std::vector<std::pair<StatementType, void *>> statements;
  // Helpful function to decode the above into what we actually want
  const std::vector<Statement> get_statements() const;
  std::string token_literal() { return t.literal; }
  std::string string();
  bool operator==(const BlockStatement &other) const;
};

//class ArrayLiteral {
//  public:
//  ArrayLiteral(){};
//  ArrayLiteral(Token t, std::vector<std::pair<ExpressionType, void *>> values) : t(t), values(values){};
//  Token t;
//  std::vector<std::pair<ExpressionType, void *>> values;
//  const std::vector<Expression> get_values();
//  std::string token_literal() { return "\"ArrayLiteral\""; }
//  std::string string() { return token_literal(); };
//  bool operator==(const ArrayLiteral &) const { return true; }
//};

class CallExpression {
  public:
  Token t;
  Expression *function;
  std::vector<Expression*> arguments;
  CallExpression(Token t, Expression* function, std::vector<Expression*> arguments) : t(std::move(t)), function(function), arguments(std::move(arguments)){};
  CallExpression(Expression* function, std::vector<Expression*> arguments) : function(function), arguments(std::move(arguments)){};
  std::string token_literal() { return t.literal; };
  std::string string();
  bool operator==(const CallExpression &other) const;
};

class FunctionLiteral {
  public:
  FunctionLiteral(Token t, std::vector<Identifier *> parameters, BlockStatement *body) : t(t), parameters(parameters), body(body){};
  FunctionLiteral(std::vector<Identifier *> parameters, BlockStatement *body) : parameters(parameters), body(body){};
  Token t;
  std::vector<Identifier *> parameters;
  BlockStatement *body;
  std::string token_literal() { return t.literal; }
  std::string string();
  bool operator==(const FunctionLiteral &other) const;
};

class IfExpression {
  public:
  IfExpression(){};
  IfExpression(Token t, Expression *e, BlockStatement *consequence, std::optional<BlockStatement *> alternative) : t(t), condition(e), consequence(consequence), alternative(alternative){};
  Token t;
  Expression *condition;
  BlockStatement *consequence;
  std::optional<BlockStatement *> alternative;

  std::string token_literal() { return t.literal; }
  std::string string();
  bool operator==(const IfExpression &other) const;
};

class StringLiteral {
  public:
  StringLiteral(){};
  StringLiteral(Token t, std::string value) : t(t), value(value){};
  Token t;
  std::string value;
  std::string token_literal() { return "\"" + value + "\""; }
  std::string string() { return token_literal(); };
  bool operator==(const StringLiteral &other) const { return other.value == value; }
};


class CharLiteral {
  public:
  CharLiteral(){};
  CharLiteral(Token t, char value) : t(t), value(value){};
  Token t;
  char value;
  std::string token_literal() { return "\'" + std::string(1, value) + "\'"; }
  std::string string() { return token_literal(); };
  bool operator==(const CharLiteral &other) const { return other.value == value; }
};

class BooleanLiteral {
  public:
  BooleanLiteral(){};
  BooleanLiteral(Token t, bool value) : t(t), value(value){};
  Token t;
  bool value;
  std::string token_literal() { return value ? "true" : "false"; }
  std::string string() { return token_literal(); };
  bool operator==(const BooleanLiteral &other) const { return other.value == value; }
};

class Identifier {
  public:
  Identifier() : token(Token(IDENT, "DEFAULT")){};
  Identifier(Token t) : token(t){};
  Token token;
  std::string token_literal() { return token.literal; }
  std::string string();
  bool operator==(const Identifier &other) const { return other.token.literal == token.literal; }
};

class IntegerLiteral {
  public:
  IntegerLiteral(Token t, int64_t val) : t(t), val(val){};
  IntegerLiteral(int64_t val) : val(val){};
  Token t;
  int64_t val;
  std::string token_literal() { return std::to_string(val); }
  std::string string() { return token_literal(); };
  bool operator==(const IntegerLiteral &other) const {
    return other.val == val;
  }
};

class FloatLiteral {
  public:
  FloatLiteral(Token t, double val) : t(t), val(val){};
  FloatLiteral(double val) : val(val){};
  Token t;
  double val;
  std::string token_literal() { return std::to_string(val); }
  std::string string() { return token_literal(); };
  bool operator==(const FloatLiteral &other) const {
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
  InfixExpression(Token, Expression *left, std::string op, Expression *right) {
    this->left = left;
    this->op = op;
    this->right = right;
  };

  InfixExpression(Expression *left, std::string op, Expression *right) : op(op), left(left), right(right){};
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
  LetStatement() : token(Token(LET, "let")){};
  LetStatement(Identifier name, Expression value) : token(Token(LET, "let")), name(name), value(value){};
  Token token;
  Identifier name;
  Expression value;
  std::string token_literal() { return token.literal; }
  std::string string();
  bool operator==(const LetStatement &) const;
};

class ReturnStatement {
  public:
  ReturnStatement(Expression return_value) : token(Token(token_t::RETURN, "Return")), return_value(return_value){};
  ReturnStatement() : token(Token(token_t::RETURN, "Return")), return_value(Expression()){};
  Token token;
  Expression return_value;
  std::string token_literal() { return token.literal; }
  std::string string();
  bool operator==(const ReturnStatement &) const;
};

class ExpressionStatement {
  public:
  // TODO : Not sure what to do for the token here
  ExpressionStatement() : token(Token(ILLEGAL, "")), e(Expression()){};
  ExpressionStatement(Expression e) : token(Token(ILLEGAL, "")), e(e){};
  std::string token_literal() { unimplemented(); }
  std::string string();
  Token token;
  Expression e;
  bool operator==(const ExpressionStatement &other) const;
};

class Program {
  public:
  Program() {
    statements = {};
  };

  Program(std::vector<Statement> statements) : statements(statements){};
  std::string token_literal();
  std::string string();
  std::vector<Statement> statements;
  std::optional<LexerError> error = std::nullopt;
  bool operator==(const Program &other) const;
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
std::ostream &operator<<(std::ostream &os, const IfExpression &obj);
std::ostream &operator<<(std::ostream &os, const BlockStatement &obj);
std::ostream &operator<<(std::ostream &os, const PrefixExpression &obj);
std::ostream &operator<<(std::ostream &os, const InfixExpression &obj);
std::ostream &operator<<(std::ostream &os, const Identifier &obj);
std::ostream &operator<<(std::ostream &os, const BooleanLiteral &obj);
std::ostream &operator<<(std::ostream &os, const FunctionLiteral &obj);
std::ostream &operator<<(std::ostream &os, const IntegerLiteral &obj);
std::ostream &operator<<(std::ostream &os, const FloatLiteral &obj);
//std::ostream &operator<<(std::ostream &os, const ArrayLiteral &obj);
std::ostream &operator<<(std::ostream &os, const CallExpression &obj);
std::ostream &operator<<(std::ostream &os, const StringLiteral &obj);
std::ostream &operator<<(std::ostream &os, const CharLiteral &obj);
std::ostream &operator<<(std::ostream &os, const Program &obj);

#endif// MONKE_CPP_AST_H
