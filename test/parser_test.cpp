#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include "lexer.h"
#include "token.h"
#include "ast.h"
#include "parser.h"


TEST(Token, BasicTokenTest) {
    std::string input = R"(
    let x = 5;
    let y = 10; 
    let foobar = 838383;)";


    Lexer *l = new Lexer(input);
    Parser p = Parser(l);

    auto program = p.parse_program();
    
    ASSERT_EQ(program.statements.size(), 3);
}
