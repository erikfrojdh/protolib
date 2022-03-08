#pragma once
#include "protolib/FileWrapper.hpp"

namespace pl
{
    
class FileWrapper{
    // std::ifstream ifs;

  public:
    FileWrapper(std::filesystem::path fname) {
        fmt::print("FileWrapper({})\n", fname.c_str());
    }
    virtual size_t n_frames() const = 0;
    virtual size_t frame_number(size_t fn) = 0;
    virtual ~FileWrapper() { fmt::print("~FileWrapper()\n"); }
    virtual void read_into(std::byte* image_buf) = 0;

    virtual void seek(size_t frame_number) = 0;
    virtual size_t tell() = 0;
};


} // namespace pl
