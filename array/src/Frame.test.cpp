#include <catch2/catch.hpp>

#include "protolib/Frame.hpp"

using namespace pl;
TEST_CASE("Default construction"){
    //This constructs an empty and not so useful object
    Frame img;
    REQUIRE(img.rows() == 0);
    REQUIRE(img.cols() == 0);
    REQUIRE(img.data() == nullptr);
    REQUIRE(img.bits_per_pixel() == 0);
    REQUIRE(img.bytes_per_pixel() == 0);
    REQUIRE(img.total_bytes()== 0);
    REQUIRE(img.shape() == std::array<ssize_t,2>{0,0});
}

TEST_CASE("Construct a frame holding uint8_t"){
    Frame img(10, 5, 8); //row, col, bits_per_pixel
    REQUIRE(img.rows() == 10);
    REQUIRE(img.cols() == 5);
    REQUIRE(img.shape() == std::array<ssize_t,2>{10,5});
    REQUIRE(img.data() != nullptr);
    REQUIRE(img.bits_per_pixel() == 8);
    REQUIRE(img.bytes_per_pixel() == 1);
    REQUIRE(img.total_bytes()== 50);
}

TEST_CASE("Move assign a frame"){
    Frame a(10,5,8);
    Frame b(7,3,32);

    //copy in some data in b
    std::vector<uint32_t> vec(7*3);
    for (uint32_t i=0; i!=vec.size(); ++i)
        vec[i] = i;
    memcpy(b.data(), vec.data(), vec.size()*sizeof(decltype(vec)::value_type));

    std::byte* b_data = b.data(); //to check after move

    a = std::move(b);

    REQUIRE(a.rows() == 7);
    REQUIRE(a.cols() == 3);
    REQUIRE(a.shape() == std::array<ssize_t,2>{7,3});
    REQUIRE(a.bits_per_pixel() == 32);
    REQUIRE(a.bytes_per_pixel() == 4);
    REQUIRE(a.total_bytes() == 84);
    REQUIRE(a.data()==b_data); //a took over the data from b
    
    REQUIRE(a(0,0) == 0.0);
    REQUIRE(a(0,1) == 1.0);
    REQUIRE(a(6,2) == 20.0);

    //the moved from object will be in a "valid but unspecified" state
    //hence no tests for it

}