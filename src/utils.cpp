//
// Created by Sanford, William on 9/19/24.
//

#include <fstream>
#include <sstream>

#include "utils.h"


std::string read_file(std::string filename){
  std::ifstream file(filename);
  if (!file.is_open()) {
    return "Error: Unable to open file";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

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


