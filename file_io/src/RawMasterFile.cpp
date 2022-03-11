
#include "protolib/RawMasterFile.hpp"
#include "protolib/FileInfo.hpp"
#include "protolib/RawFile.hpp"
#include "protolib/defs.hpp"

#include <cstring>
#include <fmt/core.h>
#include <fstream>
namespace fs = std::filesystem;

namespace pl {

RawMasterFile::RawMasterFile() {}

RawMasterFile::RawMasterFile(const fs::path &fname) {
    parse_fname(fname);
    parse_master_file();
    find_number_of_subfiles();

    find_geometry();
    open_subfiles();
}

void RawMasterFile::parse_fname(const std::filesystem::path &fname) {
    base_path = fname.parent_path();
    base_name = fname.stem();

    auto pos = base_name.rfind("_");
    findex = std::stoi(base_name.substr(pos + 1));
    pos = base_name.find("_master_");
    base_name.erase(pos);
}

// TODO! Deal with fast quad and missing files
void RawMasterFile::find_number_of_subfiles() {
    int n_mod = 0;
    while (fs::exists(data_fname(n_mod, 0))) {
        n_mod++;
    }
    n_subfiles_ = n_mod;
}

int RawMasterFile::n_subfiles() const { return n_subfiles_; }

fs::path RawMasterFile::path() { return base_path; }

fs::path RawMasterFile::data_fname(int mod_id, int file_id) {
    return base_path /
           fmt::format("{}_d{}_f{}_{}.raw", base_name, mod_id, file_id, findex);
}

fs::path RawMasterFile::master_fname() const {
    return base_path / fmt::format("{}_master_{}.raw", base_name, findex);
}

int RawMasterFile::rows() const { return rows_; }

int RawMasterFile::cols() const { return cols_; }

void RawMasterFile::find_geometry() {
    uint16_t r{};
    uint16_t c{};
    for (int i = 0; i != n_subfiles_; ++i) {
        auto h = read_header(data_fname(i, 0));
        r = std::max(r, h.row);
        c = std::max(c, h.column);

        positions.push_back({h.row, h.column});
    }
    r++;
    c++;

    rows_ = r * subfile_rows_;
    cols_ = c * subfile_cols_;
}

size_t RawMasterFile::bytes_per_subframe() const {
    return subfile_cols_ * subfile_rows_ * bitdepth_ / 8;
}

size_t RawMasterFile::bytes_per_frame() const {
    return cols_ * rows_ * bitdepth_ / 8;
}

void RawMasterFile::read_into(std::byte *buffer) {
    // we got called with a contigious buffer
    // caller 100% has to make sure there is space!
    // now we need to index into it in the right way
    // copy data to the right location etc.

    // optimize for the case where we can directly
    // write into the buffer (eg single column)

    if (cols_ == subfile_cols_) {
        for (auto &sf : subfiles) {
            std::visit(
                [&buffer](auto &f) {
                    f.read_into(buffer);
                    buffer += f.bytes_per_frame();
                },
                sf);
        }
    } else {
        for (auto &sf : subfiles) {
            std::vector<std::byte> part(bytes_per_subframe());
            const size_t line_length = cols_ * bitdepth_ / 8;
            const size_t subline_length = subfile_cols() * bitdepth_ / 8;
            std::visit(
                [&](auto &f) {
                    // read
                    auto h = f.read_into(part.data());

                    // copy to place
                    std::byte *src = &part[0];
                    std::byte *dst = buffer + h.column * subline_length +
                                     h.row * line_length * subfile_rows();
                    for (ssize_t row = 0; row != subfile_rows(); ++row) {
                        memcpy(dst, src, subline_length);
                        dst += line_length;
                        src += subline_length;
                    }
                    // f.hello();
                },
                sf);
        }
    }
}

void RawMasterFile::read_into(std::byte *buffer, size_t n_frames) {
    for (size_t i = 0; i != n_frames; ++i) {
        read_into(buffer);
        buffer += bytes_per_frame();
    }
}

void RawMasterFile::seek(size_t frame_number) {
    for (auto &sf : subfiles) {
        std::visit([frame_number](auto &f) { f.seek(frame_number); }, sf);
    }
}

size_t RawMasterFile::tell() {
    std::vector<size_t> frame_numbers(subfiles.size());

    for (size_t i = 0; i != subfiles.size(); ++i) {
        std::visit(
            [&frame_numbers, &i](auto &f) { frame_numbers[i] = f.tell(); },
            subfiles[i]);
    }
    return frame_numbers[0];
}

void RawMasterFile::parse_master_file() {
    std::ifstream ifs(master_fname());
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
            if (key == "Version") {
                version_ = value;
            } else if (key == "TimeStamp") {

            } else if (key == "Detector Type") {
                type_ = StringTo<DetectorType>(value);
            } else if (key == "Timing Mode") {
                timing_mode_ = StringTo<TimingMode>(value);
            } else if (key == "Pixels") {
                // Total number of pixels cannot be found yet looking at
                // submodule
                auto pos = value.find(',');
                subfile_cols_ = std::stoi(value.substr(1, pos));
                subfile_rows_ = std::stoi(value.substr(pos + 1));
            } else if (key == "Total Frames") {
                total_frames_ = std::stoi(value);
            } else if (key == "Dynamic Range") {
                bitdepth_ = std::stoi(value);
            }
        }
    }

    // JF doesn't write bitdepth to file
    if (type_ == DetectorType::Jungfrau)
        bitdepth_ = 16;
}

FileInfo RawMasterFile::file_info() const {
    FileInfo info;
    info.total_frames = total_frames_;
    info.rows = rows_;
    info.cols = cols_;
    info.bitdepth = bitdepth_;
    info.type = type_;
    return info;
}

void RawMasterFile::open_subfiles() {
    for (int i = 0; i != n_subfiles_; ++i) {
        // fmt::print("{}: {}\n", i, data_fname(i, 0).c_str());
        if (type_ == DetectorType::Eiger && bitdepth_ == 32) {
            if (positions[i].row % 2)
                subfiles.push_back(EigerBot<uint32_t>(
                    data_fname(i, 0), subfile_rows_, subfile_cols_));
            else
                subfiles.push_back(EigerTop<uint32_t>(
                    data_fname(i, 0), subfile_rows_, subfile_cols_));

        }

        else if (type_ == DetectorType::Jungfrau)
            subfiles.emplace_back(JungfrauRawFile(data_fname(i, 0), subfile_rows_,
                                               subfile_cols_));
        else
            throw std::runtime_error("File not supported");
    }
}

std::string RawMasterFile::version() const { return version_; }
DetectorType RawMasterFile::type() const { return type_; }
TimingMode RawMasterFile::timing_mode() const { return timing_mode_; }
ssize_t RawMasterFile::total_frames() const { return total_frames_; }
int RawMasterFile::subfile_cols() const { return subfile_cols_; }
int RawMasterFile::subfile_rows() const { return subfile_rows_; }
uint8_t RawMasterFile::bitdepth() const { return bitdepth_; }

bool is_master_file(std::filesystem::path fpath) {
    std::string stem = fpath.stem();
    if (stem.find("_master_") != std::string::npos)
        return true;
    else
        return false;
}

} // namespace pl