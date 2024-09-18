//
// Created by William Sanford on 7/14/2023.
//

#include <iostream>
#include "ast.h"
#include "token.h"
#include "utils.h"

std::string LetStatement::string() {
  return str_join({
          this->token_literal(),
          " ",
          ::token_literal(name),
          " = ",
          ::token_literal(value),
          ";"
  });
}

std::string ReturnStatement::string(){
  return str_join({
          this->token_literal(),
          " ",
          ::token_literal(return_value),
          ";"
  });
}

std::string Identifier::string(){
  return token_literal();
}

auto Expression_string(Expression &expression) -> std::string {
  return std::visit(overloads{
    [](auto &i) -> std::string { return i.string(); },
  }, expression);
};

auto Statement_string(Statement &expression) -> std::string {
  return std::visit(overloads{
    [](auto &i) -> std::string { return i.string(); },
  }, expression);
};

template<typename T, std::enable_if_t<!is_variant_v<T>, int>>
auto token_literal(T node) -> std::string {
  return node.token_literal();
}

template<typename T, std::enable_if_t<is_variant_v<T>, int>>
auto token_literal(T node) -> std::string {
  return std::visit(overloads {
    [](auto &n) ->std::string { return token_literal(n); },
  }, node);
}

std::string string(Node n){
  return std::visit(overloads{
          [](Statement &statement) ->std::string {return Statement_string(statement); },
          [](Expression &expression) ->std::string {return Expression_string(expression); }
  }, n);
}

std::string Program::token_literal(){
  std::string out;
  for (const auto &statement : statements) {
    out += ::token_literal(statement) + '\n';
  }
  return out;
}

std::string Program::string() {
  std::string out;
  for (const auto &statement : statements) {
    out += ::string(statement) + '\n';
  }
  return out;
}

template<typename T, typename U>
bool compareHelper(const U& lhs, const T& rhs) {
  if (const U* rhsPtr = std::get_if<U>(&rhs)) {
    return lhs == *rhsPtr;
  }
  return false;
}

template<typename T>
bool operator==(const T& lhs, const T& rhs) {
  return std::visit(overloads{
    [&rhs](const auto& l) -> bool {
      return compareHelper<T>(l, rhs);
    },
  }, lhs);
}

template<typename  T>
bool is_node_type(Node &n){
  std::unreachable();
};

template<>
bool is_node_type<LetStatement>(Node &n){
  return std::holds_alternative<Statement>(n) &&
    std::holds_alternative<LetStatement>(std::get<Statement>(n));
}

template<>
bool is_node_type<ReturnStatement>(Node &n){
  return std::holds_alternative<Statement>(n) &&
    std::holds_alternative<ReturnStatement>(std::get<Statement>(n));
}
