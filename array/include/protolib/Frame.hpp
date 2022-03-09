#pragma once

#include "protolib/ImageData.hpp"
#include "protolib/DataSpan.hpp"
#include <fmt/format.h>

namespace pl
{
/*
Provides a dead simple interface to an array in memory

Open:
* Can we assume integer values for pixels? 

*/
class Frame{
    ssize_t nrows{};
    ssize_t ncols{};
    uint8_t bitdepth_{};
    std::unique_ptr<std::byte[]> data_{};

public:
    Frame() = default;
    Frame(size_t nr, size_t nc, uint8_t dr);
    Frame(const Frame&) = delete; //not sure we want to copy construct
    Frame &operator=(const Frame&) = delete; //copy assign
    Frame &operator=(Frame&&) = default;
    ~Frame() = default;
    
    Frame(Frame&&) = default;
    
    double operator()(size_t row, size_t col);
    ssize_t n_rows();
    ssize_t n_cols();
    uint8_t bitdepth();
    std::byte* data();

    template<typename T>
    DataSpan<T, 2> view();
};

} // namespace pl

// RawFile(const RawFile &) = default;
//     RawFile &operator=(const RawFile &) = default;
//     RawFile(RawFile &&) = default;
//     RawFile &operator=(RawFile &&) = default;
//     ~RawFile() = default;