#include <quest/scene/console.hpp>

#include <halcyon/utility/guard.hpp>

#include <quest/commands/defs.hpp>
#include <quest/constants.hpp>
#include <quest/game.hpp>
#include <quest/helpers.hpp>

#include <numeric>
#include <random>

using namespace hq;
using namespace hq::scene;
using namespace hal::literals;

namespace {
    constexpr std::string_view PREFIX_TEXT { "raine1@Arctic~ %" };

    constexpr hal::coord_t PADDING_PC { 0.015_crd };

    constexpr hal::color
        INPUT_COLOR { hal::colors::white },
        BACKGROUND_COLOR { hal::colors::black, 128 },
        CURSOR_COLOR { hal::colors::white, 128 },
        PLACEHOLDER_COLOR { 0x808080 },
        PREFIX_COLOR { hal::colors::green };

    constexpr hal::coord::point TEXT_OFFSET { 10, 10 };
    constexpr delta_t           CURSOR_BLINK_TIME { 0.5 };
    constexpr std::size_t       DESIRED_MAX_CHARS { 128 };
    constexpr flag_bitmask      ENABLERS { flag::enable_draw, flag::enable_update, flag::block_process };

    cmd::status cmd_build(HQ_CMD_PARAMS) {
        static_cast<void>(g);

        out << "Quest v0.1 built @ " __TIMESTAMP__ << '\n';
        return cmd::status::ok;
    }

    cmd::status cmd_exit(HQ_CMD_PARAMS) {
        static_cast<void>(out);

        g.running = false;
        return cmd::status::ok;
    }

    struct {
        std::string_view                               name;
        hal::func_ptr<cmd::status, HQ_CMD_PARAM_TYPES> cmd;
    } constexpr commands[] {
        { "build", cmd_build },
        { "exit", cmd_exit }
    };
}

console::console(game& g)
    : base { flag::enable_process }
    , m_placeholderIndex { std::size(console_placeholders) }
    , m_font { find_sized_font(g, "assets/Ubuntu Mono.ttf", static_cast<hal::pixel_t>(g.renderer.size()->y * 0.045)) }
    , m_padding { g.renderer.size()->x * PADDING_PC }
    , m_texBegin { TEXT_OFFSET.x + size_text(m_font, PREFIX_TEXT).x + m_padding }
    , m_wrap { static_cast<hal::pixel_t>(g.renderer.size()->x - m_texBegin - m_padding) }
    , m_outline { { m_texBegin, TEXT_OFFSET.y }, size_text(m_font, " ") }
    , m_maxChars { static_cast<std::uint16_t>(std::min(g.renderer.props().max_texture_size() / m_outline.size.x, static_cast<hal::coord_t>(DESIRED_MAX_CHARS))) }
    , m_lineChars { static_cast<std::uint8_t>(m_wrap / m_outline.size.x) }
    , m_repaint { false }
    , m_cursorVis { true } {
    m_wrap -= m_wrap % static_cast<hal::pixel_t>(m_outline.size.x);

    std::iota(std::begin(m_placeholderOrder), std::end(m_placeholderOrder), 0);

    HAL_WARN_IF(!m_font.mono(), "<Console> \"", m_font.family(), "\" is not a mono font. Character spacing will probably be incorrect.");
}

void console::process(game& g) {
    for (const auto& evt : g.polled()) {
        switch (evt.kind()) {
            using enum hal::event::type;

        case key_pressed:
            if (process(g, evt.keyboard().key(), g.systems)) {
                if (flags.all(ENABLERS)) {
                    flags -= ENABLERS;
                    deactivate(g);
                } else {
                    flags += ENABLERS;
                    activate(g);
                }
            };
            break;

        case text_input:
            if (is_active()) {
                process(evt.text_input().text());
            }

            break;

        default:
            break;
        }
    }
}

void console::activate(game& g) {
    m_cursorTime = 0.0;
    m_cursorVis  = true;

    m_prefix = g.atlas_add(m_font.render_blended(PREFIX_TEXT, PREFIX_COLOR));
    m_line   = g.atlas_add(make_placeholder());

    g.systems.events.text_input_start(g.window);
}

