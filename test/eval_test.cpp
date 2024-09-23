//
// Created by Sanford, William on 9/21/24.
//

#include <gtest/gtest.h>
#include <ranges>
#include <eval.h>

TEST(Eval, IntExpressionTest) {
    std::vector<std::tuple<std::string, int64_t> > tests = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5 + 5 + 5 - 10", 10},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 + -50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 * -10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
    };
    for (const auto &[input, correct]: tests) {
        auto evaluated = eval_program(input);
        ASSERT_EQ(evaluated, Object(Integer(correct)));
    }
}

TEST(Eval, BooleanExpressionTest) {
    std::vector<std::tuple<std::string, bool> > tests = {
        {"true", true},
        {"false", false},
        {"!true;", false},
        {"!false", true},
        {"!5", false},
        {"!!true", true},
        {"!!false", false},
        {"!!5", true},
        {"true == true", true},
        {"false == false", true},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 < 2) == true", true},
        {"(1 < 2) == false", false},
        {"(1 > 2) == true", false},
        {"(1 > 2) == false", true},
    };

    for (const auto &[input, correct]: tests) {
        auto evaluated_result = eval_program(input);
        ASSERT_TRUE(evaluated_result.has_value());
        ASSERT_EQ(evaluated_result.value(), Object(BooleanObject(correct)));
    }
}

TEST(Eval, IfExpressionTest) {
    std::vector<std::tuple<std::string, Object> > tests = {
        {"if (true) { 10 }", Object(Integer(10))},
        {"if (false) { 10 }", Object(Null())},
        {"if (1) { 10 }", Object(Integer(10))},
        {"if (1 < 2) { 10 }", Object(Integer(10))},
        {"if (1 > 2) { 10 }", Null()},
        {"if (1 > 2) { 10 } else { 20 }", Object(Integer(20))},
        {"if (1 < 2) { 10 } else { 20 }", Object(Integer(10))},
    };

    for (const auto &[input, correct]: tests) {
        auto evaluated_result = eval_program(input);
        ASSERT_TRUE(evaluated_result.has_value());
        if (!std::holds_alternative<Null>(evaluated_result.value()) || !std::holds_alternative<Null>(correct)) {
            ASSERT_EQ(evaluated_result.value(), correct);
        }
    }
}

TEST(Eval, ReturnStatementTest) {
    std::vector<std::tuple<std::string, int64_t> > tests = {
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9;", 10},
        {"if (10 > 1) { \n if (10 > 1) { \n return 10; \n} return 1; \n}", 10}
    };
    for (const auto &[input, correct]: tests) {
        auto evaluated_result = eval_program(input);
        ASSERT_TRUE(evaluated_result.has_value());
        ASSERT_EQ(evaluated_result.value(), Object(Integer(correct)));
    }
}

TEST(Eval, ErrorHandlingTest) {
    std::vector<std::tuple<std::string, std::string> > tests = {
        {
            "5 + true;",
            "type mismatch: INTEGER + BOOLEAN",
        },
        {
            "5 + true; 5;",
            "type mismatch: INTEGER + BOOLEAN",
        },
        {
            "-true",
            "unknown operator: -BOOLEAN",
        },
        {
            "true + false;",
            "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
            "5; true + false; 5",
            "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
            "if (10 > 1) { true + false; }",
            "unknown operator: BOOLEAN + BOOLEAN",
        },
    };
    for (const auto &[input, correct]: tests) {
        auto evaluated_result = eval_program(input);
        ASSERT_TRUE(!evaluated_result.has_value());
        ASSERT_EQ(evaluated_result.value(), get_msg(evaluated_result.error()));
    }
}