//
// Created by Sanford, William on 9/21/24.
//

#include <variant>
#include <typeinfo>
#include <optional>

#include "eval.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "object.h"

ObjectResult eval_program(std::string input) {
    auto *l = new Lexer(input);
    Parser p = Parser(l);
    Program program = p.parse_program();
    return eval(program);
}

ObjectResult eval(Statement &node) {
    return std::visit([](auto &&arg) { return eval(arg); }, node);
}

ObjectResult eval(LetStatement &node) {
    unimplemented();
}

ObjectResult eval(BlockStatement &node) {
    auto result = ObjectResult(Object(Null()));
    auto statements = node.get_statements();
    for (auto &stmt: statements) {
        result = eval(stmt);
        if (result.has_value() && std::holds_alternative<ReturnObject>(result.value())) {
            return result;
        }
    }
    return result;
}

ObjectResult eval(ReturnStatement &node) {
    return eval(node.return_value).and_then([&](auto obj) { return ObjectResult(ReturnObject(new Object(obj))); });
}

bool is_truthy(Object obj) {
    if (std::holds_alternative<BooleanObject>(obj)) {
        return std::get<BooleanObject>(obj).value;
    }
    if (std::holds_alternative<Null>(obj)) {
        return false;
    }
    return true;
}

ObjectResult eval(IfExpression &node) {
    auto condition_obj = eval(*node.condition);
    if (condition_obj.has_value() && is_truthy(condition_obj.value())) {
        return eval(*node.consequence);
    }

    if (!node.alternative.has_value()) {
        return Null();
    }

    return eval(*node.alternative.value());
};

ObjectResult eval(Expression &node) {
    return std::visit([](auto &&arg) { return eval(arg); }, node);
}

ObjectResult eval(ExpressionStatement &node) {
    return eval(node.e);
}

ObjectResult eval(IntegerLiteral &node) {
    return Integer(node.val);
}

ObjectResult eval(Boolean &node) {
    return BooleanObject(node.value);
}

ObjectResult eval(PrefixExpression &node) {
    return eval(*node.right).
            and_then([&](auto right) {
                return eval_prefix_expression(node.op, right);
            });
};

ObjectResult eval(InfixExpression &node) {
    using ResultObjectPair = std::expected<std::pair<Object, Object>, Error>;
    return eval(*node.left).and_then([&](auto left) -> ResultObjectPair {
        return eval(*node.right).and_then([&](auto right) -> ResultObjectPair {
            return std::make_pair(left, right);
        });
    }).and_then([&](auto obj) {
        // TODO : Can we destructure this?
        auto [left, right] = obj;
        return eval_infix_expression(node.op, left, right);
    });
};
ObjectResult eval(Identifier &node) { unimplemented(); };
ObjectResult eval(CallExpression &node) { unimplemented(); };
ObjectResult eval(FunctionLiteral &node) { unimplemented(); };

ObjectResult eval(Program &node) {
    auto result = ObjectResult(Object(Null()));
    for (auto &stmt: node.statements) {
        result = eval(stmt);
        if (result.has_value() && std::holds_alternative<ReturnObject>(result.value())) {
            return *std::get<ReturnObject>(result.value()).value;
        }
    }
    return result;
}

#define INT_OP(operator, t) if (strcmp(op.c_str(), #operator) == 0) { return t(left_int.value operator right_int.value); }

ObjectResult eval_infix_expression(std::string &op, Object &left, Object &right) {
    if (std::holds_alternative<BooleanObject>(left) && std::holds_alternative<BooleanObject>(right)) {
        if (op == "==") {
            return BooleanObject(std::get<BooleanObject>(left).value == std::get<BooleanObject>(right).value);
        }
        if (op == "!=") {
            return BooleanObject(std::get<BooleanObject>(left).value != std::get<BooleanObject>(right).value);
        }
    }
    if (!std::holds_alternative<Integer>(left) || !std::holds_alternative<Integer>(right)) {
        std::cerr << "Cannot evaluate infix expression with non-integer values" << std::endl;
        unimplemented();
    }
    auto left_int = std::get<Integer>(left);
    auto right_int = std::get<Integer>(right);

    INT_OP(+, Integer);
    INT_OP(-, Integer);
    INT_OP(*, Integer);
    INT_OP(/, Integer);
    INT_OP(+, Integer);
    INT_OP(>, BooleanObject);
    INT_OP(>=, BooleanObject);
    INT_OP(<, BooleanObject);
    INT_OP(<=, BooleanObject);

    unimplemented();
}

ObjectResult eval_prefix_expression(std::string &op, Object &right) {
    if (op == "!") {
        BooleanObject b = cast<Object, BooleanObject>(right);
        b.value = !b.value;
        return b;
    }
    if (op == "-") {
        if (!std::holds_alternative<Integer>(right)) {
            std::cerr << inspect(right) << std::endl;
            std::cerr << "Cannot negate a non-integer" << std::endl;
            return Null();
        }

        Integer i = std::get<Integer>(right);
        i.value = -i.value;
        return i;
    }
    unimplemented();
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
                          [](BooleanObject b) { return cast<BooleanObject, TO>(b); },
                          [](Null n) { return cast<Null, TO>(n); },
                          [](ReturnObject n) { return cast<ReturnObject, TO>(n); }
                      }, from);
}

template<typename T>
T cast(T from) {
    return from;
}


/*
 * Casting to Booleans
 */
template<>
BooleanObject cast(Boolean from) {
    return {from.value};
}

template<>
BooleanObject cast(Integer from) {
    return {from.value != 0};
}

template<>
BooleanObject cast(Null from) {
    return {false};
}

template<>
Integer cast(IntegerLiteral from) {
    return {from.val};
}

template<>
ReturnObject cast(BooleanObject from) {
    return ReturnObject(new Object(from));
}

template<>
BooleanObject cast(ReturnObject from) {
    return {false};
}
