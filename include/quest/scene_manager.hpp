#pragma once

#include <quest/scenes/base.hpp>
#include <quest/scenes/main_menu.hpp>

#include <quest/stack_poly.hpp>

namespace HQ {
    // A LIFO scene manager.
    class scene_manager {
    public:
        using base_up = std::unique_ptr<scene::base>;

        scene_manager() = default;

        template <std::derived_from<scene::base> T>
        void push_back(T* ptr) {
            m_stack.emplace_back(ptr);
        }

        void pop_back();

        void process(const hal::event::handler& event);
        void update(delta_t time, hal::renderer& rnd);

    private:
        std::vector<base_up> m_stack;
    };
}