#pragma once

#include <algorithm>
#include <map>
#include <unordered_map>
#include <vector>

#include "protolib/defs.hpp"
#include "protolib/ImageData.hpp"

namespace pl {

template <typename T> class ClusterFinder {
  public:
    struct Hit {
        int16_t size{};
        int16_t row{};
        int16_t col{};
        uint16_t reserved{}; // for alignment
        T energy{};
        T max{};
    };

  private:
    const std::array<ssize_t, 2> shape_;
    DataSpan<T, 2> original_;
    ImageData<int, 2> labeled_;
    ImageData<bool, 2> binary_;
    T threshold_;
    int current_label;
    const std::array<int, 4> di{{0, -1, -1, -1}};
    const std::array<int, 4> dj{{-1, -1, 0, 1}};
    std::map<int, int> child;
    std::vector<Hit> hits;
    int check_neighbours(int i, int j);

  public:
    ClusterFinder(image_shape shape, T threshold)
        : shape_(shape), labeled_(shape, 0), binary_(shape),
          threshold_(threshold) {
        hits.reserve(2000);
    }

    ImageData<int, 2> labeled() { return labeled_; }

    void find_clusters(DataSpan<T, 2> img);
    void single_pass(DataSpan<T, 2> img);
    void first_pass();
    void second_pass();
    void store_clusters();

    std::vector<Hit> steal_hits() {
        std::vector<Hit> tmp;
        std::swap(tmp, hits);
        return tmp;
    };
    void clear_hits() { hits.clear(); };

    void print_connections() {
        fmt::print("Connections:\n");
        for (auto it = child.begin(); it != child.end(); ++it) {
            fmt::print("{} -> {}\n", it->first, it->second);
        }
    }
    size_t total_clusters() const{
        //TODO! fix for stealing
        return hits.size();
    }

  private:
    void add_link(int from, int to) {
        // we want to add key from -> value to
        // fmt::print("add_link({},{})\n", from, to);
        auto it = child.find(from);
        if (it == child.end()) {
            child[from] = to;
        } else {
            // found need to disambiguate
            if (it->second == to)
                return;
            else {
                if (it->second > to) {
                    // child[from] = to;
                    auto old = it->second;
                    it->second = to;
                    add_link(old, to);
                }else{
                    //found value is smaller than what we want to link
                    add_link(to, it->second);
                }
            }
        }
    }
};
template <typename T> int ClusterFinder<T>::check_neighbours(int i, int j) {
    std::vector<int> neighbour_labels;

    for (int k = 0; k < 4; ++k) {
        const auto row = i + di[k];
        const auto col = j + dj[k];
        if (row >= 0 && col >= 0 && row<shape_[0] && col < shape_[1]) {
            auto tmp = labeled_.value(i + di[k], j + dj[k]);
            if (tmp != 0)
                neighbour_labels.push_back(tmp);
        }
    }

    if (neighbour_labels.size() == 0) {
        return 0;
    } else {

        // need to sort and add to union field
        std::sort(neighbour_labels.rbegin(), neighbour_labels.rend());
        auto first = neighbour_labels.begin();
        auto last = std::unique(first, neighbour_labels.end());
        if (last - first == 1)
            return *neighbour_labels.begin();

        for (auto current = first; current != last - 1; ++current) {
            auto next = current + 1;
            add_link(*current, *next);
        }
        return neighbour_labels.back(); //already sorted

    }
}

template <typename T> void ClusterFinder<T>::find_clusters(DataSpan<T, 2> img) {
    original_ = img;
    labeled_ = 0;
    current_label = 0;
    child.clear();
    first_pass();
    // print_connections();
    second_pass();
    store_clusters();
}

template <typename T> void ClusterFinder<T>::single_pass(DataSpan<T, 2> img) {
    original_ = img;
    labeled_ = 0;
    current_label = 0;
    child.clear();
    first_pass();
    // print_connections();
    // second_pass();
    // store_clusters();
}

template <typename T> void ClusterFinder<T>::first_pass() {

    for (int i = 0; i < original_.size(); ++i) {
        binary_(i) = (original_(i) > threshold_);
    }

    for (int i = 0; i < shape_[0]; ++i) {
        for (int j = 0; j < shape_[1]; ++j) {

            // do we have someting to process?
            if (binary_(i, j)) {
                auto tmp = check_neighbours(i, j);
                if (tmp != 0) {
                    labeled_(i, j) = tmp;
                } else {
                    labeled_(i, j) = ++current_label;
                }
            }
        }
    }
}

template <typename T> void ClusterFinder<T>::second_pass() {

    // for (ssize_t i=0; i!=labeled_.size(); ++i){
    //     auto current_label = labeled_(i);
    //     if (current_label!=0){
    //         auto it = child.find(current_label);
    //         while(it != child.end()){
    //             current_label = it->second;
    //             it = child.find(current_label);
    //         }
    //         labeled_(i) = current_label;
    //     }
    // }

    for (ssize_t i = 0; i != labeled_.size(); ++i) {
        auto current_label = labeled_(i);
        if (current_label != 0) {
            auto it = child.find(current_label);
            while (it != child.end()) {
                current_label = it->second;
                it = child.find(current_label);
            }
            labeled_(i) = current_label;
        }
    }
}

template <typename T> void ClusterFinder<T>::store_clusters() {

    // Accumulate hit information in a map
    // Do we always have monotonic increasing
    // labels? Then vector?
    std::unordered_map<int, Hit> h_size;
    for (int i = 0; i < shape_[0]; ++i) {
        for (int j = 0; j < shape_[1]; ++j) {
            if (labeled_(i, j) != 0) {
                Hit &record = h_size[labeled_(i, j)];
                record.size += 1;
                record.energy += original_(i, j);
                if (record.max < original_(i, j)) {
                    record.row = i;
                    record.col = j;
                    record.max = original_(i, j);
                }
            }
        }
    }

    // save hits to a vector
    //  hits.reserve(hits.size() + h_size.size());
    for (const auto &h : h_size) {
        hits.push_back(h.second);
    }
}

} // namespace pl