#define _CRT_SECURE_NO_WARNINGS

#include <halcyon/system.hpp>
#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

#include <quest/game.hpp>
#include <quest/helpers.hpp>

#include <charconv>
#include <filesystem>

using namespace hq;

template <typename T>
using lims = std::numeric_limits<T>;

namespace {
    hal::window create_window(hal::proxy::video v, args a) {
        constexpr hal::c_string title { "HalQuest" };

        return {
            v,
            title,
            v.display_info_native(0)->size() * 0.75,
            { cond_enum(hal::window::flag::hidden, a["--dump"]), hal::window::flag::resizable }
        };
    }

    hal::renderer create_renderer(hal::lref<const hal::window> wnd, args a) {
        hal::renderer rnd { wnd, cond_enum(hal::renderer::flag::vsync, !a["--no-vsync"]) };
        rnd.blend(hal::blend_mode::alpha);
        return rnd;
    }
}

args::args(int argc, char** argv)
    : args { argc, argv, std::nothrow } {
}

args::args(int argc, char** argv, std::nothrow_t)
    : m_span { const_cast<const char**>(argv), static_cast<std::size_t>(argc) } {
}

std::size_t args::size() const {
    return m_span.size();
}

bool args::operator[](std::string_view what) const {
    for (std::size_t i { 0 }; i < m_span.size(); ++i) {
        if (what == m_span[i]) {
            return true;
        }
    }

    return false;
}

game::game(args a) try
    : systems{}
    , img{ hal::image::init_format::jpg }
    , window{ create_window(systems, a) }
    , renderer{ create_renderer(window, a) }
    , scenes{ *this }
    , timescale{ 1.0 }
    , running{ true }
    , screenshot{ false } {
    if (a["--dump"]) {
        HAL_DEBUG_TIMER(i);

        HAL_PRINT("<Dump> Platform:\t", hal::platform());
        HAL_PRINT("<Dump> Window:\t\t", window);
        HAL_PRINT("<Dump> Renderer:\t", renderer.info().get());
        HAL_PRINT("<Dump> CPU:\t\t", hal::cpu::info);
        HAL_PRINT("<Dump> RAM:\t\t", hal::total_ram(), " MiB");

        HAL_PRINT("<Dump> Finished in ", std::fixed, i, '.');

        running = false;
    }

} catch (hal::exception e) {
    HAL_PRINT("Exception raised: ", e.with_error());
}

void game::main_loop() {
    hal::timer timer;

    while (running) {
        m_delta = timer();
        timer.reset();

        collect_events();

        scenes.update(*this);

        if (screenshot) [[unlikely]] {
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
    namespace fs = std::filesystem;

    hal::surface s { renderer.read_pixels() };

#define HQ_SCREENSHOT_PFX "screenshot-"
#define HQ_SCREENSHOT_EXT ".png"

    constexpr std::size_t
        digits { std::numeric_limits<decltype(digits)>::digits10 },
        pfxlen { hal::strlen(HQ_SCREENSHOT_PFX) }, extlen { hal::strlen(HQ_SCREENSHOT_EXT) },
        max_ss_attempts { std::numeric_limits<decltype(max_ss_attempts)>::max() };

    char filename[pfxlen + digits + extlen + 1] { HQ_SCREENSHOT_PFX };

    const fs::path directory { "screenshots" };

    fs::path current;

    if (!fs::is_directory(directory)) {
        fs::create_directory(directory);
    }

    std::size_t i { 0 };

    do {
        std::strcpy(std::to_chars(filename + pfxlen, std::end(filename) - 1, i++).ptr, HQ_SCREENSHOT_EXT);
    } while (fs::exists(current = directory / filename)
        && i != max_ss_attempts);

    // Rust users can suck it, this is true paranoia
    if (i == max_ss_attempts) [[unlikely]] {
        HAL_PRINT(hal::debug::severity::warning, "Exhausted numbering of screenshots. Aborting save.");
        return;
    }

    img.save(s, hal::image::save_format::png, current);

#undef HQ_SCREENSHOT_EXT
#undef HQ_SCREENSHOT_PFX
}

void game::collect_events() {
    m_polled.clear();

    // Process global events here...
    while (systems.events.poll(m_eventHandler)) {
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
