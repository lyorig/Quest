#pragma once

#include <memory>

// move_only_function.hpp:
// A barebones implementation of `std::move_only_function` from `<functional>`,
// because not all compilers support it yet.

namespace hq {
    namespace detail {
        template <typename Ret, typename... Args>
        struct callable_base {
            virtual Ret operator()(Args&&...) = 0;
            virtual ~callable_base()          = default;
        };

        template <typename F, typename Ret, typename... Args>
        struct callable : callable_base<Ret, Args...> {
            F functor;

            callable(F&& f)
                : functor { std::forward<F>(f) } { }

            virtual Ret operator()(Args&&... args) override {
                return functor(std::forward<Args>(args)...);
            }

            virtual ~callable() = default;
        };
    }

    template <typename>
    class move_only_function { };

    template <typename Ret, typename... Args>
    class move_only_function<Ret(Args...)> {
    private:
        using base = detail::callable_base<Ret, Args...>;

        template <typename F>
        using clbl = detail::callable<F, Ret, Args...>;

    public:
        move_only_function() = default;

        template <typename F>
        move_only_function(F&& f)
            : m_ptr { std::make_unique<clbl<F>>(std::forward<F>(f)) } {
        }

        move_only_function(const move_only_function&)            = delete;
        move_only_function& operator=(const move_only_function&) = delete;

        move_only_function(move_only_function&&)            = default;
        move_only_function& operator=(move_only_function&&) = default;

        Ret operator()(Args&&... args) {
            return m_ptr->operator()(std::forward<Args>(args)...);
        }

    private:
        std::unique_ptr<base> m_ptr;
    };
}
