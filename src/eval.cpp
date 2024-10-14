//
// Created by Sanford, William on 9/21/24.
//

#include <cstring>
#include <format>
#include <optional>
#include <typeinfo>
#include <variant>

#include "ast.h"
#include "eval.h"
#include "lexer.h"
#include "object.h"
#include "parser.h"

ObjectResult eval_program(std::string input) {
  auto *l = new Lexer(input);
  Parser p = Parser(l);
  Program program = p.parse_program();
  if (program.error.has_value()) {
    return std::unexpected(program.error.value());
  }
  return evaluate(program);
}

ObjectResult eval(Statement &node, std::shared_ptr<Environment> env) {
  return std::visit([&](auto &&arg) { return eval(arg, env); }, node);
}

ObjectResult eval(LetStatement &node, std::shared_ptr<Environment> env) {
  return eval(node.value, env).and_then([&](auto value) -> ObjectResult { env->set(node.name.string(), value); return ObjectResult (Null()); });
}

ObjectResult eval(BlockStatement &node, std::shared_ptr<Environment> env) {
  auto result = ObjectResult(Object(Null()));
  auto statements = node.get_statements();
  for (auto &stmt: statements) {
    result = eval(stmt, env);
    if (!result.has_value()) return result;
    if (std::holds_alternative<ReturnObject>(result.value())) {
      return result;
    }
  }
  return result;
}

ObjectResult eval(ReturnStatement &node, std::shared_ptr<Environment> env) {
  return eval(node.return_value, env).and_then([&](auto obj) { return ObjectResult(ReturnObject(new Object(obj))); });
}

bool is_truthy(Object obj) {
  if (std::holds_alternative<Boolean>(obj)) {
    return std::get<Boolean>(obj).value;
  }
  if (std::holds_alternative<Null>(obj)) {
    return false;
  }
  return true;
}

ObjectResult eval(IfExpression &node, std::shared_ptr<Environment> env) {
  auto condition_obj = eval(*node.condition, env);
  if (condition_obj.has_value() && is_truthy(condition_obj.value())) {
    return eval(*node.consequence, env);
  }

  if (!node.alternative.has_value()) {
    return Null();
  }

  return eval(*node.alternative.value(), env);
};

ObjectResult eval(Expression &node, std::shared_ptr<Environment> env) {
  return std::visit([&](auto &&arg) { return eval(arg, env); }, node);
}

ObjectResult eval(ExpressionStatement &node, std::shared_ptr<Environment> env) {
  return eval(node.e, std::move(env));
}

ObjectResult eval(IntegerLiteral &node, std::shared_ptr<Environment> env) {
  return Integer(node.val);
}

ObjectResult eval(FloatLiteral &node, std::shared_ptr<Environment> env) {
  return Float(node.val);
}

ObjectResult eval(StringLiteral &node, std::shared_ptr<Environment> env) {
  return String(node.value);
}

ObjectResult eval(CharLiteral &node, std::shared_ptr<Environment> env) {
  return Char(node.value);
}

ObjectResult eval(BooleanLiteral &node, std::shared_ptr<Environment> env) {
  return Boolean(node.value);
}

ObjectResult eval(PrefixExpression &node, std::shared_ptr<Environment> env) {
  return eval(*node.right, env).and_then([&](auto right) {
    return eval_prefix_expression(node.op, right, env);
  });
};

ObjectResult eval(InfixExpression &node, std::shared_ptr<Environment> env) {
  using ResultObjectPair = std::expected<std::pair<Object, Object>, Error>;
  return eval(*node.left, env).and_then([&](auto left) -> ResultObjectPair {
                                return eval(*node.right, env).and_then([&](auto right) -> ResultObjectPair {
                                  return std::make_pair(left, right);
                                });
                              })
          .and_then([&](auto obj) -> ObjectResult {
            // TODO : Can we destructure this?
            auto [left, right] = obj;
            if (left.index() != right.index()) {
              auto err = std::format("type mismatch: {} {} {}", get_type_name(left), node.op, get_type_name(right));
              return std::unexpected(TypeError(err));
            }
            return eval_infix_expression(node.op, left, right, env);
          });
};
ObjectResult eval(Identifier &node, std::shared_ptr<Environment> env) {
  return env->get(node.string());
}


