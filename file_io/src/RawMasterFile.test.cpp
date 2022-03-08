#include "catch2/catch.hpp"
#include "protolib/RawMasterFile.hpp"

using pl::RawMasterFile;

TEST_CASE("RawMasterFile parses basic file names"){

    std::string fname = "run_master_0.raw";
    RawMasterFile f(fname);
    REQUIRE(f.path() == "");
    REQUIRE(f.raw_file(0,1)== "run_d0_f1_0.raw");
}