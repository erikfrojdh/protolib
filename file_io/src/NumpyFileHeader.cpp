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
    uint16_t static_header_length = magic_str.size() + sizeof(major_ver_) +
                           sizeof(minor_ver_) + sizeof(uint16_t);
    uint16_t header_length = static_header_length + description.size();
    
    //align to 64
    header_length += 64 - (header_length % 64);
    std::string header(header_length, '\x20');

    //adjust so that header_length reflect the bytes that can be read
    header_length -= static_header_length;

    char* dst = &header[0];
    memcpy(dst, &magic_str[0], sizeof(magic_str));
    dst += sizeof(magic_str);
    memcpy(dst, &major_ver_, sizeof(major_ver_));
    dst += sizeof(major_ver_);
    memcpy(dst, &minor_ver_, sizeof(minor_ver_));
    dst += sizeof(minor_ver_);
    memcpy(dst, &header_length, sizeof(header_length));
    dst += sizeof(header_length);
    memcpy(dst, &description[0], description.size());

    return header;
}

} // namespace pl