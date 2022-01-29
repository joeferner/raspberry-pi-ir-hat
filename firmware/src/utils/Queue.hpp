#ifndef _QUEUE_HPP_
#define _QUEUE_HPP_

#include "test/test.hpp"

template <typename T, size_t TSize>
class QueueIterator;

template <typename T, size_t TSize>
class Queue {
private:
    friend class QueueIterator<T, TSize>;

    T items[TSize];
    volatile size_t read;
    volatile size_t write;
    volatile size_t available;

public:
    Queue() : read(0), write(0), available(0) {}

    void clear() {
        __disable_irq();
        read = 0;
        write = 0;
        available = 0;
        __enable_irq();
    }

    void push(T v) {
        if (isFull()) {
            pop();            
        }
        __disable_irq();
        items[write] = v;
        write = (write + 1) % TSize;
        available++;
        assert_param(((write - read) % TSize) == available);
        __enable_irq();
    }

    T pop() {
        if (available == 0) {
            assert_param(0);
            return items[read];
        }
        __disable_irq();
        T result = items[read];
        read = (read + 1) % TSize;
        available--;
        assert_param(((write - read) % TSize) == available);
        __enable_irq();
        return result;
    }

    const QueueIterator<T, TSize> begin() {
        return QueueIterator(this, read);
    }

    const QueueIterator<T, TSize> begin(size_t offset) {
        if (offset > available) {
            assert_param(0);
        }
        return QueueIterator(this, read + offset);
    }

    const QueueIterator<T, TSize> end() {
        return QueueIterator(this, write);
    }

    bool isFull() const {
        return available == (TSize - 1);
    }

    bool isEmpty() const {
        return available == 0;
    }

    size_t getAvailable() const {
        return available;
    }

    T operator[](size_t offset) {
        if (offset > available) {
            assert_param(0);
        }
        return items[(read + offset) % TSize];
    }
};

template <typename T, size_t TSize>
class QueueIterator {
private:
    friend class Queue<T, TSize>;

    Queue<T, TSize>* queue;
    size_t offset;

    QueueIterator(Queue<T, TSize>* queue, size_t offset) : queue(queue), offset(offset) {}

public:
    bool operator!=(const QueueIterator<T, TSize>& other) const {
        return (this->queue != other.queue) || (this->offset != other.offset);
    }

    QueueIterator<T, TSize>& operator++(int) {
        offset = (offset + 1) % TSize;
        return *this;
    }

    T operator*() const {
        return this->queue->items[this->offset];
    }
};

#endif
