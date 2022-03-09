/*
Looping over a file accessing pixel 100,100 in all frames. 
Since we don't know the data type of the stored data we
need to use the generice container Frame and convert pixel
values to double. 

In this example the actual file could be from any supported
detector and from any supported file type. 
*/

#include <fmt/format.h>
#include <filesystem>
#include "protolib/File.hpp"

int main() { 
    //For convenience
    using pl::File;
    namespace fs = std::filesystem;

    //This would be a jungfrau file with 100 frames
    fs::path fpath = pl::test_data_path()/"jungfrau/run_master_0.raw";

    File file(fpath); 
    double total = 0;
    for(auto& image : file){
        total += image(100,100);
    }
    fmt::print("The sum of pixel 100,100 is {}\n", total);
}