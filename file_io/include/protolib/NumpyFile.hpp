#pragma once


#include <filesystem>

namespace pl{
class NumpyFile{

    public:
        NumpyFile();
        NumpyFile(const std::filesystem::path& fpath );
};

}

