#pragma once
#include <filesystem>
//Class to read a master file
//hold names for subfiles etc

namespace pl{

class RawMasterFile{

    std::filesystem::path base_path;
    std::string base_name;

public:
    RawMasterFile(const std::filesystem::path& fname);
    std::filesystem::path path();

    std::filesystem::path raw_file(int mod_id, int file_id);

};

}