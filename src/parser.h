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
    size_t cur_token_index = 0;
    Token peek_token;
    size_t peek_token_index = 1;

    std::vector<Token> tokens = {};
    std::vector<std::string> errors;
    std::optional<LexerError> error = std::nullopt;

    void next_token();
    bool cur_token_is(token_t t);
    bool peek_token_is(token_t t);
    bool expect_peek(token_t t);
    precedence peek_precedence();
    precedence cur_precedence();

    std::vector<std::string> get_errors();
    
    void peek_error(token_t t);
    std::optional<Statement> parse_statement();
    std::optional<Statement> parse_let_statement();
    std::optional<Statement> parse_return_statement();
    std::optional<Statement> parse_expression_statement();
    std::optional<BlockStatement> parse_block_statement();

    std::optional<Expression> parse_expression(precedence p);
    std::optional<Expression> prefix_parse_fns(token_t t);
    std::optional<Expression> parse_identifier();
    std::optional<Expression> parse_boolean();
    std::optional<Expression> parse_string();
    std::optional<Expression> parse_char();
    std::optional<Expression> parse_float();
    std::optional<Expression> parse_array_literal();
    std::optional<Expression> parse_int_literal();
    std::optional<Expression> parse_prefix_expression();
    std::optional<Expression> parse_grouped_expression();
    std::optional<Expression> parse_if_expression();
    std::optional<Expression> parse_function_expression();
    std::optional<std::vector<Identifier*>> parse_function_parameters();
    std::optional<Expression> parse_infix_expression(Expression left);
    std::optional<Expression> parse_call_expression(Expression left);
    std::optional<std::vector<Expression *>> parse_call_arguments();
    std::optional<std::function<std::optional<Expression>(Expression)>> infix_parse_fns(token_t t);

    Program parse_program();
};


#endif //MONKE_CPP_PARSER_H
