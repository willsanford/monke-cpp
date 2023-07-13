#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include "token.h"
#include "lexer.h"
#define MULTILINE_STRING(s) #s
TEST(Token, BasicTokenTest) {
    std::string input = "=+(){},;";

    std::vector<Token> expected_tokens = {
            Token(token_t::ASSIGN, '='),
            Token(token_t::PLUS, '+'),
            Token(token_t::LPAREN, '('),
            Token(token_t::RPAREN, ')'),
            Token(token_t::LBRACE, '{'),
            Token(token_t::RBRACE, '}'),
            Token(token_t::COMMA, ','),
            Token(token_t::SEMICOLON, ';'),
            Token(token_t::EOF_, '\0')
    };

    Lexer l = Lexer(input);

    bool eq = true;
    for (auto t : expected_tokens){
        Token tok = l.next_token();
        eq &= (tok.literal == t.literal);
        eq &= (tok.ttype == t.ttype);
    }
    ASSERT_TRUE(eq);
}

TEST(Token, AdvancedTokenTest) {
    std::string input = R"(let five = 5;
    let ten = 10;
    let add = fn(x, y){
        x + y;
    }
    let result = add(five, ten);
    !-/*5;
    5 < 10 > 5;
    if (5 < 10){
        return true;
    }else{
        return false;
    }
    10 == 10;
    10 != 9;)";

    std::vector<Token> expected_tokens = {
            Token(token_t::LET, "let"),
            Token(token_t::IDENT, "five"),
            Token(token_t::ASSIGN, "="),
            Token(token_t::INT, "5"),
            Token(token_t::SEMICOLON, ";"),
            Token(token_t::LET, "let"),
            Token(token_t::IDENT, "ten"),
            Token(token_t::ASSIGN, "="),
            Token(token_t::INT, "10"),
            Token(token_t::SEMICOLON, ";"),
            Token(token_t::LET, "let"),
            Token(token_t::IDENT, "add"),
            Token(token_t::ASSIGN, "="),
            Token(token_t::FUNCTION, "fn"),
            Token(token_t::LPAREN, "("),
            Token(token_t::IDENT, "x"),
            Token(token_t::COMMA, ","),
            Token(token_t::IDENT, "y"),
            Token(token_t::RPAREN, ")"),
            Token(token_t::LBRACE, "{"),
            Token(token_t::IDENT, "x"),
            Token(token_t::PLUS, "+"),
            Token(token_t::IDENT, "y"),
            Token(token_t::SEMICOLON, ";"),
            Token(token_t::RBRACE, "}"),
            Token(token_t::LET, "let"),
            Token(token_t::IDENT, "result"),
            Token(token_t::ASSIGN, "="),
            Token(token_t::IDENT, "add"),
            Token(token_t::LPAREN, "("),
            Token(token_t::IDENT, "five"),
            Token(token_t::COMMA, ","),
            Token(token_t::IDENT, "ten"),
            Token(token_t::RPAREN, ")"),
            Token(token_t::SEMICOLON, ";"),
            Token(token_t::BANG, "!"),
            Token(token_t::MINUS, "-"),
            Token(token_t::SLASH, "/"),
            Token(token_t::ASTERISK, "*"),
            Token(token_t::INT, "5"),
            Token(token_t::SEMICOLON, ";"),
            Token(token_t::INT, "5"),
            Token(token_t::LT, "<"),
            Token(token_t::INT, "10"),
            Token(token_t::GT, ">"),
            Token(token_t::INT, "5"),
            Token(token_t::SEMICOLON, ";"),
            Token(token_t::IF, "if"),
            Token(token_t::LPAREN, "("),
            Token(token_t::INT, "5"),
            Token(token_t::LT, "<"),
            Token(token_t::INT, "10"),
            Token(token_t::RPAREN, ")"),
            Token(token_t::LBRACE, "{"),
            Token(token_t::RETURN, "return"),
            Token(token_t::TRUE, "true"),
            Token(token_t::SEMICOLON, ";"),
            Token(token_t::RBRACE, "}"),
            Token(token_t::ELSE, "else"),
            Token(token_t::LBRACE, "{"),
            Token(token_t::RETURN, "return"),
            Token(token_t::FALSE, "false"),
            Token(token_t::SEMICOLON, ";"),
            Token(token_t::RBRACE, "}"),
            Token(token_t::INT, "10"),
            Token(token_t::EQ, "=="),
            Token(token_t::INT, "10"),
            Token(token_t::SEMICOLON, ";"),
            Token(token_t::INT, "10"),
            Token(token_t::NotEq, "!="),
            Token(token_t::INT, "9"),
            Token(token_t::SEMICOLON, ";"),
            Token(token_t::EOF_, '\0'),
    };

    Lexer l = Lexer(input);

    bool eq = true;
    for (auto t : expected_tokens){
        Token tok = l.next_token();
        eq &= (tok.literal == t.literal);
        eq &= (tok.ttype == t.ttype);
    }
    ASSERT_TRUE(eq);
}
