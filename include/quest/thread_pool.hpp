#pragma once

#include <quest/util/move_only_function.hpp>

#include <halcyon/system.hpp>

#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

namespace hq {
    class thread_pool {
    public:
        template <std::invocable F>
        using future_of = std::future<std::invoke_result_t<F>>;

        // Default-initializes all threads.
        thread_pool();

        // Stops all threads.
        ~thread_pool();

        // Queue a task.
        // This returns a futu
        template <std::invocable F>
        future_of<F> run(F&& f) {
            std::packaged_task pt { std::forward<F>(f) };
            future_of<F>       fut { pt.get_future() };

            {
                std::unique_lock lock { m_mutex };
                m_tasks.emplace_back([&, pt = std::move(pt)] mutable {
                    pt();
                });
            }

            m_cv.notify_one();

            return fut;
        }

    private:
        void worker();

        // See line 49 of `CMakeLists.txt` for the `HQ_CPU_COUNT` macro.
        std::thread m_threads[HQ_CPU_COUNT];
        std::mutex  m_mutex;

        std::condition_variable m_cv;

        // `move_only_function` is just a pointer under the hood.
        // This means that moving them around is practically free,
        // so using `std::vector` is preferable over `std::queue`.
        std::vector<move_only_function<void()>> m_tasks;

        // We could use `std::stop_source`, but that's just more overhead.
        // Plus, Apple Clang doesn't support it, because of course it doesn't.
        bool m_stop;
    };
}
