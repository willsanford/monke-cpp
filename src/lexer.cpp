//
// Created by wsanf on 7/12/23.
//

#include <optional>
#include <vector>
#include <ctype.h>
#include "lexer.h"
bool is_digit(char c){
    return ('0' <= c && c <= '9');
}

bool is_quote(char c){
    return c == '"';
}
bool is_letter(char c){
    return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_'));
}

Lexer::Lexer(std::string input) : input(input), position(0), read_position(0), c('\0') {
    read_char();
}


std::optional<Token> Lexer::lex_integral() {
    std::string digits = {};
    bool neg = false;
    if (c == '-') {
        digits.push_back(c);
        read_char();
        neg = true;
    }

    bool f = false;
    while (is_digit(c) || c == '.') {
        digits.push_back(c);
        if (c == '.') f = true;
        read_char();
    }

    if (neg && !f) return std::nullopt;

    if (f) {
        return Token(FLOAT, digits);
    }
    return Token(INT, digits);
}
Token Lexer::next_token(){
    skip_whitespace();

    Token t;

    if (c == '\0') {
        t = Token(EOF_, c);
    }else if (char_map.count(c) > 0) {
        t = Token(char_map[c], c);
    } else if (c == '=') {
        if (peek_char() == '=') {
            read_char();
            t = Token(EQ, "==");
        } else {
            t = Token(ASSIGN, c);
        }
    } else if (c == '!') {
        if (peek_char() == '='){
            read_char();
            t = Token(NotEq, "!=");
        } else {
            t = Token(BANG, c);
        }
    }else if (c == '\'') {
        read_char();
        t = Token(CHAR, c);
        read_char();
    } else if (c == '"') {
        read_char();
        int p = position;
        while (c != '"') {
            read_char();
        }
        t = Token(STRING, input.substr(p, position - p));
    } else {
        if (is_letter(c)){
            t = Token(read_forward(is_letter));
        } else if (is_digit(c)) {
            // TODO : Expand the expected token to everything
            auto out = lex_integral();
            if (out.has_value()) {
                t = out.value();
            }else {
                t = Token(ILLEGAL, c);
            }
        } else {
            t = Token(ILLEGAL, c);
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
    if (read_position < input.size()){
        return input[read_position];
    }else {
        return '\0';
    }
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
