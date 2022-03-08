#pragma once
#include <string_view>
#include <fmt/format.h>
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

enum class DetectorType { Jungfrau, Eiger, Mythen };

template <class T> DetectorType StringTo(std::string_view name) {
    if (name == "Jungfrau")
        return DetectorType::Jungfrau;
    else if (name == "Eiger")
        return DetectorType::Eiger;
    else if (name == "Mythen")
        return DetectorType::Mythen;
    else {
        auto msg = fmt::format("Could not decode dector from: \"{}\"", name);
        throw std::runtime_error(msg);
    }
}

} // namespace pl
