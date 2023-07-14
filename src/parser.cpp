//
// Created by William Sanford on 7/14/2023.
//
#include "parser.h"

Parser::Parser(Lexer *l) : l(l) {
    next_token();
    next_token();
}

void Parser::next_token() {
    cur_token = peek_token;
    peek_token = l->next_token();
}

Program Parser::parse_program() {}