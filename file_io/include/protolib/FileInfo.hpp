#pragma once
#include <cstdint>
#include <cstddef>

#include "protolib/defs.hpp"

namespace pl{

struct FileInfo {
    size_t n_frames{};
    size_t n_rows{};
    size_t n_cols{};
    uint8_t bitdepth{};
    DetectorType type{};

    size_t bytes_per_frame() const{
        return n_rows*n_cols*bitdepth/8;
    }
    size_t pixles() const{
        return n_rows*n_cols;
    }
};

}

