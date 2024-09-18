//
// Created by William Sanford on 7/14/2023.
//

#ifndef MONKE_CPP_PARSER_H
#define MONKE_CPP_PARSER_H

#include <vector>
#include <functional>
#include <optional>
#include "token.h"
#include "lexer.h"
#include "ast.h"
class Parser {
public:
    Parser(Lexer* l);
    Lexer *l;

    Token cur_token;
    Token peek_token;

    std::vector<std::string> errors;

    void next_token();
    bool cur_token_is(token_t t);
    bool peek_token_is(token_t t);
    bool expect_peek(token_t t);


    std::vector<std::string> get_errors();
    
    void peek_error(token_t t);
    std::optional<Statement> parse_statement();
    std::optional<Statement> parse_let_statement();
    std::optional<Statement> parse_return_statement();
    std::optional<Statement> parse_expression_statement();

    std::optional<Expression> parse_expression(precedence p);
    std::optional<Expression> parse_integer_literal();
    std::optional<std::function<Expression(void)>> prefix_parse_fns(token_t t);
    Expression infix_parse_fns(token_t t, Expression ex);

    Program parse_program();
};


#endif //MONKE_CPP_PARSER_H
