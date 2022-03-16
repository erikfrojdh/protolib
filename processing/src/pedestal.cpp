#include "protolib/pedestal.hpp"
#include "protolib/File.hpp"
#include "protolib/RawMasterFile.hpp"
#include "protolib/conversion_helper.hpp"

#include "tbb/parallel_for.h"
namespace pl {


ImageData<double, 2> mean_from_file(const std::filesystem::path &fpath) {
    File f(fpath);
    //For Jungfrau with 14 bits we can sum ~260k frames before
    //overflowing an uint32_t. 
    ImageData<uint32_t, 2> total{f.shape(), 0};
    ImageData<uint16_t, 2> current{f.shape()};
    for (size_t i = 0; i != f.total_frames(); ++i) {
        f.read_into(current.buffer());

        // this is our kernel
         for (ssize_t j = 0; j != current.size(); ++j) {
             auto val = current(j);
             total(j) += get_value(val);
         }

        //for this op and uint32 parallel for doesn't seem to help
        // tbb::parallel_for(tbb::blocked_range<int>(0, current.size()),
        //                   [&](tbb::blocked_range<int> r) {
        //                       for (int j = r.begin(); j < r.end(); ++j) {
        //                           auto val = current(j);
        //                           total(j) += get_value(val);
        //                       }
        //                   });
    }

    ImageData<double, 2> mean{f.shape()};
    double n_frames = f.total_frames();
    std::transform(total.begin(), total.end(), mean.begin(),
                   [&n_frames](auto &v) { return v / n_frames; });

    return mean;
}
ImageData<double, 3> pd_from_file(const std::filesystem::path &fpath) {

    RawMasterFile f(fpath);
    auto findex = f.file_index();

    Shape<3> pd_shape{3, f.rows(), f.cols()};
    ImageData<double, 3> pedestal{pd_shape};

    auto dst = pedestal.buffer();
    for (int i = 0; i != 3; ++i) {
        auto p = mean_from_file(f.path() / f.master_fname());
        memcpy(dst, p.buffer(), p.total_bytes());
        dst += p.total_bytes();
        f.set_file_index(++findex);
    }
    return pedestal;
}

} // namespace pl