#include <catch2/catch.hpp>

#include "protolib/utils.hpp"
TEST_CASE("str2vec"){
    REQUIRE(std::vector<int>{1,2,3}==pl::str2vec<int>("1,2,3"));
    REQUIRE(std::vector<int>{10}==pl::str2vec<int>("10,"));
    REQUIRE(std::vector<int>{10}==pl::str2vec<int>("10"));
}
TEST_CASE("find_between") {
    std::string s = "some text: 'value', and some more text";
    auto r = pl::find_between('\'', s);
    REQUIRE(r == "value");
}

TEST_CASE("Find text between :"){
    std::string s = "Here is some: text with: in it";
    auto r = pl::find_between(':', s);
    REQUIRE(r == " text with");

}

TEST_CASE("find_between ()") {
    std::string s = "some text with a (value) between ( and )";
    auto r = pl::find_between('(', ')', s);
    REQUIRE(r == "value");
}

TEST_CASE("parse empty string") {
    std::string s;
    REQUIRE(pl::str2vec<int>(s).empty());
}

TEST_CASE("parse single element vec") {
    std::string s = "(50,)";
    REQUIRE(pl::str2vec<int>(s) == std::vector<int>{50});
}

TEST_CASE("parse two element vec") {
    std::string s = "(789, 788,)";
    REQUIRE(pl::str2vec<int>(s) == std::vector<int>{789, 788});
}

TEST_CASE("parse three element vec") {
    std::string s = "(1, 2, 3)";
    REQUIRE(pl::str2vec<ssize_t>(s) == std::vector<ssize_t>{1, 2, 3});
}

TEST_CASE("vec to string"){
    std::vector<int> vec{1,2,3};
    REQUIRE(pl::vec2str(vec) == "(1, 2, 3,)");
}

TEST_CASE("vec to string 2"){
    std::vector<ssize_t> vec{379, 1};
    REQUIRE(pl::vec2str(vec) == "(379, 1,)");
}

TEST_CASE("empty vector to string"){
    std::vector<int> vec;
    REQUIRE(pl::vec2str<int>(vec) == "()");
}

TEST_CASE("split a range in three parts"){
    auto res = pl::split_task(0, 1000, 3);
    REQUIRE(res.size()== 3);
    REQUIRE(res[0].first == 0);
    REQUIRE(res[0].second == 333);
    REQUIRE(res[1].first == 333);
    REQUIRE(res[1].second == 666);
    REQUIRE(res[2].first == 666);
    REQUIRE(res[2].second == 1000);
}