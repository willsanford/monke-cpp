//
// Created by William Sanford on 7/14/2023.
//
#include "parser.h"
#include "ast.h"
#include "token.h"
#include <cstddef>

Parser::Parser(Lexer *l) : l(l) {
    next_token();
    next_token();
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
    return false;
  }
}

Statement Parser::parse_statement(){
  switch (cur_token.ttype) {
    case ::LET:
      return parse_let_statement();

    default:
      return NullStatement(); 
  }
}

Statement Parser::parse_let_statement(){

  LetStatement stmt = LetStatement();
  stmt.token = cur_token;

  if (!expect_peek(::IDENT)){
    return NullStatement();
  }

  stmt.name = Identifier(cur_token, cur_token.literal);

  if (!expect_peek(::ASSIGN)){
    return NullStatement();
  }

  // TODO :: We're skipping the expressions until we encounter a semicolon
  while (!cur_token_is(::SEMICOLON)){
    next_token();
  }

  return stmt;
}

Program Parser::parse_program() {
  Program *p = new Program(); 

  
}
