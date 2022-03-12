
#include "protolib/protolib.hpp"

int main(){

    using pl::File;
    namespace fs = std::filesystem;

    //This would be a jungfrau file with 100 frames
    fs::path fpath = pl::test_data_path()/"jungfrau/run_master_2.raw";
    
    //Read one frame into a buffer
    File f(fpath);
    std::byte* buffer = new std::byte[f.bytes_per_frame()];
    f.read_into(buffer);

    //not to be forgotten
    delete[] buffer;

}