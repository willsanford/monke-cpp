//
// Created by Sanford, William on 9/21/24.
//

#ifndef EVAL_H
#define EVAL_H

#include <optional>

#include "object.h"
#include "parser.h"
#include "ast.h"

ObjectResult evaluate(Program &node);
ObjectResult eval(Statement &node, Environment &env);
ObjectResult eval(IntegerLiteral &node, Environment &env);
ObjectResult eval(LetStatement &node, Environment &env);
ObjectResult eval(ReturnStatement &node, Environment &env);
ObjectResult eval(ExpressionStatement &node, Environment &env);
ObjectResult eval(Expression &node, Environment &env);
ObjectResult eval(BlockStatement &node, Environment &env);
ObjectResult eval(Identifier &node, Environment &env);
ObjectResult eval(PrefixExpression &node, Environment &env);
ObjectResult eval(InfixExpression &node, Environment &env);
ObjectResult eval(BooleanLiteral &node, Environment &env);
ObjectResult eval(CallExpression &node, Environment &env);
ObjectResult eval(IfExpression &node, Environment &env);
ObjectResult eval(FunctionLiteral &node, Environment &env);

ObjectResult eval_program(std::string program);
ObjectResult eval_prefix_expression(std::string &op, Object &right, Environment &env);
ObjectResult eval_infix_expression(std::string &op, Object &left, Object &right, Environment &env);
std::vector<ObjectResult> eval_expressions(std::vector<Expression> &expressions, Environment &env);
ObjectResult apply_function(Function &fn, std::vector<Object> &args);
Environment extend_function_env(Function &fn, std::vector<Object> &args);
Environment enclose_env(Environment* outer);

template <typename FROM, typename TO>
TO cast(FROM from);

template <typename FROM, typename TO>
TO cast(Object from);

template <typename T>
T cast(T from);


/*
 * Casting to Booleans
 */
template <>
Boolean cast(BooleanLiteral from);

template <>
Boolean cast(Integer from);

template <>
Boolean cast(Null from);

/*
 * Casting to Booleans
 */
template <>
Boolean cast(BooleanLiteral from);

template <>
Boolean cast(ReturnObject from);

template <>
Boolean cast(Integer from);

template <>
Boolean cast(Null from);

template <>
Integer cast(IntegerLiteral from);


template <>
ReturnObject cast(Boolean from);

bool is_truthy(Object obj);
#endif //EVAL_H

