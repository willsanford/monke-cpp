//
// Created by wsanf on 7/12/23.
//

#include <optional>
#include <vector>
#include <functional>
#include <spdlog/spdlog.h>

#include "lexer.h"
bool is_digit(char c){
    return '0' <= c && c <= '9';
}

bool is_quote(char c){
    return c == '"';
}
bool is_letter(char c){
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

std::pair<std::string, size_t> get_position(std::string &input, size_t position) {
    std::istringstream stream(input);
    std::string line;
    size_t current_pos = 0;
    size_t column_number = 1;

    while (std::getline(stream, line)) {
        size_t line_length = line.length() + 1; // +1 for newline character

        if (current_pos + line_length > position) {
            column_number = position - current_pos + 1;
            return std::make_pair(line, column_number);
        }

        current_pos += line_length;
    }

    // If position is beyond string length, return last line and last column
    return std::make_pair(line, line.length() + 1);
}
Lexer::Lexer(std::string input) : input(input), position(0), read_position(0), c('\0') {
    SPDLOG_TRACE("Lexer initialized");
    read_char();

    // Initialize the lexers
    #define F(f) [&]() -> std::optional<LexerResult> { return f(); }
    lexers = {
            F(lex_eof),
            F(lex_char_map),
            F(lex_eq),
            F(lex_bang),
            F(lex_tick),
            F(lex_quote),
            F(lex_ident),
            F(lex_integral),
            F(lex_illegal),
    };
}

auto Lexer::run_lexers() -> LexerResult {
    for (const auto& lexer: lexers) {
        if (auto out = lexer(); out.has_value()) {
            return out.value();
        }
    }
    return std::unexpected(LexerError("No lexers matched", input, position));
}
auto Lexer::lex_eof() -> std::optional<LexerResult> {
    if (c != '\0') return std::nullopt;
    return Token(EOF_, c);
}

auto Lexer::lex_char_map() -> std::optional<LexerResult > {
    if (!char_map.contains(c)) return std::nullopt;
    Token t = Token(char_map[c], c);
    read_char();
    return t;
}

auto Lexer::lex_eq() -> std::optional<LexerResult > {
    if (c != '=') return std::nullopt;
    if (peek_char() == '=') {
        read_char();
        read_char();
        return Token(EQ, "==");
    }
    read_char();
    return Token(ASSIGN, '=');
}

auto Lexer::lex_bang() -> std::optional<LexerResult > {
    if (c != '!') return std::nullopt;
    if (peek_char() == '='){
        read_char();
        read_char();
        return Token(NotEq, "!=");
    }
    read_char();
    return Token(BANG, '!');
}

auto Lexer::lex_tick() -> std::optional<LexerResult > {
    if (c != '\'') return std::nullopt;
    read_char();
    Token t = Token(CHAR, c);
    read_char();
    if (c != '\'') {
        SPDLOG_ERROR("Expected char to end with '");
        return std::unexpected(LexerError("Expected ' after char", input, position - 1));
    }
    read_char();
    return t;
}

auto Lexer::lex_quote() -> std::optional<LexerResult > {
    if (c != '"') return std::nullopt;
    read_char();
    int p = position;
    while (c != '"') {
        read_char();
        if (c == '\0') return std::unexpected(LexerError("Expected String to end with \"", input, position - 1));
    }
    Token t = Token(STRING, input.substr(p, position - p));
    read_char();
    return t;
}

auto Lexer::lex_ident() -> std::optional<LexerResult > {
    if (!is_letter(c)) return std::nullopt;
    Token t = Token(read_forward(is_letter));
    read_char();
    if (is_digit(c)) {
        return std::unexpected(LexerError("Ident cannot end with a digit", input, position - 1));
    }
    return t;
}

auto Lexer::lex_integral() -> std::optional<LexerResult > {
    if (!is_digit(c)) return std::nullopt;
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

    if (neg && !f) return std::unexpected(LexerError("No support for negative ints yet", input, position));

    if (f) {
        return Token(FLOAT, digits);
    }
    return Token(INT, digits);
}

auto Lexer::lex_illegal() -> std::optional<LexerResult > {
    return std::unexpected(LexerError("Illegal character", input, position));
}
LexerResult Lexer::next_token(){
    skip_whitespace();
    return run_lexers();
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
    }
    return '\0';
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
