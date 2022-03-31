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
    void close();

    template <typename T, ssize_t Ndim> ImageData<T, Ndim> read_as() {
        auto vec = shape();
        if (vec.size() != Ndim)
            throw std::runtime_error("pof");

        // check that the type matches!

        auto shape = make_array<Ndim>(vec);

        ImageData<T, Ndim> data{shape};
        ifs_.read(reinterpret_cast<char *>(data.buffer()),
                  data.size() *
                      sizeof(typename ImageData<T, Ndim>::value_type));
        return data;
    };

    //Save any type that has a .shape(), ::value_type and .total_bytes()
    //TODO! Generalize? 
    template <typename T>
    static void save(std::filesystem::path fpath, T &data) {

        auto s = data.shape();
        dynamic_shape shape(s.begin(), s.end());

        DataType dt(typeid(typename T::value_type));
        NumpyFileHeader h(dt, shape);
        auto header_str = h.str();

        std::ofstream ofs(fpath);
        ofs.write(&header_str[0], header_str.size());
        ofs.write(reinterpret_cast<char *>(data.buffer()), data.total_bytes());
    }
};

} // namespace pl
