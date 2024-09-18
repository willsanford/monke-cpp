//
// Created by wsanf on 9/15/24.
//

#ifndef MONKE_CPP_UTILS_H
#define MONKE_CPP_UTILS_H

#include <string>
#include <vector>
#include <algorithm>
#include <ranges>
std::string str_join(std::vector<std::string> strs){
  std::string result;
  for (auto str : strs) {
    result += str;
  }
  return result;
}


#endif//MONKE_CPP_UTILS_H
