//
// Created by William Sanford on 7/14/2023.
//

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

auto Expression_token_literal(Expression &expression) -> std::string {
  return std::visit(overloads{
    [](Identifier &i) -> std::string { return i.token_literal(); }
  }, expression);
};

auto Expression_string(Expression &expression) -> std::string {
  return std::visit(overloads{
    [](Identifier &i) -> std::string { return i.string(); }
  }, expression);
};

auto Statement_token_literal(Statement &statement) -> std::string {
  return std::visit(overloads{
    [](NullStatement &s) -> std::string { return s.token_literal(); },
    [](LetStatement &s) -> std::string { return s.token_literal(); },
    [](ReturnStatement &s) -> std::string { return s.token_literal(); },
  }, statement);
};

auto Statement_string(Statement &statement) -> std::string {
  return std::visit(overloads{
          [](NullStatement &s) -> std::string { return s.string(); },
          [](LetStatement &s) -> std::string { return s.string(); },
          [](ReturnStatement &s) -> std::string { return s.string(); },
  }, statement);
};
auto token_literal(Node n) -> std::string {
  return std::visit(overloads{
    [](Statement &n) ->std::string {return Statement_token_literal(n); },
    [](Expression &n) ->std::string {return Expression_token_literal(n); }
  }, n);
}

std::string string(Node n){
  return std::visit(overloads{
          [](Statement &n) ->std::string {return Statement_string(n); },
          [](Expression &n) ->std::string {return Expression_string(n); }
  }, n);
}

std::string Program::token_literal(){
    if (!statements.empty()){
        return ::token_literal(statements[0]);
    } else {
        return "";
    }
}

std::string Program::string() {
    if (!statements.empty()){
        return ::string(statements[0]);
    } else {
        return "";
    }
}

template<typename  T>
bool is_node_type(Node &n){
  std::unreachable();
};

template<>
bool is_node_type<LetStatement>(Node &n){
  if (std::holds_alternative<Statement>(n)){
    return std::holds_alternative<LetStatement>(std::get<Statement>(n));
  }
  return false;
}

template<>
bool is_node_type<ReturnStatement>(Node &n){
  if (std::holds_alternative<Statement>(n)){
    return std::holds_alternative<ReturnStatement>(std::get<Statement>(n));
  }
  return false;
}
