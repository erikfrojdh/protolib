
#include "protolib/RawMasterFile.hpp"
#include "protolib/FileInfo.hpp"
#include "protolib/RawFile.hpp"
#include "protolib/defs.hpp"

#include <cstring>
#include <fmt/core.h>
#include <fstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace fs = std::filesystem;

// TODO! Push more knowledge into the subfiles
//  configure gappixels, module gaps etc.

namespace pl {

RawMasterFile::RawMasterFile() {}

RawMasterFile::RawMasterFile(const fs::path &fname)
    : RawMasterFile(fname, RawMasterFile::config{}) {}

RawMasterFile::RawMasterFile(const fs::path &fname, RawMasterFile::config cfg)
    : cfg_(cfg) {
    if (!fs::exists(fname)) {
        auto msg = fmt::format("File: {} not found", fname.c_str());
        throw std::runtime_error(msg);
    }

    parse_fname(fname);
    parse_master_file();
    find_number_of_subfiles();

    if (cfg_ == RawMasterFile::config{}) {
        // do the default stuff
    }
    find_geometry();
    open_subfiles();
}

void RawMasterFile::parse_fname(const fs::path &fname) {
    base_path = fname.parent_path();
    base_name = fname.stem();
    ext = fname.extension();

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
    return base_path / fmt::format("{}_master_{}{}", base_name, findex, ext);
}

ssize_t RawMasterFile::rows() const { return rows_; }
ssize_t RawMasterFile::cols() const { return cols_; }

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

    rows_ += (r - 1) * cfg_.module_gap_row;
}

size_t RawMasterFile::bytes_per_subframe() const {
    return subfile_cols_ * subfile_rows_ * bitdepth_ / 8;
}

size_t RawMasterFile::bytes_per_frame() const {
    return cols_ * rows_ * bitdepth_ / 8;
}

int RawMasterFile::file_index() const { return findex; }
void RawMasterFile::set_file_index(int i) { findex = i; }

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

size_t RawMasterFile::frame_number(size_t fn) {
    auto r = std::visit(
        [fn](auto &f) {
            auto h = f.read_header(fn);
            return h.frameNumber;
        },
        subfiles[0]);
    seek(fn);
    return r;
}
void RawMasterFile::seek(size_t frame_number) {
    if (frame_number >= total_frames_)
        throw std::runtime_error("Requested frame number is larger than "
                                 "number of frames in file");
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

    // Parse old style master file
    if (ext == ".raw") {
        for (std::string line; std::getline(ifs, line);) {

            if (line == "#Frame Header")
                break;

            auto pos = line.find(":");
            auto key_pos = pos;

            while (key_pos != std::string::npos &&
                   std::isspace(line[--key_pos]))
                ;

            if (key_pos != std::string::npos) {
                auto key = line.substr(0, key_pos + 1);
                auto value = line.substr(pos + 2);

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
                } else if (key == "Quad") {
                    quad_ = (value == "1");
                }
            }
        }
    } else if (ext == ".json") {
        json j;
        ifs >> j;
        double v = j["Version"];
        version_ = fmt::format("{:.1f}", v); // TODO parse Major ver minor ver?
        type_ = StringTo<DetectorType>(j["Detector Type"].get<std::string>());
        timing_mode_ =
            StringTo<TimingMode>(j["Timing Mode"].get<std::string>());
        total_frames_ = j["Frames in File"];
        subfile_cols_ = j["Pixels"]["x"];
        subfile_rows_ = j["Pixels"]["y"];
        if (type_ == DetectorType::Moench)
            bitdepth_ = 16;
        else
            bitdepth_ = j["Dynamic Range"];

        // only Eiger had quad
        if (type_ == DetectorType::Eiger) {
            quad_ = (j["Quad"] == 1);
        }
    } else {
        throw std::runtime_error("Not a master file");
    }

    // JF doesn't write bitdepth to file
    if (type_ == DetectorType::Jungfrau)
        bitdepth_ = 16;

    assert(bitdepth_ != 0);
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
        if (type_ == DetectorType::Eiger) {
            bool flip = positions[i].row % 2;
            if (quad_)
                flip = !flip;

            if (bitdepth_ == 32) {
                if (flip)
                    subfiles.push_back(EigerBot<uint32_t>(
                        data_fname(i, 0), subfile_rows_, subfile_cols_));
                else
                    subfiles.push_back(EigerTop<uint32_t>(
                        data_fname(i, 0), subfile_rows_, subfile_cols_));

            } else if (bitdepth_ == 16) {
                if (flip)
                    subfiles.push_back(EigerBot<uint16_t>(
                        data_fname(i, 0), subfile_rows_, subfile_cols_));
                else
                    subfiles.push_back(EigerTop<uint16_t>(
                        data_fname(i, 0), subfile_rows_, subfile_cols_));

            } else if (bitdepth_ == 8) {
                if (flip)
                    subfiles.push_back(EigerBot<uint8_t>(
                        data_fname(i, 0), subfile_rows_, subfile_cols_));
                else
                    subfiles.push_back(EigerTop<uint8_t>(
                        data_fname(i, 0), subfile_rows_, subfile_cols_));
            } else {
                throw std::runtime_error("Unrecognized bit depth");
            }

        } else if (type_ == DetectorType::Jungfrau)
            subfiles.emplace_back(JungfrauRawFile(
                data_fname(i, 0), subfile_rows_, subfile_cols_));
        else if (type_ == DetectorType::Mythen3)
            subfiles.push_back(EigerTop<uint32_t>(
                data_fname(i, 0), subfile_rows_, subfile_cols_));
        else if (type_ == DetectorType::Moench)
            subfiles.emplace_back(Moench03RawFile(
                data_fname(i, 0), subfile_rows_, subfile_cols_));
        else
            throw std::runtime_error("File not supported");
    }
}

std::string RawMasterFile::version() const { return version_; }
DetectorType RawMasterFile::type() const { return type_; }
TimingMode RawMasterFile::timing_mode() const { return timing_mode_; }
size_t RawMasterFile::total_frames() const { return total_frames_; }
int RawMasterFile::subfile_cols() const { return subfile_cols_; }
int RawMasterFile::subfile_rows() const { return subfile_rows_; }
uint8_t RawMasterFile::bitdepth() const { return bitdepth_; }
uint8_t RawMasterFile::bytes_per_pixel() const { return bitdepth_ / 8; }

bool is_master_file(std::filesystem::path fpath) {
    std::string stem = fpath.stem();
    if (stem.find("_master_") != std::string::npos)
        return true;
    else
        return false;
}

} // namespace pl