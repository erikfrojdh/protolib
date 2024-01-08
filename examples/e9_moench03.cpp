
#include "protolib/protolib.hpp"
#include <filesystem>

int main(){ 
    

    fs::path fpath = "/mnt/sls_det_storage/moench_data/EMF/2023-06-19/NPS_60keV/run_d0_f0_0.raw";
    fmt::print("{}\n", fpath.c_str());


    constexpr ssize_t rows = 400;
    constexpr ssize_t cols = 400;

    auto f = pl::Moench03RawFile(fpath, rows, cols);

    for (int i = 0; i<10; ++i){
        pl::ImageData<uint16_t> img({rows, cols});
        auto h = f.read_into(img.buffer());
        fmt::print("Read frame: {}\n", h.frameNumber);
    }

}