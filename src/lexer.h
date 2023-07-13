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
    Lexer(std::string input) : input(input), position(0), read_position(0), c('\0') {
        read_char();
    }
    Token next_token();

private:
    std::string input;
    size_t position;
    size_t read_position;
    char c;

    std::unordered_map<char, token_t> char_map = {
            {';', token_t::SEMICOLON},
            {')', token_t::RPAREN},
            {'(', token_t::LPAREN},
            {'}', token_t::RBRACE},
            {'{', token_t::LBRACE},
            {',', token_t::COMMA},
            {'+', token_t::PLUS},
            {'-', token_t::MINUS},
            {'*', token_t::ASTERISK},
            {'/', token_t::SLASH},
            {'<', token_t::LT},
            {'>', token_t::GT},
            {'\0', token_t::EOF_}
    };


    void read_char();
    char peek_char();
    void skip_whitespace();
    std::string read_forward(bool (*pred)(char));

};

#endif //MONKE_CPP_LEXER_H
