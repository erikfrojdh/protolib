#include <catch2/catch.hpp>

#include "protolib/ClusterFinder.hpp"
#include "protolib/ImageData.hpp"
#include "protolib/defs.hpp"
using namespace pl;

TEST_CASE("Find clusters on adjacent edges"){

    ImageData<double, 2> image{Shape<2>{10,10}, 0};
    image(5,9) = 8;
    image(6,9) = 32;
    image(6,0) = 2;

    DataSpan<double, 2> span(image.data(), image.shape());

    ClusterFinder<double>cf(image.shape(), 0.5);

    cf.find_clusters(span);
    REQUIRE(cf.total_clusters() == 2);
    auto hits = cf.steal_hits();
    REQUIRE(hits[0].size == 1);
    REQUIRE(hits[0].energy == 2);
    REQUIRE(hits[1].size == 2);
    REQUIRE(hits[1].energy == 40);
    //TODO! Not depend on order of clusters? 
}