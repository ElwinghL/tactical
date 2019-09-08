#ifndef QUEUES_H
#define QUEUES_H

#include <mutex>
#include <queue>
#include <utility>

/**
 * A clone of gf::Queue needed to avoid to
 * construct unnecessary object before call of poll
 */
template<typename T>
class PollingQueue {
public:
    template<typename U>
    void push(U&& value);

    [[nodiscard]] bool empty() const;

    [[nodiscard]] T peek() const;
    void pop();
    T poll();

private:
    std::queue<T> m_queue{};
    mutable std::mutex m_mutex{};
};

#include "impl/pollingqueue.h"

#endif // QUEUES_H
