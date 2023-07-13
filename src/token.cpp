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
