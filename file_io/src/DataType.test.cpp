

#include "protolib/DataType.hpp"
#include <catch2/catch.hpp>

using pl::DataType;
using pl::endian;

TEST_CASE("Construct from typeid") {
    REQUIRE(DataType(typeid(int)) == typeid(int));
    REQUIRE(DataType(typeid(int)) != typeid(double));
}

TEST_CASE("Construct from string") {
    if (endian::native == endian::little){
        REQUIRE(DataType("<i1") == typeid(int8_t));
        REQUIRE(DataType("<u1") == typeid(uint8_t));
        REQUIRE(DataType("<i2") == typeid(int16_t));
        REQUIRE(DataType("<u2") == typeid(uint16_t));
        REQUIRE(DataType("<i4") == typeid(int));
        REQUIRE(DataType("<u4") == typeid(unsigned));
        REQUIRE(DataType("<i4") == typeid(int32_t));
        REQUIRE(DataType("<i8") == typeid(long));
        REQUIRE(DataType("<i8") == typeid(int64_t));
        REQUIRE(DataType("<u4") == typeid(uint32_t));
        REQUIRE(DataType("<u8") == typeid(uint64_t));
        REQUIRE(DataType("f4") == typeid(float));
        REQUIRE(DataType("f8") == typeid(double));
    }

    if (endian::native == endian::big){
        REQUIRE(DataType(">i1") == typeid(int8_t));
        REQUIRE(DataType(">u1") == typeid(uint8_t));
        REQUIRE(DataType(">i2") == typeid(int16_t));
        REQUIRE(DataType(">u2") == typeid(uint16_t));
        REQUIRE(DataType(">i4") == typeid(int));
        REQUIRE(DataType(">u4") == typeid(unsigned));
        REQUIRE(DataType(">i4") == typeid(int32_t));
        REQUIRE(DataType(">i8") == typeid(long));
        REQUIRE(DataType(">i8") == typeid(int64_t));
        REQUIRE(DataType(">u4") == typeid(uint32_t));
        REQUIRE(DataType(">u8") == typeid(uint64_t));
        REQUIRE(DataType("f4") == typeid(float));
        REQUIRE(DataType("f8") == typeid(double));
    }

}

TEST_CASE("Construct from string with endianess"){
    //TODO! handle big endian system in test!
    REQUIRE(DataType("<i4") == typeid(int32_t));
    REQUIRE_THROWS(DataType(">i4") == typeid(int32_t));
}


TEST_CASE("Convert to string") { REQUIRE(DataType(typeid(int)).str() == "<i4"); }

