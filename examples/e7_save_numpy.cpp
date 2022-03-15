#include "protolib/protolib.hpp"


int main(){


    //Create some data
    pl::ImageData<uint16_t, 2> img(pl::Shape<2>{10,15});
    for(uint16_t i=0; i!=img.size(); ++i)
        img(i) = i;

    //write to a numpy file
    pl::NumpyFile::save("test.npy", img);


}