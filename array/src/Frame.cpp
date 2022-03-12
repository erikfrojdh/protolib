#include "protolib/Frame.hpp"

namespace pl {

Frame::Frame(size_t nr, size_t nc, uint8_t dr)
    : nrows(nr), ncols(nc), bitdepth_(dr) {
    data_ = std::make_unique<std::byte[]>(nrows * ncols * bitdepth_ / 8);
}
double Frame::operator()(size_t row, size_t col) const {
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
ssize_t Frame::rows() const { return nrows; }
ssize_t Frame::cols() const { return ncols; }
uint8_t Frame::bits_per_pixel() const { return bitdepth_; }
uint8_t Frame::bytes_per_pixel() const {return bitdepth_/8;}
size_t Frame::total_bytes() const {return nrows*ncols*bytes_per_pixel();}

image_shape Frame::shape() const{
    return {nrows, ncols};
}

std::byte *Frame::data() { return data_.get(); }


template<typename T>
DataSpan<T,2> Frame::view(){
    return DataSpan<T,2>(reinterpret_cast<T*>(data_.get()), std::array<ssize_t, 2>{nrows, ncols});

}

template DataSpan<uint8_t, 2> Frame::view<uint8_t>();
template DataSpan<uint16_t, 2> Frame::view<uint16_t>();
template DataSpan<uint32_t, 2> Frame::view<uint32_t>();

} // namespace pl
