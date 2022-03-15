#pragma once

#include "protolib/ImageData.hpp"
#include "protolib/NumpyFileHeader.hpp"
#include "protolib/defs.hpp"
#include <filesystem>
#include <fstream>

/*
https://numpy.org/devdocs/reference/generated/numpy.lib.format.html

6 bytes magic string: \x93NUMPY
1 byte major ver
1 byte minor ver
2 bytes little endian uint16_t HEADER_LEN (divisible with 64)
len(magic string) + 2 + len(length) + HEADER_LEN padded with space \x20
*/



namespace pl {
class NumpyFile {
    NumpyFileHeader header_;
    std::ifstream ifs_;

  public:
    NumpyFile();
    NumpyFile(const std::filesystem::path &fpath);
    dynamic_shape shape() const;
    DataType dtype() const;

    template <typename T, ssize_t Ndim> ImageData<T, Ndim> read_as() {
        auto vec = shape();
        if (vec.size() != Ndim)
            throw std::runtime_error("pof");

        //check that the type matches!

        auto shape = make_array<Ndim>(vec);

        ImageData<T, Ndim> data{shape};
        ifs_.read(reinterpret_cast<char *>(data.buffer()),
                  data.size() *
                      sizeof(typename ImageData<T, Ndim>::value_type));
        return data;
    };
};

} // namespace pl
