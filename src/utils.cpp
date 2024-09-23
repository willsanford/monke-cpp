//
// Created by Sanford, William on 9/19/24.
//


#include "utils.h"

std::string str_join(std::vector<std::string> strs){
  std::string result;
  for (auto str : strs) {
    result += str;
  }
  return result;
}

std::string str_join(std::vector<std::string> strs, std::string delim){
  std::string result;
  if (strs.size() > 0) {
    for (size_t i = 0; i < strs.size() - 1; i++) {
      result += strs[i] + delim;
    }
    result += strs[strs.size()-1];
  }
  return result;
}


