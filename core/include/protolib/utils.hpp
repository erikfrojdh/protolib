#pragma once
#include <vector>
#include <string_view>
#include "protolib/defs.hpp"
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


//compare array and vector
template <size_t Size>
bool operator==(const std::array<ssize_t, Size>& lhs, const dynamic_shape &rhs) {
    if (lhs.size()!=rhs.size())
        return false;
    for(size_t i = 0; i!=rhs.size(); ++i)
        if(lhs[i]!=rhs[i])
            return false;
    return true;
}


#define TIMED_FUNC(x, N) timed_func(#x, N, &x)
void timed_func(std::string name, int n_times, void (*func)());


}