#pragma once

#include <array>

#include <halcyon/ttf.hpp>

#include <quest/animatable.hpp>
#include <quest/sprite.hpp>

#include <quest/scene/base.hpp>
#include <random>

// state/rest.hpp:
// The rest of the state types.

namespace hq::scene {
    class main_menu final : public base {
    public:
        main_menu(game& g);

        void process(game& g);
        void update(game& g);
        void draw(game& g);

    private:
        void set_widget(std::size_t index, const hal::font& fnt, std::string_view text);
        void switch_theme();

        animatable<hal::color, easing::in_out::bezier> m_theme;

        std::array<sprite, 4> m_widgets;
        hal::coord::rect      m_outline;

        hal::u8 m_currentTheme;

        std::mt19937_64                                    m_mt;
        std::uniform_int_distribution<hal::color::value_t> m_uid;
    };
}
