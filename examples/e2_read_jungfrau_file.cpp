
#include "protolib/RawFile.hpp"
#include <filesystem>

#include "protolib/ImageData.hpp"

int main() {

    std::filesystem::path fpath =
        "/home/l_frojdh/software/hdf5maker/tests/data/run_d0_f0_0.raw";

    auto f = pl::JungfrauRawFile(fpath);

    auto img = f.read_frame();

    for (int i = 0; i<10; ++i)
        fmt::print("pixel(3,{})={}\n", i, img(3,i));


}