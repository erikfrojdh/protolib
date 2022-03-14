#include "protolib/NumpyFile.hpp"
#include "protolib/defs.hpp"
#include <array>
#include <catch2/catch.hpp>
#include <vector>

#include "protolib/utils.hpp"
using namespace pl;



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