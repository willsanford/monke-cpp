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
    using Node::token_literal;
};

class NullStatement : public Statement {
public:
    std::string token_literal(){ return "NULL"; }
};

class Expression : public Node {
public:
    using Node::token_literal;
};

class Identifier : public Expression {
public:
    using Expression::token_literal;

    Identifier(Token t, std::string n);
    Identifier(){}
    Token token; // Always token_t::IDENT
    std::string value;
};

class LetStatement : public Statement {
public:
    LetStatement();
    Token token;
    Identifier name;
    Expression value;
    std::string token_literal();
};


class Program {
public:
    Program();
    std::string token_literal();
    std::vector<Statement> statements;
};

#endif //MONKE_CPP_AST_H
