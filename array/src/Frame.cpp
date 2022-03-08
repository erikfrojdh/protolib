#include "protolib/Frame.hpp"

namespace pl {

Frame::Frame(size_t nr, size_t nc, uint8_t dr)
    : nrows(nr), ncols(nc), bitdepth_(dr) {
    data_ = std::make_unique<std::byte[]>(nrows * ncols * bitdepth_ / 8);
}
double Frame::operator()(size_t row, size_t col) {
    const size_t i = (row * ncols + col) * (bitdepth_ / 8);
    if (bitdepth_ == 8){
        return static_cast<double>(
            *reinterpret_cast<uint8_t *>(data_.get() + i));
    }
    else if (bitdepth_ == 16) {
        return static_cast<double>(
            *reinterpret_cast<uint16_t *>(data_.get() + i));
    }else if(bitdepth_ == 32){
        return static_cast<double>(
            *reinterpret_cast<uint32_t *>(data_.get() + i));
    }
    return 3.14;
}
size_t Frame::n_rows() { return nrows; }
size_t Frame::n_cols() { return ncols; }
uint8_t Frame::bitdepth() { return bitdepth_; }

std::byte *Frame::data() { return data_.get(); }

ImageData<double> Frame::to_double() {
    ImageData<double> img({nrows, ncols});
    for (size_t i = 0; i < nrows; ++i) {
        for (size_t j = 0; j < ncols; ++j) {
            img(i, j) = (*this)(i, j);
        }
    }
    return img;
}

} // namespace pl
