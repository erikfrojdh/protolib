#include "protolib/defs.hpp"
#include <catch2/catch.hpp>

using namespace pl;

TEST_CASE("String conversion of detector type"){
    REQUIRE(StringTo<DetectorType>("Mythen3")== DetectorType::Mythen3);
     REQUIRE(StringTo<DetectorType>("Eiger")== DetectorType::Eiger);
     REQUIRE(StringTo<DetectorType>("Jungfrau")== DetectorType::Jungfrau);
}

// TEST_CASE("From string to type"){
//     REQUIRE(ToString)
// }