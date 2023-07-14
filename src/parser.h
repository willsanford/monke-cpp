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
    Program parse_program();

};


#endif //MONKE_CPP_PARSER_H
