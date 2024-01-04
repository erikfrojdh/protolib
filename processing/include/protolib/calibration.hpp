#pragma once

#include "protolib/ImageData.hpp"
#include "protolib/conversion_helper.hpp"
#include "protolib/defs.hpp"
#include "protolib/utils.hpp"

//#include <tbb/parallel_for.h>
//#include <tbb/flow_graph.h>
#include <future>
namespace pl {

template <typename T>
ImageData<T> apply_calibration(DataSpan<uint16_t, 2> raw_data,
                               DataSpan<T, 3> pedestal,
                               DataSpan<T, 3> calibration) {

    ImageData<T> img(raw_data.shape());
    // tbb::parallel_for(
    //     tbb::blocked_range<int>(0, raw_data.shape(0)),
    //     [&](tbb::blocked_range<int> r) {
    //         for (int row = r.begin(); row < r.end(); ++row) {
    //             for (ssize_t col = 0; col != raw_data.shape(1); ++col) {
    //                 const auto pixel = raw_data(row, col);
    //                 const auto gain = get_gain(pixel);
    //                 const auto adc_val = get_value(pixel);
    //                 T val =
    //                     (static_cast<T>(adc_val) - pedestal(gain, row, col))
    //                     / calibration(gain, row, col);
    //                 img(row, col) = val;
    //             }
    //         }
    //     });

    for (ssize_t row = 0; row != raw_data.shape(0); ++row) {
        for (ssize_t col = 0; col != raw_data.shape(1); ++col) {
            const auto pixel = raw_data(row, col);
            const auto gain = get_gain(pixel);
            const auto adc_val = get_value(pixel);
            T val = (static_cast<T>(adc_val) - pedestal(gain, row, col)) /
                    calibration(gain, row, col);
            img(row, col) = val;
        }
    }

    return img;
}

template <typename T, typename Iterator>
ImageData<T> sum_raw_frames(Iterator first, Iterator last,
                            DataSpan<T, 3> pedestal, DataSpan<T, 3> calibration,
                            T threshold) {

    ImageData<T> total(first->shape());
    for (; first != last; ++first) {
        auto raw_data = first->template view<uint16_t>();
//        tbb::parallel_for(tbb::blocked_range<int>(0, raw_data.shape(0)),
//                          [&](tbb::blocked_range<int> r) {
//                              for (int row = r.begin(); row < r.end(); ++row) {
//                                  for (ssize_t col = 0;
//                                       col != raw_data.shape(1); ++col) {
//                                      const auto pixel = raw_data(row, col);
//                                      const auto gain = get_gain(pixel);
//                                      const auto adc_val = get_value(pixel);
 //                                     T val = (static_cast<T>(adc_val) -
//                                               pedestal(gain, row, col)) /
//                                              calibration(gain, row, col);
//                                      if (val > threshold)
//                                          total(row, col) += val;
//                                  }
//                              }
//                          });
    }
    return total;
}

template <typename T>
ImageData<T> sum_range_from_file(const fs::path &fname, DataSpan<T, 3> pedestal,
                                 DataSpan<T, 3> calibration, T threshold,
                                 ssize_t start, ssize_t stop) {

    File f(fname);
    fmt::print("summing: {}->{}\n", start, stop);
    ImageData<T> total(f.shape(), 0);
    for (ssize_t i = start; i != stop; ++i) {
        f.seek(i);
        auto raw_data = f.read_as<uint16_t>();

        for (ssize_t row = 0; row != raw_data.shape(0); ++row) {
            for (ssize_t col = 0; col != raw_data.shape(1); ++col) {
                const auto pixel = raw_data(row, col);
                const auto gain = get_gain(pixel);
                const auto adc_val = get_value(pixel);
                T val = (static_cast<T>(adc_val) - pedestal(gain, row, col)) /
                        calibration(gain, row, col);
                if (val > threshold)
                    total(row, col) += val;
            }
        }
    }
    return total;
}

template <typename T>
ImageData<T> sum_raw_file(const fs::path &fname, DataSpan<T, 3> pedestal,
                          DataSpan<T, 3> calibration, T threshold) {
    File f(fname);
    return sum_raw_frames(f.begin(), f.end(), pedestal, calibration, threshold);
}

template <typename T>
ImageData<T> sum_raw_file_parallel(const fs::path &fname,
                                   DataSpan<T, 3> pedestal,
                                   DataSpan<T, 3> calibration, T threshold) {
    File f(fname);
    size_t n_threads = 8;
    auto parts = split_task(0, f.total_frames(), n_threads);
    ImageData<T> total(f.shape(), 0);

    std::vector<std::future<ImageData<T, 2>>> result;
    for (auto p : parts){
        result.push_back(std::async(&sum_range_from_file<T>, fname, pedestal, calibration, threshold, p.first, p.second));
    }

    fmt::print("running\n");
    for (auto& r : result)
        total += r.get();

    fmt::print("bye\n");
    return total;
}




} // namespace pl
