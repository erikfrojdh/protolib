#include <fmt/format.h>
#include <filesystem>
#include "protolib/File.hpp"
#include "protolib/RawMasterFile.hpp"

int main() { 
    //For convenience
    using pl::File;
    using pl::RawMasterFile;
    namespace fs = std::filesystem;

    //This would be a jungfrau file with 100 frames
    fs::path fpath = pl::test_data_path()/"eiger/run_master_0.raw";
    // fs::path fpath = pl::test_data_path()/"jungfrau/run_master_0.raw";

    RawMasterFile file(fpath); 
    // double total = 0;
    // for(auto& image : file){
    //     total += image(100,100);
    // }
    // fmt::print("The sum of pixel 100,100 is {}\n", total);
}