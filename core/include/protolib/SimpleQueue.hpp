#pragma once
#include <atomic>

//almost entirely based on
//https://github.com/facebook/folly/blob/main/folly/ProducerConsumerQueue.h

namespace pl{

constexpr std::size_t hardware_destructive_interference_size = 128;

template <typename T> class SimpleQueue {
    // Benchmark and investigate effects of padding
    char pad0_[hardware_destructive_interference_size];
    alignas(hardware_destructive_interference_size)
        std::atomic<uint32_t> readIndex_{0};
    alignas(hardware_destructive_interference_size)
        std::atomic<uint32_t> writeIndex_{0};
    std::size_t size;
    T *records_;

  public:
    SimpleQueue(uint32_t qsize) : size(qsize + 1), records_(new T[size]) {}
    ~SimpleQueue() { delete[] records_; }

    std::size_t sizeGuess() const {
        int ret = writeIndex_.load(std::memory_order_acquire) -
                  readIndex_.load(std::memory_order_acquire);
        if (ret < 0) {
            ret += static_cast<int>(size);
        }
        return ret;
    }

//   template <class... Args>
//   bool write(Args&&... recordArgs) {
//     auto const currentWrite = writeIndex_.load(std::memory_order_relaxed);
//     auto nextRecord = currentWrite + 1;
//     if (nextRecord == size_) {
//       nextRecord = 0;
//     }
//     if (nextRecord != readIndex_.load(std::memory_order_acquire)) {
//       new (&records_[currentWrite]) T(std::forward<Args>(recordArgs)...);
//       writeIndex_.store(nextRecord, std::memory_order_release);
//       return true;
//     }

//     // queue is full
//     return false;
//   }
    template <class... Args>
    bool push(Args&&... recordArgs) {
        auto const currentWrite = writeIndex_.load(std::memory_order_relaxed);
        auto nextRecord = currentWrite + 1;
        if (nextRecord == size) {
            nextRecord = 0;
        }
        if (nextRecord != readIndex_.load(std::memory_order_acquire)) {
            new (&records_[currentWrite]) T(std::forward<Args>(recordArgs)...);
            writeIndex_.store(nextRecord, std::memory_order_release);
            return true;
        }
        return false;
    }

//       // move (or copy) the value at the front of the queue to given variable
//   bool read(T& record) {
//     auto const currentRead = readIndex_.load(std::memory_order_relaxed);
//     if (currentRead == writeIndex_.load(std::memory_order_acquire)) {
//       // queue is empty
//       return false;
//     }

//     auto nextRecord = currentRead + 1;
//     if (nextRecord == size_) {
//       nextRecord = 0;
//     }
//     record = std::move(records_[currentRead]);
//     records_[currentRead].~T();
//     readIndex_.store(nextRecord, std::memory_order_release);
//     return true;
//   }

    bool pop(T &record) {
        auto const currentRead = readIndex_.load(std::memory_order_relaxed);
        if (currentRead == writeIndex_.load(std::memory_order_acquire)) {
            return false;
        }
        auto nextRecord = currentRead + 1;
        if (nextRecord == size) {
            nextRecord = 0;
        }
        record = std::move(records_[currentRead]);
        records_[currentRead].~T();
        readIndex_.store(nextRecord, std::memory_order_release);
        return true;
    }
};

}