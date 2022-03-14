#pragma once
#include <vector>
#include <string_view>
namespace pl{


std::string_view find_between(const char d1, const char d2,
                                     std::string_view sv);

std::string_view find_between(const char delimiter,
                                     std::string_view sv);

//TODO! check for integer types
template<typename T>
std::vector<T> str2vec(std::string_view sv);

template<typename T>
std::string vec2str(const std::vector<T> vec);


}