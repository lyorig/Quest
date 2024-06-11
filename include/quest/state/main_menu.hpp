#pragma once

#include <vector>

#include <quest/animatable.hpp>
#include <quest/sprite.hpp>

#include <quest/state/base.hpp>

// state/rest.hpp:
// The rest of the state types.

namespace HQ::state {
    class main_menu final : public base {
    public:
        main_menu(hal::renderer& rnd, hal::ttf::context& ttf);

        void  process(const hal::event::handler& event) override;
        base* update(hal::f64 elapsed, hal::renderer& rnd) override;

    private:
        void switch_theme();

        animatable<hal::color, easing::in_out::bezier> m_theme;

        std::vector<sprite> m_widgets;

        hal::u8 m_currentTheme;
    };
}