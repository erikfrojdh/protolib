#pragma once
#include <filesystem>

#include "protolib/defs.hpp"
#include "protolib/RawFile.hpp"
//Class to read a master file
//hold names for subfiles etc

#include <vector>
#include <variant>

namespace pl{
struct FileInfo;

struct xy{
    int row;
    int col;
};

class RawMasterFile{

    std::filesystem::path base_path;
    std::string base_name;
    int findex{};
    int n_subfiles_{};
    ssize_t total_frames_{};

    std::string version_;
    DetectorType type_;
    TimingMode timing_mode_;
    
    int subfile_cols_{};
    int subfile_rows_{};

    int rows_{};
    int cols_{};
    uint8_t bitdepth_;
    
    void find_number_of_subfiles();
    void parse_master_file();
    void open_subfiles();
    void find_geometry();
    std::vector<xy> positions;

    std::vector<RawFileVariants> subfiles;

public:
    RawMasterFile();
    RawMasterFile(const std::filesystem::path& fname);
    void parse_fname(const std::filesystem::path& fname);
    std::filesystem::path path();
    std::filesystem::path data_fname(int mod_id, int file_id);
    std::filesystem::path master_fname() const;
    
    //Query about the file content
    std::string version() const;
    //timestamp
    DetectorType type() const;
    TimingMode timing_mode() const;

    int subfile_rows() const;
    int subfile_cols() const;
    int cols() const;
    int rows() const;

    size_t bytes_per_frame() const;
    size_t bytes_per_subframe() const;

    int n_subfiles() const;
    ssize_t total_frames() const;
    uint8_t bitdepth() const;

    FileInfo file_info() const;


    //Reading
    void read_into(std::byte* buffer);
    void read_into(std::byte* buffer, size_t n_frames);

    size_t tell();
    void seek(size_t frame_number);

};

template<typename Header=sls_detector_header>
Header read_header(const fs::path fname){
    
    Header h{};
    FILE* fp = fp = fopen(fname.c_str(), "r");
    if(!fp)
        throw std::runtime_error(fmt::format("Could not open: {} for reading", fname.c_str()));
    
    size_t rc = fread(reinterpret_cast<char *>(&h), sizeof(h), 1, fp);
    fclose(fp);
    if(rc != 1)
        throw std::runtime_error("Could not read header from file");
    return h;

}

bool is_master_file(std::filesystem::path fpath);


}