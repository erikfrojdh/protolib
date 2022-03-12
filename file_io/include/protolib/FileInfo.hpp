#pragma once
#include <cstdint>
#include <cstddef>

#include "protolib/defs.hpp"

namespace pl{

struct FileInfo {
    size_t total_frames{};
    ssize_t rows{};
    ssize_t cols{};
    uint8_t bitdepth{};
    DetectorType type{};

    size_t bytes_per_frame() const{
        return rows*cols*bitdepth/8;
    }
    size_t pixles() const{
        return rows*cols;
    }
};

}

