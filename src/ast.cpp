//
// Created by William Sanford on 7/14/2023.
//

#include "ast.h"

std::string Node::token_literal() {
    return "";
}
std::string Statement::token_literal() {
    return "";
}
std::string Expression::token_literal() {
    return "";
}

std::string LetStatement::token_literal() {
    return token.literal;
}

std::string Identifier::token_literal() {
    return token.literal;
}

std::string Program::token_literal() {
    if (!statements.empty()){
        return statements.at(0).token_literal();
    } else {
        return "";
    }
}