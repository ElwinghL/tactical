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
    void push(U&& value)
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_queue.push(std::forward<U>(value));
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_queue.empty();
    }

    T peek() const
    {
        assert(!empty());
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_queue.front();
    }

    void pop()
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_queue.pop();
    }

    T poll()
    {
        assert(!empty());
        std::lock_guard<std::mutex> lock{m_mutex};

        auto elt = m_queue.front();
        m_queue.pop();
        return elt;
    }

private:
    std::queue<T> m_queue{};
    mutable std::mutex m_mutex{};
};

#endif // QUEUES_H
