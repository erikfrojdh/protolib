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
namespace impl{
    struct FileIterator{
        using value_type = Frame;
        using iterator_category = std::input_iterator_tag;
        using pointer = Frame*;
        using reference = Frame&;
        File* f_ptr{nullptr};
        Frame frame{};
        FileIterator(File* f);
        FileIterator(const FileIterator&) = delete;
        FileIterator() = default;
        FileIterator(FileIterator&&) = default;
        FileIterator& operator=(FileIterator&&) = default;

        reference operator*();
        pointer operator->();
        FileIterator& operator++();
        bool operator==(const FileIterator& other);
        bool operator!=(const FileIterator& other);

    };
}

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
    File(const File&) = delete;
    File(File&&) = default;
    File& operator=(const File&) = delete;
    File& operator=(File&&) = default;
    ~File() = default;
    size_t total_frames() const;
    void seek(size_t frame_number);
    size_t tell() const;
    size_t frame_number(size_t fn);
    std::array<size_t, 2> shape();
    uint8_t bitdepth();


    //Reading
    Frame read_frame();
    void read_into(std::byte* image_buf);
    void read_into(std::byte* image_buf, size_t n_frames);
    
    
    size_t bytes_per_frame() const;
    using iterator = impl::FileIterator;
    iterator begin(){
        return iterator(this);
    }
    iterator end(){
        return iterator();
    }
};


std::filesystem::path test_data_path();

} // namespace pl