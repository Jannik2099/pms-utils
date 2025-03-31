#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

namespace pms_utils::test {

class Threadpool {
private:
    std::size_t size_;
    std::vector<std::thread> threads;
    std::function<void()> func_;

public:
    [[nodiscard]] std::size_t size() const { return size_; }

    void join() {
        std::ranges::for_each(threads, &std::thread::join);
        threads.clear();
    }

    Threadpool(std::function<void()> func, std::size_t size = std::thread::hardware_concurrency())
        : size_(size), func_(std::move(func)) {
        if (size_ == 0) {
            throw std::invalid_argument{"Threadpool size must be greater than 0"};
        }

        threads.reserve(size_);
        for (std::size_t i = 0; i < size_; i++) {
            threads.emplace_back(func_);
        }
    }
    ~Threadpool() { join(); }

    Threadpool(const Threadpool &) = delete;
    Threadpool &operator=(const Threadpool &) = delete;

    Threadpool(Threadpool &&) = default;
    Threadpool &operator=(Threadpool &&) = default;
};

} // namespace pms_utils::test
