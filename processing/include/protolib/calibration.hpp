#pragma once

#include "protolib/ImageData.hpp"
#include "protolib/defs.hpp"
#include "protolib/utils.hpp"
#include "protolib/conversion_helper.hpp"

namespace pl {

template <typename T>
ImageData<T> apply_calibration(DataSpan<uint16_t, 2> raw_data,
                               DataSpan<T, 3> pedestal,
                               DataSpan<T, 3> calibration) {

    ImageData<T> img(raw_data.shape());

    for (ssize_t row = 0; row != raw_data.shape(0); ++row) {
        for (ssize_t col = 0; col != raw_data.shape(1); ++col) {
            img(row, col) = 5;
            auto pixel = raw_data(row,col);
            auto gain = get_gain(pixel);
            auto adc_val = get_value(pixel);
            T val = (static_cast<T>(adc_val) - pedestal(gain, row, col)) /
                      calibration(gain, row, col);
            img(row, col) = val;
        }
    }

    return img;
}

} // namespace pl