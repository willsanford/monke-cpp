#include <gtest/gtest.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>
#include <ranges>
#include <memory>

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

bool test_bool(Expression &e, bool value) {
    if (!std::holds_alternative<Boolean>(e)) {
        std::cerr << "The following expression should be an bool: " << e << std::endl;
        return false;
    }
    auto int_l = std::get<Boolean>(e);

    if (int_l.t.ttype != token_t::TRUE && int_l.t.ttype != token_t::FALSE) {
        std::cerr << "The following expression should be an bool: " << e << std::endl;
        return false;
    }

    if (int_l.value != value) {
        std::cerr << "The following expression should be an bool with value : " << value << " : " << e <<
                std::endl;
        return false;
    }
    return true;
}

bool test_int_literal(Expression &e, int64_t value) {
    if (!std::holds_alternative<IntegerLiteral>(e)) {
        std::cerr << "The following expression should be an int literal: " << e << std::endl;
        return false;
    }
    auto int_l = std::get<IntegerLiteral>(e);

    if (int_l.t.ttype != token_t::INT) {
        std::cerr << "The following expression should be an int literal: " << e << std::endl;
        return false;
    }

    if (int_l.val != value) {
        std::cerr << "The following expression should be an int literal with value : " << value << " : " << e <<
                std::endl;
        return false;
    }
    return true;
}

bool test_ident(Expression &e, std::string value) {
    if (!std::holds_alternative<Identifier>(e)) {
        std::cerr << "The following expression should be an identifier: " << e << std::endl;
        return false;
    }
    auto ident = std::get<Identifier>(e);

    if (ident.token.ttype != token_t::IDENT) {
        std::cerr << "The following expression should be an identifier: " << e << std::endl;
        return false;
    }

    if (ident.string() != value) {
        std::cerr << "The following expression should be an identifier with value : " << value << " : " << e <<
                std::endl;
        return false;
    }
    return true;
}

template<typename T>
bool test_literal_exp(Expression &e, T val) {
    unimplemented();
}

template<>
bool test_literal_exp(Expression &e, int64_t val) {
    return test_int_literal(e, val);
}

template<>
bool test_literal_exp(Expression &e, std::string val) {
    return test_ident(e, val);
}

template<>
bool test_literal_exp(Expression &e, bool val) {
    return test_bool(e, val);
}

template<typename T>
bool test_infix_expression(Expression &e, T left, std::string op, T right) {
    if (!std::holds_alternative<InfixExpression>(e)) {
        std::cerr << "The following expression should be an infix expression: " << e << std::endl;
        return false;
    }
    auto infix = std::get<InfixExpression>(e);

    if (infix.op != op) {
        std::cerr << "The following expression should be an infix expression with op : " << op << " : " << e <<
                std::endl;
        return false;
    }

    if (!test_literal_exp(*infix.left, left)) {
        std::cerr << "The following expression should be an infix expression with left : " << left << " : " << e <<
                std::endl;
        return false;
    }

    if (!test_literal_exp(*infix.right, right)) {
        std::cerr << "The following expression should be an infix expression with right : " << right << " : " << e <<
                std::endl;
        return false;
    }
    return true;
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
    std::vector<std::string> inputs = {
        "let x = 5;",
        "let y = 9;",
        "let apple = foobar;"
    };

    std::vector<Program> correct_programs = {
        Program({
            LetStatement(
                Identifier(Token(IDENT, "x")),
                Expression(IntegerLiteral(5))
            )
        }),

        Program({
            LetStatement(
                Identifier(Token(IDENT, "y")),
                Expression(IntegerLiteral(9))
            )
        }),

        Program({
            LetStatement(
                Identifier(Token(IDENT, "apple")),
                Identifier(Token(IDENT, "foobar"))
            )
        }),
    };

    for (const auto &[input, correct_program]: std::views::zip(inputs, correct_programs)) {
        Lexer *l = new Lexer(input);
        Parser p = Parser(l);

        Program program = p.parse_program();
        ASSERT_EQ(program, correct_program);
    }
}

TEST(Parser, ReturnStatementTest) {
    std::vector<std::string> inputs ={
        "return 5;",
        "return x + 9;",
    };

    std::vector<Program> correct_programs = {
        Program({
            ReturnStatement(
                Expression(IntegerLiteral(5))
            )
        }),
        Program({
            ReturnStatement(
                InfixExpression(
                    new Expression(Identifier(Token(IDENT, "x"))),
                    "+",
                    new Expression(IntegerLiteral(9))
                )

            )
        }),
    };

    for (const auto &[input, correct_program]: std::views::zip(inputs, correct_programs)) {
        Lexer *l = new Lexer(input);
        Parser p = Parser(l);

        Program program = p.parse_program();
        ASSERT_EQ(program, correct_program);
    }
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
    std::vector<Statement> program_statements = {
        ExpressionStatement(IntegerLiteral(Token(token_t::INT, "5"), 5)),
    };
    ASSERT_EQ(program.statements, program_statements);
    // TODO: Check that this comes out valid
}

