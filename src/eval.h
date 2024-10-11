//
// Created by Sanford, William on 9/21/24.
//

#ifndef EVAL_H
#define EVAL_H

#include <optional>

#include "object.h"
#include "parser.h"
#include "ast.h"

// Evaluation functions
ObjectResult evaluate(Program &node);
ObjectResult eval(Statement &node, std::shared_ptr<Environment> env);
ObjectResult eval(IntegerLiteral &node, std::shared_ptr<Environment> env);
ObjectResult eval(FloatLiteral &node, std::shared_ptr<Environment> env);
ObjectResult eval(LetStatement &node, std::shared_ptr<Environment> env);
ObjectResult eval(ReturnStatement &node, std::shared_ptr<Environment> env);
ObjectResult eval(ExpressionStatement &node, std::shared_ptr<Environment> env);
ObjectResult eval(Expression &node, std::shared_ptr<Environment> env);
ObjectResult eval(BlockStatement &node, std::shared_ptr<Environment> env);
ObjectResult eval(Identifier &node, std::shared_ptr<Environment> env);
ObjectResult eval(PrefixExpression &node, std::shared_ptr<Environment> env);
ObjectResult eval(InfixExpression &node, std::shared_ptr<Environment> env);
ObjectResult eval(BooleanLiteral &node, std::shared_ptr<Environment> env);
ObjectResult eval(StringLiteral &node, std::shared_ptr<Environment> env);
ObjectResult eval(CharLiteral &node, std::shared_ptr<Environment> env);
ObjectResult eval(CallExpression &node, std::shared_ptr<Environment> env);
ObjectResult eval(IfExpression &node, std::shared_ptr<Environment> env);
ObjectResult eval(FunctionLiteral &node, std::shared_ptr<Environment> env);

// Helper evaluation functions
ObjectResult eval_program(std::string program);
ObjectResult eval_prefix_expression(std::string &op, Object &right, std::shared_ptr<Environment> env);
ObjectResult eval_infix_expression(std::string &op, Object &left, Object &right, std::shared_ptr<Environment> env);
std::vector<ObjectResult> eval_expressions(std::vector<Expression> &expressions, Environment &env);
ObjectResult apply_function(Function &fn, std::vector<Object> &args);
std::shared_ptr<Environment> extend_function_env(Function &fn, std::vector<Object> &args);
std::shared_ptr<Environment> enclose_env(Environment* outer);
ObjectResult unwrap_return_value(ObjectResult &obj);

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

