#include <gtest/gtest.h>

#include <string>
#include <iostream>

#include "lexer.h"
#include "token.h"
#include "ast.h"
#include "parser.h"

void print_errors(Parser &p){
  if (!p.get_errors().empty()){
    for (auto e: p.get_errors()){
      std::cout << "Error: " << e << std::endl;
    }
  }
}

TEST(Token, BasicParserTest) {
  std::string input = R"(
  let x = 5;
  let y = 10; 
  let foobar = 838383;)";

  Lexer *l = new Lexer(input);
  Parser p = Parser(l);

  auto program = p.parse_program();
  
  if (!p.get_errors().empty()){
    for (auto e: p.get_errors()){
      std::cout << "Error: " << e << std::endl;
    }
  }

  ASSERT_EQ(p.get_errors().size(), 0);

  
  ASSERT_EQ(program.statements.size(), 3);
}
TEST(Token, LetStatementTest) {
  std::string input = R"(
  let x = 5;
  let y = 10; 
  let foobar = 838383;)";

  Lexer *l = new Lexer(input);
  Parser p = Parser(l);

  Program program = p.parse_program();

  print_errors(p);
  ASSERT_EQ(program.statements.size(), 3);
  for (auto s: program.statements) ASSERT_EQ(s.t, NodeType::LetStatement_);

}

TEST(Token, ReturnStatementTest) {
  std::string input = R"(
  return 5;
  return 10; 
  return 234234234234;)";

  Lexer *l = new Lexer(input);
  Parser p = Parser(l);

  Program program = p.parse_program();

  print_errors(p);
  ASSERT_EQ(program.statements.size(), 3);
  for (auto s: program.statements) ASSERT_EQ(s.t, NodeType::ReturnStatement_);
}
