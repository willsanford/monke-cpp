//
// Created by wsanf on 9/15/24.
//
#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include "ast.h"

#define MULTILINE_STRING(s) #s
TEST(AST, LetStatementStringTest) {
  Program p;
  LetStatement l;
  l.name = Identifier("some_name");
  l.value = Expression(Identifier("some_value"));
  p.statements = {
                  l
  };
  ASSERT_EQ(p.string(), "let some_name = some_value;");
}
