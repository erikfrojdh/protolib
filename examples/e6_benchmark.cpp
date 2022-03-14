/*
Testing out various ways to sum all pixels in a file. 
Kind of stupid example but gives a feeling for the performance 
and ways to interact with the library
*/

#include "protolib/protolib.hpp"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fmt/core.h>
#include <string>
#include <thread>

#include <cstdio> // for reference implementation

using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

using namespace std::chrono_literals;
namespace fs = std::filesystem;

#define TIME(x) timed(#x, N, &x)

void timed(std::string name, int n_times, void (*func)()) {
    auto t0 = high_resolution_clock::now();
    for (int i = 0; i != n_times; ++i) {
        func();
    }
    auto t1 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t1 - t0;
    fmt::print("{} took {}ms\n", name, ms_double.count());
}

fs::path p = pl::test_data_path() / "jungfrau/1k/run_master_0.raw";
fs::path direct_path = pl::test_data_path() / "jungfrau/1k/run_d0_f0_0.raw";

void test() { std::this_thread::sleep_for(1ms); }

void sum_image() {
    pl::File f(p);
    double total = 0;
    for (auto &img : f) {
        for (int i = 0; i != img.rows(); ++i)
            for (int j = 0; j != img.cols(); ++j)
                total += img(i, j);
    }
    fmt::print("{}\n", total);
}

void sum_view() {
    pl::File f(p);
    double total = 0;
    for (auto &img : f) {
        auto view = img.view<uint16_t>();
        total += std::accumulate(view.begin(), view.end(), 0u);
    }
    fmt::print("{}\n", total);
}

void sum_image_typed() {
    pl::File f(p);
    uint64_t total = 0;
    pl::ImageData<uint16_t> img(f.shape());
    for (size_t i = 0; i != f.total_frames(); ++i) {
        f.read_into(img.buffer());
        total += std::accumulate(img.begin(), img.end(), 0u);
    }
    fmt::print("{}\n", total);
}

void sum_image_direct() {
    pl::JungfrauRawFile f(direct_path, 512, 1024);
    uint64_t total = 0;
    pl::ImageData<uint16_t> img(f.shape());
    for (size_t i = 0; i != f.n_frames(); ++i) {
        f.read_into(img.buffer());
        total += std::accumulate(img.begin(), img.end(), std::uint64_t{});
    }
    fmt::print("{}\n", total);
}

struct make_sum {
    uint64_t operator()(pl::Frame &img) {
        auto view = img.view<uint16_t>();
        return std::accumulate(view.begin(), view.end(), 0ul);
    }
};

// Wold need Frame to be copyable to fully satisfy input_iterator
// might add back later
// void using_algorithms() {
//     pl::File f(p);
//     auto total =
//         std::transform_reduce(f.begin(), f.end(), 0ul, std::plus{}, make_sum{});
//     fmt::print("{}\n", total);
// }

void with_known_type(){
    pl::File f(p);
    uint64_t total=0;
    for (size_t i=0; i!=f.total_frames(); ++i){
        auto img = f.read_as<uint16_t>();
        total += std::accumulate(img.begin(), img.end(), 0u);
    }
    fmt::print("{}\n", total);
}

//reference implementation to compare against
//depending on machine load times dominate
void reference_implementation(){
    constexpr size_t pixels_per_frame = 512*1024;
    constexpr size_t bytes_per_frame = 512*1024*sizeof(uint16_t);
    constexpr size_t header_size = 112;
    uint16_t* data = new uint16_t[pixels_per_frame];
    uint64_t total = 0;

    FILE* fp = fopen(direct_path.c_str(), "r");
    for(int i = 0; i!=1000; ++i){
        fseek(fp, header_size, SEEK_CUR);
        fread(data, bytes_per_frame, 1, fp);
        uint32_t frame_total=0; //can overflow for all max!
        for (size_t j = 0; j!=pixels_per_frame; ++j)
            frame_total += data[j];
        total += frame_total;
    }
    delete[] data;
    fclose(fp);
    fmt::print("{}\n", total);
}


int main() {

    const size_t N = 10;

    fmt::print("Running each test {} times\n", N);
    // timed("test", N, &test);
    // TIME(test);
    // TIME(sum_image);
    TIME(sum_image_direct);
    TIME(sum_image_typed);
    // TIME(sum_view);
    // TIME(using_algorithms);
    TIME(with_known_type);
    TIME(reference_implementation);
}