void console::deactivate(game& g) {
    g.systems.events.text_input_stop(g.window);

    g.atlas.free(m_prefix);
    g.atlas.free(m_line);

    m_field.clear();
    set_cursor();
}

bool console::process(game& g, hal::keyboard::key k, hal::proxy::video vid) {
    switch (k) {
        using enum hal::keyboard::key;

    case F1:
        return true;

    case F12:
        if (flags[ENABLERS]) {
            g.screenshot = false; // Intercept screenshot requests.
        }
        break;

    default: {
        const field::action op { m_field.process(k, vid) };

        if (m_field.text.size() > m_maxChars) {
            m_field.trim(m_maxChars);
        }

        switch (op) {
            using enum field::action;

        case text_added:
        case text_removed:
            m_repaint = true;
            // Intentional fallthrough.

        case cursor_moved:
            set_cursor();
            break;

        default:
            break;
        }
    } break;
    };

    return false;
}

void console::update(game& g) {
    m_cursorTime += g.delta();

    if (m_cursorTime >= CURSOR_BLINK_TIME) {
        m_cursorTime -= CURSOR_BLINK_TIME;
        m_cursorVis = !m_cursorVis;
    }
}

void console::draw(game& g) {
    hal::lref<hal::renderer> rnd { g.renderer };

    hal::guard::color lock { rnd, BACKGROUND_COLOR };
    rnd->fill();

    g.atlas_draw(m_prefix).to(TEXT_OFFSET).render();

    hal::coord::point where { m_texBegin, TEXT_OFFSET.y };

    hal::coord::rect   crd;
    const hal::pixel_t m_line_size_x = g.atlas.area(m_line).size.x;

    crd.size.x = static_cast<hal::coord_t>(std::min(m_line_size_x, m_wrap));
    crd.size.y = m_outline.size.y;

    using namespace hal::literals;

    for (; m_line_size_x - crd.pos.x > 0;
        where.y += m_outline.size.y,
        crd.pos.x += m_wrap,
        crd.size.x = std::min<hal::coord_t>(m_line_size_x - crd.pos.x, static_cast<hal::coord_t>(m_wrap))) {
        g.atlas_draw(m_line).from(crd).to(where).render();
    }

    if (m_cursorVis) {
        lock.set(CURSOR_COLOR);
        rnd->fill(m_outline);
    }

    if (m_repaint) {
        m_repaint = false;
        g.atlas_replace(m_line, make_line());
    }
}

void console::process(std::string_view inp) {
    m_repaint = m_field.process(inp);

    if (m_field.text.size() > m_maxChars) {
        m_field.trim(m_maxChars);
    }

    set_cursor();
}

bool console::is_active() {
    return flags[ENABLERS];
}

void console::write(std::string_view data) {
    // TODO
}

hal::surface console::make_line() {
    if (m_field.text.empty()) {
        return make_placeholder();
    } else {
        return m_font.render_blended(m_field.text, INPUT_COLOR);
    }
}

hal::surface console::make_placeholder() {
    return m_font.render_blended(generate_placeholder(), PLACEHOLDER_COLOR);
}

std::string_view console::generate_placeholder() {
    if (m_placeholderIndex == std::size(console_placeholders)) { // Need to refresh.
        HAL_PRINT("<Console> Shuffling placeholder indices...");
        std::shuffle(std::begin(m_placeholderOrder), std::end(m_placeholderOrder), std::mt19937_64 { std::random_device {}() });
        m_placeholderIndex = 0;
    }

    return console_placeholders[m_placeholderOrder[m_placeholderIndex++]];
}

void console::set_cursor() {
    m_outline.pos.x = m_texBegin + (m_field.cursor % m_lineChars) * m_outline.size.x;
    m_outline.pos.y = TEXT_OFFSET.y + m_outline.size.y * static_cast<std::uint8_t>(m_field.cursor / m_lineChars);

    m_cursorTime = CURSOR_BLINK_TIME / 2.0;
    m_cursorVis  = true;
}
