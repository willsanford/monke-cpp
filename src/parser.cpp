//
// Created by William Sanford on 7/14/2023.
//
#include "parser.h"

#include <algorithm>

#include "ast.h"
#include "token.h"
#include <format>
#include <functional>
#include <variant>

Parser::Parser(Lexer *l) : l(l) {
  next_token();
  next_token();
}

precedence Parser::peek_precedence() {
  return get_precedence(peek_token.ttype).value_or(precedence::lowest);
}
precedence Parser::cur_precedence() {
  return get_precedence(cur_token.ttype).value_or(precedence::lowest);
}

std::vector<std::string> Parser::get_errors() {
  return errors;
}

void Parser::peek_error(token_t t) {
  std::string msg = "Expected next token to be " + get_token_name(t) + " but got " + get_token_name(peek_token.ttype) + " instead";

  errors.push_back(msg);
}
void Parser::next_token() {
  cur_token = peek_token;
  peek_token = l->next_token();
}

bool Parser::cur_token_is(token_t t) {
  return (cur_token.ttype == t);
}

bool Parser::peek_token_is(token_t t) {
  return (peek_token.ttype == t);
}

bool Parser::expect_peek(token_t t) {
  if (peek_token_is(t)) {
    next_token();
    return true;
  } else {
    peek_error(t);
    return false;
  }
}

std::optional<Statement> Parser::parse_statement() {
  switch (cur_token.ttype) {
    case ::LET:
      return parse_let_statement();
    case ::RETURN:
      return parse_return_statement();
    default:
      return parse_expression_statement();
  }
}

std::optional<Statement> Parser::parse_expression_statement() {
  ExpressionStatement stmt = ExpressionStatement();
  auto expr_opt = parse_expression(precedence::lowest);
  if (peek_token_is(token_t::SEMICOLON)) {
    next_token();
  }
  if (!expr_opt.has_value()) {
    errors.push_back(std::format("Expression could not be formed"));
    return std::nullopt;
  }
  stmt.e = expr_opt.value();
  return stmt;
}

std::optional<Expression> Parser::parse_expression(precedence p) {
  auto left_exp = prefix_parse_fns(cur_token.ttype);
  if (!left_exp.has_value()) {
    return std::nullopt;
  }
  while (!peek_token_is(token_t::SEMICOLON) && p < peek_precedence()) {
    std::cout << "starting next" << std::endl;
    auto infix = infix_parse_fns(cur_token.ttype);
    if (!infix.has_value()) {
      return left_exp;
    }
    std::cout << "starting next2" << std::endl;
    next_token();
    left_exp = infix.value()(left_exp.value());
    std::cout << "starting next3" << std::endl;
    if (!left_exp.has_value()) return std::nullopt;
  }
  std::cout << "starting next4" << std::endl;
  std::cout << left_exp.value() << std::endl;
  return left_exp;
}

std::optional<Expression> Parser::parse_identifier() {
  return {cur_token};
}
std::optional<Expression> Parser::parse_int_literal() {
  auto int_str = cur_token.literal;

  if (int_str.size() == 0 || !std::all_of(int_str.begin(), int_str.end(), ::isdigit)) {
    errors.push_back(std::format("Cannot parse the int {}", int_str));
    return std::nullopt;
  }
  uint64_t val = std::stoull(int_str);
  return IntegerLiteral({cur_token, val});
}

std::optional<Expression> Parser::parse_prefix_expression() {
  auto e = PrefixExpression();
  e.t = Token(cur_token);
  e.op = cur_token.literal;
  next_token();
  auto expr_opt = parse_expression(precedence::prefix);
  if (!expr_opt.has_value()) {
    errors.push_back(std::format("Cannot parse this expression"));
    return std::nullopt;
  }
  e.right = new Expression(expr_opt.value());
  return e;
}

std::optional<Expression> Parser::parse_infix_expression(Expression left) {
  auto infix = InfixExpression();
  infix.t = cur_token;
  infix.op = cur_token.literal;
  infix.left = new Expression(left);

  auto prec = cur_precedence();
  next_token();
  auto expr_opt = parse_expression(prec);
  if (expr_opt.has_value()) {
    infix.right = new Expression(expr_opt.value());
    return infix;
  } else {
    return std::nullopt;
  }
}

std::optional<Expression> Parser::prefix_parse_fns(token_t t) {
  switch (t) {
    case ::IDENT:
      return parse_identifier();
    case ::INT:
      return parse_int_literal();
    case ::BANG:
    case ::MINUS:
      return parse_prefix_expression();
    default:
      errors.push_back(std::format("No prefix parse function for {} found", get_token_name(t)));
      return std::nullopt;
  }
};


std::optional<std::function<std::optional<Expression>(Expression)>> Parser::infix_parse_fns(token_t t) {
  switch (t) {
    case ::PLUS:
    case ::MINUS:
    case ::SLASH:
    case ::ASTERISK:
    case ::EQ:
    case ::NotEq:
    case ::LT:
    case ::GT:
      return [&](Expression e) -> std::optional<Expression> {
        return parse_infix_expression(e);
      };
  }
}
std::optional<Statement> Parser::parse_return_statement() {
  ReturnStatement stmt = ReturnStatement();

  next_token();

  // We're skipping the expression until we encounter a semicolon
  while (!cur_token_is(::SEMICOLON))
    next_token();

  return stmt;
}

std::optional<Statement> Parser::parse_let_statement() {
  LetStatement stmt = LetStatement();
  stmt.token = cur_token;

  if (!expect_peek(::IDENT))
    return std::nullopt;

  stmt.name = Identifier(cur_token.literal);

  if (!expect_peek(::ASSIGN))
    return std::nullopt;

  // TODO :: We're skipping the expressions until we encounter a semicolon
  while (!cur_token_is(::SEMICOLON))
    next_token();

  return stmt;
}

Program Parser::parse_program() {
  Program *p = new Program();

  while (cur_token.ttype != ::EOF_) {
    std::optional<Statement> stmt_opt = parse_statement();
    if (stmt_opt.has_value()) {
      p->statements.emplace_back(stmt_opt.value());
    }

    next_token();
  }
  return *p;
}
