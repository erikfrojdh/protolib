// SPDX-License-Identifier: LGPL-3.0-or-other
// Copyright (C) 2021 Contributors to the SLS Detector Package
#include <chrono>

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
using pl::File;
using pl::Frame;

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

    m.def("test_data_path", &pl::test_data_path);


    py::class_<pl::File::iterator>(m, "FileIterator")
     .def(py::init<>())
     .def("__next__", [](File::iterator &self){
         if (self==File::iterator{})
            throw py::stop_iteration();
         Frame* ptr = new Frame;
         *ptr = std::move(*self);
         ++self;
         return return_frame(ptr);
     });

    py::class_<pl::File>(m, "File")
        .def(py::init<std::filesystem::path>())
        .def("total_frames", &pl::File::total_frames)
        .def("tell", &pl::File::tell)
        .def("seek", &pl::File::seek)
        .def("__enter__", [](pl::File &self){return &self;} )
        .def("__exit__", [](pl::File &self, const py::object &, const py::object &, const py::object &){
            //close the file? 
        } )
        .def("__iter__", [](pl::File &self){return self.begin();})
        .def("test_read", [](pl::File &self){
            pl::Frame* ptr = new pl::Frame;
            *ptr = self.read_frame();
            return return_frame(ptr);
        })
        .def("frame_number", &pl::File::frame_number)
        .def("read", [](pl::File &self, size_t n_frames = 0){
            py::array image;
            if(n_frames == 0)
                n_frames = self.total_frames();
                
            std::array<size_t, 3> shape{n_frames, self.shape()[0], self.shape()[1]};
            if (self.bitdepth() == 8) {
                image = py::array_t<uint8_t>(shape);
            } else if (self.bitdepth() == 16) {
                image = py::array_t<uint16_t>(shape);
            }else if (self.bitdepth() == 32) {
                image = py::array_t<uint32_t>(shape);
            }
            self.read_into(reinterpret_cast<std::byte *>(image.mutable_data()), n_frames);
            return image;
        }, py::arg() = 0)
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


#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
