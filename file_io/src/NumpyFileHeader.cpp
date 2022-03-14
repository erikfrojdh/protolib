#include "protolib/NumpyFileHeader.hpp"

namespace pl{

NumpyArrDescr::NumpyArrDescr(std::string_view sv) {
        constexpr char key_0[] = "\'descr\'";
        auto pos = sv.find(key_0);
        pos = sv.find_first_of(':', pos);
        descr_ =
            find_between('\'', std::string_view(&sv[pos], sv.size() - pos));

        constexpr char key_1[] = "\'fortran_order\'";
        pos = sv.find(key_1);
        pos = sv.find_first_of(':', pos);
        auto b =
            find_between('\'', std::string_view(&sv[pos], sv.size() - pos));
        fortran_order_ = (b == "True") ? true : false;

        constexpr char key_2[] = "\'shape\'";
        pos = sv.find(key_2);
        pos = sv.find_first_of(':', pos);
        auto s = find_between('(', ')',
                              std::string_view(&sv[pos], descr_.size() - pos));
        shape_ = str2vec<ssize_t>(s);
    }

NumpyArrDescr::NumpyArrDescr(DataType dt, std::vector<ssize_t> shape){
    descr_ = dt.str();
    shape_ = shape;
}
DataType NumpyArrDescr::dtype() const{
    return DataType{typeid(int)};
}






NumpyFileHeader::NumpyFileHeader(DataType dt, std::vector<ssize_t> sh)
        : major_ver_(1), minor_ver_(0), descr(dt, sh){

        }


}