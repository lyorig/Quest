#include <quest/state/rest.hpp>

using namespace HQ::state;

main_menu::main_menu(hal::renderer& rnd, hal::ttf::context& ttf)
    : m_theme { rnd.draw_color() }
    , m_currentTheme { 0 } {
    const auto font = ttf.load(hal::access("assets/m5x7.ttf"), 16);

    const auto surf = font.render("HalodaQuest");

    m_widgets.push_back({ rnd.make_texture(surf), { 75, 0 }, hal::scale::width(rnd.size().x / 2)(surf.size()) });
}

void main_menu::Process(const hal::event::handler& event) {
    switch (event.event_type()) {
        using enum hal::event::type;

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
                if (event.mouse_button().pos() | wgt.hitbox()) {
                    m_theme.start(SwitchTheme(), 0.5);
                    break;
                }
            }
        }
        break;

    default:
        break;
    }
}

Base* main_menu::Update(delta_t elapsed) {
    m_theme.update(elapsed);

    return nullptr;
}

void main_menu::Draw(hal::renderer& rnd) const {
    rnd.draw_color(m_theme.value());

    for (const auto& wgt : m_widgets) {
        wgt.draw(rnd);
    }
}

hal::color main_menu::SwitchTheme() {
    using namespace hal::palette;

    // Technically, this starts on the 2nd color.
    constexpr hal::color colors[] { blue, red, black, orange };
    m_currentTheme = ++m_currentTheme % std::size(colors);

    return colors[m_currentTheme];
}

console::console(hal::ttf::font&& fnt)
    : m_font { std::move(fnt) }
    , m_active { false } {
}

void console::Draw(hal::renderer& rnd) const {
    constexpr hal::color Background { hal::palette::weezer_blue, 64 };

    if (m_active) {
        hal::renderer::color_lock lock { rnd, Background };
        rnd.fill_target();
    }
}

void console::Log(std::string&& str) {
    m_entries.push_back(std::move(str));
}

bool console::Active() const {
    return m_active;
}

void console::Toggle(hal::renderer& rnd) {
    if (m_active)
        Hide();
    else
        Show(rnd);
}

void console::Show(hal::renderer& rnd) {

    m_active = true;

    m_tex = rnd.make_texture(m_font.render("Amogus"));

    HAL_PRINT("Showing console");
}

void console::Hide() {
    m_active = false;

    m_tex.reset();

    HAL_PRINT("Hiding console");
}