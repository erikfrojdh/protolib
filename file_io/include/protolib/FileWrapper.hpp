#pragma once


namespace pl
{
    
class FileWrapper{
  public:
    virtual size_t n_frames() const = 0;
    virtual size_t frame_number(size_t fn) = 0;
    virtual void read_into(std::byte* image_buf) = 0;
    virtual void read_into(std::byte* image_buf, size_t n_frames) = 0;
    virtual void seek(size_t frame_number) = 0;
    virtual size_t tell() = 0;

    virtual ~FileWrapper() = default;
};


} // namespace pl
