#include "protolib/DataType.hpp"
#include <string_view>
namespace pl {

DataType::DataType(const std::type_info &t) {
    if (t == typeid(int32_t))
        type_ = TypeIndex::INT32;
    else if (t == typeid(uint32_t))
        type_ = TypeIndex::UINT32;
    else if (t == typeid(int64_t))
        type_ = TypeIndex::INT64;
    else if (t == typeid(long))
        type_ = TypeIndex::INT64;
    else if (t == typeid(uint64_t))
        type_ = TypeIndex::UINT64;
    else if (t == typeid(float))
        type_ = TypeIndex::FLOAT;
    else if (t == typeid(double))
        type_ = TypeIndex::DOUBLE;
    else
        throw std::runtime_error(
            "Could not construct data type. Type not supported.");
}

DataType::DataType(DataType::TypeIndex ti):type_(ti){}

DataType::DataType(std::string_view sv) {

    // Check if the file is using our native endianess
    if (auto pos = sv.find_first_of("<>"); pos != std::string_view::npos) {
        const auto endianess = [](const char c) {
            if (c == '<')
                return endian::little;
            return endian::big;
        }(sv[pos]);
        if (endianess != endian::native) {
            throw std::runtime_error("Non native endianess not supported");
        }
    }

    // we are done with the endianess so we can remove the prefix
    sv.remove_prefix(std::min(sv.find_first_not_of("<>"), sv.size()));

    if (sv == "i4")
        type_ = TypeIndex::INT32;
    else if (sv == "u4")
        type_ = TypeIndex::UINT32;
    else if (sv == "i8")
        type_ = TypeIndex::INT64;
    else if (sv == "u8")
        type_ = TypeIndex::UINT64;
    else if (sv == "f4")
        type_ = TypeIndex::FLOAT;
    else if (sv == "f8")
        type_ = TypeIndex::DOUBLE;
    else
        throw std::runtime_error(
            "Could not construct data type. Type no supported.");
}

std::string DataType::str() const {
    switch (type_) {
    case TypeIndex::INT32:
        return "i4";
    case TypeIndex::UINT32:
        return "u4";
    case TypeIndex::INT64:
        return "i8";
    case TypeIndex::UINT64:
        return "u8";
    case TypeIndex::FLOAT:
        return "f4";
    case TypeIndex::DOUBLE:
        return "f8";
    case TypeIndex::ERROR:
        return "ERROR";
    }
    return {};
}

bool DataType::operator==(const DataType &other) const noexcept {
    return type_ == other.type_;
}
bool DataType::operator!=(const DataType &other) const noexcept {
    return !(*this == other);
}

bool DataType::operator==(const std::type_info &t) const {
    return DataType(t) == *this;
}
bool DataType::operator!=(const std::type_info &t) const {
    return DataType(t) != *this;
}

// bool DataType::operator==(DataType::TypeIndex ti) const { return type_ == ti; }

// bool DataType::operator!=(DataType::TypeIndex ti) const { return type_ != ti; }

} // namespace pl