#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

namespace hq {
    class thread_pool {
    public:
        enum {
            NUM_THREADS = 32
        };

        // Default-initializes all threads.
        thread_pool();
        ~thread_pool();

        template <typename F>
        auto run(F&& f) {
            using ret_t    = std::invoke_result_t<F>;
            using future_t = std::future<ret_t>;

            std::packaged_task pt { std::forward<F>(f) };
            future_t           future_object { pt.get_future() };

            {
                std::unique_lock lock { m_mutex };
                m_taskQueue.emplace([pt = std::move(pt)] mutable {
                    pt();
                });
            }

            m_cv.notify_one();

            return future_object;
        }

    private:
        void worker();

        std::thread m_threads[NUM_THREADS];
        std::mutex  m_mutex;

        std::condition_variable m_cv;

        std::queue<std::function<void()>> m_taskQueue;

        bool m_shouldStop;
    };
}
