//
// Created by William Sanford on 7/14/2023.
//

#ifndef MONKE_CPP_PARSER_H
#define MONKE_CPP_PARSER_H

#include "token.h"
#include "lexer.h"
#include "ast.h"
class Parser {
public:
    Parser(Lexer* l);
    Lexer *l;

    Token cur_token;
    Token peek_token;

    void next_token();
    bool cur_token_is(token_t t);
    bool peek_token_is(token_t t);
    bool expect_peek(token_t t);

    Statement parse_statement();
    Statement parse_let_statement();

    Program parse_program();
};


#endif //MONKE_CPP_PARSER_H
