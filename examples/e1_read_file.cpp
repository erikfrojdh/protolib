
#include <fmt/format.h>
#include <filesystem>

#include "protolib/File.hpp"

int main() { 
    //For convenience
    using pl::File;
    namespace fs = std::filesystem;

    //This would be a jungfrau file with 100 frames
    fs::path fpath = pl::test_data_path()/"jungfrau/run_master_0.raw";

    File f(fpath);
    fmt::print("Frames: {}\n", f.total_frames());

    auto image = f.read_frame();
    fmt::print("pixel(100,100) = {}\n", image(100,100));

    //If we know the data type stored in the file we can make a view
    //refering to that type
    auto view = image.view<uint16_t>();
    fmt::print("pixel(100,100) = {}\n", view(100,100));

}