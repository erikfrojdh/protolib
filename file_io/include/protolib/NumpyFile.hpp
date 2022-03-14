#pragma once

#include "protolib/NumpyFileHeader.hpp"
#include "protolib/defs.hpp"
#include <filesystem>

namespace pl{
class NumpyFile{
    NumpyFileHeader header_;
    public:
        NumpyFile();
        NumpyFile(const std::filesystem::path& fpath );
        dynamic_shape shape() const;
};

}

