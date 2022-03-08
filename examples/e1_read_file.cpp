
#include <string>
#include <fmt/format.h>
#include "protolib/file.hpp"

int main() { 
    
    // //Ideal case
    // std::string fname = "/Users/erik/data/clara/000_01-Mar-2022_110029/run_master_0.raw";
    // fmt::print("Loading: {}\n", fname);
    // auto f = pl::open_file(fname); 

    std::string fname = "/Users/erik/data/clara/000_01-Mar-2022_110029/run_d0_f0_0.raw";
    auto f = pl::JungfrauRawFile(fname);
    fmt::print("Number of frames: {}\n", f.n_frames());

    for (int i = 0; i<10; ++i){
        auto h = f.read_header(i);
        fmt::print("Frame number {}\n", h.frameNumber);
    }
    
    
    
}