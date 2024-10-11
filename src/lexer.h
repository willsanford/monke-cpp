//
// Created by wsanf on 7/12/23.
//

#ifndef MONKE_CPP_LEXER_H
#define MONKE_CPP_LEXER_H

#include <string>
#include <unordered_map>
#include <iostream>
#include "token.h"


bool is_digit(char c);
bool is_letter(char c);

class Lexer {
public:
    Lexer(std::string input);
    Token next_token();
    std::optional<Token> lex_integral();
private:
    std::string input;
    size_t position;
    size_t read_position;
    char c;

    std::unordered_map<char, token_t> char_map = {
            {';', SEMICOLON},
            {')', RPAREN},
            {'(', LPAREN},
            {'}', RBRACE},
            {'{', LBRACE},
            {',', COMMA},
            {'+', PLUS},
            {'-', MINUS},
            {'*', ASTERISK},
            {'/', SLASH},
            {'<', LT},
            {'>', GT},
            {'\0', EOF_}
    };


    void read_char();
    char peek_char();
    void skip_whitespace();
    std::string read_forward(bool (*pred)(char));

};

#endif //MONKE_CPP_LEXER_H
