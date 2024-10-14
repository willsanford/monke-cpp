#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include "token.h"
#include "lexer.h"
#define MULTILINE_STRING(s) #s
TEST(Token, BasicTokenTest) {
    std::string input = "=+(){},;";

    std::vector expected_tokens = {
            Token(ASSIGN, '='),
            Token(PLUS, '+'),
            Token(LPAREN, '('),
            Token(RPAREN, ')'),
            Token(LBRACE, '{'),
            Token(RBRACE, '}'),
            Token(COMMA, ','),
            Token(SEMICOLON, ';'),
            Token(EOF_, '\0')
    };

    Lexer l = Lexer(input);

    bool eq = true;
    for (auto t : expected_tokens){
        auto tok_exp = l.next_token();
        if (!tok_exp.has_value()) return;
        // std::cout << tok_exp.value().literal << std::endl;
        auto tok = tok_exp.value();
        ASSERT_EQ(tok.literal, t.literal);
        ASSERT_EQ(tok.ttype, t.ttype);
    }
    ASSERT_TRUE(eq);
}

TEST(Token, AdvancedTokenTest) {
    std::string input = R"(
    [5, 10];
    let a = "Some string";
    let five = 5;
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

    std::vector expected_tokens = {
            Token(LBRACKET, "["),
            Token(INT, "5"),
            Token(COMMA, ","),
            Token(INT, "10"),
            Token(RBRACKET, "]"),
            Token(SEMICOLON, ";"),
            Token(LET, "let"),
            Token(IDENT, "a"),
            Token(ASSIGN, "="),
            Token(STRING, "Some string"),
            Token(SEMICOLON, ";"),
            Token(LET, "let"),
            Token(IDENT, "five"),
            Token(ASSIGN, "="),
            Token(INT, "5"),
            Token(SEMICOLON, ";"),
            Token(LET, "let"),
            Token(IDENT, "ten"),
            Token(ASSIGN, "="),
            Token(INT, "10"),
            Token(SEMICOLON, ";"),
            Token(LET, "let"),
            Token(IDENT, "add"),
            Token(ASSIGN, "="),
            Token(FUNCTION, "fn"),
            Token(LPAREN, "("),
            Token(IDENT, "x"),
            Token(COMMA, ","),
            Token(IDENT, "y"),
            Token(RPAREN, ")"),
            Token(LBRACE, "{"),
            Token(IDENT, "x"),
            Token(PLUS, "+"),
            Token(IDENT, "y"),
            Token(SEMICOLON, ";"),
            Token(RBRACE, "}"),
            Token(LET, "let"),
            Token(IDENT, "result"),
            Token(ASSIGN, "="),
            Token(IDENT, "add"),
            Token(LPAREN, "("),
            Token(IDENT, "five"),
            Token(COMMA, ","),
            Token(IDENT, "ten"),
            Token(RPAREN, ")"),
            Token(SEMICOLON, ";"),
            Token(BANG, "!"),
            Token(MINUS, "-"),
            Token(SLASH, "/"),
            Token(ASTERISK, "*"),
            Token(INT, "5"),
            Token(SEMICOLON, ";"),
            Token(INT, "5"),
            Token(LT, "<"),
            Token(INT, "10"),
            Token(GT, ">"),
            Token(INT, "5"),
            Token(SEMICOLON, ";"),
            Token(IF, "if"),
            Token(LPAREN, "("),
            Token(INT, "5"),
            Token(LT, "<"),
            Token(INT, "10"),
            Token(RPAREN, ")"),
            Token(LBRACE, "{"),
            Token(RETURN, "return"),
            Token(TRUE, "true"),
            Token(SEMICOLON, ";"),
            Token(RBRACE, "}"),
            Token(ELSE, "else"),
            Token(LBRACE, "{"),
            Token(RETURN, "return"),
            Token(FALSE, "false"),
            Token(SEMICOLON, ";"),
            Token(RBRACE, "}"),
            Token(INT, "10"),
            Token(EQ, "=="),
            Token(INT, "10"),
            Token(SEMICOLON, ";"),
            Token(INT, "10"),
            Token(NotEq, "!="),
            Token(INT, "9"),
            Token(SEMICOLON, ";"),
            Token(EOF_, '\0'),
    };

    Lexer l = Lexer(input);

    for (auto t : expected_tokens){
        auto tok_exp = l.next_token();
        if (!tok_exp.has_value()) return;
        auto tok = tok_exp.value();
        ASSERT_EQ(tok.literal, t.literal);
        ASSERT_EQ(tok.ttype, t.ttype);
    }
}
