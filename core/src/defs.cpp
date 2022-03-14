#include "protolib/defs.hpp"
namespace fs = std::filesystem;
namespace pl
{
    

template <> DetectorType StringTo(std::string_view name) {
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

template <>TimingMode StringTo(std::string_view mode){
    if (mode == "auto")
        return TimingMode::Auto;
    else if(mode == "trigger")
        return TimingMode::Trigger;
    else{
        auto msg = fmt::format("Could not decode timing mode from: \"{}\"", mode);
        throw std::runtime_error(msg);
    }
}


std::filesystem::path test_data_path() {
    const char* env_p = std::getenv("PL_TEST_DATA");
    if(!env_p)
        throw std::runtime_error("Test data path not set. Export PL_TEST_DATA to run tests!");
    return fs::path(env_p);
}

} // namespace pl