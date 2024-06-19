#pragma once

#include <quest/scenes/base.hpp>

namespace HQ {
    class game;

    // A LIFO scene manager.
    class scene_manager {
    public:
        using base_up = std::unique_ptr<scene::base>;

        scene_manager() = default;

        void update(game& g);

        std::vector<base_up> active, parked;
    };
}