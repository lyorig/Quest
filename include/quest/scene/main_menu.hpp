#pragma once

#include "halcyon/types/color.hpp"
#include <quest/animatable.hpp>
#include <quest/sprite.hpp>

#include <quest/scene/base.hpp>

#include <halcyon/ttf.hpp>

#include <array>

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

            animatable<hal::color, easing::linear> c { hal::palette::white };

            enum class dir : bool {
                up,
                down
            } d { dir::down };
        };

        void set_widget(std::size_t index, hal::ref<hal::font> fnt, std::string_view text);
        void switch_theme();

        animatable<hal::color, easing::out::quad> m_theme;

        std::array<widget, 4> m_widgets;

        std::uint8_t m_currentTheme;
    };

    static_assert(interface<main_menu>);
}
