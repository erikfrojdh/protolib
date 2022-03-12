#pragma once

#include "protolib/ImageData.hpp"
#include "protolib/DataSpan.hpp"
#include "protolib/defs.hpp"
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
    ssize_t rows() const;
    ssize_t cols() const;
    uint8_t bitdepth() const;
    uint8_t bytes_per_pixel() const;
    image_shape shape() const;
    std::byte* data();

    template<typename T>
    DataSpan<T, 2> view();
};

} // namespace pl
