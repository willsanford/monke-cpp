//
// Created by Sanford, William on 9/21/24.
//

#include <variant>
#include <format>

#include "utils.h"
#include "object.h"


std::string Integer::inspect() {
  return std::format("Integer({})", value);
}

std::string Boolean::inspect() {
  return std::format("BooleanLiteral({})", value ? "true" : "false");
}

std::string Null::inspect() {
  return "Null";
}

std::string ReturnObject::inspect() {
  return std::format("ReturnObject({})", ::inspect(*value));
}
std::string Function::inspect() {
  std::vector<std::string> params;
  std::transform(parameters.begin(), parameters.end(), std::back_inserter(params), [](Identifier s) { return s.string(); });
  auto p = str_join(params, ",");
  // return std::format("fn({}){\n{}\n}", p, body.string());
  return "";

}

std::string inspect(Object obj) {
  std::visit([](auto &&arg) {return arg.inspect(); }, obj);
}

std::string get_msg(Error obj) {
  return std::visit([](auto &&arg) {return arg.message; }, obj);
}

std::string get_type_name(Object &obj){
  if (std::holds_alternative<Integer>(obj)){
    return "Integer";
  }
  if (std::holds_alternative<Boolean>(obj)){
    return "BooleanLiteral";
  }
  if (std::holds_alternative<Null>(obj)){
    return "Null";
  }
  if (std::holds_alternative<ReturnObject>(obj)){
    return "ReturnObject";
  }
  unimplemented();
}

ObjectResult Environment::get(std::string ident) {
  if (env.contains(ident)){
    return env.at(ident);
  }

  if (outer != nullptr && outer->env.contains(ident)){
    return env.at(ident);
  }

  auto err = std::format("identifier not found: {}", ident);
  return std::unexpected(TypeError(err));
}

void Environment::set(std::string ident, Object obj) {
  env.insert({ident, obj});
}
