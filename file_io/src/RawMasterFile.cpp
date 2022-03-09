
#include "protolib/RawMasterFile.hpp"
#include "protolib/FileInfo.hpp"
#include "protolib/defs.hpp"

#include <fstream>
#include <fmt/core.h>

namespace fs = std::filesystem;

namespace pl {

RawMasterFile::RawMasterFile() {}

RawMasterFile::RawMasterFile(const fs::path &fname) {
    parse_fname(fname);
}

void RawMasterFile::parse_fname(const std::filesystem::path &fname) {
    base_path = fname.parent_path();
    fmt::print("base_path: {}\n", base_path.c_str());
    base_name = fname.stem();

    auto pos = base_name.rfind("_");
    fmt::print("sub: {}\n", base_name.substr(pos + 1));
    findex = std::stoi(base_name.substr(pos + 1));
    pos = base_name.find("_master_");
    base_name.erase(pos);
    fmt::print("base_name: {}\n", base_name.c_str());
}

fs::path RawMasterFile::path() { return base_path; }

fs::path RawMasterFile::data_fname(int mod_id, int file_id) {
    return base_path /
           fmt::format("{}_d{}_f{}_{}.raw", base_name, mod_id, file_id, findex);
}

FileInfo RawMasterFile::parse_master_file(){
    return read_raw_master_file(base_path/fmt::format("{}_master_{}.raw", base_name, findex));
}

FileInfo read_raw_master_file(std::filesystem::path fpath) {
    FileInfo info;
    std::ifstream ifs(fpath.c_str());
    for (std::string line; std::getline(ifs, line);) {

        if (line == "#Frame Header")
            break;

        auto pos = line.find(":");

        auto key_pos = pos;

        while (key_pos != std::string::npos && std::isspace(line[--key_pos]))
            ;

        if (key_pos != std::string::npos) {
            auto key = line.substr(0, key_pos + 1);
            auto value = line.substr(pos + 2);
            fmt::print("\"{}\": \"{}\"\n", key, value);

            // do the actual parsing
            if (key == "Total Frames") {
                info.n_frames = std::stoi(value);
            } else if (key == "Detector Type") {
                info.type = StringTo<DetectorType>(value);
            } else if (key == "Pixels"){
                info.n_rows = 512;
                info.n_cols = 1024;
            }
        }
    }

    return info;
}



bool is_master_file(std::filesystem::path fpath) {
    std::string stem = fpath.stem();
    if (stem.find("_master_") != std::string::npos)
        return true;
    else
        return false;
}


} // namespace pl