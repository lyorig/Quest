#include <quest/state/rest.hpp>

using namespace HQ::State;

MainMenu::MainMenu(hal::renderer& rnd, hal::ttf::context& ttf)
    : m_theme { rnd.draw_color() }
    , m_currentTheme { 0 } {
    const auto font = ttf.load(hal::access("assets/m5x7.ttf"), 16);

    const auto surf = font.render("HalodaQuest");

    m_widgets.push_back({ rnd.make_texture(surf), { 75, 0 }, hal::scale::width(rnd.size().x / 2)(surf.size()) });
}

void MainMenu::Process(const hal::event::handler& event) {
    switch (event.event_type()) {
        using enum hal::event::handler::type;

    case key_pressed:
        switch (event.keyboard().button()) {
            using enum hal::keyboard::button;

        default:
            break;
        }
        break;

    case mouse_pressed:
        if (event.mouse_button().button() == hal::mouse::button::left) {
            for (const auto& wgt : m_widgets) {
                if (event.mouse_button().pos() | wgt.Hitbox()) {
                    m_theme.Start(SwitchTheme(), 0.5);
                    break;
                }
            }
        }
        break;

    default:
        break;
    }
}

Base* MainMenu::Update(hal::f64 elapsed) {
    m_theme.Update(elapsed);

    return nullptr;
}

void MainMenu::Draw(hal::renderer& rnd) const {
    rnd.draw_color(m_theme.Value());

    for (const auto& wgt : m_widgets) {
        wgt.Draw(rnd);
    }
}

hal::color MainMenu::SwitchTheme() {
    using namespace hal::palette;

    // Technically, this starts on the 2nd color.
    constexpr hal::color colors[] { blue, red, black, orange };
    m_currentTheme = ++m_currentTheme % std::size(colors);

    return colors[m_currentTheme];
}