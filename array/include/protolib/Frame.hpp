#pragma once

#include "protolib/DataSpan.hpp"
#include "protolib/ImageData.hpp"
#include "protolib/defs.hpp"
#include <fmt/format.h>

namespace pl {
/*
Provides a simple way to hold image data in memory.

Open:
 * Can we assume integer values for pixels or do we need to add float support?

*/
class Frame {
    ssize_t nrows{};
    ssize_t ncols{};
    uint8_t bitdepth_{};
    std::unique_ptr<std::byte[]> data_{nullptr};

  public:
    Frame() = default;
    Frame(size_t nr, size_t nc, uint8_t dr);
    Frame(Frame &&) = default;
    Frame &operator=(Frame &&) = default;
    Frame(const Frame &) = delete;            // disable copy constructor
    Frame &operator=(const Frame &) = delete; // and copy assign
    ~Frame() = default;

    ssize_t rows() const;
    ssize_t cols() const;
    image_shape shape() const;
    uint8_t bits_per_pixel() const;
    uint8_t bytes_per_pixel() const;
    size_t total_bytes() const;
    
    std::byte *data();
    double operator()(size_t row, size_t col) const;
    template <typename T> DataSpan<T, 2> view();
};

} // namespace pl
