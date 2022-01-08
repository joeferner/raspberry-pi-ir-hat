#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_

template <typename T, size_t TSize>
class Queue {
private:
    T items[TSize];
    volatile size_t read;
    volatile size_t write;
    volatile size_t available;

public:
    Queue() : read(0), write(0), available(0) {}

    void push(T v) {
        items[write] = v;
        write = (write + 1) % TSize;
        available++;
    }

    T pop() {
        if (available == 0) {
            assert_param(0);
            return items[read];
        }
        T result = items[read];
        read = (read + 1) % TSize;
        available--;
        return result;
    }

    const T* begin() {
        return &items[read];
    }

    const T* end() {
        return &items[write];
    }

    bool isFull() const {
        return available == TSize;
    }

    bool isEmpty() const {
        return available == 0;
    }

    size_t getAvailable() const {
        return available;
    }
};

#endif
