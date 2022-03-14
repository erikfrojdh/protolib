#pragma once

#include "protolib/NumpyFileHeader.hpp"
#include "protolib/ImageData.hpp"
#include "protolib/defs.hpp"
#include <filesystem>

namespace pl{
class NumpyFile{
    NumpyFileHeader header_;
    public:
        NumpyFile();
        NumpyFile(const std::filesystem::path& fpath );
        dynamic_shape shape() const;
        DataType dtype() const;

        template<typename T, ssize_t Ndim>
        ImageData<T, Ndim> read_as(){
            auto vec = shape();
            if (vec.size() != Ndim)
                throw std::runtime_error("pof");
            std::array<ssize_t, Ndim> shape{};
            std::copy(vec.begin(), vec.end(), shape.begin());
            ImageData<T, Ndim> data{shape};
            return data;
        };
};

}

