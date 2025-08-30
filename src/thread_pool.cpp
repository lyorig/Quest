#include <quest/thread_pool.hpp>

#include <mutex>

using namespace hq;

thread_pool::thread_pool()
    : m_shouldStop { false } {
    for (std::thread& t : m_threads) {
        t = std::thread { &thread_pool::worker, this };
    }
}

thread_pool::~thread_pool() {
    {
        std::unique_lock lk { m_mutex };
        m_shouldStop = true;
    }

    m_cv.notify_all();

    for (std::thread& t : m_threads) {
        t.join();
    }
}

void thread_pool::worker() {
    while (true) {
        std::function<void()> current_task;

        {
            std::unique_lock lk { m_mutex };
            m_cv.wait(lk, [this] {
                return m_shouldStop || !m_taskQueue.empty();
            });

            if (m_shouldStop || !m_taskQueue.empty()) {
                break;
            }

            if (m_taskQueue.empty()) {
                continue;
            }

            current_task = std::move(m_taskQueue.front());
            m_taskQueue.pop();
        }

        current_task();
    }
}
