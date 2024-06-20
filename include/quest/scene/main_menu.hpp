#pragma once

#include <array>

#include <quest/animatable.hpp>
#include <quest/sprite.hpp>

#include <quest/scene/base.hpp>

// state/rest.hpp:
// The rest of the state types.

namespace HQ::scene {
    class main_menu final : public base {
    public:
        main_menu(game& g);

        action process(game& g) override;
        void   update(game& g) override;
        void   draw(hal::renderer& rnd) override;

        void activate(game& g) override;
        void deactivate() override;

        virtual std::string_view name() const override;

    private:
        void set_widget(std::size_t index, const hal::font& fnt, std::string_view text);
        void switch_theme();

        animatable<hal::color, easing::in_out::bezier> m_theme;

        std::array<sprite, 4> m_widgets;
        hal::coord::rect      m_outline;

        hal::u8 m_currentTheme;
    };
}