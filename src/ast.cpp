//
// Created by William Sanford on 7/14/2023.
//

#include "ast.h"
#include "token.h"


auto Expression_token_literal(Expression &expression) -> std::string {
  return std::visit(overloads{
    [](Identifier &i) -> std::string { return i.token_literal(); }
  }, expression);
};

auto Expression_token_string(Expression &expression) -> std::string {
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

auto _token_literal(Node n) -> std::string {
  return std::visit(overloads{
    [](Statement &n) ->std::string {return Statement_token_literal(n); },
    [](Expression &n) ->std::string {return Expression_token_literal(n); }
  }, n);
}

std::string Program::token_literal() {
    if (!statements.empty()){
        return _token_literal(statements[0]);
    } else {
        return "";
    }
}
std::string _string(Node n){
  return "";
}

std::string Program::string() {
    if (!statements.empty()){
        return _string(statements[0]);
    } else {
        return "";
    }
}
