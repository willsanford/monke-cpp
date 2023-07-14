//
// Created by William Sanford on 7/14/2023.
//

#ifndef MONKE_CPP_AST_H
#define MONKE_CPP_AST_H

#include <vector>
#include <string>

#include "token.h"

class Node {
public:
    std::string token_literal();
};

class Statement : public Node {
public:
    std::string token_literal();
};

class Expression : Node {
public:
    std::string token_literal();
};

class Identifier : Expression {
public:
    Token token; // Always token_t::IDENT
    std::string value;

    std::string token_literal();
};

class LetStatement : Statement {
public:
    Token token;
    Identifier name;
    Expression value;
    std::string token_literal();
};


class Program {
public:
    std::string token_literal();
    std::vector<Statement> statements;
};

#endif //MONKE_CPP_AST_H
