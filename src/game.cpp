#define _CRT_SECURE_NO_WARNINGS

#include <halcyon/utility/timer.hpp>

#include <quest/game.hpp>
#include <quest/helpers.hpp>

#include <charconv>
#include <filesystem>

using namespace hq;

template <typename T>
using lims = std::numeric_limits<T>;

args::args(int argc, char** argv)
    : m_span { const_cast<const char**>(argv), static_cast<std::size_t>(argc) } {
    HAL_ASSERT(argc <= m_checked.size(), "Too many arguments (max. ", m_checked.size(), ", actual ", argc, ')');
}

bool args::operator[](std::string_view what) const {
    for (std::size_t i { 0 }; m_checked.test(i) && i < m_span.size(); ++i) {
        if (what == m_span[i]) {
            m_checked.set(i);
            return true;
        }
    }

    return false;
}

game::game(args a)
    : img { hal::image::init_format::jpg }
    , window { video, "HalQuest", hal::tag::fullscreen }
    , renderer { window, { hal::renderer::flag::accelerated, cond_enum(hal::renderer::flag::vsync, !a["--no-vsync"]) } }
    , scenes { *this }
    , atlas { renderer, renderer.size() / 2 }
    , timescale { 1.0 }
    , running { true }
    , screenshot { false } {
    renderer.blend(hal::blend_mode::alpha);
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
    HAL_DEBUG_TIMER(tmr);

    hal::surface s { renderer.read_pixels() };

#define HQ_SCREENSHOT_PFX "screenshot-"
#define HQ_SCREENSHOT_EXT ".png"

    constexpr std::size_t
        digits { std::numeric_limits<decltype(digits)>::digits10 },
        pfxlen { hal::strlen(HQ_SCREENSHOT_PFX) }, extlen { hal::strlen(HQ_SCREENSHOT_EXT) };

    char filename[pfxlen + digits + extlen + 1] { HQ_SCREENSHOT_PFX };

    const std::filesystem::path directory { "screenshots" };

    std::filesystem::path current;

    if (!std::filesystem::is_directory(directory)) {
        std::filesystem::create_directory(directory);
    }

    std::size_t i { 0 };

    do {
        std::strcpy(std::to_chars(filename + pfxlen, std::end(filename) - 1, i++).ptr, HQ_SCREENSHOT_EXT);
    } while (std::filesystem::exists(current = directory / filename)
        && i != lims<decltype(i)>::max());

    // Rust users can suck it, this is true paranoia
    if (i == lims<decltype(i)>::max()) [[unlikely]] {
        HAL_PRINT(hal::debug::severity::warning, "Exhausted numbering of screenshots. Aborting save.");
        return;
    }

    img.save(s, hal::image::save_format::png, current);

    HAL_PRINT("<Game> Saved ", current.filename().string(), " in ", tmr);

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

            case enter: {
                hal::surface s { { 200, 200 } };
                s.fill(hal::palette::cyan);
                atlas.add(renderer, s);
            } break;

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

    renderer.draw(atlas.tex)();
}
