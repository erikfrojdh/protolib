#pragma once
#include <bitset>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string_view>

#include "protolib/FileInfo.hpp"
#include "protolib/FileWrapper.hpp"
#include "protolib/Frame.hpp"

namespace pl {

// Top level access, RAII, runtime polymorphism, as general as possible
// Add seek? 
// read 
// minimize exposed area 
// smooth operation over multiple files
// Here we know nothing about the type of file we are going to read from

class File {
    std::unique_ptr<FileWrapper> fp;
    FileInfo meta;

  public:
    File(std::filesystem::path fpath);
    size_t total_frames() const;
    void seek(size_t frame_number);
    size_t tell() const;
    size_t frame_number(size_t fn);
    std::array<size_t, 2> shape();
    uint8_t bitdepth();
    Frame read_frame();
    void read_into(std::byte* image_buf);
    size_t bytes_per_frame() const;
};

} // namespace pl