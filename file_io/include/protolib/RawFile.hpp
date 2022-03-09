#pragma once
#include "protolib/FileInfo.hpp"
#include "protolib/ImageData.hpp"

#include <filesystem>
#include <fstream>
#include <cassert>
namespace pl {





// concrete class meant to hold a raw file
//  *read header from the file
//  *read data from the file
//  *extend to read multiple files? f0, f1 etc?
template <class Header, class DataType, size_t Nrows = 512, size_t Ncols = 1024>
class RawFile {
    std::ifstream ifs;
    size_t size_{};
    size_t n_frames_{};

  public:
    RawFile(std::filesystem::path fname) {
        size_ = std::filesystem::file_size(fname);
        n_frames_ = size_ / (sizeof(Header) + sizeof(DataType) * Nrows * Ncols);
        ifs.open(fname, std::ifstream::binary);
    }

    RawFile(const RawFile&) = default;
    RawFile& operator=(const RawFile&) = default;
    RawFile(RawFile&&) = default;
    RawFile& operator=(RawFile&&) = default;
    ~RawFile() = default;


    size_t n_frames() const { return n_frames_; }


    void seek(size_t frame_number){
        if (frame_number >= n_frames_)
            throw std::runtime_error("Requested frame number is larger than number of frames in file");

        ifs.seekg((sizeof(Header) + sizeof(DataType) * Nrows * Ncols) *
                  frame_number);
    }

    Header read_header(size_t frame_number) {
        seek(frame_number);
        auto h = read_header();
        seek(frame_number);
        return h;
    }

    size_t tell(){
        auto pos = ifs.tellg();
        assert(pos % (sizeof(Header) + sizeof(DataType) * Nrows * Ncols) == 0);
        return pos/(sizeof(Header) + sizeof(DataType) * Nrows * Ncols);
    }

    Header read_header() {
        Header h{};
        ifs.read(reinterpret_cast<char *>(&h), sizeof(h));
        return h;
    }

    size_t frame_number(size_t fn) { return read_header(fn).frameNumber; }

    Header read_into(std::byte* buffer){
        auto h = read_header();
        ifs.read(reinterpret_cast<char *>(buffer), sizeof(DataType) * Nrows * Ncols);
        return h;
    }

    ImageData<DataType> read_frame(){
        ImageData<DataType> img({Nrows, Ncols});
        read_into(img.buffer());
        return img;
    }


};

using JungfrauRawFile = RawFile<sls_detector_header, uint16_t>;

} // namespace pl