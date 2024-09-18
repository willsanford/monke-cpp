//
// Created by wsanf on 9/15/24.
//

#ifndef MONKE_CPP_UTILS_H
#define MONKE_CPP_UTILS_H

#include <string>
#include <vector>
#include <algorithm>
std::string str_join(std::vector<std::string> strs){
  return std::ranges::fold_left(strs, "", [](std::string a, std::string b) -> std::string{ return a + b; });
}


#endif//MONKE_CPP_UTILS_H
