#include <quest/state/intro.hpp>

using namespace HQ::State;

Intro::Intro(hal::video::renderer& rnd, hal::ttf::context& ttf)
    : m_text { rnd.load(ttf.load(hal::access("assets/m5x7.ttf"), 48).render("[Intro screen]", hal::palette::black).resize(2.0)) }
    , m_coord { hal::coord_rect { hal::tag::as_size, rnd.size() }.anchor(hal::anchor::center, m_text.size()) }
{
}

Type Intro::Update(App& app, hal::f64 elapsed)
{
    while (app.event.poll())
    {
        switch (app.event.event_type())
        {
            using enum hal::event::type;
            using enum hal::keyboard::button;

        case key_pressed:
            if (app.event.keyboard().button() == esc)
            {
                delete this;
                return Type::MainMenu;
            }
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