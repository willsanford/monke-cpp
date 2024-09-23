//
// Created by wsanf on 9/15/24.
//

#ifndef MONKE_CPP_UTILS_H
#define MONKE_CPP_UTILS_H

#include <string>
#include <vector>
#include <algorithm>
#include <variant>
#include <iostream>

// Primary template for non-variant types
template<typename T, typename = void>
struct is_variant : std::false_type {};

// Specialization for std::variant
template<typename... Args>
struct is_variant<std::variant<Args...>> : std::true_type {};
template<typename T>
inline constexpr bool is_variant_v = is_variant<T>::value;

template<class... Ts>
struct overloads : Ts... {
  using Ts::operator()...;
};

template<class... Ts>
overloads(Ts...) -> overloads<Ts...>;

template<typename T, typename U>
bool compareHelper(const U &lhs, const T &rhs) {
  if (const U *rhsPtr = std::get_if<U>(&rhs)) {
    return lhs == *rhsPtr;
  }
  return false;
}

template<typename T, std::enable_if_t<is_variant_v<T>, int>>
bool operator==(const T &lhs, const T &rhs) {
  return std::visit(overloads{
                            [&rhs](const auto &l) -> bool {
                              return compareHelper<T>(l, rhs);
                            },
                    },
                    lhs);
}
std::string str_join(std::vector<std::string> strs);
std::string str_join(std::vector<std::string> strs, std::string delim);

[[noreturn]] inline void unimplemented(const char* file, int line) {
  std::cerr << "Error: Unimplemented code called at " << file << ":" << line << std::endl;
  std::abort();
}

#define unimplemented() unimplemented(__FILE__, __LINE__)

#endif//MONKE_CPP_UTILS_H
