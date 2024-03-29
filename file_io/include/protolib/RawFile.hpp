#pragma once
#include "protolib/FileInfo.hpp"
#include "protolib/FilePtr.hpp"
#include "protolib/ImageData.hpp"

#include <cassert>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <variant>

namespace fs = std::filesystem;

namespace pl {

// concrete class meant to hold a raw file
//  *read header from the file
//  *read data from the file
//  *Does it need to know the data type?
// no bounds checking on this level

template <typename T> struct crtp {
    T &underlying() { return static_cast<T &>(*this); }
    T const &underlying() const { return static_cast<T const &>(*this); }
};

template <class Header, class DataType, class T>
class RawFile : public crtp<T> {

  protected:
    FilePtr fp;
    const ssize_t rows_{};
    const ssize_t cols_{};
    const size_t frames_per_file_{};
    std::string base_name_template;
    int sub_file_index_{};
    size_t current_frame_{};
    void parse_fname(std::string fname) {
        auto pos = fname.find_last_of('f');
        if (pos == std::string::npos && pos < fname.size())
            throw std::runtime_error(
                "Malformed filename, should follow name_dX_fY_Z.raw");

        // replace subfile index with {} to be used for formatting file names
        ++pos;
        size_t digits = 0;
        sub_file_index_ = std::stoi(fname.substr(pos), &digits);
        fname.replace(pos, digits, "{}");
        base_name_template = std::move(fname);
    }

    void open_file(int i) {
        sub_file_index_ = i; // check that file is open first??
        fp = FilePtr(fmt::format(base_name_template, i).c_str());
        auto msg = fmt::format(base_name_template, i);
        fmt::print("open: {}\n", msg);

        // assert(current_frame_ == sub_file_index_*frames_per_file_);
    }

  public:
    using value_type = DataType;

    RawFile(fs::path fname, ssize_t rows, ssize_t cols)
        : fp(fname.c_str()), rows_(rows), cols_(cols),
          frames_per_file_(
              fs::file_size(fname) /
              (sizeof(Header) + sizeof(DataType) * rows_ * cols_)) {

        parse_fname(fname);
    }

    RawFile(const RawFile &) = delete;
    RawFile &operator=(const RawFile &) = delete;
    RawFile(RawFile &&) = default;
    RawFile &operator=(RawFile &&) = default;
    ~RawFile() = default;

    size_t n_frames() const { return frames_per_file_; }
    std::array<ssize_t, 2> shape() const { return {rows_, cols_}; }

    void seek(size_t frame_number) {
        if (auto index = sub_file_index(frame_number);
            index != sub_file_index_) {
            open_file(index);
        }
        size_t frame_number_in_file = frame_number % frames_per_file_;

        // fmt::print("Going to file: {} and position {}\n", sub_file_index_,
        //            frame_number_in_file);

        fseek(fp.get(),
              (sizeof(Header) + bytes_per_frame()) * frame_number_in_file,
              SEEK_SET);
        current_frame_ = frame_number;
    }

    size_t tell() {
        return current_frame_;
        // auto pos = ftell(fp.get());
        // assert(pos % (sizeof(Header) + sizeof(DataType) * rows_ * cols_) ==
        // 0); return pos / (sizeof(Header) + sizeof(DataType) * rows_ * cols_);
    }

    int sub_file_index(size_t frame_number) const {
        return frame_number / frames_per_file_;
    }

    Header read_header(size_t frame_number) {
        seek(frame_number);
        auto h = read_header();
        seek(frame_number);
        return h;
    }

    ssize_t cols() const { return cols_; }
    ssize_t rows() const { return rows_; }

    Header read_header() {
        // need to check that we can read
        if (current_frame_ == frames_per_file_ * (sub_file_index_ + 1)) {
            open_file(sub_file_index_ + 1);
        }

        Header h{};
        size_t rc = fread(reinterpret_cast<char *>(&h), sizeof(h), 1, fp.get());

#ifdef PLDEBUG
        ssize_t pos = ftell(fp.get());
        ssize_t frame_in_file = current_frame_ % frames_per_file_;
        ssize_t expected =
            frame_in_file * (bytes_per_frame() + sizeof(Header)) +
            sizeof(Header);
        // fmt::print("frame_in_file: {}\npos: {}\nexpected: {}\n",
        // frame_in_file, pos, expected);
        assert(pos == expected);
#endif

        if (rc != 1)
            throw std::runtime_error("Could not read header from file");
        return h;
    }

    size_t frame_number(size_t fn) { return read_header(fn).frameNumber; }

