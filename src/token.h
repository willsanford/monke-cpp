//
// Created by wsanf on 7/12/23.
//

#ifndef MONKE_CPP_TOKEN_H
#define MONKE_CPP_TOKEN_H

#include <string>
#include <unordered_map>

enum token_t {
    ILLEGAL,
    EOF_, // To not conflict with the EOF define

    // Identifiers and Literals
    IDENT,
    INT,

    // Operators
    EQ,
    NotEq,
    PLUS,
    ASSIGN,
    MINUS,
    BANG,
    ASTERISK,
    SLASH,
    LT,
    GT,

    // Delimiters
    COMMA,
    SEMICOLON,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE, // Keywords
    FUNCTION,
    LET,
    TRUE,
    FALSE,
    IF,
    ELSE,
    RETURN,
};

std::string get_token_name(token_t t);

class Token {
public:
    token_t ttype;
    std::string literal;

    /**
     * Default token constructor
     */
    Token();

    /**
     * Constructor from a token type and literal
     * @param t
     * @param l
     */
    Token(token_t t, std::string l);

    Token(token_t t, char l);

    /**
     * Constructor with ident lookup initialization
     * @param l
     */
    Token(std::string l);

    /**
     * If the literal exists in the token map, then return the corresponding token, otherwise return IDENT
     * @param literal
     * @return
     */
    token_t lookup_ident(std::string literal_);

    bool operator==(const Token& t);
private:
};

#endif //MONKE_CPP_TOKEN_H
