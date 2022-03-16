#include "protolib/utils.hpp"

#include <sstream>
#include <chrono>

namespace pl{

std::string_view find_between(const char d1, const char d2,
                                     std::string_view sv) {
    auto start = sv.find_first_of(d1) + 1;
    auto stop = sv.find_first_of(d2, start);
    auto size = stop - start;
    return std::string_view(&sv[start], size);
}

std::string_view find_between(const char delimiter,
                                     std::string_view sv) {
    return find_between(delimiter, delimiter, sv);
}

template<typename T>
std::string vec2str(const std::vector<T> vec) {
    std::ostringstream oss;
    oss << "(";
    if (!vec.empty()) {
        auto it = vec.cbegin();
        oss << *it++ << ',';
        while (it != vec.cend())
            oss << ' ' << *it++ << ',';
    }
    oss << ")";
    return oss.str();
}

template<typename T>
std::vector<T> str2vec(std::string_view sv) {
    std::vector<T> vec;
    vec.reserve(3);
    size_t curr = 0;
    constexpr char number[] = "0123456789";
    size_t next = sv.find_first_of(number, curr);
    while (next != std::string_view::npos) {
        //TODO this should be templated!
        int res = std::atoi(&sv[next]);
        vec.push_back(res);
        curr = sv.find_first_of(',', next);
        next = sv.find_first_of(number, curr);
    }
    return vec;
}

template std::vector<ssize_t> str2vec(std::string_view sv);
template std::string vec2str(const std::vector<ssize_t> vec);

template std::vector<int> str2vec(std::string_view sv);
template std::string vec2str(const std::vector<int> vec);


void timed_func(std::string name, int n_times, void (*func)()) {
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i != n_times; ++i) {
        func();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = t1 - t0;
    fmt::print("{} took {}ms\n", name, ms_double.count());
}


}

