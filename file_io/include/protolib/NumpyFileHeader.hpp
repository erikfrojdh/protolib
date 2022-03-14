#include <array>
#include <fmt/core.h>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
// #include <iostream>

#include "protolib/DataType.hpp"
#include "protolib/utils.hpp"
#include "protolib/defs.hpp"
namespace pl {



class NumpyArrDescr {
    std::string descr_;
    bool fortran_order_{false};
    std::vector<ssize_t> shape_;

  public:
    NumpyArrDescr() = default;
    NumpyArrDescr(const NumpyArrDescr &) = default;
    NumpyArrDescr(NumpyArrDescr &&) = default;
    NumpyArrDescr &operator=(const NumpyArrDescr &) = default;
    
    NumpyArrDescr(std::string_view sv);
    NumpyArrDescr(DataType dt, dynamic_shape shape);

    dynamic_shape shape() const { return shape_; }
    std::string descr() const { return descr_; }
    bool fortran_order() const noexcept { return fortran_order_; }
    std::string str() const {
        return fmt::format(
            "{{'descr': '{}', 'fortran_order': {}, 'shape': {}, }}", descr_,
            (fortran_order_) ? "True" : "False", vec2str(shape_));
    }
};

class NumpyFileHeader {
    uint8_t major_ver_{};
    uint8_t minor_ver_{};
    NumpyArrDescr descr;

  public:
    static constexpr std::array<char, 6> magic_str{'\x93', 'N', 'U',
                                                   'M',    'P', 'Y'};
    uint8_t major_ver() const noexcept { return major_ver_; }
    uint8_t minor_ver() const noexcept { return minor_ver_; }

    

    dynamic_shape shape() const { return descr.shape(); }
    size_t ndim() const noexcept { return descr.shape().size(); }
    std::string dtype() const { return descr.descr(); }
    bool fortran_order() const noexcept { return descr.fortran_order(); }
    NumpyFileHeader() = default;
    NumpyFileHeader(DataType dt, dynamic_shape sh);

    static NumpyFileHeader fromFile(std::ifstream &f) {
        std::array<char, 6> tmp{};
        f.read(tmp.data(), tmp.size());
        if (tmp != NumpyFileHeader::magic_str) {
            for (auto item : tmp)
                fmt::print("{}, ", int(item));
            fmt::print("\n");
            throw std::runtime_error("Not a numpy file");
        }
        NumpyFileHeader h;
        f.read(reinterpret_cast<char *>(&h.major_ver_), 1);
        f.read(reinterpret_cast<char *>(&h.minor_ver_), 1);

        uint16_t header_len;
        f.read(reinterpret_cast<char *>(&header_len), 2);

        std::string header;
        header.resize(header_len);
        f.read(&header[0], header_len);
        h.descr = NumpyArrDescr(header);
        return h;
    }

    static NumpyFileHeader fromFile(const std::string &fname) {
        std::ifstream f(fname, std::ios::binary);
        if (!f)
            throw std::runtime_error("File not found");
        return NumpyFileHeader::fromFile(f);
    }
};

} // namespace pl