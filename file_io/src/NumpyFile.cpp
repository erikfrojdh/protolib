#include "protolib/NumpyFile.hpp"

namespace fs = std::filesystem;

namespace pl{

NumpyFile::NumpyFile(){};
NumpyFile::NumpyFile(const fs::path& fpaht)
:ifs_(fpaht)
{
    header_ = NumpyFileHeader::fromFile(ifs_);
}

dynamic_shape NumpyFile::shape() const{
    return header_.shape();
}

DataType NumpyFile::dtype() const{
    return DataType(header_.dtype());
}

void NumpyFile::close(){
    ifs_.close();
}

}