ObjectResult eval(FunctionLiteral &node, std::shared_ptr<Environment> env) {
  std::vector<Identifier> params;
  std::transform(node.parameters.begin(), node.parameters.end(), std::back_inserter(params), [](Identifier *i) { return *i; });
  return Function(params, *node.body, env);
}

std::vector<ObjectResult> eval_expressions(std::vector<Expression> &expressions, std::shared_ptr<Environment> env) {
  std::vector<ObjectResult> results;
  for (auto ex: expressions) {
    auto res = eval(ex, env);
    results.push_back(res);
    if (!res.has_value()) break;
  }
  return results;
}

ObjectResult eval(CallExpression &node, std::shared_ptr<Environment> env) {
  // Either functionliteral or ident
  auto res = eval(*node.function, env);
  if (!res.has_value()) return res;
  std::vector<Expression> arg_exes;
  std::transform(node.arguments.begin(), node.arguments.end(), std::back_inserter(arg_exes), [](Expression *e) { return *e; });
  auto args_ex = eval_expressions(arg_exes, env);
  if (!args_ex.back().has_value()) return args_ex.back();

  std::vector<Object> args;
  std::transform(args_ex.begin(), args_ex.end(), std::back_inserter(args), [](const auto a) { return a.value(); });

  if (!std::holds_alternative<Function>(res.value())) {
    unimplemented();// If this happens something has gone wrong
  }
  auto fn = std::get<Function>(res.value());
  return apply_function(fn, args);
}

std::shared_ptr<Environment> enclose_env(std::shared_ptr<Environment> outer) {
  std::shared_ptr<Environment> env = std::make_shared<Environment>(Environment());
  env->outer = outer;
  return env;
}

std::shared_ptr<Environment> extend_function_env(Function &fn, std::vector<Object> &args) {
  auto env = enclose_env(fn.env);
  if (fn.parameters.size() != args.size()) {
    unimplemented();// This should not happen
  }

  for (size_t i = 0; i < args.size(); i++) {
    env->set(fn.parameters.at(i).string(), args.at(i));
  }
  return env;
}

ObjectResult apply_function(Function &fn, std::vector<Object> &args) {
  auto extended_env = extend_function_env(fn, args);
  auto evaluated = eval(fn.body, extended_env);
  return unwrap_return_value(evaluated);
}

ObjectResult unwrap_return_value(ObjectResult &obj_res) {
  return obj_res.and_then([&](auto &obj) {
    if (std::holds_alternative<ReturnObject>(obj)) {
      return ObjectResult(*std::get<ReturnObject>(obj).value);
    }
    return ObjectResult(obj);
  });
}

ObjectResult evaluate(Program &node) {
  auto env = std::make_shared<Environment>(Environment());
  auto result = ObjectResult(Object(Null()));
  for (auto &stmt: node.statements) {
    result = eval(stmt, env);
    if (!result.has_value()) return result;
    if (std::holds_alternative<ReturnObject>(result.value())) {
      return *std::get<ReturnObject>(result.value()).value;
    }
  }
  return result;
}

