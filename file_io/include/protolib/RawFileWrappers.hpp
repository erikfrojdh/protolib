

#pragma once

namespace pl {

class JungfrauFileWrapper : public FileWrapper {
    std::unique_ptr<JungfrauRawFile> fp;

  public:
    JungfrauFileWrapper(std::filesystem::path fname) : FileWrapper(fname) {
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

    void seek(size_t frame_number) override{
        fp->seek(frame_number);
    }

    size_t tell() override {
        return fp->tell();
    }
    ~JungfrauFileWrapper(){};
};

} // namespace pl
