#pragma once

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename T>
class BoundedRingBuffer {
public:
    explicit BoundedRingBuffer(
        std::size_t capacity)
        : buffer_(capacity),
          capacity_(capacity) {

        if (capacity == 0) {
            throw std::invalid_argument(
                "Ring buffer capacity must be greater than zero");
        }
    }

    BoundedRingBuffer(
        const BoundedRingBuffer&) = delete;

    BoundedRingBuffer& operator=(
        const BoundedRingBuffer&) = delete;

    bool full() const {
        return size_ == capacity_;
    }

    bool empty() const {
        return size_ == 0;
    }

    std::size_t size() const {
        return size_;
    }

    std::size_t capacity() const {
        return capacity_;
    }

    template <typename U>
    bool push(U&& value) {

        if (full()) {
            return false;
        }

        buffer_[tail_] =
            std::forward<U>(value);

        tail_ =
            (tail_ + 1) % capacity_;

        ++size_;

        return true;
    }

    std::optional<T> pop() {

        if (empty()) {
            return std::nullopt;
        }

        T value =
            std::move(buffer_[head_]);

        head_ =
            (head_ + 1) % capacity_;

        --size_;

        return value;
    }

private:
    std::vector<T> buffer_;

    std::size_t capacity_;

    std::size_t head_ = 0;

    std::size_t tail_ = 0;

    std::size_t size_ = 0;
};
