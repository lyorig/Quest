#include <charconv>
#include <quest/game.hpp>

#include <cstring>
#include <filesystem>

#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

#include <quest/constants.hpp>
#include <quest/helpers.hpp>

#include <quest/scene/console.hpp>
#include <quest/scene/main_menu.hpp>

using namespace hq;

namespace hq::consts {
    constexpr std::string_view window_name { "HalQuest" };
}

namespace {
    consteval std::size_t len(const char* str) {
        return std::char_traits<char>::length(str);
    }
}

args::args(int argc, char** argv)
    : m_span { const_cast<const char**>(argv), static_cast<std::size_t>(argc) } {
}

bool args::operator[](std::string_view what) const {
    for (auto arg : m_span) {
        if (arg == what) {
            return true;
        }
    }

    return false;
}

game::game(args a)
    : video { m_context }
    , audio { m_context }
    , img { hal::image::init_format::jpg }
    , window { video, consts::window_name, hal::tag::fullscreen }
    , renderer { window, { hal::renderer::flag::accelerated, cond_enum(hal::renderer::flag::vsync, !a["--no-vsync"]) } }
    , atlas { renderer, { 128, 128 } }
    , timescale { 1.0 }
    , running { true }
    , screenshot { false } {
    renderer.blend(hal::blend_mode::blend);

    scenes.add(std::make_unique<scene::main_menu>(*this));
    scenes.add(std::make_unique<scene::console>(*this));
}

void game::main_loop() {
    hal::timer timer;

    while (running) {
        m_delta = timer();
        timer.reset();

        collect_events();

        scenes.update(*this);

        if (screenshot) {
            take_screenshot();
            screenshot = false;
        }

        renderer.present();
    }
}

const game::event_vector& game::polled() const {
    return m_polled;
}

delta_t game::delta() const {
    return m_delta * timescale;
}

void game::take_screenshot() const {
    hal::surface s { renderer.read_pixels(atlas.fmt) };

#define HQ_SCREENSHOT_PFX "screenshot-"
#define HQ_SCREENSHOT_EXT ".png"

    constexpr std::size_t
        digits { std::numeric_limits<std::size_t>::digits10 },
        pfxlen { len(HQ_SCREENSHOT_PFX) }, extlen { len(HQ_SCREENSHOT_EXT) };

    char filename[pfxlen + digits + extlen + 1] { HQ_SCREENSHOT_PFX };

    const std::filesystem::path directory { "screenshots" };

    if (!std::filesystem::is_directory(directory)) {
        std::filesystem::create_directory(directory);
    }

    std::size_t i { 0 };

    do {
        std::strcpy(std::to_chars(filename + pfxlen, std::end(filename) - 1, i++).ptr, HQ_SCREENSHOT_EXT);
    } while (std::filesystem::exists(directory / filename));

    img.save(s, hal::image::save_format::png, directory / filename);

#undef HQ_SCREENSHOT_EXT
#undef HQ_SCREENSHOT_PFX
}

void game::collect_events() {
    m_polled.clear();

    // Process global events here...
    while (video.events.poll(m_eventHandler)) {
        switch (m_eventHandler.kind()) {
            using enum hal::event::type;

        case quit_requested:
            running = false;
            break;

        case key_pressed:
            switch (m_eventHandler.keyboard().button()) {
                using enum hal::keyboard::button;

            case F12:
                screenshot = true;
                break;

            default:
                m_polled.push_back(m_eventHandler);
            }
            break;

        default:
            m_polled.push_back(m_eventHandler);
            break;
        }
    }
}
