// SPDX-License-Identifier: LGPL-3.0-or-other
// Copyright (C) 2021 Contributors to the SLS Detector Package
#include <chrono>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>
#include <vector>

#include <fmt/format.h>

#include "np_helper.hpp"

// #include "typecaster.h"
#include "protolib/File.hpp"
#include "protolib/Frame.hpp"
using ds = std::chrono::duration<double>;

namespace py = pybind11;

PYBIND11_MODULE(_protolib, m) {
    m.doc() = R"pbdoc(
        C/C++ API
        -----------------------
        .. warning ::

            This is the compiled c extension. You probably want to look at the
            interface provided by sls instead.

    )pbdoc";

    m.def("test", []() {
        auto image_ptr = new pl::ImageData<double>({15, 30});
        return return_image_data(image_ptr);
    });

    py::class_<pl::File>(m, "File")
        .def(py::init<std::filesystem::path>())
        .def("total_frames", &pl::File::total_frames)
        .def("tell", &pl::File::tell)
        .def("seek", &pl::File::seek)
        .def("frame_number", &pl::File::frame_number)
        .def("read_frame", [](pl::File &self) {
            py::array image;
            if (self.bitdepth() == 8) {
                image = py::array_t<uint8_t>(self.shape());
            } else if (self.bitdepth() == 16) {
                image = py::array_t<uint16_t>(self.shape());
            }else if (self.bitdepth() == 32) {
                image = py::array_t<uint32_t>(self.shape());
            }
            self.read_into(reinterpret_cast<std::byte *>(image.mutable_data()));
            return image;
        });

    py::class_<pl::Frame>(m, "Frame")
        .def(py::init<size_t, size_t, uint8_t>())
        .def("__call__", &pl::Frame::operator());
    // .def("to_double", [](pl::Frame &self){
    //     pl::ImageData<double>* image_ptr;
    //     image_ptr = self.to_double();
    //     return return_image_data(image_ptr);
    // });
    // .def("read_frame", [](pl::File &obj) {
    //     auto image = py::array_t<uint16_t>(obj.shape());
    //     obj.read_into(reinterpret_cast<std::byte *>(image.mutable_data()));
    //     return image;
    // });

    // .def(py::init<const std::string &>())
    // .def(py::init<uint32_t>())
    // .def(py::init<const IpAddr &>())
    // .def("hex", &IpAddr::hex)
    // .def("uint32", &IpAddr::uint32)
    // .def(py::self == py::self)
    // .def("__repr__", &IpAddr::str)
    // .def("str", &IpAddr::str);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
