#pragma once

#include "protolib/RawMasterFile.hpp"

namespace pl {

class RawFileWrapper : public FileWrapper {
    RawMasterFile m;

  public:
    RawFileWrapper(const std::filesystem::path &fname):m(fname) {
        //initialized by RawMasterFile
    }

    size_t frame_number(size_t fn) override { return m.frame_number(fn); }

    FileInfo file_info() const override{
        return m.file_info();
    }

    void read_into(std::byte *image_buf) override {
        m.read_into(image_buf);
    }

    void read_into(std::byte *image_buf, size_t n_frames) override{
        m.read_into(image_buf, n_frames);
    }

    void seek(size_t frame_number) override {
        m.seek(frame_number);
    }

    size_t tell() override {
        return m.tell();
    }
};


} // namespace pl
