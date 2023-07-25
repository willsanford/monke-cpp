//
// Created by wsanf on 7/12/23.
//
#include "token.h"

Token::Token() : ttype(token_t::ILLEGAL) , literal("\0"){};
Token::Token(token_t t, std::string l) : ttype(t), literal(l) {};
Token::Token(token_t t, char l) : ttype(t), literal(std::string(1, l)) {};
Token::Token(std::string l) : ttype(lookup_ident(l)), literal(l) {};
token_t Token::lookup_ident(std::string literal_) {
    if (literal_ == "let") {
        return token_t::LET;
    } else if (literal_ == "if"){
       return token_t::IF;
    }else if (literal_ == "true"){
        return token_t::TRUE;
    }else if (literal_ == "else"){
        return token_t::ELSE;
    }else if (literal_ == "false") {
        return token_t::FALSE;
    }else if (literal_ == "fn") {
        return token_t::FUNCTION;
    }else if (literal_ == "return"){
        return token_t::RETURN;
    }else{
        return token_t::IDENT;
    }
}
bool Token::operator==(const Token& t){
    return (ttype == t.ttype) && (literal == t.literal);
}

std::string get_token_name(token_t t){
  switch (t) {
    case ILLEGAL:
      return "illegal";
      break;
    case EOF_:
      return "EOF";
      break;
    case IDENT:
      return "IDENT";
      break;
    case INT:
      return "INT";
      break;
    case EQ:
      return "EQ";
      break;
    case NotEq:
      return "NotEq";
      break;
    case PLUS:
      return "PLUS";
      break;
    case ASSIGN:
      return "ASSIGN";
      break;
    case MINUS:
      return "MINUS";
      break;
    case BANG:
      return "BANG";
      break;
    case ASTERISK:
      return "ASTERISK";
      break;
    case SLASH:
      return "SLASH";
      break;
    case LT:
      return "LT";
      break;
    case GT:
      return "GT";
      break;
    case COMMA:
      return "COMMA";
      break;
    case SEMICOLON:
      return "SEMICOLON";
      break;
    case LPAREN:
      return "LPAREN";
      break;
    case RPAREN:
      return "RPAREN";
      break;
    case LBRACE:
      return "LBRACE";
      break;
    case RBRACE:
      return "RBRACE";
      break;
    case FUNCTION:
      return "FUNCTION";
      break;
    case LET:
      return "LET";
      break;
    case TRUE:
      return "TRUE";
      break;
    case FALSE:
      return "FALSE";
      break;
    case IF:
      return "IF";
      break;
    case ELSE:
      return "ELSE";
      break;
    case RETURN:
      return "RETURN";
      break;
  }
}
