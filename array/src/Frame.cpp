#include "protolib/Frame.hpp"

namespace pl {

Frame::Frame(size_t nr, size_t nc, uint8_t dr)
    : nrows(nr), ncols(nc), bitdepth_(dr) {
        if(bitdepth_%8 || bitdepth_>64){
            auto msg = fmt::format("Bitdept: {} is not supported", bitdepth_);
            throw std::runtime_error(msg);
        }
        data_ = std::make_unique<std::byte[]>(nrows * ncols * bitdepth_ / 8);
      }

double Frame::operator()(size_t row, size_t col) const {
    const size_t offset = (row * ncols + col) * (bitdepth_ / 8);
    auto pos = data_.get() + offset;

    if (bitdepth_ == 8) {
        return static_cast<double>(
            *reinterpret_cast<uint8_t *>(pos));
    } else if (bitdepth_ == 16) {
        return static_cast<double>(
            *reinterpret_cast<uint16_t *>(pos));
    } else if (bitdepth_ == 32) {
        return static_cast<double>(
            *reinterpret_cast<uint32_t *>(pos));
    }else if (bitdepth_ == 64) {
        return static_cast<double>(
            *reinterpret_cast<uint64_t *>(pos));
    }
    throw std::runtime_error("Unsupported bitdepth");
}
ssize_t Frame::rows() const { return nrows; }
ssize_t Frame::cols() const { return ncols; }
uint8_t Frame::bits_per_pixel() const { return bitdepth_; }
uint8_t Frame::bytes_per_pixel() const { return bitdepth_ / 8; }
size_t Frame::total_bytes() const { return nrows * ncols * bytes_per_pixel(); }

image_shape Frame::shape() const { return {nrows, ncols}; }

std::byte *Frame::data() { return data_.get(); }

template <typename T> DataSpan<T, 2> Frame::view() {
    if (sizeof(T) != bytes_per_pixel()) {
        auto msg = fmt::format(
            "Size of data in frame ({}) does not match the requested view ({})",
            sizeof(T), bytes_per_pixel());
        throw std::runtime_error(msg);
    }

    return DataSpan<T, 2>(reinterpret_cast<T *>(data_.get()),
                          image_shape{nrows, ncols});
}

template DataSpan<uint8_t, 2> Frame::view<uint8_t>();
template DataSpan<uint16_t, 2> Frame::view<uint16_t>();
template DataSpan<uint32_t, 2> Frame::view<uint32_t>();

} // namespace pl
