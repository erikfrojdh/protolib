

#pragma once
#include <variant>
#include <vector>

#include "protolib/RawMasterFile.hpp"
namespace pl {

using Raw = std::variant<JungfrauRawFile>;

// At the moment only single files.
// Try to extend to multiple files later. 
class RawFileWrapper : public FileWrapper {

    RawMasterFile m;
    std::vector<Raw> raw_files;

  public:
    RawFileWrapper(const std::filesystem::path &fname) {
        m.parse_fname(fname);
        FileInfo fi = m.parse_master_file();
        if (fi.type == DetectorType::Jungfrau) {

            raw_files.push_back(JungfrauRawFile(m.data_fname(0, 0)));
        }
    }
    size_t n_frames() const override { return 3; }
    size_t frame_number(size_t fn) override { return 3; }

    void read_into(std::byte *image_buf) override {

        std::visit(
            [image_buf](auto &f) {
                auto h = f.read_into(image_buf);
                fmt::print("Read frame: {}\n", h.frameNumber);
            },
            raw_files[0]);
    }

    void read_into(std::byte *image_buf, size_t n_frames) override{
                std::visit(
            [image_buf, n_frames](auto &f) {
                auto h = f.read_into(image_buf, n_frames);
                // fmt::print("Read frame: {}\n", h.frameNumber);
            },
            raw_files[0]);
    }

    void seek(size_t frame_number) override {
        std::visit([frame_number](auto &f) { f.seek(frame_number); },
                   raw_files[0]);
    }

    size_t tell() override {
        return std::visit([](auto &f) { return f.tell(); }, raw_files[0]);
    }
};

class JungfrauFileWrapper : public FileWrapper {
    std::unique_ptr<JungfrauRawFile> fp;

  public:
    JungfrauFileWrapper(std::filesystem::path fname) {
        fp = std::make_unique<JungfrauRawFile>(fname);
    }
    size_t n_frames() const override { return fp->n_frames(); }
    size_t frame_number(size_t fn) override {
        return fp->read_header(fn).frameNumber;
    }

    void read_into(std::byte *image_buf) override {
        auto h = fp->read_into(image_buf);
        fmt::print("Read frame: {}\n", h.frameNumber);
    }

    void seek(size_t frame_number) override { fp->seek(frame_number); }

    size_t tell() override { return fp->tell(); }
    ~JungfrauFileWrapper(){};
};

} // namespace pl
