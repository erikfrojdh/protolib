#include "protolib/NumpyFileHeader.hpp"

namespace pl {

NumpyArrDescr::NumpyArrDescr(std::string_view sv) {
    constexpr char key_0[] = "\'descr\'";
    auto pos = sv.find(key_0);
    pos = sv.find_first_of(':', pos);
    descr_ = find_between('\'', std::string_view(&sv[pos], sv.size() - pos));

    constexpr char key_1[] = "\'fortran_order\'";
    pos = sv.find(key_1);
    pos = sv.find_first_of(':', pos);
    auto b = find_between('\'', std::string_view(&sv[pos], sv.size() - pos));
    fortran_order_ = (b == "True") ? true : false;

    constexpr char key_2[] = "\'shape\'";
    pos = sv.find(key_2);
    pos = sv.find_first_of(':', pos);
    auto s =
        find_between('(', ')', std::string_view(&sv[pos], descr_.size() - pos));
    shape_ = str2vec<ssize_t>(s);
}

NumpyArrDescr::NumpyArrDescr(DataType dt, std::vector<ssize_t> shape) {
    descr_ = dt.str();
    shape_ = shape;
}
DataType NumpyArrDescr::dtype() const { return DataType{typeid(int)}; }

// String representation of header description
std::string NumpyArrDescr::str() const {
    return fmt::format("{{'descr': '{}', 'fortran_order': {}, 'shape': {}, }}",
                       descr_, (fortran_order_) ? "True" : "False",
                       vec2str(shape_));
}



NumpyFileHeader::NumpyFileHeader(DataType dt, std::vector<ssize_t> sh)
    : major_ver_(1), minor_ver_(0), descr(dt, sh) {}

  
// string representation of the header, as it will be written to file
std::string NumpyFileHeader::str() const {
    auto description = descr.str();
    size_t header_length = magic_str.size() + sizeof(major_ver_) +
                           sizeof(minor_ver_) + sizeof(uint16_t) + description.size();
    
    fmt::print("header_length: {}\n", header_length);
    header_length += 64 - (header_length % 64);
    fmt::print("header_length: {}\n", header_length);
    std::string header(header_length, '\x20');
    fmt::print("header.size(): {}\n", header.size());
    return header;
}

} // namespace pl