    Header read_into(std::byte *buffer) {
        // This is our customization point for reading
        //
        auto h = read_header();
        size_t rc = this->underlying().read_impl(buffer);
        if (rc != 1)
            throw std::runtime_error("Could not read frame data from file");
        ++current_frame_;
        return h;
    }

    std::vector<Header> read_into(std::byte *buffer, size_t n_frames) {
        std::vector<Header> header(n_frames);
        for (size_t i = 0; i != n_frames; ++i) {
            header[i] = read_into(buffer);
            buffer += bytes_per_frame();
        }
        return header;
    }

    size_t bytes_per_frame() const { return sizeof(DataType) * rows_ * cols_; }

    size_t pixels_per_frame() const { return rows_ * cols_; }

    ImageData<DataType> read_frame() {
        ImageData<DataType> img({rows_, cols_});
        read_into(img.buffer());
        return img;
    }
};

template <class T>
struct Normal : public RawFile<sls_detector_header, T, Normal<T>> {
    size_t read_impl(std::byte *buffer) {
        size_t rc = fread(reinterpret_cast<char *>(buffer),
                          sizeof(T) * Normal::rows_ * Normal::cols_, 1,
                          Normal::fp.get());
        return rc;
    }
};

template <class T>
struct FlipRows : public RawFile<sls_detector_header, T, FlipRows<T>> {
    size_t read_impl(std::byte *buffer) {

        // read to temporary buffer
        // TODO! benchmark direct reads
        std::vector<std::byte> tmp(FlipRows::bytes_per_frame());
        size_t rc = fread(reinterpret_cast<char *>(&tmp[0]),
                          sizeof(T) * FlipRows::rows_ * FlipRows::cols_, 1,
                          FlipRows::fp.get());

        // copy to place
        const size_t start =
            FlipRows::cols_ * (FlipRows::rows_ - 1) * sizeof(T);
        const size_t row_size = FlipRows::cols_ * sizeof(T);
        auto dst = buffer + start;
        auto src = &tmp[0];

        for (int i = 0; i != FlipRows::rows_; ++i) {
            memcpy(dst, src, row_size);
            dst -= row_size;
            src += row_size;
        }

        return rc;
    }
};

template <class T>
struct ReorderM03 : public RawFile<sls_detector_header, T, ReorderM03<T>> {
    size_t read_impl(std::byte *buffer) {

        // TODO! Cleanup =)
        std::vector<T> tmp(ReorderM03::pixels_per_frame());
        size_t rc =
            fread(reinterpret_cast<char *>(&tmp[0]),
                  ReorderM03::bytes_per_frame(), 1, ReorderM03::fp.get());

        int adc_nr[32] = {300, 325, 350, 375, 300, 325, 350, 375, 200, 225, 250,
                          275, 200, 225, 250, 275, 100, 125, 150, 175, 100, 125,
                          150, 175, 0,   25,  50,  75,  0,   25,  50,  75};
        int sc_width = 25;
        int nadc = 32;
        int pixels_per_sc = 5000;

        auto dst = reinterpret_cast<T *>(buffer);
        int pixel = 0;
        for (int i = 0; i != pixels_per_sc; ++i) {
            for (int i_adc= 0; i_adc != nadc; ++i_adc) {
                int col = adc_nr[i_adc] + (i % sc_width);
                int row;
                if ((i_adc / 4) % 2 == 0)
                    row = 199 - int(i / sc_width);
                else
                    row = 200 + int(i / sc_width);

                dst[col + row * 400] = tmp[pixel];
                pixel++;
            }
        }
        return rc;
    }
};

template <class T> using EigerTop = RawFile<sls_detector_header, T, Normal<T>>;
template <class T>
using EigerBot = RawFile<sls_detector_header, T, FlipRows<T>>;

using JungfrauRawFile =
    RawFile<sls_detector_header, uint16_t, Normal<uint16_t>>;
using Moench03RawFile =
    RawFile<sls_detector_header, uint16_t, ReorderM03<uint16_t>>;
using EigerRawFile32 = RawFile<sls_detector_header, uint32_t, Normal<uint32_t>>;
using EigerRawFile8 = RawFile<sls_detector_header, uint8_t, Normal<uint8_t>>;

// double check usage
using RawFileVariants =
    std::variant<Moench03RawFile, JungfrauRawFile, EigerTop<uint32_t>,
                 EigerBot<uint32_t>, EigerBot<uint16_t>, EigerTop<uint8_t>,
                 EigerBot<uint8_t>>;

} // namespace pl