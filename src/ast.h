//
// Created by William Sanford on 7/14/2023.
//

#ifndef MONKE_CPP_AST_H
#define MONKE_CPP_AST_H

#include <string>
#include <utility>
#include <variant>
#include "token.h"

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


template <class... Ts> struct overloads : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloads(Ts...) -> overloads<Ts...>;

class NullStatement{
public:
  NullStatement(){}
  std::string token_literal() { return "NullStatement"; }
  std::string string() { return "NullStatement"; };
};

class Identifier {
public:
  Identifier() : token(Token(token_t::IDENT, "DEFAULT")){};
  Identifier(std::string value) : token(Token(token_t::IDENT, value)){};
  Token token;
  std::string token_literal() { return token.literal; }
  std::string string();
};

typedef std::variant<Identifier> Expression;
class LetStatement {
public:
  LetStatement() : token(Token(token_t::LET, "let")) {};
  Token token;
  Identifier name;
  Expression value;
  std::string token_literal() { return token.literal; }
  std::string string();
};

class ReturnStatement {
public:
  ReturnStatement(Expression return_value) : token(Token(token_t::RETURN, "Return")) , return_value(return_value){};
  ReturnStatement() : token(Token(token_t::RETURN, "Return")), return_value(Expression()){}; 
  Token token;
  Expression return_value;
  std::string token_literal() { return token.literal; }
  std::string string();
};

class ExpressionStatement {
  public:
      ExpressionStatement(Expression e) : e(e){};
      Token token
      Expression e


};
typedef std::variant<NullStatement, LetStatement, ReturnStatement, ExpressionStatement> Statement;
typedef std::variant<Statement, Expression> Node;

auto token_literal(Node n) -> std::string;


template<typename  T>
bool is_node_type(Node &n);

template<>
bool is_node_type<LetStatement>(Node &n);

template<>
bool is_node_type<ReturnStatement>(Node &n);

class Program {
public:
  Program(){
    statements = {};

  };
  std::string token_literal();
  std::string string();
  std::vector<Node> statements;
};

#endif // MONKE_CPP_AST_H
