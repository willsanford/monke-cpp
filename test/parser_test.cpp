#include <gtest/gtest.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"

void print_errors(Parser &p) {
  if (!p.get_errors().empty()) {
    for (auto e: p.get_errors()) {
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

  if (!p.get_errors().empty()) {
    for (auto e: p.get_errors()) {
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
    ASSERT_TRUE(is_statement_type<LetStatement>(s));
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
    ASSERT_TRUE(is_statement_type<ReturnStatement>(s));
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
  std::vector<std::tuple<std::string, std::string, uint64_t>> tests = {
          {"!5;", "!", 5},
          {"-15;", "-", 15}};

  for (const auto &[input, prefix, val]: tests) {
    Parser parser = Parser(new Lexer(input));
    Program p = parser.parse_program();
    ASSERT_EQ(p.statements.size(), 1);
    auto exp_statement_opt = p.statements[0];

    // Create the correct expression statement
    auto pre = PrefixExpression(
            prefix,
            new Expression(IntegerLiteral(Token(token_t::INT, std::to_string(val)), val)));
    auto e = ExpressionStatement(pre);
    auto exp_statement = std::get<ExpressionStatement>(std::get<Statement>(exp_statement_opt));
    ASSERT_EQ(exp_statement, e);
  }
}

TEST(Parser, InfixExpressionTest) {
  std::vector<std::tuple<std::string, uint64_t, std::string, uint64_t>> tests = {
          {"5 + 5;", 5, "+", 5},
          {"5 - 5;", 5, "-", 5},
          {"5 * 5;", 5, "*", 5},
          {"5 / 5;", 5, "/", 5},
          {"5 > 5;", 5, ">", 5},
          {"5 < 5;", 5, "<", 5},
          {"5 == 5;", 5, "==", 5},
          {"5 != 5;", 5, "!=", 5}};

  for (const auto &[input, left_val, op, right_val]: tests) {
    Program p = Parser(new Lexer(input)).parse_program();
    ASSERT_EQ(p.statements.size(), 1);
    auto exp_statement_opt = p.statements[0];

    // Create the correct expression statement
    auto inf = InfixExpression();
    auto e = ExpressionStatement(inf);
    auto exp_statement = std::get<ExpressionStatement>(std::get<Statement>(exp_statement_opt));
    ASSERT_EQ(exp_statement, e);
  }
}
