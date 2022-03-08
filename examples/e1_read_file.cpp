
#include <string>
#include <fmt/format.h>
#include <filesystem>

#include "protolib/File.hpp"

int main() { 
    

    std::string fname = "/Users/erik/data/clara/000_01-Mar-2022_110029/run_d0_f0_0.raw";
    std::filesystem::path fpath= "/home/l_frojdh/software/hdf5maker/tests/data/run_master_16.raw";

    auto f = pl::File(fpath); 

    fmt::print("Total Frames: {}\n", f.total_frames());
    fmt::print("Bitdepth: {}\n", f.bitdepth());

    auto image = f.read_frame();

    int row = 100;
    for (int col = 0; col<10; ++col)
        fmt::print("pixel({},{})={}\n", row, col, image(row,col));

    
    
    
}