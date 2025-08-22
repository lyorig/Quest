#define _CRT_SECURE_NO_WARNINGS

#include <halcyon/hint.hpp>
#include <halcyon/image.hpp>
#include <halcyon/system.hpp>

#include <halcyon/utility/strutil.hpp>
#include <halcyon/utility/timer.hpp>

#include <quest/game.hpp>
#include <quest/helpers.hpp>

#include <charconv>
#include <filesystem>

using namespace hq;

namespace {
    hal::window create_window(hal::proxy::video v) {
        return { v, "HalQuest [Regent]", { 1280, 720 } };
    }

    hal::renderer create_renderer(hal::ref<hal::window> wnd, args a) {
        hal::renderer rnd { wnd };
        rnd.blend(hal::blend_mode::alpha);
        rnd.vsync(!a["--no-vsync"]);

        return rnd;
    }
}

args::args(int argc, char** argv)
    : m_span { const_cast<const char**>(argv), static_cast<std::size_t>(argc) } {
}

args::pos_t args::size() const {
    return m_span.size();
}

args::info args::operator[](std::string_view what) const {
    for (pos_t i { 0 }; i < static_cast<int>(m_span.size()); ++i) {
        if (what == m_span[i]) {
            return { i };
        }
    }

    return { info::invalid_pos() };
}

hal::c_string args::operator[](pos_t p) const {
    return m_span[p];
}

game::game(args a) try
    : systems{}
    , window{ create_window(systems) }
    , renderer{ create_renderer(window, a) }
    , scenes{ *this }
    , timescale{ 1.0 }
    , running{ true }
    , screenshot{ false } {
    using hal::debug::severity::init;

    HAL_PRINT(init, "<Game> Initialized.");
    HAL_PRINT("<Game> Base path: ", loader.base());
    HAL_PRINT("<Game> Pref path: ", hal::fs::pref_path("IdleFour", "HalodaQuest"));
} catch (hal::exception e) {
    HAL_PRINT("Exception raised: ", e.with_error());
}

void game::main_loop() {
    hal::timer timer;

    while (running) {
        m_delta = timer.get();
        timer.reset();

        collect_events();

        scenes.update(*this);

        if (screenshot) {
            take_screenshot();
            screenshot = false;
        }

        atlas.debug_draw(renderer, { 400, 200 }, hal::colors::blue, hal::colors::weezer_blue);

        renderer.present();
    }
}

const game::event_vector& game::polled() const {
    return m_polled;
}

delta_t game::delta() const {
    return m_delta * timescale;
}

texture_atlas::id_t game::atlas_add(hal::surface s) {
    return atlas.add(renderer, std::move(s));
}

void game::atlas_replace(texture_atlas::id_t id, hal::surface s) {
    atlas.replace(id, renderer, std::move(s));
}

void game::atlas_pack() {
    atlas.pack(renderer);
}

texture_atlas_copyer game::atlas_draw(texture_atlas::id_t id) {
    return atlas.draw(id, renderer);
}

void game::take_screenshot() const {
    namespace fs = std::filesystem;

    hal::surface s { renderer.read_pixels() };

#define HQ_SCREENSHOT_PFX "screenshot-"
#define HQ_SCREENSHOT_EXT ".png"

    constexpr std::size_t
        digits { lim<decltype(digits)>::digits10 },
        pfxlen { hal::strlen(HQ_SCREENSHOT_PFX) }, extlen { hal::strlen(HQ_SCREENSHOT_EXT) },
        max_ss_attempts { lim<decltype(max_ss_attempts)>::max() };

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

    // Rust users can suck it, this is true paranoia.
    if (i == max_ss_attempts) {
        HAL_PRINT(hal::debug::severity::warning, "Exhausted numbering of screenshots. Aborting save.");
        return;
    }

    hal::image::save::png(s, current);

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
                break;
            }

            break;

        default:
            break;
        }

        m_polled.push_back(m_eventHandler);
    }
}
