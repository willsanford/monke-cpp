//
// Created by wsanf on 7/12/23.
//

#include "lexer.h"
bool is_digit(char c){
    return ('0' <= c && c <= '9');
}

bool is_letter(char c){
    return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_'));
}

Lexer::Lexer(std::string input) : input(input), position(0), read_position(0), c('\0') {
    read_char();
}
Token Lexer::next_token(){
    skip_whitespace();

    Token t;

    if (c == '\0') {
        t = Token(token_t::EOF_, c);
    }else if (char_map.count(c) > 0) {
        t = Token(char_map[c], c);
    } else if (c == '=') {
        if (peek_char() == '=') {
            read_char();
            t = Token(token_t::EQ, "==");
        } else {
            t = Token(token_t::ASSIGN, c);
        }
    } else if (c == '!'){
        if (peek_char() == '='){
            read_char();
            t = Token(token_t::NotEq, "!=");
        } else {
            t = Token(token_t::BANG, c);
        }
    } else {
        if (is_letter(c)){
            t = Token(read_forward(is_letter));
        } else if (is_digit(c)) {
            t = Token(token_t::INT, read_forward(is_digit));
        } else {
            t = Token(token_t::ILLEGAL, c);
        }
    }
    read_char();

    return t;
}

void Lexer::read_char() {
    if (read_position >= input.size()){
        c = '\0';
    } else {
        c = input[read_position];
    }
    position = read_position;
    read_position += 1;
}

char Lexer::peek_char(){
    return read_position >= input.size() ? '\0' : input[read_position];
}

std::string Lexer::read_forward(bool (*pred)(char)) {
    size_t position_ = position;
    while (pred(peek_char()))
        read_char();

    return input.substr(position_, position - position_ + 1);
}

void Lexer::skip_whitespace(){
    while (c == ' ' || c == '\t' || c == '\n' || c == '\r')
        read_char();
}
