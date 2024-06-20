#pragma once

#include <quest/scene/base.hpp>
#include <quest/static_vector.hpp>

namespace HQ {
    class game;

    // A LIFO scene manager.
    class scene_manager {
    public:
        using base_up      = std::unique_ptr<scene::base>;
        using scene_vector = static_vector<base_up, 2>;

        scene_manager() = default;

        void update(game& g);

        void add_active(base_up&& scn);
        void add_parked(base_up&& scn);

    private:
        scene_vector m_active, m_parked;

        // Where to start drawing from.
        scene_vector::iterator m_cachedLastOpaque;

        scene_vector::iterator find_last_opaque();
    };
}