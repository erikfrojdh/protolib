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
#include "protolib/RawFile.hpp"
#include "protolib/Frame.hpp"


#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <sysexits.h>
#include <unistd.h>

using pl::File;
using pl::Frame;

namespace py = pybind11;

namespace fs = std::filesystem;

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

    m.def("call_overhead", [](){return 5;});

    m.def("sum_pixel", [](fs::path p){
        pl::File f(p);
        double total = 0;
        for (auto& frame : f)
            total += frame(100,100);
        return total;
    });

    m.def("clean_read", [](fs::path p){
        int fd;
        fd = open(p.c_str(), O_RDONLY);
        double total = 0;
        constexpr size_t frame_size = 512*1024*2;
        constexpr size_t header_size = 112;
        constexpr size_t pos = (1024*100+100)*2;
        std::byte* buffer = new std::byte[frame_size];
        for(int i = 0; i!=100; ++i){
            lseek (fd, header_size, SEEK_CUR);
            size_t bytes_read = read(fd, reinterpret_cast<char*>(buffer), frame_size);
            total += *reinterpret_cast<uint16_t*>(buffer+pos);
        }
        close(fd);
        delete[] buffer;
        return total;

    });

    m.def("sum_pixel_direct", [](fs::path p){
        pl::JungfrauRawFile f{p, 512,1024};
        double total = 0;
        for (size_t i=0; i!=100; ++i){
            auto img = f.read_frame();
            total += img(100,100);
        }
        return total;
    });

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
        .def("read", [](pl::File &self, ssize_t n_frames = 0){
            py::array image;
            if(n_frames == 0)
                n_frames = self.total_frames();
                
            std::array<ssize_t, 3> shape{n_frames, self.shape()[0], self.shape()[1]};
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
            fmt::print("Ping: {}x{}\n", self.shape()[0], self.shape()[1]);
            fmt::print("Bitdepth: {}\n", self.bitdepth());
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

    py::class_<pl::JungfrauRawFile>(m, "JungfrauRawFile")
        .def(py::init<fs::path, size_t, size_t>())
        .def("read", [](pl::JungfrauRawFile& self, ssize_t n_frames = 0){
            py::array image;
            if(n_frames == 0)
                n_frames = self.n_frames();
            std::array<ssize_t, 3> shape{n_frames, self.rows(), self.cols()};

            py::array_t<uint16_t> data(shape);
            self.read_into(reinterpret_cast<std::byte *>(data.mutable_data()), n_frames);
            return data;
            
        }, py::arg() = 0);
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
