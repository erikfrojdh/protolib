#include "protolib/File.hpp"
#include "protolib/RawFile.hpp"
#include "protolib/RawFileWrappers.hpp"
#include "protolib/RawMasterFile.hpp"

namespace fs = std::filesystem;

namespace pl {

File::File(const fs::path &fpath) {
    // Find file type at this time only raw files
    if (fpath.extension() == ".raw" && is_master_file(fpath)) {
        fp = std::make_unique<RawFileWrapper>(fpath);
        meta = fp->file_info();
    } else {
        throw std::runtime_error("File type not supported");
    }
}

size_t File::total_frames() const { return meta.total_frames; }

void File::seek(size_t frame_number) { fp->seek(frame_number); }
size_t File::tell() const { return fp->tell(); }

size_t File::frame_number(size_t fn) { return fp->frame_number(fn); }

uint8_t File::bits_per_pixel() const{ return meta.bitdepth; }

Frame File::read_frame() {
    Frame frame(meta.rows, meta.cols, meta.bitdepth);
    fp->read_into(frame.data());
    return frame;
}

uint8_t File::bytes_per_pixel() const { return meta.bitdepth / 8; }

template <typename T> ImageData<T> File::read_as() {
    //Conversion could be both expensive and lossy
    if (sizeof(T) != bytes_per_pixel()) {
        auto msg = fmt::format("size of data type in file ({}) does not match "
                               "size of requested type ({})",
                               bytes_per_pixel(), sizeof(T));
        throw std::runtime_error(msg);
    }
    
    ImageData<T> img(shape());
    read_into(img.buffer());
    return img;
}

//Templates would otherwise be missing from library
template ImageData<uint8_t, 2> File::read_as<uint8_t>();
template ImageData<uint16_t, 2> File::read_as<uint16_t>();
template ImageData<uint32_t, 2> File::read_as<uint32_t>();


File::iterator File::begin() { return iterator(this); }
File::iterator File::end() { return iterator(); }

image_shape File::shape() const { return {meta.rows, meta.cols}; }

void File::read_into(std::byte *image_buf) { fp->read_into(image_buf); }

void File::read_into(std::byte *image_buf, size_t n_frames) {
    fp->read_into(image_buf, n_frames);
}

size_t File::bytes_per_frame() const { return meta.bytes_per_frame(); }

namespace impl {

FileIterator::FileIterator(File *f) : f_ptr(f) { frame = f_ptr->read_frame(); }

FileIterator &FileIterator::operator++() {
    if (f_ptr->tell() < f_ptr->total_frames())
        frame = f_ptr->read_frame();
    else
        f_ptr = nullptr;
    return *this;
}

bool FileIterator::operator==(const FileIterator &other) {
    return f_ptr == other.f_ptr;
}
bool FileIterator::operator!=(const FileIterator &other) {
    return f_ptr != other.f_ptr;
}

FileIterator::pointer FileIterator::operator->() { return &frame; }

FileIterator::reference FileIterator::operator*() { return frame; }

} // namespace impl

std::filesystem::path test_data_path() {
    const char* env_p = std::getenv("PL_TEST_DATA");
    if(!env_p)
        throw std::runtime_error("Test data path not set. Export PL_TEST_DATA to run tests!");
    return fs::path(env_p);
}

} // namespace pl
