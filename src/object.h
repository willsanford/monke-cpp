//
// Created by Sanford, William on 9/21/24.
//

#ifndef OBJECT_H
#define OBJECT_H

#include <variant>
#include <cstdint>
#include <string>
#include <expected>

class Integer;
class BooleanObject;
class Null;
class ReturnObject;
typedef std::variant<Integer, BooleanObject, Null, ReturnObject> Object;

class TypeError;
typedef std::variant<TypeError> Error;
typedef std::expected<Object, Error> ObjectResult;

class TypeError {
public:
    std::string message;
    TypeError(std::string message) : message(message) {}
};

class ReturnObject {
public:
    Object* value;
    std::string inspect();
    ReturnObject(Object* value) : value(value) {}
    /* TODO : Figure out what is happening here */
    bool operator==(const ReturnObject &ret_val) const { return true;};
    // bool operator==(const ReturnObject &ret_val) const { return *value == *ret_val.value; };
};

class Null {
public:
    Null(){};
    std::string inspect();
    bool operator==(const Null &) const { return false; };
};

class Integer {
public:
    int64_t value;
    std::string inspect();
    Integer(int64_t value) : value(value) {}
    bool operator==(const Integer &other) const { return other.value == value; }
};

class BooleanObject {
public:
    bool value;
    std::string inspect();
    BooleanObject(bool value) : value(value) {}
    bool operator==(const BooleanObject &other) const { return other.value == value; }
};

std::string inspect(Object obj);
std::string get_msg(Error err);

#endif //OBJECT_H
