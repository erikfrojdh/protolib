
#include "protolib/File.hpp"

int main(){

    const char* fname = "/home/l_frojdh/software/hdf5maker/tests/data/run_master_16.raw";
    pl::File f(fname);
    
    std::byte* buffer = new std::byte[f.bytes_per_frame()];
    f.read_into(buffer);

    //not to be forgotten
    delete[] buffer;

}