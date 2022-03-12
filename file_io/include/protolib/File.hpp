#pragma once
#include <filesystem>
#include <fstream>
#include <memory>
#include <string_view>

#include "protolib/FileInfo.hpp"
#include "protolib/FileWrapper.hpp"
#include "protolib/Frame.hpp"

namespace pl {

class File;
class Frame;

namespace impl {
struct FileIterator {
    using value_type = Frame;
    using iterator_category = std::input_iterator_tag;
    using pointer = Frame *;
    using reference = Frame &;
    File *f_ptr{nullptr};
    Frame frame{};
    FileIterator(File *f);
    FileIterator(const FileIterator &) = delete;
    FileIterator() = default;
    FileIterator(FileIterator &&) = default;
    FileIterator &operator=(FileIterator &&) = default;

    reference operator*();
    pointer operator->();
    FileIterator &operator++();
    bool operator==(const FileIterator &other);
    bool operator!=(const FileIterator &other);
};
} // namespace impl

// Top level access, RAII, runtime polymorphism, as general as possible
// read
// minimize exposed area
// smooth operation over multiple files
// Here we know nothing about the type of file we are going to read from
class File {
    std::unique_ptr<FileWrapper> fp;
    FileInfo meta;

  public:
    File(const std::filesystem::path &fpath);
    File(File &&) = default;
    File(const File &) = delete;            // copy constructor and assignment
    File &operator=(const File &) = delete; // disabled since we hold a file
    File &operator=(File &&) = default;
    ~File() = default;

    size_t total_frames() const;
    void seek(size_t frame_number);
    size_t tell() const;
    size_t frame_number(size_t fn);
    image_shape shape() const;
    uint8_t bits_per_pixel() const;
    uint8_t bytes_per_pixel() const;
    size_t bytes_per_frame() const;

    // Reading
    Frame read_frame();
    void read_into(std::byte *image_buf);
    void read_into(std::byte *image_buf, size_t n_frames);
    template <typename T> ImageData<T> read_as();

    using iterator = impl::FileIterator;
    iterator begin();
    iterator end();
};

std::filesystem::path test_data_path();

} // namespace pl