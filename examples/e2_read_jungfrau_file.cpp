

#include "protolib/File.hpp"
#include "protolib/RawFile.hpp"
#include "protolib/ImageData.hpp"

#include <filesystem>

int main() {

    //For convenience
    using pl::File;
    using pl::ImageData;
    namespace fs = std::filesystem;

    //This would be a jungfrau file with 100 frames
    fs::path fpath = pl::test_data_path()/"jungfrau/run_d0_f0_0.raw";

    auto f = pl::JungfrauRawFile(fpath);

    ImageData<uint16_t> img = f.read_frame();

    for (int i = 0; i<10; ++i)
        fmt::print("pixel(3,{})={}\n", i, img(3,i));


}