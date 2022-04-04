#include "protolib/RawMasterFile.hpp"
#include "catch2/catch.hpp"
#include "protolib/File.hpp"
#include "protolib/defs.hpp"


using pl::RawMasterFile;

TEST_CASE("RawMasterFile parses basic file names") {
    std::string fname = "run_master_0.raw";
    RawMasterFile f;
    f.parse_fname(fname);
    REQUIRE(f.path() == "");
    REQUIRE(f.data_fname(0, 1) == "run_d0_f1_0.raw");
    REQUIRE(f.data_fname(12, 333) == "run_d12_f333_0.raw");
    REQUIRE(f.data_fname(1, 3) == "run_d1_f3_0.raw");
    REQUIRE(f.data_fname(123, 2) == "run_d123_f2_0.raw");
}

TEST_CASE("RawMasterFile parses basic file names with path") {
    std::string fname = "/data/folder/run_master_32.raw";
    RawMasterFile f;
    f.parse_fname(fname);
    REQUIRE(f.path() == "/data/folder");
    REQUIRE(f.data_fname(1, 2) == "/data/folder/run_d1_f2_32.raw");
}

TEST_CASE("Guessing if file is master from filename") {
    REQUIRE(pl::is_master_file("run_master_0.raw"));
    REQUIRE_FALSE(pl::is_master_file("master_d0_f1_0.raw"));
}

TEST_CASE("Parse an Eiger 500k File") {
    auto fpath = pl::test_data_path() / "eiger/run_master_0.raw";
    // RawMasterFile f(fpath);

    // REQUIRE(f.version() == "6.3");
    // //Timestamp
    // REQUIRE(f.type() == pl::DetectorType::Eiger);
    // REQUIRE(f.timing_mode() == pl::TimingMode::Auto);
    // //size
    // //pixels
    // REQUIRE(f.subfile_cols() == 512);
    // REQUIRE(f.subfile_rows() == 256);

    // REQUIRE(f.n_subfiles() == 4);
    // REQUIRE(f.total_frames() == 1);

    // REQUIRE(f.bitdepth() == 32);
    // TODO! map<string, string>
    // pl::read_raw_master_file(fpath);
}

TEST_CASE("Opening a file that doesn't exists,throws") {

    REQUIRE_THROWS(RawMasterFile("/dev/shm/nonexistent/run_master_0.raw"));
}

TEST_CASE("Loop over multi part files") {
    auto fpath = pl::test_data_path() / "mythen3/series/run_master_0.raw";
    RawMasterFile f(fpath);
    REQUIRE(f.total_frames() == 134);
    REQUIRE(f.type() == pl::DetectorType::Mythen3);
    REQUIRE(f.version() == "6.4");

    std::byte* buffer = new std::byte[f.bytes_per_frame()];
    for (size_t i = 0; i!=f.total_frames(); ++i) {
        REQUIRE(f.tell() == i);
        f.read_into(buffer);

    }
    delete[] buffer;
}


TEST_CASE("json master file"){
    auto fpath = pl::test_data_path() / "mythen3/run_v7_master_0.json";
    RawMasterFile f(fpath);
    REQUIRE(f.total_frames() == 10);
    REQUIRE(f.type() == pl::DetectorType::Mythen3);
    REQUIRE(f.version() == "7.0");
    REQUIRE(f.subfile_rows()== 1);
    REQUIRE(f.subfile_cols()== 3840);
}