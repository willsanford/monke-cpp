//
// Created by Sanford, William on 9/21/24.
//

#ifndef OBJECT_H
#define OBJECT_H

#include <cstdint>
#include <expected>
#include <string>
#include <unordered_map>
#include <variant>
#include <memory>

#include "ast.h"
#include "lexer.h"

class Environment;

class Integer;
class Boolean;
class Float;
class Null;
class String;
class Char;
class ReturnObject;
class Function;
typedef std::variant<Float, Integer, String, Char, Boolean, Null, ReturnObject, Function> Object;

class TypeError;
typedef std::variant<TypeError, LexerError> Error;
typedef std::expected<Object, Error> ObjectResult;

std::string get_type_name(Object &obj_res);

class TypeError {
  public:
  std::string message;
  std::string get_msg() { return message; };
  TypeError(std::string message) : message(message) {}
  bool operator==(const TypeError &) const { return true; };
};

class Function {
  public:
      Function(std::vector<Identifier> parameters, BlockStatement body, std::shared_ptr<Environment> env) : parameters(parameters), body(body), env(env){};
      std::vector<Identifier> parameters;
      BlockStatement body;
      std::shared_ptr<Environment> env;
      std::string inspect();
    bool operator==(const Function &) const { return true; };
};

class ReturnObject {
  public:
  Object *value;
  std::string inspect();
  ReturnObject(Object *value) : value(value) {}
  /* TODO : Figure out what is happening here */
  bool operator==(const ReturnObject &) const { return true; };
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

class Float{
  public:
  double value;
  std::string inspect();
  Float(double value) : value(value) {}
  bool operator==(const Float &other) const { return other.value == value; }
};

class String {
  public:
  std::string value;
  std::string inspect();
  String(std::string value) : value(value) {}
  bool operator==(const String &other) const { return other.value == value; }
};

class Char {
  public:
  char value;
  std::string inspect();
  Char(char value) : value(value) {}
  bool operator==(const Char &other) const { return other.value == value; }
};

class Boolean {
  public:
  bool value;
  std::string inspect();
  Boolean(bool value) : value(value) {}
  bool operator==(const Boolean &other) const { return other.value == value; }
};

std::string inspect(Object obj);
std::string get_msg(Error err);

class Environment {
  public:
  std::optional<std::shared_ptr<Environment>> outer;
  std::unordered_map<std::string, Object> env;
  ObjectResult get(std::string ident);
  void set(std::string ident, Object obj);
};

std::string print_env(Environment &env);

#endif//OBJECT_H
