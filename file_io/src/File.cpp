#include "protolib/File.hpp"
#include "protolib/RawFile.hpp"
#include "protolib/RawMasterFile.hpp"
#include "protolib/RawFileWrappers.hpp"

namespace pl {

File::File(std::filesystem::path fpath) {
    // Find file type
    if (fpath.extension() == ".raw" && is_master_file(fpath)) {
        
        //Do we need this knowledge here or can we ask the wrapper?
        meta = read_raw_master_file(fpath);
        if (meta.type == DetectorType::Jungfrau) {
            fmt::print("Jungfrau file\n");
            meta.bitdepth = 16;
        }

        fp = std::make_unique<RawFileWrapper>(fpath);
        // fp = std::make_unique<JungfrauFileWrapper>(
        //     fpath.parent_path() / std::filesystem::path("run_d0_f0_16.raw"));
    }

    else {
        throw std::runtime_error("File type not supported");
    }
}

size_t File::total_frames() const { return meta.n_frames; }

void File::seek(size_t frame_number){
    fp->seek(frame_number);
}

size_t File::tell() const{
    return fp->tell();
}

size_t File::frame_number(size_t fn) { return fp->frame_number(fn); }

uint8_t File::bitdepth() { return meta.bitdepth; }

Frame File::read_frame() {
    Frame frame(meta.n_rows, meta.n_cols, meta.bitdepth);
    fp->read_into(frame.data());
    return frame;
}

std::array<size_t, 2> File::shape(){
    return {meta.n_rows, meta.n_cols};
}

void File::read_into(std::byte* image_buf){
    fp->read_into(image_buf);
}

size_t File::bytes_per_frame() const {
    return meta.bytes_per_frame();
}

} // namespace pl
