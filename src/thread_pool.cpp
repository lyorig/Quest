#include <quest/thread_pool.hpp>

#include <mutex>

using namespace hq;

thread_pool::thread_pool()
    : m_stop { false } {
    for (std::thread& t : m_threads) {
        t = std::thread { &thread_pool::worker, this };
    }
}

thread_pool::~thread_pool() {
    m_stop = true;
    m_cv.notify_all();

    for (std::thread& t : m_threads) {
        t.join();
    }
}

void thread_pool::worker() {
    while (true) {
        std::unique_lock lk { m_mutex };

        m_cv.wait(lk, [this] {
            return !m_tasks.empty() || m_stop;
        });

        if (!m_tasks.empty()) {
            move_only_function<void()> task { std::move(m_tasks.front()) };
            m_tasks.erase(m_tasks.begin());
            lk.unlock();
            task();
        } else {
            return;
        }
    }
}
