#pragma once
#include <string_view>
#include <fmt/format.h>
#include <array>
#include <vector>
#include <filesystem>

namespace pl {

using image_shape = std::array<ssize_t, 2>;
using dynamic_shape = std::vector<ssize_t>;

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

enum class DetectorType { Jungfrau, Eiger, Mythen };

enum class TimingMode {Auto, Trigger};

template<class T> 
T StringTo(std::string_view sv){
    return T(sv);
}

template <> DetectorType StringTo(std::string_view name);

template <>TimingMode StringTo(std::string_view mode);

std::filesystem::path test_data_path();
} // namespace pl
