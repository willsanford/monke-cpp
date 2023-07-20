//
// Created by William Sanford on 7/14/2023.
//

#include "ast.h"
#include "token.h"

std::string Node::token_literal() {
    return "";
}

std::string LetStatement::token_literal() {
    return token.literal;
}

Identifier::Identifier(Token t, std::string n) : token(t) , value(n) {}

Program::Program(){

};

std::string Program::token_literal() {
    if (!statements.empty()){
        return statements.at(0).token_literal();
    } else {
        return "";
    }
}
