#pragma once

#include <iostream>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "protolib/Frame.hpp"
#include "protolib/ImageData.hpp"

namespace py = pybind11;

// Pass image data back to python as a numpy array
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


py::array return_frame(pl::Frame *ptr) {
    py::capsule free_when_done(ptr, [](void *f) {
        pl::Frame *foo = reinterpret_cast<pl::Frame *>(f);
        delete foo;
    });

    const uint8_t item_size = ptr->bytes_per_pixel();
    if (item_size == 1)
        return py::array_t<uint8_t>(
            std::array<ssize_t, 2>{ptr->rows(), ptr->cols()},
            std::array<ssize_t, 2>{
                ptr->cols() * item_size,
                item_size},
            reinterpret_cast<uint8_t *>(ptr->data()),
            free_when_done);
    else if (item_size == 2)
        return py::array_t<uint16_t>(
            std::array<ssize_t, 2>{ptr->rows(), ptr->cols()},
            std::array<ssize_t, 2>{
                ptr->cols() * item_size,
                item_size}, 
            reinterpret_cast<uint16_t *>(ptr->data()),
            free_when_done);
    else if (item_size == 4)
        return py::array_t<uint32_t>(
            std::array<ssize_t, 2>{ptr->rows(), ptr->cols()},
            std::array<ssize_t, 2>{
                ptr->cols() * item_size,
                item_size}, 
            reinterpret_cast<uint32_t *>(ptr->data()),
            free_when_done);
    return {};
}
