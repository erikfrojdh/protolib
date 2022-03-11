#include <fmt/format.h>
#include <filesystem>
#include "protolib/File.hpp"
#include "protolib/RawMasterFile.hpp"

int main() { 
    //For convenience
    using pl::File;
    using pl::RawMasterFile;
    namespace fs = std::filesystem;

    //Find an Eiger frame
    fs::path fpath = pl::test_data_path()/"eiger/sample_master_2.raw";
    
    File f(fpath);
    auto image = f.read_frame();

    fmt::print("Shape: {}x{}\n", image.rows(), image.cols());
    fmt::print("Frames: {}\n", f.total_frames());
    

    // This way of summing is not the most efficient
    // since we cast to a double before summing. If 
    // performance matters use a typed interface. 
    double sum=0;
    for (ssize_t row=0; row != image.rows(); ++row){
        for (ssize_t col = 0; col != image.cols(); ++col){
            sum += image(row, col);
        }
    }
    fmt::print("Sum of image: {}\n", sum);
}