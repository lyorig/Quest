#pragma once

#include <vector>

#include <halcyon/utility/metaprogramming.hpp>

// rfem.hpp:
// The Really Fast Entity Manager.

namespace hq {
    template <hal::meta::innermost T>
    class rfem_leaf {
    protected:
        std::vector<T> vec;

        template <std::invocable<T&> Func>
        void visit(Func&& f) {
            for (auto& obj : vec) {
                f(obj);
            }
        }
    };

    template <hal::meta::innermost... Ts>
    class rfem : public rfem_leaf<Ts>... {
    public:
        rfem() = default;

        // Directly access an entity vector.
        template <hal::meta::one_of<Ts...> T>
        std::vector<T>& get() {
            return rfem_leaf<T>::vec;
        }

        // Directly access an entity vector. (const overload)
        template <hal::meta::one_of<Ts...> T>
        const std::vector<T>& get() const {
            return rfem_leaf<T>::vec;
        }

        template <hal::meta::one_of<Ts...> T, typename... Args>
            requires std::constructible_from<T, Args...>
        void spawn(Args&&... ctor_args) {
            get<T>().emplace_back(std::forward<Args>(ctor_args)...);
        }

        // Invoke a functor to all entities.
        template <typename Func>
            requires(std::invocable<Func, Ts&> && ...)
        void visit_all(Func&& f) {
            (rfem_leaf<Ts>::visit(std::forward<Func>(f)), ...);
        }
    };
}
