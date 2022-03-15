#include "protolib/NumpyFileHeader.hpp"
#include "protolib/defs.hpp"
#include <catch2/catch.hpp>

using namespace pl;

TEST_CASE("Load numpy header from file") {
    auto h = NumpyFileHeader::fromFile(test_data_path()/"numpy/np_1d.npy");
    REQUIRE(h.major_ver() == 1);
    REQUIRE(h.minor_ver() == 0);
    REQUIRE(h.dtype() == "<i4");
    REQUIRE(h.fortran_order() == false);
    REQUIRE(h.shape() == dynamic_shape{10});
}

TEST_CASE("Numpy file header from DataType and shape"){
    NumpyFileHeader h(DataType(typeid(int)), {10,15});
    REQUIRE(h.fortran_order() == false);
    REQUIRE(h.shape() == dynamic_shape{10,15});
}

TEST_CASE("Parse 1d string") {
    std::string s =
        "{'descr': '<i4', 'fortran_order': False, 'shape': (10,), }";
    auto r = NumpyArrDescr(s);
    REQUIRE(r.descr() == "<i4");
    REQUIRE(r.fortran_order() == false);
    REQUIRE(r.shape() == dynamic_shape{10});
    
    //we get the same string back 
    REQUIRE(r.str() == s);
}

TEST_CASE("build string to write to"){
    NumpyFileHeader h(DataType::INT32, {19,32});
    REQUIRE(h.shape() == dynamic_shape{19,32});

    auto s =  h.str();
    REQUIRE((s.size() % 64) == 0);

    REQUIRE(s.substr(0,6) == "\x93NUMPY");
    REQUIRE(s[6] == '\x01');
    REQUIRE(s[7] == '\x00');
    REQUIRE(*reinterpret_cast<uint16_t*>(&s[8]) == s.size()-10);

    auto p1 = s.find("{");
    auto p2 = s.find("}");

    auto descr = s.substr(p1, p2-p1+1);
    REQUIRE(descr == "{'descr': '<i4', 'fortran_order': False, 'shape': (19, 32,), }");
    

}
