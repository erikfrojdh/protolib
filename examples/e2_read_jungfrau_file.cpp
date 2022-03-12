
#include "protolib/protolib.hpp"

#include <filesystem>

int main() {

    // For convenience
    using pl::ImageData;
    namespace fs = std::filesystem;

    // This would be a jungfrau file with 100 frames
    fs::path fpath = pl::test_data_path() / "jungfrau/run_d0_f0_1.raw";

    auto f = pl::JungfrauRawFile(fpath, 512, 1024);

    // Now we can read directly to a typed image
    ImageData<uint16_t> image = f.read_frame();

    // Here is a simple example how to count pixels in gain 1
    int count = 0;
    for (const auto &pixel : image) {
        if (pixel >> 14 == 1)
            count++;
    }
    fmt::print("{} pixels are in gain 1\n", count);


    // Or with one of the standard algorithms
    auto count2 = std::count_if(image.begin(), image.end(),
                            [](const auto &p) { return (p >> 14) == 1; });
    fmt::print("{} pixels are in gain 1\n", count2);
}