TEST(Parser, BooleanLiteralExpressionTest) {
    std::vector<std::string> inputs = {"true;", "false"};
    std::vector<bool> corrects = {true, false};
    for (const auto &[input, correct]: std::views::zip(inputs, corrects)) {
        auto *l = new Lexer(input);
        Parser p = Parser(l);

        Program program = p.parse_program();
        ASSERT_EQ(program.statements.size(), 1);
        std::vector<Statement> program_statements = {
            ExpressionStatement(Boolean(Token(correct ? token_t::TRUE : token_t::FALSE, correct ? "true" : "false"),
                                        correct)),
        };
        ASSERT_EQ(program.statements, program_statements);
        // TODO: Check that this comes out valid
    }
}

TEST(Parser, PrefixExpressionTest) {
    std::vector<std::tuple<std::string, std::string, int64_t> > tests = {
        {"!5;", "!", 5},
        {"-15;", "-", 15}
    };

    for (const auto &[input, prefix, val]: tests) {
        Parser parser = Parser(new Lexer(input));
        Program p = parser.parse_program();
        ASSERT_EQ(p.statements.size(), 1);
        auto s = p.statements[0];
        ASSERT_TRUE(std::holds_alternative<ExpressionStatement>(s));
        auto expr = std::get<ExpressionStatement>(s);

        // Create the correct expression statement
        auto pre = PrefixExpression(
            prefix,
            new Expression(IntegerLiteral(Token(token_t::INT, std::to_string(val)), val)));
        auto e = ExpressionStatement(pre);
        ASSERT_EQ(expr, e);
    }

    std::vector<std::tuple<std::string, std::string, int64_t> > tests2 = {
        {"!true;", "!", true},
        {"!false;", "!", false},
    };

    for (const auto &[input, prefix, val]: tests2) {
        Parser parser = Parser(new Lexer(input));
        Program p = parser.parse_program();
        ASSERT_EQ(p.statements.size(), 1);
        auto s = p.statements[0];
        ASSERT_TRUE(std::holds_alternative<ExpressionStatement>(s));
        auto expr = std::get<ExpressionStatement>(s);

        // Create the correct expression statement
        auto pre = PrefixExpression(
            prefix,
            new Expression(Boolean(Token(token_t::INT, std::to_string(val)), val)));
        auto e = ExpressionStatement(pre);
        ASSERT_EQ(expr, e);
    }
}

TEST(Parser, InfixExpressionTest) {
    std::vector<std::tuple<std::string, int64_t, std::string, int64_t> > tests = {
        {"5 - 5;", 5, "-", 5},
        {"5 + 5;", 5, "+", 5},
        {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5},
        {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5},
        {"5 != 5;", 5, "!=", 5},
    };

    for (const auto &[input, left_val, op, right_val]: tests) {
        Program p = Parser(new Lexer(input)).parse_program();
        ASSERT_EQ(p.statements.size(), 1);
        ASSERT_TRUE(std::holds_alternative<ExpressionStatement>(p.statements[0]));
        auto exp_statement = std::get<ExpressionStatement>(p.statements[0]);


        test_infix_expression(exp_statement.e, left_val, op, right_val);
    }

    std::vector<std::tuple<std::string, bool, std::string, bool> > tests2 = {
        {"true == true", true, "==", true},
        {"true != false", true, "!=", false},
        {"false == false", false, "==", false},
    };

    for (const auto &[input, left_val, op, right_val]: tests2) {
        Program p = Parser(new Lexer(input)).parse_program();
        ASSERT_EQ(p.statements.size(), 1);
        ASSERT_TRUE(std::holds_alternative<ExpressionStatement>(p.statements[0]));
        auto exp_statement = std::get<ExpressionStatement>(p.statements[0]);


        test_infix_expression(exp_statement.e, left_val, op, right_val);
    }
}

