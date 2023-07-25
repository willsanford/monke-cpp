//
// Created by William Sanford on 7/14/2023.
//
#include "parser.h"
#include "ast.h"
#include "token.h"
#include <cassert>
#include <cstddef>

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

Statement Parser::parse_statement(){
  std::cout << "statement: " << cur_token.literal << std::endl;
  switch (cur_token.ttype) {
    case ::LET:
      return parse_let_statement();
    case ::RETURN:
      return parse_return_statement();
    default:
      return NullStatement(); 
  }
}

Statement Parser::parse_return_statement(){
  ReturnStatement stmt = ReturnStatement();

  next_token();

  // We're skipping the expression until we encounter a semi colon
  while (!cur_token_is(::SEMICOLON))
    next_token();

  return stmt;
}

Statement Parser::parse_let_statement(){
  LetStatement stmt = LetStatement();
  stmt.token = cur_token;

  if (!expect_peek(::IDENT))
    return NullStatement();
  
  stmt.name = Identifier(cur_token, cur_token.literal);

  if (!expect_peek(::ASSIGN))
    return NullStatement();
  
  // TODO :: We're skipping the expressions until we encounter a semicolon
  while (!cur_token_is(::SEMICOLON))
    next_token();
  
  return stmt;
}

Program Parser::parse_program() {
  Program *p = new Program(); 
  
  while (cur_token.ttype != ::EOF_){
    Statement stmt = parse_statement();
    if (stmt.t != NodeType::NullStatement_)
      p->statements.push_back(stmt);
    
    next_token();
  }
  return *p;
}
