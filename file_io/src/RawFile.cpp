#include "protolib/RawFile.hpp"

#include <filesystem>
#include <fstream>
#include <cstdint>
#include <fmt/format.h>

namespace pl {

bool is_master_file(std::filesystem::path fpath) {
    std::string stem = fpath.stem();
    if (stem.find("_master_") != std::string::npos)
        return true;
    else
        return false;
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
            // fmt::print("\"{}\": \"{}\"\n", key, value);

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
} // namespace pl
