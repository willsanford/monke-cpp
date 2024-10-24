//
// Created by William Sanford on 7/14/2023.
//
#include "parser.h"

#include <algorithm>

#include "ast.h"
#include "token.h"
#include <format>
#include <functional>
#include <optional>
#include <variant>

Parser::Parser(Lexer *l) : l(l) {
  auto t = l->next_token();
  while (t.has_value()) {
    tokens.push_back(t.value());
    if (t.value().ttype == EOF_) break;
    t = l->next_token();
  }
  if (!t.has_value()) {
    error = t.error();
    return;
  }
  cur_token = tokens[cur_token_index];
  peek_token = tokens[peek_token_index];
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
  cur_token_index++;
  cur_token_index = cur_token_index % tokens.size();
  peek_token_index++;
  peek_token_index = peek_token_index % tokens.size();
  cur_token = tokens[cur_token_index];
  peek_token = tokens[peek_token_index];
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
  }

  peek_error(t);
  return false;
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

std::optional<Expression> Parser::parse_string() {
  return StringLiteral(cur_token, cur_token.literal);
}

std::optional<Expression> Parser::parse_char() {
  return CharLiteral(cur_token, cur_token.literal[0]);
}

std::optional<Expression> Parser::parse_expression(precedence p) {
  auto left_exp_opt = prefix_parse_fns(cur_token.ttype);
  if (!left_exp_opt.has_value()) {
    return std::nullopt;
  }
  auto left_exp = left_exp_opt.value();
  while (!peek_token_is(token_t::SEMICOLON) && p < peek_precedence()) {
    auto infix_opt = infix_parse_fns(peek_token.ttype);
    if (!infix_opt.has_value()) {
      return left_exp;
    }
    auto infix = infix_opt.value();
    next_token();
    left_exp_opt = infix(left_exp);
    if (!left_exp_opt.has_value()) return std::nullopt;
    left_exp = left_exp_opt.value();
  }
  return left_exp;
}

std::optional<Expression> Parser::parse_identifier() {
  return {cur_token};
}

std::optional<Expression> Parser::parse_boolean() {
  return BooleanLiteral(cur_token, cur_token_is(::TRUE));
}

std::optional<Expression> Parser::parse_int_literal() {
  auto int_str = cur_token.literal;

  if (int_str.size() == 0 || !std::all_of(int_str.begin(), int_str.end(), ::isdigit)) {
    errors.push_back(std::format("Cannot parse the int {}", int_str));
    return std::nullopt;
  }
  int64_t val = std::stoull(int_str);
  return IntegerLiteral({cur_token, val});
}

std::optional<Expression> Parser::parse_array_literal() {
  /*
  std::vector<std::shared_ptr<Expression>> values;

  // Move past bracket
  next_token();

  if (peek_token_is(RBRACKET)) {
    next_token();
    return ArrayLiteral(cur_token, values);
  }

  while (peek_token_is(RBRACKET) == false) {
    auto value = parse_expression(precedence::lowest);
    if (!value.has_value()) return std::nullopt;
    auto ptr = // TODO : Figure out how to do this correctly
    values.push_back(value.value());
    if (peek_token_is(RBRACKET)) break;
    next_token();
    if (!expect_peek(COMMA)) return std::nullopt;
  }
  */
  return std::nullopt;
}

std::optional<Expression> Parser::parse_float() {
  auto str = cur_token.literal;
  if (str.size() == 0 || !std::all_of(str.begin(), str.end(), [](auto c) -> bool { return std::isdigit(c) || c == '.' || c == '-'; })) {
    errors.push_back(std::format("Cannot parse the float {}", str));
    return std::nullopt;
  }

  double val = std::stod(str);
  return FloatLiteral({cur_token, val});
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
  }
  return std::nullopt;
}

std::optional<Expression> Parser::parse_grouped_expression() {
  next_token();

  auto exp = parse_expression(precedence::lowest);
  if (!expect_peek(token_t::RPAREN)) {
    return std::nullopt;
  }
  return exp;
}

std::optional<BlockStatement> Parser::parse_block_statement() {
  std::vector<std::pair<StatementType, void *>> stmts;
  Token bt = cur_token;
  next_token();

  while (!cur_token_is(RBRACE) && !cur_token_is(EOF_)) {
    auto stmt = parse_statement();
    if (stmt.has_value()) {
      if (std::holds_alternative<LetStatement>(stmt.value())) {
        stmts.push_back({StatementType::LS, new Statement(stmt.value())});
      }
      if (std::holds_alternative<ReturnStatement>(stmt.value())) {
        stmts.push_back({StatementType::RS, new Statement(stmt.value())});
      }
      if (std::holds_alternative<BlockStatement>(stmt.value())) {
        stmts.push_back({StatementType::BS, new Statement(stmt.value())});
      }
      if (std::holds_alternative<ExpressionStatement>(stmt.value())) {
        stmts.push_back({StatementType::ES, new Statement(stmt.value())});
      }
    }
    next_token();
  }
  return BlockStatement(bt, stmts);
}

