#pragma once
#include <filesystem>
//Class to read a master file
//hold names for subfiles etc

namespace pl{
struct FileInfo;

class RawMasterFile{

    std::filesystem::path base_path;
    std::string base_name;
    int findex{};

public:
    RawMasterFile();
    RawMasterFile(const std::filesystem::path& fname);
    void parse_fname(const std::filesystem::path& fname);
    FileInfo parse_master_file();
    std::filesystem::path path();
    std::filesystem::path data_fname(int mod_id, int file_id);


};


bool is_master_file(std::filesystem::path fpath);
FileInfo read_raw_master_file(std::filesystem::path fpath);

}