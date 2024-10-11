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

std::string Float::inspect() {
  return std::format("Float({})", value);
}

std::string Boolean::inspect() {
  return std::format("BooleanLiteral({})", value ? "true" : "false");
}

std::string String::inspect() {
  return std::format("String({})", value);
}

std::string Char::inspect() {
  return std::format("Char({})", value);
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
  // TODO :: make this work
  // return std::format("fn({}){\n{}\n}", p, body.string());
  return "Fn";

}

std::string inspect(Object obj) {
  return std::visit([](auto &&arg) {return arg.inspect(); }, obj);
}

std::string get_msg(Error obj) {
  return std::visit([](auto &&arg) {return arg.message; }, obj);
}

std::string get_type_name(Object &obj){
  return std::visit(overloads{
                    [](Integer) { return "Integer"; },
                    [](String) { return "String"; },
                    [](Char) { return "Char"; },
                    [](Float) { return "Float"; },
                    [](Boolean) { return "BooleanLiteral"; },
                    [](Null) { return "Null"; },
                    [](ReturnObject) { return "ReturnObject"; },
                    [](Function) { return "Function"; },
                      },
                    obj);
  unimplemented();
}

ObjectResult Environment::get(std::string ident) {
  if (env.contains(ident)){
    return env.at(ident);
  }

  if (outer.has_value()){
    auto out = outer.value()->get(ident);
    if (out.has_value()) {
      return out.value();
    }
  }

  auto err = std::format("identifier not found: {}", ident);
  std::cout << err << std::endl;
  return std::unexpected(TypeError(err));
}

void Environment::set(std::string ident, Object obj) {
  env.insert({ident, obj});
}

std::string print_env(Environment &env) {
  std::string out = "";
  for (const auto &[key, val] : env.env) {
    out += "Inner:\n";
    out += "K: " + key + " | V: " + inspect(val) + "\n";
    if (env.outer.has_value()) {
      out += "Outer:\n";
      out += print_env(*env.outer.value());
      out += "End Outer\n";
    }
    out += "End Inner\n";
  }
  return out;
}
