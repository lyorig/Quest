#pragma once

#include <array>

#include <quest/animatable.hpp>
#include <quest/sprite.hpp>

#include <quest/scenes/base.hpp>

// state/rest.hpp:
// The rest of the state types.

namespace HQ::scene {
    class main_menu final : public base {
    public:
        main_menu(hal::renderer& rnd, hal::ttf::context& ttf);

        type update(game& g) override;

    private:
        void set_widget(std::size_t index, const hal::font& fnt, std::string_view text);
        void switch_theme();

        animatable<hal::color, easing::in_out::bezier> m_theme;

        std::array<sprite, 4> m_widgets;

        hal::u8 m_currentTheme;
    };
}