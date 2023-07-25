//
// Created by William Sanford on 7/14/2023.
//

#ifndef MONKE_CPP_AST_H
#define MONKE_CPP_AST_H

#include <vector>
#include <string>

#include "token.h"

enum class NodeType {
    Node_,
    Statement_,
    NullStatement_,
    Expression_,
    Identifier_,
    LetStatement_,
    ReturnStatement_
};

class Node {
public:
    std::string token_literal();
    NodeType t = NodeType::Node_;
};

class Statement : public Node {
public:
    using Node::token_literal;
    NodeType t = NodeType::Statement_;
};

class NullStatement : public Statement {
public:
    std::string token_literal(){ return "NULL"; }
    NodeType t = NodeType::NullStatement_;
};

class Expression : public Node {
public:
    using Node::token_literal;
    NodeType t = NodeType::Expression_;
};

class Identifier : public Expression {
public:
    using Expression::token_literal;
    NodeType t = NodeType::Identifier_;

    Identifier(Token t, std::string n);
    Identifier(){}
    Token token; // Always token_t::IDENT
    std::string value;
};

class LetStatement : public Statement {
public:
    NodeType t = NodeType::LetStatement_;
    LetStatement();
    Token token;
    Identifier name;
    Expression value;
    std::string token_literal();
};

class ReturnStatement : public Statement {
public:
    NodeType t = NodeType::ReturnStatement_;
    ReturnStatement();

    Token token; // Always token_t::RETURN
    Expression return_value;
    
    std::string token_literal();
};

class Program {
public:
    Program();
    std::string token_literal();
    std::vector<Statement> statements;
};

#endif //MONKE_CPP_AST_H
