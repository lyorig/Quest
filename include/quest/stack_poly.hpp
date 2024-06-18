#pragma once

#include <algorithm>
#include <utility>

#include <halcyon/utility/concepts.hpp>

namespace HQ {
    template <hal::meta::bare Base, hal::meta::bare... Ts>
        requires(std::is_base_of_v<Base, Ts> && ...)
    class stack_poly {
    public:
        template <hal::meta::one_of<Ts...> T, typename... Args>
            requires std::constructible_from<T, Args...>
        stack_poly(std::in_place_type_t<T>, Args&&... args) {
            emplace<T>(std::forward<Args>(args)...);
        }

        ~stack_poly() {
            reset();
        }

        template <hal::meta::one_of<Ts...> T, typename... Args>
            requires std::constructible_from<T, Args...>
        void emplace(Args&&... args) {
            std::construct_at(reinterpret_cast<T*>(m_data.data()), std::forward<Args>(args)...);
        }

        void reset() {
            std::destroy_at(reinterpret_cast<Base*>(m_data.data()));
        }

        Base* get() {
            return reinterpret_cast<Base*>(m_data.data());
        }

        const Base* get() const {
            return reinterpret_cast<const Base*>(m_data.data());
        }

        Base* operator->() {
            return get();
        }

        const Base* operator->() const {
            return get();
        }

        Base& operator*() {
            return *get();
        }

        const Base& operator*() const {
            return *get();
        }

    private:
        std::array<std::byte, std::max({ sizeof(Ts)... })> m_data;
    };
}