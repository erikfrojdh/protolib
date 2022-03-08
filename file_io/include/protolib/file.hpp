
#include <memory>
#include <string_view>
#include <bitset>
#include <fstream>
#include <filesystem>
namespace pl {

typedef struct {
    uint64_t frameNumber;
    uint32_t expLength;
    uint32_t packetNumber;
    uint64_t bunchId;
    uint64_t timestamp;
    uint16_t modId;
    uint16_t row;
    uint16_t column;
    uint16_t reserved;
    uint32_t debug;
    uint16_t roundRNumber;
    uint8_t detType;
    uint8_t version;
    uint8_t packetMask[64];
} sls_detector_header;



class FileObject {
    std::ifstream ifs;
  public:
    FileObject(std::string_view fname) {
        fmt::print("FileObject({})\n", fname);

    }
    virtual size_t n_frames() const = 0;
    virtual ~FileObject() { fmt::print("~FileObject()\n"); }
};

template <class Header, class DataType, size_t Nrows = 512, size_t Ncols = 1024>
class RawFileObject : public FileObject {
    std::ifstream ifs;
    size_t size_{};
    size_t n_frames_{};

  public:
    RawFileObject(std::string_view fname) : FileObject(fname) {
        size_ = std::filesystem::file_size(fname);
        n_frames_ = size_/(sizeof(Header)+sizeof(DataType)*Nrows*Ncols);
        ifs.open(fname, std::ifstream::binary);
    }
    size_t n_frames() const override { 
       return n_frames_;

    }
    Header read_header(size_t frame_number){
        ifs.seekg((sizeof(Header)+sizeof(DataType)*Nrows*Ncols)*frame_number);
        return read_header();
    }

    Header read_header(){
        Header h{};
        ifs.read(reinterpret_cast<char*>(&h), sizeof(h));
        return h;
    }
    ~RawFileObject() {}
};

using JungfrauRawFile = RawFileObject<sls_detector_header, uint16_t>;

// Top level access, RAII, runtime polymorphism
class File {
    std::unique_ptr<FileObject> fp;

  public:
    File(std::string_view fname) {
        fp = std::make_unique<RawFileObject<sls_detector_header, uint16_t>>(fname);
    }
    size_t n_frames() { return fp->n_frames(); }
};

File open_file(std::string_view fname) { return File(fname); }

} // namespace pl