#pragma once

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>

#include "protolib/ImageData.hpp"

namespace py = pybind11;

// accept a numpy array and pass a view to a c++ function
template <typename T, ssize_t Ndim>
py::array return_image_data(pl::ImageData<T, Ndim> *image) {

    py::capsule free_when_done(image, [](void *f) {
        pl::ImageData<T, Ndim> *foo =
            reinterpret_cast<pl::ImageData<T, Ndim> *>(f);
        delete foo;
    });

    return py::array_t<T>(
        image->shape(),        // shape
        image->byte_strides(), // C-style contiguous strides for double
        image->data(),         // the data pointer
        free_when_done);       // numpy array references this parent
}
