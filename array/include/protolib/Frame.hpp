#pragma once

#include "protolib/ImageData.hpp"
#include <fmt/format.h>

namespace pl
{
/*
Provides a dead simple interface to an array in memory

Open:
* Can we assume integer values for pixels? 

*/
class Frame{
    size_t nrows;
    size_t ncols;
    uint8_t bitdepth_;
    std::unique_ptr<std::byte[]> data_;

public:
    Frame(size_t nr, size_t nc, uint8_t dr);
    double operator()(size_t row, size_t col);
    size_t n_rows();
    size_t n_cols();
    uint8_t bitdepth();
    std::byte* data();
    ImageData<double> to_double();

};

} // namespace pl

