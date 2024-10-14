//
// Created by wsanf on 7/12/23.
//

#ifndef MONKE_CPP_LEXER_H
#define MONKE_CPP_LEXER_H

#include <string>
#include <unordered_map>
#include <expected>
#include <iostream>
#include <functional>
#include "token.h"


// Get the line and column number of a given position in the input string
std::pair<std::string, size_t> get_position(std::string &input, size_t position);
class LexerError {
public:
    std::string message;
    std::string line;
    size_t column;
    std::string get_msg() {
        return "Lexer Error: " + message;
    };
    LexerError(std::string message, std::string &input, size_t position) : message(message) {
        auto [line, column] = get_position(input, position);
        this->line = line;
        this->column = column;
        this->message += '\n';
        this->message += line;
        this->message += '\n';
        for (size_t i = 0; i < column; i++) {
            this->message += ' ';
        }
        this->message += '^';
    }
};

bool is_digit(char c);
bool is_letter(char c);


typedef std::expected<Token, LexerError> LexerResult;
class Lexer {
public:
    Lexer(std::string input);
    LexerResult next_token();

    /*
     * Lexing functions
     */
    auto lex_eof() -> std::optional<LexerResult >;
    auto lex_char_map() -> std::optional<LexerResult >;
    auto lex_eq() -> std::optional<LexerResult >;
    auto lex_bang() -> std::optional<LexerResult >;
    auto lex_tick() -> std::optional<LexerResult >;
    auto lex_quote() -> std::optional<LexerResult >;
    auto lex_ident() -> std::optional<LexerResult >;
    auto lex_integral() -> std::optional<LexerResult >;
    auto lex_illegal() -> std::optional<LexerResult >;
    auto run_lexers() -> LexerResult;

private:
    std::string input;
    size_t position;
    size_t read_position;
    char c;

    std::vector<std::function<std::optional<LexerResult>()>> lexers;
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
