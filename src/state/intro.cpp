#include <quest/state/intro.hpp>

using namespace HQ::State;

Intro::Intro(hal::video::renderer& rnd, hal::ttf::context& ttf)
    : m_text { rnd.load(ttf.load(hal::access("assets/m5x7.ttf"), 48).render("Made with Halcyon", hal::palette::cyan)) }
{
}

Type Intro::Update(hal::event::handler& event, hal::f64 elapsed)
{
    while (event.poll())
    {
        switch (event.event_type())
        {
            using enum hal::event::type;
            using enum hal::keyboard::button;

        case key_pressed:
            if (event.keyboard().button() == esc)
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
    rnd.draw(m_text)();
}