ObjectResult eval_infix_expression(std::string &op, Object &left, Object &right, std::shared_ptr<Environment> env) {
  /*
   * Infix operators for booleans
   */
  if (std::holds_alternative<Boolean>(left) && std::holds_alternative<Boolean>(right)) {
    if (op == "==") {
      return Boolean(std::get<Boolean>(left).value == std::get<Boolean>(right).value);
    }
    if (op == "!=") {
      return Boolean(std::get<Boolean>(left).value != std::get<Boolean>(right).value);
    }
  }

  /*
   * Infix operators for strings
   */
  if (std::holds_alternative<String>(left) && std::holds_alternative<String>(right)) {
    if (op == "==") {
      return Boolean(std::get<String>(left).value == std::get<String>(right).value);
    }
    if (op == "+") {
      return String(std::get<String>(left).value + std::get<String>(right).value);
    }
  }

  /*
   * Infix for chars
   */
  if (std::holds_alternative<Char>(left) && std::holds_alternative<Char>(right)) {
    if (op == "==") {
      return Boolean(std::get<Char>(left).value == std::get<Char>(right).value);
    }

    // Two chars becomes a string
    if (op == "+") {
      return String(std::string({std::get<Char>(left).value, std::get<Char>(right).value}));
    }
  }


  #define OP(operator, t) \
    if (strcmp(op.c_str(), #operator) == 0) { return t(left_val.value operator right_val.value); }
  /*
   * Infix operators for integers
   */
  if (std::holds_alternative<Integer>(left) && std::holds_alternative<Integer>(right)) {
    auto left_val = std::get<Integer>(left);
    auto right_val = std::get<Integer>(right);

    OP(+, Integer);
    OP(-, Integer);
    OP(*, Integer);
    OP(/, Integer);
    OP(+, Integer);
    OP(>, Boolean);
    OP(>=, Boolean);
    OP(<, Boolean);
    OP(<=, Boolean);
  }

  /*
   * Infix operators for floats
   */
  if (std::holds_alternative<Float>(left) && std::holds_alternative<Float>(right)) {
    auto left_val = std::get<Float>(left);
    auto right_val = std::get<Float>(right);

    OP(+, Float);
    OP(-, Float);
    OP(*, Float);
    OP(/, Float);
    OP(+, Float);
    OP(>, Boolean);
    OP(>=, Boolean);
    OP(<, Boolean);
    OP(<=, Boolean);
  }
  auto err = std::format("unknown operator: {} {} {}", get_type_name(left), op, get_type_name(right));
  return std::unexpected(TypeError(err));
}

ObjectResult eval_prefix_expression(std::string &op, Object &right, std::shared_ptr<Environment> env) {
  if (op == "!") {
    if (auto *b = std::get_if<Boolean>(&right)) {
      return Boolean(!b->value);
    }
  }
  if (op == "-") {
    if (auto *i = std::get_if<Integer>(&right)) {
      return Integer(-i->value);
    }
    if (auto *i = std::get_if<Float>(&right)) {
      return Float(-i->value);
    }
  }
  std::string err = std::format("unknown operator: {}{}", op, get_type_name(right));
  return std::unexpected(TypeError(err));
}


template<typename FROM, typename TO>
TO cast(FROM from) {
  // We can cast variable back to themselves
  if (std::is_same<FROM, TO>()) {
    return from;
  }

  std::cout << "Casting from " << typeid(from).name() << " to " << typeid(TO).name() << std::endl;
  unimplemented();
}

template<typename FROM, typename TO>
TO cast(Object from) {
  return std::visit(overloads{
                            [](Integer i) { return cast<Integer, TO>(i); },
                            [](Boolean b) { return cast<Boolean, TO>(b); },
                            [](Null n) { return cast<Null, TO>(n); },
                            [](ReturnObject n) { return cast<ReturnObject, TO>(n); }},
                    from);
}

template<typename T>
T cast(T from) {
  return from;
}


/*
 * Casting to Booleans
 */
template<>
Boolean cast(BooleanLiteral from) {
  return {from.value};
}

template<>
Boolean cast(Integer from) {
  return {from.value != 0};
}

template<>
Boolean cast(Null from) {
  return {false};
}

template<>
Integer cast(IntegerLiteral from) {
  return {from.val};
}

template<>
ReturnObject cast(Boolean from) {
  return ReturnObject(new Object(from));
}

template<>
Boolean cast(ReturnObject from) {
  return {false};
}
