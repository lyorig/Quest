#include <quest/state/main_menu.hpp>

using namespace HQ::State;

Type MainMenu::Update(App& app, hal::f64 elapsed)
{
    static_cast<void>(elapsed);

    while (app.event.poll())
    {
        switch (app.event.event_type())
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
    static_cast<void>(rnd);
}