TEST(Parser, ParserOperatorPrecedenceTest) {
    std::vector<std::pair<std::string, std::string> > tests = {
        {
            "-a * b",
            "((-a) * b)",
        },
        {
            "!-a",
            "(!(-a))",
        },
        {
            "a + b + c",
            "((a + b) + c)",
        },
        {
            "a + b - c",
            "((a + b) - c)",
        },
        {
            "a * b * c",
            "((a * b) * c)",
        },
        {
            "a * b / c",
            "((a * b) / c)",
        },
        {
            "a + b / c",
            "(a + (b / c))",
        },
        {
            "a + b * c + d / e - f",
            "(((a + (b * c)) + (d / e)) - f)",
        },
        {
            "3 + 4; -5 * 5",
            "(3 + 4)((-5) * 5)",
        },
        {
            "5 > 4 == 3 < 4",
            "((5 > 4) == (3 < 4))",
        },
        {
            "5 < 4 != 3 > 4",
            "((5 < 4) != (3 > 4))",
        },
        {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        {
            "true",
            "true",
        },
        {
            "false",
            "false",
        },
        {
            "3 > 5 == false",
            "((3 > 5) == false)",
        },
        {
            "3 < 5 == true",
            "((3 < 5) == true)",
        },
        {
            "1 + (2 + 3) + 4",
            "((1 + (2 + 3)) + 4)",
        },
        {
            "(5 + 5) * 2",
            "((5 + 5) * 2)",
        },
        {
            "2 / (5 + 5)",
            "(2 / (5 + 5))",
        },
        {
            "-(5 + 5)",
            "(-(5 + 5))",
        },
        {
            "!(true == true)",
            "(!(true == true))",
        },
        {
            "add(b * c)",
            "add((b * c))",
        },
        {
            "a + add(b * c) + d",
            "((a + add((b * c))) + d)",
        },
        {
            "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
            "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
        },
        {
            "add(a + b + c * d / f + g)",
            "add((((a + b) + ((c * d) / f)) + g))",
        },
    };

    for (auto &[input, expected]: tests) {
        auto p = Parser(new Lexer(input));
        auto program = p.parse_program();
        ASSERT_EQ(program.string(), expected);
    }
}

TEST(Parser, IfExpressionTest) {
    std::vector<std::string> inputs = {
        "if (x < y) { x }",
        "if (x < y) { x } else { y }",
    };

    std::vector programs = {
        Program({
            ExpressionStatement(
                IfExpression(
                    Token(IF, "if"),
                    new Expression(
                        InfixExpression(
                            Token(ILLEGAL, "sjkldhfjksdh"),
                            new Expression(Identifier(Token(IDENT, "x"))),
                            "<",
                            new Expression(Identifier(Token(IDENT, "y")))
                        )
                    ),
                    new BlockStatement({
                        {StatementType::ES, (Statement *) new ExpressionStatement(Identifier(Token(IDENT, "x")))}
                    }),
                    std::nullopt
                )
            )
        }),

        Program({
            ExpressionStatement(
                IfExpression(
                    Token(token_t::IF, "if"),
                    new Expression(
                        InfixExpression(
                            Token(ILLEGAL, "sjkldhfjksdh"),
                            new Expression(Identifier(Token(IDENT, "x"))),
                            "<",
                            new Expression(Identifier(Token(IDENT, "y")))
                        )
                    ),
                    new BlockStatement({
                        {StatementType::ES, (Statement *) new ExpressionStatement(Identifier(Token(IDENT, "x")))}
                    }),
                    new BlockStatement({
                        {StatementType::ES, (Statement *) new ExpressionStatement(Identifier(Token(IDENT, "y")))}
                    })
                )
            )
        }),
    };


    for (const auto &[input, program]: std::views::zip(inputs, programs)) {
        auto p = Parser(new Lexer(input));
        auto parsed_program = p.parse_program();
        ASSERT_EQ(parsed_program, program);
    }
}

TEST(Parser, FunctionLiteralTest) {
    std::string input = "fn(x, y) { x + y; }";
    Lexer *l = new Lexer(input);
    Parser p = Parser(l);
    auto program = p.parse_program();
    for (const auto e: p.errors) std::cout << e << std::endl;
    // ASSERT_EQ(p.errors.size(), 0);

    auto correct_program = Program({
        ExpressionStatement(FunctionLiteral(
            {new Identifier(Token(IDENT, "x")), new Identifier(Token(IDENT, "y"))},
            new BlockStatement({
                {
                    StatementType::ES, (Statement *) new ExpressionStatement(
                        InfixExpression(
                            Token(ILLEGAL, "sdfsdfsdf"),
                            new Expression(Identifier(Token(IDENT, "x"))),
                            "+",
                            new Expression(Identifier(Token(IDENT, "y")))

                        )
                    )
                }
            })))
    });

    ASSERT_EQ(correct_program, program);
}

TEST(Parser, FunctionParameterParsingTest) {
    std::vector<std::string> inputs = {
        "fn() { }",
        "fn(x) { }",
        "fn(x,y,z) { }",
    };

    std::vector<std::vector<Identifier *> > correct = {
        {},
        {new Identifier(Token(IDENT, "x"))},
        {new Identifier(Token(IDENT, "x")), new Identifier(Token(IDENT, "y")), new Identifier(Token(IDENT, "z"))},
    };

    for (const auto &[input, correct_params]: std::views::zip(inputs, correct)) {
        Lexer *l = new Lexer(input);
        Parser p = Parser(l);
        auto program = p.parse_program();
        auto correct_program = Program({
            ExpressionStatement(FunctionLiteral(
                correct_params,
                new BlockStatement({})))
        });
        ASSERT_EQ(program, correct_program);
    }
}

TEST(Parser, CallExpressionParsingTest) {
    std::string input = "add(1, 2 * 3, 4 + 5);";

    Lexer *l = new Lexer(input);
    Parser p = Parser(l);
    auto program = p.parse_program();
    auto correct_program = Program({
        ExpressionStatement(CallExpression(
            new Expression(Identifier(Token(IDENT, "add"))),
            {
                new Expression(IntegerLiteral(1)),
                new Expression(InfixExpression(
                    new Expression(IntegerLiteral(2)),
                    "*",
                    new Expression(IntegerLiteral(3))
                )),
                new Expression(InfixExpression(
                    new Expression(IntegerLiteral(4)),
                    "+",
                    new Expression(IntegerLiteral(5))
                )),
            }
        ))
    });
    ASSERT_EQ(program, correct_program);
}
