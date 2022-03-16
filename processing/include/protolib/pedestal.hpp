#pragma once

#include <filesystem>
#include "protolib/ImageData.hpp"


namespace pl{

ImageData<double, 2> mean_from_file(const std::filesystem::path &fpath);
ImageData<double, 3> pd_from_file(const std::filesystem::path& fpath);

}