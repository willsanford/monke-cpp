//
// Created by Sanford, William on 9/21/24.
//

#ifndef EVAL_H
#define EVAL_H

#include <optional>

#include "object.h"
#include "parser.h"
#include "ast.h"

ObjectResult eval(Program &node);
ObjectResult eval(Statement &node);
ObjectResult eval(IntegerLiteral &node);
ObjectResult eval(LetStatement &node);
ObjectResult eval(ReturnStatement &node);
ObjectResult eval(ExpressionStatement &node);
ObjectResult eval(Expression &node);
ObjectResult eval(BlockStatement &node);
ObjectResult eval(Identifier &node);
ObjectResult eval(PrefixExpression &node);
ObjectResult eval(InfixExpression &node);
ObjectResult eval(Boolean &node);
ObjectResult eval(CallExpression &node);
ObjectResult eval(IfExpression &node);
ObjectResult eval(FunctionLiteral &node);

ObjectResult eval_program(std::string program);
ObjectResult eval_prefix_expression(std::string &op, Object &right);
ObjectResult eval_infix_expression(std::string &op, Object &left, Object &right);

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
BooleanObject cast(Boolean from);

template <>
BooleanObject cast(Integer from);

template <>
BooleanObject cast(Null from);

/*
 * Casting to Booleans
 */
template <>
BooleanObject cast(Boolean from);

template <>
BooleanObject cast(ReturnObject from);

template <>
BooleanObject cast(Integer from);

template <>
BooleanObject cast(Null from);

template <>
Integer cast(IntegerLiteral from);


template <>
ReturnObject cast(BooleanObject from);

bool is_truthy(Object obj);
#endif //EVAL_H

