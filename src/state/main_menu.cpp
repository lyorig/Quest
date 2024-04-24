#include <quest/state/main_menu.hpp>
#include <quest/state/window_expansion.hpp>

using namespace HQ::State;

MainMenu::MainMenu(App& app)
    : m_theme { app.renderer.draw_color() }
    , m_currentTheme { 0 }
{
}

Type MainMenu::Update(App& app, hal::f64 elapsed)
{
    if (m_theme.Update(elapsed)) [[likely]]
    {
        app.renderer.draw_color(m_theme.Value());
    }

    else
    {
        m_theme.Start(SwitchTheme(), 4.0);
    }

    while (app.event.poll())
    {
        switch (app.event.event_type())
        {
            using enum hal::event::type;

        case quit_requested:
            return Type::Quit;

        case key_pressed:
            switch (app.event.keyboard().button())
            {
                using enum hal::keyboard::button;

            default:
                break;
            }

        default:
            break;
        }
    }

    return Type::None;
}

void MainMenu::Draw(hal::renderer& rnd) const
{
    static_cast<void>(rnd);
}

hal::color MainMenu::SwitchTheme()
{
    using namespace hal::palette;

    // Technically, this starts on the 2nd color.
    constexpr hal::color colors[] { blue, cyan, red, white, black, orange };
    static_assert(colors[1] != WindowExpansion::EndColor());
    m_currentTheme = ++m_currentTheme % std::size(colors);

    return colors[m_currentTheme];
}