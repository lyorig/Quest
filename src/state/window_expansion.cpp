#include <quest/state/window_expansion.hpp>

using namespace HQ::State;

WindowExpansion::WindowExpansion(const hal::window& wnd, const hal::system::video& vid, hal::renderer& rnd)
    : m_wndSize { wnd.size() }
    , m_wndPos { wnd.pos() }
    , m_drawCol { StartColor() } {
    constexpr Delta Time { 1.0 };

    rnd.draw_color(StartColor());

    m_wndSize.Start(vid.displays[wnd.display_index()].size(), Time);
    m_wndPos.Start({ 0, 0 }, Time);
    m_drawCol.Start(EndColor(), Time);
}

Type WindowExpansion::Update(App& app, hal::f64 elapsed) {
    if (m_wndSize.Update(elapsed)) {
        app.window.size(m_wndSize.Value());

        m_wndPos.Update(elapsed);
        app.window.pos(m_wndPos.Value());

        m_drawCol.Update(elapsed);
        app.renderer.draw_color(m_drawCol.Value());

        while (app.event.poll()) {
            switch (app.event.event_type()) {
                using enum hal::event_handler::type;

            case quit_requested:
                return Type::Quit;

            default:
                break;
            }
        }

        return Type::None;
    }

    // We're done.

    app.window.fullscreen(true);

    return Type::MainMenu;
}

void WindowExpansion::Draw(hal::renderer& rnd) const {
    static_cast<void>(rnd);
}