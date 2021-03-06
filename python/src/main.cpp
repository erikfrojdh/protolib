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
#include "protolib/ClusterFinder.hpp"
#include "protolib/pedestal.hpp"
#include "protolib/protolib.hpp"

#include <fcntl.h> // for the clean_read example

using namespace pl;

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

    py::class_<pl::File::iterator>(m, "FileIterator")
        .def(py::init<>())
        .def("__next__", [](File::iterator &self) {
            if (self == File::iterator{})
                throw py::stop_iteration();
            Frame *ptr = new Frame;
            *ptr = std::move(*self);
            ++self;
            return return_frame(ptr);
        });

    // In the module1.cpp binding code for module1:
    // py::class_<Pet>(m, "Pet")
    //     .def(py::init<std::string>())
    //     .def_readonly("name", &Pet::name);

    py::class_<File>(m, "File")
        .def(py::init<std::filesystem::path>())
        .def_property_readonly("total_frames", &File::total_frames)
        .def_property_readonly("tell", &pl::File::tell)
        .def("seek", &pl::File::seek)
        .def_property_readonly("shape", &pl::File::shape)
        .def_property_readonly("bits_per_pixel", &pl::File::bits_per_pixel)
        .def_property_readonly("bytes_per_frame", &pl::File::bytes_per_frame)
        .def("__enter__", [](pl::File &self) { return &self; })
        .def("__exit__",
             [](pl::File &self, const py::object &, const py::object &,
                const py::object &) {
                 // close the file is done on destruction
             })
        .def("__iter__", [](pl::File &self) { return self.begin(); })
        .def("test_read",
             [](pl::File &self) {
                 pl::Frame *ptr = new pl::Frame;
                 *ptr = self.read_frame();
                 return return_frame(ptr);
             })
        .def("frame_number", &pl::File::frame_number)
        .def(
            "read",
            [](pl::File &self, ssize_t n_frames = 0) {
                py::array image;
                if (n_frames == 0)
                    n_frames = self.total_frames() - self.tell();

                // collapse shape if a shape is 1
                std::vector<ssize_t> shape;
                shape.reserve(3);
                shape.push_back(n_frames);
                for (auto val : self.shape())
                    if (val > 1)
                        shape.push_back(val);

                const uint8_t item_size = self.bytes_per_pixel();
                if (item_size == 1) {
                    image = py::array_t<uint8_t>(shape);
                } else if (item_size == 2) {
                    image = py::array_t<uint16_t>(shape);
                } else if (item_size == 4) {
                    image = py::array_t<uint32_t>(shape);
                }
                self.read_into(
                    reinterpret_cast<std::byte *>(image.mutable_data()),
                    n_frames);
                return image;
            },
            py::arg() = 0)
        .def("read_frame", [](pl::File &self) {
            const uint8_t item_size = self.bytes_per_pixel();
            py::array image;
            std::vector<ssize_t> shape;
            shape.reserve(2);
            for (auto val : self.shape())
                if (val > 1)
                    shape.push_back(val);
            if (item_size == 1) {
                image = py::array_t<uint8_t>(shape);
            } else if (item_size == 2) {
                image = py::array_t<uint16_t>(shape);
            } else if (item_size == 4) {
                image = py::array_t<uint32_t>(shape);
            }
            self.read_into(reinterpret_cast<std::byte *>(image.mutable_data()));
            return image;
        });

    py::class_<pl::Frame>(m, "Frame")
        .def(py::init<size_t, size_t, uint8_t>())
        .def("__call__", &pl::Frame::operator());

    py::class_<pl::RawMasterFile>(m, "RawMasterFile")
        .def(py::init<fs::path>())
        .def(py::init<fs::path, pl::RawFileConfig>())
        .def("read", &do_read<pl::RawMasterFile>);

    py::class_<pl::RawFileConfig>(m, "RawFileConfig")
        .def(py::init<>())
        .def_readwrite("module_gap_row", &pl::RawFileConfig::module_gap_row)
        .def_readwrite("module_gap_col", &pl::RawFileConfig::module_gap_col);

    py::class_<pl::JungfrauRawFile>(m, "JungfrauRawFile")
        .def(py::init<fs::path, size_t, size_t>())
        .def(
            "read",
            [](pl::JungfrauRawFile &self, ssize_t n_frames = 0) {
                py::array image;
                if (n_frames == 0)
                    n_frames = self.n_frames();
                std::array<ssize_t, 3> shape{n_frames, self.rows(),
                                             self.cols()};

                py::array_t<uint16_t> data(shape);
                self.read_into(
                    reinterpret_cast<std::byte *>(data.mutable_data()),
                    n_frames);
                return data;
            },
            py::arg() = 0);

    /////////////////////////////////////////////////
    // Processing functions
    /////////////////////////////////////////////////

    m.def("pd_from_file", [](const fs::path &fpath) {
        auto *ptr = new pl::ImageData<double, 3>(pl::pd_from_file(fpath));
        return return_image_data(ptr);
    });

    m.def(
        "apply_calibration",
        [](py::array_t<uint16_t, py::array::c_style | py::array::forcecast>
               raw_data,
           py::array_t<double, py::array::c_style | py::array::forcecast> pd,
           py::array_t<double, py::array::c_style | py::array::forcecast> cal) {
            auto raw_span = make_span_2d(raw_data);
            auto pd_span = make_span_3d(pd);
            auto cal_span = make_span_3d(cal);
            auto frame = new pl::ImageData<double, 2>(
                pl::apply_calibration(raw_span, pd_span, cal_span));
            return return_image_data(frame);
        },
        py::arg().noconvert(), py::arg().noconvert(), py::arg().noconvert());

    m.def(
        "sum_raw_file",
        [](const fs::path &fname,
           py::array_t<double, py::array::c_style | py::array::forcecast> pd,
           py::array_t<double, py::array::c_style | py::array::forcecast> cal, double threshold) {
            auto pd_span = make_span_3d(pd);
            auto cal_span = make_span_3d(cal);
            auto frame = new pl::ImageData<double, 2>(
                pl::sum_raw_file(fname, pd_span, cal_span, threshold));
            return return_image_data(frame);
        },
        py::arg().noconvert(), py::arg().noconvert(), py::arg().noconvert(),py::arg().noconvert());

    m.def(
        "sum_raw_file_parallel",
        [](const fs::path &fname,
           py::array_t<double, py::array::c_style | py::array::forcecast> pd,
           py::array_t<double, py::array::c_style | py::array::forcecast> cal, double threshold) {
            auto pd_span = make_span_3d(pd);
            auto cal_span = make_span_3d(cal);
            auto frame = new pl::ImageData<double, 2>(
                pl::sum_raw_file_parallel(fname, pd_span, cal_span, threshold));
            return return_image_data(frame);
        },
        py::arg().noconvert(), py::arg().noconvert(), py::arg().noconvert(),py::arg().noconvert());


    PYBIND11_NUMPY_DTYPE(pl::ClusterFinder<double>::Hit, size, row, col,
                         reserved, energy, max);

    py::class_<pl::ClusterFinder<double>>(m, "ClusterFinder")
        .def(py::init<pl::image_shape, double>())
        .def("labeled",
             [](pl::ClusterFinder<double> &self) {
                 auto ptr = new pl::ImageData<int, 2>(self.labeled());
                 return return_image_data(ptr);
             })
        .def("find_clusters",
             [](pl::ClusterFinder<double> &self,
                py::array_t<double, py::array::c_style | py::array::forcecast>
                    img) {
                 auto img_span = make_span_2d(img);
                 self.find_clusters(img_span);
             })
        .def("single_pass",
             [](pl::ClusterFinder<double> &self,
                py::array_t<double, py::array::c_style | py::array::forcecast>
                    img) {
                 auto img_span = make_span_2d(img);
                 self.single_pass(img_span);
             })
        .def("hits",
             [](pl::ClusterFinder<double> &self) {
                 auto ptr = new std::vector<pl::ClusterFinder<double>::Hit>(
                     self.steal_hits());
                 return return_vector(ptr);
             })
        .def("total_clusters", &pl::ClusterFinder<double>::total_clusters)
        .def("print_connections",
             &pl::ClusterFinder<double>::print_connections);

    /////////////////////////////////////////////////
    // Testing, playing around and random bits of code
    /////////////////////////////////////////////////
    m.def("test", []() {
        auto image_ptr = new pl::ImageData<double>({15, 30});
        return return_image_data(image_ptr);
    });

    m.def("test_data_path", &pl::test_data_path);

    m.def("call_overhead", []() { return 5; });

    m.def("sum_pixel", [](fs::path p) {
        pl::File f(p);
        double total = 0;
        for (auto &frame : f)
            total += frame(100, 100);
        return total;
    });

    m.def("clean_read", [](fs::path p) {
        int fd;
        fd = open(p.c_str(), O_RDONLY);
        double total = 0;
        constexpr size_t frame_size = 512 * 1024 * 2;
        constexpr size_t header_size = 112;
        constexpr size_t pos = (1024 * 100 + 100) * 2;
        std::byte *buffer = new std::byte[frame_size];
        for (int i = 0; i != 100; ++i) {
            lseek(fd, header_size, SEEK_CUR);
            read(fd, reinterpret_cast<char *>(buffer), frame_size);
            total += *reinterpret_cast<uint16_t *>(buffer + pos);
        }
        close(fd);
        delete[] buffer;
        return total;
    });

    m.def("sum_pixel_direct", [](fs::path p) {
        pl::JungfrauRawFile f{p, 512, 1024};
        double total = 0;
        for (size_t i = 0; i != 100; ++i) {
            auto img = f.read_frame();
            total += img(100, 100);
        }
        return total;
    });
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
