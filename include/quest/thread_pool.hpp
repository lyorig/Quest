#pragma once

#include <quest/util/move_only_function.hpp>

#include <condition_variable>
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

        // Stops all threads.
        ~thread_pool();

        template <typename F>
        auto run(F&& f) {
            using ret_t    = std::invoke_result_t<F>;
            using future_t = std::future<ret_t>;

            std::packaged_task<ret_t()> pt { std::forward<F>(f) };
            future_t                    fut { pt.get_future() };

            {
                std::unique_lock lock { m_mutex };
                m_jobs.emplace([&, pt = std::move(pt)] mutable {
                    pt();
                });
            }

            m_cv.notify_one();

            return fut;
        }

    private:
        void worker();

        std::thread m_threads[NUM_THREADS];
        std::mutex  m_mutex;

        std::condition_variable m_cv;

        std::queue<move_only_function<void()>> m_jobs;

        // We could use `std::stop_source`, but that's just more overhead.
        bool m_stop;
    };
}
