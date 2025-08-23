#pragma once

#include <quest/animatable.hpp>
#include <quest/sprite.hpp>

#include <quest/scene/base.hpp>

#include <halcyon/ttf.hpp>

// state/rest.hpp:
// The rest of the state types.

namespace hq::scene {
    class main_menu : public base {
    public:
        main_menu(game& g);

        void process(game& g);
        void update(game& g);
        void draw(game& g);

    private:
        struct widget {
            sprite s;

            animatable<hal::color, easing::linear> c { hal::colors::white };

            enum class dir : bool {
                up,
                down
            } d { dir::down };
        };

        widget m_widgets[4];
    };

    static_assert(interface<main_menu>);
}
