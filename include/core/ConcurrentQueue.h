#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <utility>

template <typename T>
class ConcurrentQueue
{
private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable condition;
    bool closed = false;

public:
    ConcurrentQueue() = default;

    ConcurrentQueue(const ConcurrentQueue&) = delete;
    ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;

    void push(const T& value)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);

            if (closed)
            {
                return;
            }

            queue.push(value);
        }

        condition.notify_one();
    }

    void push(T&& value)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);

            if (closed)
            {
                return;
            }

            queue.push(std::move(value));
        }

        condition.notify_one();
    }

    std::optional<T> waitAndPop()
    {
        std::unique_lock<std::mutex> lock(mutex);

        condition.wait(
            lock,
            [this]
            {
                return closed || !queue.empty();
            }
        );

        if (queue.empty())
        {
            return std::nullopt;
        }

        T value = std::move(queue.front());
        queue.pop();

        return value;
    }

    std::optional<T> tryPop()
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (queue.empty())
        {
            return std::nullopt;
        }

        T value = std::move(queue.front());
        queue.pop();

        return value;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex);

        return queue.empty();
    }

    std::size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex);

        return queue.size();
    }

    void close()
    {
        {
            std::lock_guard<std::mutex> lock(mutex);

            closed = true;
        }

        condition.notify_all();
    }

    bool isClosed() const
    {
        std::lock_guard<std::mutex> lock(mutex);

        return closed;
    }
};

#endif