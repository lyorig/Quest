#include <quest/state/main_menu.hpp>

using namespace HQ::State;

Type MainMenu::Update(hal::event::handler& event, hal::f64 elapsed)
{
    while (event.poll())
    {
        switch (event.event_type())
        {
            using enum hal::event::type;

        case quit_requested:
            return Type::Quit;

        default:
            break;
        }
    }

    return Type::None;
}

void MainMenu::Draw(hal::video::renderer& rnd) const
{
}
