#ifndef QUEUES_H
#define QUEUES_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

/**
 * Queue blocking execution when trying to pop a value
 */
template<typename T>
class BlockingQueue {
public:
    /**
     * Insert a value at the end of the queue
     * \param value Value to insert
     */
    template<typename U>
    void push(U&& value)
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_queue.push(std::forward<U>(value));
        m_cv.notify_all();
    }

    /**
     * Get the front value of the queue and remove it.
     * If the queue is empty, block the current thread until a value
     * get into the queue or until fail is called
     *
     * \param[out] value Value got from the queue. Not modified when fail is called.
     * \return Whether the function succeed or not
     */
    bool pop(T& value)
    {
        std::unique_lock<std::mutex> lock{m_mutex};

        m_cv.wait(lock, [this] {
            return m_fail || !m_queue.empty();
        });

        if (!m_fail) {
            value = m_queue.front();
            m_queue.pop();
        }

        return !m_fail;
    }

    /**
     * Unblock all threads blocked by pop.
     * All calls to pop will fail after this call.
     */
    void fail()
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_fail = true;
        m_cv.notify_all();
    }

private:
    std::queue<T> m_queue{};
    std::mutex m_mutex{};
    std::condition_variable m_cv{};

    bool m_fail{false};
};

/**
 * A clone of gf::Queue using boost::optional, avoiding to
 * construct unnecessary object before call
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

    boost::optional<T> pop()
    {
        std::lock_guard<std::mutex> lock{m_mutex};

        boost::optional<T> result;
        if (!m_queue.empty()) {
            result = m_queue.front();
            m_queue.pop();
        }

        return result;
    }

private:
    std::queue<T> m_queue{};
    std::mutex m_mutex{};
};

#endif // QUEUES_H
