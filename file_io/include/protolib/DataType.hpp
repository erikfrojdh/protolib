#pragma once
#include <string>
#include <iostream>
#include <fmt/core.h>
namespace pl {

enum class endian
{
#ifdef _WIN32
    little = 0,
    big    = 1,
    native = little
#else
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};


class DataType {
    static_assert(sizeof(long)==sizeof(int64_t), "long should be 64bits");
  public:
    enum  TypeIndex { INT8, UINT8, INT16, UINT16, INT32, UINT32, INT64, UINT64, FLOAT, DOUBLE, ERROR };
    


    explicit DataType(const std::type_info &t);
    explicit DataType(std::string_view sv);
    //not explicit to allow conversions form enum to DataType
    DataType(DataType::TypeIndex ti); 

    bool operator==(const DataType &other) const noexcept;
    bool operator!=(const DataType &other) const noexcept;
    bool operator==(const std::type_info &t) const;
    bool operator!=(const std::type_info &t) const;
    // bool operator==(DataType::TypeIndex ti) const;
    // bool operator!=(DataType::TypeIndex ti) const;
    std::string str() const;

  
  private:
    TypeIndex type_{TypeIndex::ERROR};

};

inline std::ostream &operator<<(std::ostream &os, const DataType &dt) {
    os << dt.str();
    return os;
}

} // namespace pl