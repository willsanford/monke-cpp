//
// Created by wsanf on 7/12/23.
//
#include "token.h"

Token::Token() : ttype(token_t::ILLEGAL), literal("\0"){};
Token::Token(token_t t, std::string l) : ttype(t), literal(l){};
Token::Token(token_t t, char l) : ttype(t), literal(std::string(1, l)){};
Token::Token(std::string l) : ttype(lookup_ident(l)), literal(l){};
token_t Token::lookup_ident(std::string literal_) {
  if (literal_ == "let") return LET;
  if (literal_ == "if") return IF;
  if (literal_ == "true") return TRUE;
  if (literal_ == "else") return ELSE;
  if (literal_ == "false") return FALSE;
  if (literal_ == "fn") return FUNCTION;
  if (literal_ == "return") return RETURN;
  return IDENT;
}
bool Token::operator==(const Token &t) {
  return (ttype == t.ttype) && (literal == t.literal);
}

std::string get_token_name(token_t t) {
  switch (t) {
    case ILLEGAL:
      return "illegal";
    case EOF_:
      return "EOF";
    case IDENT:
      return "IDENT";
    case INT:
      return "INT";
    case EQ:
      return "EQ";
    case NotEq:
      return "NotEq";
    case PLUS:
      return "PLUS";
    case ASSIGN:
      return "ASSIGN";
    case MINUS:
      return "MINUS";
    case BANG:
      return "BANG";
    case ASTERISK:
      return "ASTERISK";
    case SLASH:
      return "SLASH";
    case LT:
      return "LT";
    case GT:
      return "GT";
    case COMMA:
      return "COMMA";
    case SEMICOLON:
      return "SEMICOLON";
    case LPAREN:
      return "LPAREN";
    case RPAREN:
      return "RPAREN";
    case LBRACE:
      return "LBRACE";
    case RBRACE:
      return "RBRACE";
    case RBRACKET:
      return "RBRACKET";
    case LBRACKET:
      return "LBRACKET";
    case FUNCTION:
      return "FUNCTION";
    case LET:
      return "LET";
    case TRUE:
      return "TRUE";
    case FALSE:
      return "FALSE";
    case IF:
      return "IF";
    case ELSE:
      return "ELSE";
    case RETURN:
      return "RETURN";
    case STRING:
      return "STRING";
    case CHAR:
      return "CHAR";
    case FLOAT:
      return "FLOAT";
  }
}
