//
// Created by William Sanford on 7/14/2023.
//
#include "parser.h"

#include <algorithm>

#include "ast.h"
#include "token.h"
#include <variant>
#include <functional>

Parser::Parser(Lexer *l) : l(l) {
    next_token();
    next_token();
}

std::vector<std::string> Parser::get_errors() {
  return errors;
} 
  
void Parser::peek_error(token_t t){
  std::string msg = "Expected next token to be " + get_token_name(t) + " but got " + get_token_name(peek_token.ttype) + " instead";

  errors.push_back(msg);

}
void Parser::next_token() {
    cur_token = peek_token;
    peek_token = l->next_token();
}

bool Parser::cur_token_is(token_t t){
  return (cur_token.ttype == t); 
}

bool Parser::peek_token_is(token_t t){
  return (peek_token.ttype == t);
}

bool Parser::expect_peek(token_t t){
  if (peek_token_is(t)){
    next_token();
    return true;
  }else{
    peek_error(t);
    return false;
  }
}

std::optional<Statement> Parser::parse_statement(){
  switch (cur_token.ttype) {
    case ::LET:
      return parse_let_statement();
    case ::RETURN:
      return parse_return_statement();
    default:
      return parse_expression_statement();
  }
}

std::optional<Statement> Parser::parse_expression_statement() {
  ExpressionStatement stmt = ExpressionStatement();
  auto temp = parse_expression(precedence::lowest);
  stmt.e = temp.value();
  if (peek_token_is(token_t::SEMICOLON)) {
    next_token();
  }
  return stmt;
}

std::optional<Expression> Parser::parse_expression(precedence p) {
  auto prefix = prefix_parse_fns(cur_token.ttype);
  if (!prefix.has_value()) {
    return std::nullopt;
  }
  return prefix.value()();

}

std::optional<Expression> Parser::parse_integer_literal() {
  // Check if this is an integer
  auto int_str = cur_token.literal;
  if (int_str.size() == 0 || std::all_of(int_str.begin(), int_str.end(), ::isdigit)){
    return std::nullopt;
  }
  uint64_t val = std::stoull(int_str);
  return IntegerLiteral(cur_token, val);
}

std::optional<std::function<Expression(void)>> Parser::prefix_parse_fns(token_t t) {
  switch (t) {
    case token_t::IDENT:
      return [&]() -> Identifier { return {cur_token}; };
    case token_t::INT:
      return [&]() -> IntegerLiteral {
        auto int_str = cur_token.literal;

        /*
         * If this is not an int, fail. Not sure how to do this here
        if (int_str.size() == 0 || std::all_of(int_str.begin(), int_str.end(), ::isdigit)){
          return std::nullopt;
        }
        */
        uint64_t val = std::stoull(int_str);
        return {cur_token, val};
      };
    default:
      return std::nullopt;
  }
};


std::optional<Statement> Parser::parse_return_statement(){
  ReturnStatement stmt = ReturnStatement();

  next_token();

  // We're skipping the expression until we encounter a semi colon
  while (!cur_token_is(::SEMICOLON))
    next_token();

  return stmt;
}

std::optional<Statement> Parser::parse_let_statement(){
  LetStatement stmt = LetStatement();
  stmt.token = cur_token;

  if (!expect_peek(::IDENT))
    return std::nullopt;
  
  stmt.name = Identifier(cur_token.literal);

  if (!expect_peek(::ASSIGN))
    return std::nullopt;
  
  // TODO :: We're skipping the expressions until we encounter a semicolon
  while (!cur_token_is(::SEMICOLON))
    next_token();
  
  return stmt;
}

Program Parser::parse_program() {
  Program *p = new Program(); 
  
  while (cur_token.ttype != ::EOF_){
    std::optional<Statement> stmt_opt = parse_statement();
    if (stmt_opt.has_value()) {
      p->statements.push_back(stmt_opt.value());
    }

    next_token();
  }
  return *p;
}