std::optional<Expression> Parser::parse_if_expression() {
  auto exp = IfExpression();
  exp.t = cur_token;

  if (!expect_peek(token_t::LPAREN)) {
    return std::nullopt;
  }

  next_token();
  auto cond = parse_expression(precedence::lowest);
  if (!cond.has_value()) {
    errors.push_back("Could not parse the condition in the if statement");
    return std::nullopt;
  }
  exp.condition = new Expression(cond.value());

  if (!expect_peek(RPAREN)) {
    return std::nullopt;
  }

  if (!expect_peek(LBRACE)) {
    return std::nullopt;
  }

  auto cons = parse_block_statement();
  if (!cons.has_value()) {
    return std::nullopt;
  }

  exp.consequence = new BlockStatement(cons.value());

  if (peek_token_is(ELSE)) {
    next_token();
    if (!expect_peek(LBRACE)) {
      return std::nullopt;
    }

    auto alt = parse_block_statement();
    if (!alt.has_value()) {
      return std::nullopt;
    }
    exp.alternative = new BlockStatement(alt.value());
  } else {
    exp.alternative = std::nullopt;
  }

  return exp;
}

std::optional<std::vector<Identifier *>> Parser::parse_function_parameters() {
  std::vector<Identifier *> idents;
  if (peek_token_is(RPAREN)) {
    next_token();
    return std::optional(idents);
  }
  next_token();

  idents.emplace_back(new Identifier(cur_token));

  while (peek_token_is(COMMA)) {
    next_token();
    next_token();
    idents.emplace_back(new Identifier(cur_token));
  }

  if (!expect_peek(RPAREN)) {
    return std::nullopt;
  }

  return idents;
};
std::optional<Expression> Parser::parse_function_expression() {
  Token ft = cur_token;

  if (!expect_peek(LPAREN)) return std::nullopt;

  auto params = parse_function_parameters();
  if (!params.has_value()) return std::nullopt;

  if (!expect_peek(LBRACE)) return std::nullopt;

  auto body = parse_block_statement();
  if (!body.has_value()) return std::nullopt;

  return FunctionLiteral(ft, params.value(), new BlockStatement(body.value()));
};

std::optional<Expression> Parser::prefix_parse_fns(token_t t) {
  switch (t) {
    case ::IDENT:
      return parse_identifier();
    case ::TRUE:
    case ::FALSE:
      return parse_boolean();
    case ::INT:
      return parse_int_literal();
    case ::BANG:
    case ::MINUS:
      return parse_prefix_expression();
    case ::LPAREN:
      return parse_grouped_expression();
    case ::IF:
      return parse_if_expression();
    case ::STRING:
      return parse_string();
    case ::CHAR:
      return parse_char();
    case ::FLOAT:
      return parse_float();
    case ::FUNCTION:
      return parse_function_expression();
    case ::RBRACKET:
      return parse_array_literal();
    default:
      errors.push_back(std::format("No prefix parse function for {} found", get_token_name(t)));
      return std::nullopt;
  }
};

std::optional<Expression> Parser::parse_call_expression(Expression left) {
  Token t = cur_token;
  auto args = parse_call_arguments();
  if (!args.has_value()) return std::nullopt;
  return CallExpression(t, new Expression(left), args.value());
}

std::optional<std::vector<Expression *>> Parser::parse_call_arguments() {
  std::vector<Expression *> args;
  if (peek_token_is(RPAREN)) {
    next_token();
    return args;
  }

  next_token();
  auto arg = parse_expression(precedence::lowest);
  if (!arg.has_value()) return std::nullopt;
  args.push_back(new Expression(arg.value()));

  while (peek_token_is(COMMA)) {
    next_token();
    next_token();
    arg = parse_expression(precedence::lowest);
    if (!arg.has_value()) return std::nullopt;
    args.push_back(new Expression(arg.value()));
  }

  if (!expect_peek(RPAREN)) return std::nullopt;

  return args;
}

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
    case ::LPAREN:
      return [&](Expression e) -> std::optional<Expression> {
        return parse_call_expression(e);
      };
    default:
      return std::nullopt;
  }
}

std::optional<Statement> Parser::parse_return_statement() {
  ReturnStatement stmt = ReturnStatement();
  stmt.token = cur_token;

  next_token();

  auto expr = parse_expression(precedence::lowest);
  if (!expr.has_value()) return std::nullopt;
  if (peek_token_is(SEMICOLON)) next_token();
  stmt.return_value = expr.value();
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

  next_token();
  auto value = parse_expression(precedence::lowest);
  if (!value.has_value()) return std::nullopt;
  if (peek_token_is(SEMICOLON)) next_token();

  stmt.value = value.value();

  return stmt;
}

Program Parser::parse_program() {
  Program *p = new Program();
  if (error.has_value()) {
    p->error = error;
    return *p;
  }

  while (cur_token.ttype != EOF_) {
    std::optional<Statement> stmt_opt = parse_statement();
    if (stmt_opt.has_value()) {
      p->statements.emplace_back(stmt_opt.value());
    }

    next_token();
  }
  return *p;
}
