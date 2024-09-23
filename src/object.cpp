//
// Created by Sanford, William on 9/21/24.
//

#include <variant>
#include <object.h>
#include <format>

std::string Integer::inspect() {
  return std::format("Integer({})", value);
}

std::string BooleanObject::inspect() {
  return std::format("Boolean({})", value ? "true" : "false");
}

std::string Null::inspect() {
  return "Null";
}

std::string ReturnObject::inspect() {
  return std::format("ReturnObject({})", ::inspect(*value));
}

std::string inspect(Object obj) {
  std::visit([](auto &&arg) {return arg.inspect(); }, obj);
}
std::string get_msg(Error obj) {
  std::visit([](auto &&arg) {return arg.message; }, obj);
}
