#include "protolib/NumpyFile.hpp"
#include "protolib/defs.hpp"
#include <array>
#include <catch2/catch.hpp>
#include <vector>

#include "protolib/utils.hpp"
#include "protolib/ImageData.hpp"
using namespace pl;

namespace fs = std::filesystem;

TEST_CASE("Default construction of NumpyFile") { 
    NumpyFile f; 
    
}

TEST_CASE("Load a 1d file") {
    NumpyFile f(test_data_path() / "numpy/np_1d.npy");
    REQUIRE(f.shape() == dynamic_shape{10});
    REQUIRE(f.dtype() == DataType::INT32);

    auto data = f.read_as<int32_t, 1>();
    REQUIRE((data.shape() == f.shape())); //note extra () for catch
    for (int i = 0; i!=10; ++i){
        REQUIRE(data(i) == i);
    }
}


TEST_CASE("Write and read back a numpy file"){
     
    //Create some data
    pl::ImageData<int64_t, 2> img(pl::Shape<2>{10,15});
    for(int64_t i=0; i!=img.size(); ++i)
        img(i) = i;

    //write to a numpy file
    fs::path fpath = fs::temp_directory_path()/"tempfile_venowalscnla.npy";
    pl::NumpyFile::save(fpath, img);

    //read it back
    pl::NumpyFile f(fpath);
    ImageData<int64_t, 2> from_disk = f.read_as<int64_t, 2>();
    f.close();
    fs::remove(fpath);

    REQUIRE(f.dtype() == DataType::INT64);
    REQUIRE(img == from_disk);
}