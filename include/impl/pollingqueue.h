#ifndef IMPL_POLLINGQUEUE_H
#define IMPL_POLLINGQUEUE_H

template<typename T> template<typename U>
void PollingQueue<T>::push(U&& value)
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_queue.push(std::forward<U>(value));
}

template<typename T>
[[nodiscard]] bool PollingQueue<T>::empty() const
{
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_queue.empty();
}

template<typename T>
[[nodiscard]] T PollingQueue<T>::peek() const
{
    assert(!empty());
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_queue.front();
}

template<typename T>
void PollingQueue<T>::pop()
{
    std::lock_guard<std::mutex> lock{m_mutex};
    m_queue.pop();
}

template<typename T>
T PollingQueue<T>::poll()
{
    assert(!empty());
    std::lock_guard<std::mutex> lock{m_mutex};

    auto elt = m_queue.front();
    m_queue.pop();
    return elt;
}

#endif //IMPL_POLLINGQUEUE_H
