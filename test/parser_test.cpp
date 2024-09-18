#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include <variant>

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

TEST(Parser, BasicParserTest) {
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
TEST(Parser, LetStatementTest) {
  std::string input = R"(
  let x = 5;
  let y = 10; 
  let foobar = 838383;)";

  Lexer *l = new Lexer(input);
  Parser p = Parser(l);

  Program program = p.parse_program();

  print_errors(p);
  ASSERT_EQ(program.statements.size(), 3);
  for (auto s: program.statements)
    ASSERT_TRUE(is_node_type<LetStatement>(s));
}

TEST(Parser, ReturnStatementTest) {
  std::string input = R"(
  return 5;
  return 10; 
  return 234234234234;)";

  auto *l = new Lexer(input);
  Parser p = Parser(l);

  Program program = p.parse_program();

  print_errors(p);
  ASSERT_EQ(program.statements.size(), 3);
  for (auto s: program.statements)
    ASSERT_TRUE(is_node_type<ReturnStatement>(s));
}

TEST(Parser, IdentifierExpressionTest) {
  std::string input = "foobar";
  auto *l = new Lexer(input);
  Parser p = Parser(l);

  Program program = p.parse_program();
  ASSERT_EQ(program.statements.size(), 1);

  // TODO: Check that this comes out valid
}
TEST(Parser, IntegerLiteralExpressionTest) {
  std::string input = "5;";
  auto *l = new Lexer(input);
  Parser p = Parser(l);

  Program program = p.parse_program();
  ASSERT_EQ(program.statements.size(), 1);
  std::vector<Node> program_statements = {
    ExpressionStatement(IntegerLiteral(Token(token_t::INT, "5"), 5)),
  };
  ASSERT_EQ(program.statements, program_statements);
  // TODO: Check that this comes out valid
}

TEST(Parser, PrefixExpressionTest) {
  std::string input = "5;";
  ASSERT_EQ(1, 1);
}