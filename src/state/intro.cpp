#include <quest/state/intro.hpp>
#include <random>

using namespace HQ::State;

namespace
{
    std::string_view RandomText()
    {
        constexpr std::string_view phrases[] { "Womp womp.", "Made with Halcyon", "5/5/2022", "There Will Come Soft Rains\n> Evacuate Zephyrus 115.", "Loading..." };

        std::random_device rd;
        std::mt19937_64    mt { rd() };

        std::uniform_int_distribution dist { static_cast<std::size_t>(0), std::size(phrases) };

        return phrases[dist(mt) % std::size(phrases)];
    }
}

Intro::Intro(App& app)
    : m_text { app.renderer.load(app.ttf.load(hal::access("assets/pixelmix.ttf"), 48).render(RandomText(), hal::palette::black)) }
    , m_coord { (app.window.size(m_text.size() * 1.25), hal::coord_rect { hal::tag::as_size, app.renderer.size() }.anchor(hal::anchor::center, m_text.size())) }
    , m_alpha { m_text.alpha_mod() }
{
    m_alpha.Start(0, 2.0);
}

Type Intro::Update(App& app, hal::f64 elapsed)
{
    if (m_alpha.Update(elapsed))
        m_text.alpha_mod(m_alpha.Value());

    else
        return Type::WindowExpansion;

    while (app.event.poll())
    {
        switch (app.event.event_type())
        {
            using enum hal::event::type;
            using enum hal::keyboard::button;

        case key_pressed:
            if (app.event.keyboard().button() == esc)
                return Type::WindowExpansion;
            break;

        case quit_requested:
            return Type::Quit;

        default:
            break;
        }
    };

    return Type::None;
}

void Intro::Draw(hal::video::renderer& rnd) const
{
    rnd.draw(m_text).to(m_coord)();
}