#include "protolib/NumpyFile.hpp"

namespace fs = std::filesystem;

namespace pl{

NumpyFile::NumpyFile(){};
NumpyFile::NumpyFile(const fs::path& fpaht){
    header_ = NumpyFileHeader::fromFile(fpaht);
}

dynamic_shape NumpyFile::shape() const{
    return header_.shape();
}


}