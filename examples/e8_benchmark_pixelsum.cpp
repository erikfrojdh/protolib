/*
Benchmarking read and sum up pixel values. For example to calculate
the pedestal. TBB parallel_for can help but only if the operation
is expensive. Same think with the offloading read to another thread.
Maybe if both are expensive? 
*/

#include "protolib/protolib.hpp"
#include <filesystem>
#include <fmt/format.h>
#include <future>

#include "protolib/SimpleQueue.hpp"
//#include <tbb/parallel_for.h>

using namespace pl;
namespace fs = std::filesystem;

fs::path fpath = pl::test_data_path() / "jungfrau/run_master_0.raw";

void read_file(File *f, SimpleQueue<Frame> *q) {

    for (auto it = f->begin(); it != f->end(); ++it) {
        auto img = std::move(*it);
        while (!q->push(std::move(img)))
            ;
    }
}

template<typename AccType>
ImageData<AccType> sum_frames(SimpleQueue<Frame> *q, size_t total_frames) {
    ImageData<AccType, 2> total(image_shape{512, 1024}, 0);
    for (size_t i = 0; i != total_frames; ++i) {
        Frame img;
        while (!q->pop(img))
            ;
        auto current = img.view<uint16_t>();
        //tbb::parallel_for(tbb::blocked_range<int>(0, current.size()),
        //                  [&](tbb::blocked_range<int> r) {
        //                      for (int j = r.begin(); j < r.end(); ++j) {
        //                        total(j) += current(j);
        //                      }
        //                  });
    }
    return total;
}

void f(int i, int *j) {}

template <typename T>
void run_sync() {
    File f(fpath);
    SimpleQueue<Frame> q(120);
    read_file(&f, &q);
    auto s = sum_frames<T>(&q, 100);
    fmt::print("s(100,100): {}\n", s(100, 100));
}

template<typename T>
void run_async() {
    File f(fpath);
    SimpleQueue<Frame> q(10);
    auto f1 = std::async(std::launch::async, read_file, &f, &q);
    auto f2 = std::async(std::launch::async, &sum_frames<T>, &q, 100);
    f1.get();
    auto s = f2.get();
    fmt::print("s(100,100): {}\n", s(100, 100));
}

template<typename T>
void run_normal() {
    ImageData<T, 2> total(image_shape{512, 1024}, 0);
    ImageData<uint16_t, 2> current(image_shape{512, 1024}, 0);
    File f(fpath);
    for (size_t i = 0; i != f.total_frames(); ++i) {
        f.read_into(current.buffer());
        //can we use vector instructions here? 
        for (ssize_t j = 0; j != current.size(); ++j)
            total(j) += current(j);
    }
    fmt::print("s(100,100): {}\n", total(100, 100));
}

int main() {
    int n = 10;
    TIMED_FUNC(run_sync<uint64_t>, n);
    TIMED_FUNC(run_async<uint64_t>, n);
    TIMED_FUNC(run_normal<uint64_t>, n);
}
