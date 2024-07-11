#include <quest/scene/console.hpp>

#include <random>

#include <halcyon/utility/locks.hpp>

#include <quest/constants.hpp>
#include <quest/game.hpp>
#include <quest/helpers.hpp>

using namespace hq::scene;

namespace hq::consts {
    constexpr std::string_view font_path { "assets/Ubuntu Mono.ttf" }, prefix_text { "root@Console ~ %" };

    constexpr hal::coord_t padding_pc { 0.015_crd };

    constexpr hal::color input_color { hal::palette::white },
        background_color { hal::palette::black, 128 },
        cursor_color { hal::palette::white, 128 },
        placeholder_color { 0x808080 },
        prefix_color { hal::palette::green };

    constexpr hal::coord::point text_offset { 10, 10 };

    constexpr hal::font::render_type text_render_type { hal::font::render_type::blended };

    constexpr bool clear_on_close { false };

    constexpr delta_t cursor_blink_time { 0.5 };

    constexpr std::size_t desired_max_chars { 128 };
}

console::shuffle_bag::shuffle_bag()
    : m_texts {
        "[enter command here]",
        "[be not afraid]",
        "[food for thought]",
        "[waiting for user input]",
        "[rm -rf / --no-preserve-root]",
        "[at your service]",
        "[not POSIX compliant]",
        "[made with Halcyon]",
        "[start typing, please]",
        "[non-euclidean interface]",
        "[commands not included]",
        "[who needs documentation]",
        "[your turn]",
        "[segfaulting since 2021]",
        "[quoth the raven, nevermore]",
        "[sudo pacman -S lyofetch]",
        "[redacted]",
        "[is anyone there?]",
        "[licensed under the WTFPL]",
        "[openest source]",
        "[watch?v=lo5cG0FhWro]",
        "[no man page here, sorry]",
        "[womp womp]",
        "[40.7736N, 29.7564W]",
        "[sudo deez nuts]",
        "[docker? I barely know 'er!]",
        "[running out of time]",
        "[not actually random]",
        "[see you again]",
        "[forget me not]",
        "[one big CVE]"
    }
    , m_index { num_texts } {
}

const char* console::shuffle_bag::next() {
    if (m_index == num_texts) { // Need to refresh.
        HAL_PRINT("<Console> Shuffling placeholder indices...");
        std::shuffle(std::begin(m_texts), std::end(m_texts), std::mt19937_64 { std::random_device {}() });
        m_index = 0;
    }

    return m_texts[m_index++];
}

console::console(game& g)
    : base { flags::stop_process }
    , m_font { find_sized_font(g.ttf, consts::font_path, static_cast<hal::pixel_t>(g.renderer.size().y * 0.045)) }
    , m_padding { g.renderer.size().x * consts::padding_pc }
    , m_texBegin { consts::text_offset.x + m_font.size_text(consts::prefix_text).x + m_padding }
    , m_wrap { static_cast<hal::pixel_t>(g.renderer.size().x - m_texBegin - m_padding) }
    , m_outline { { m_texBegin, consts::text_offset.y }, m_font.size_text(" ") }
    , m_maxChars { static_cast<hal::u16>(std::min(g.renderer.info().max_texture_size().x / m_outline.size.x, static_cast<hal::coord_t>(consts::desired_max_chars))) }
    , m_lineChars { static_cast<hal::u8>(m_wrap / m_outline.size.x) }
    , m_active { false }
    , m_repaint { false }
    , m_cursorVis { true } {
    m_wrap -= m_wrap % static_cast<hal::pixel_t>(m_outline.size.x);

    HAL_PRINT("<Console> Initialized. Max ", m_maxChars, " chars.");
}

action console::process(game& g) {
    for (const auto& evt : g.polled()) {
        switch (evt.kind()) {
            using enum hal::event::type;

        case key_pressed:
            if (process(evt.keyboard().key(), g.video)) {
                switch_state(g);
            };
            break;

        case text_input:
            process(evt.text_input().text());
            break;

        default:
            break;
        }
    }

    return action::nothing;
}

void console::update(game& g) {
    m_cursorTime += g.delta();

    if (m_cursorTime >= consts::cursor_blink_time) {
        m_cursorTime -= consts::cursor_blink_time;
        m_cursorVis = !m_cursorVis;
    }
}

void console::draw(hal::renderer& rnd) {
    hal::lock::color lock { rnd, consts::background_color };
    rnd.fill();

    if (m_repaint) {
        m_repaint = false;
        repaint(rnd);
    }

    rnd.render(m_pfx).to(consts::text_offset)();

    hal::coord::point where { m_texBegin, consts::text_offset.y };
    hal::coord::rect  crd;
    crd.size.x = static_cast<hal::coord_t>(std::min(m_tex.size().x, m_wrap));
    crd.size.y = m_outline.size.y;

    using namespace hal::literals;

    for (; m_tex.size().x - crd.pos.x > 0;
         where.y += m_outline.size.y, crd.pos.x += m_wrap, crd.size.x = std::min<hal::coord_t>(m_tex.size().x - crd.pos.x, static_cast<hal::coord_t>(m_wrap))) {
        rnd.render(m_tex).from(crd).to(where)();
    }

    if (m_cursorVis) {
        lock.set(consts::cursor_color);
        rnd.fill(m_outline);
    }
}

void console::activate(game& g) {
    m_repaint = true;
    m_active  = true;

    m_cursorTime = 0.0;
    m_cursorVis  = true;

    m_pfx = g.renderer.make_texture(m_font.render(consts::prefix_text).fg(consts::prefix_color)(consts::text_render_type));
}

void console::deactivate() {
    m_pfx.reset();
    m_tex.reset();

    if constexpr (consts::clear_on_close) {
        m_field.text.clear();
    }

    m_active = false;
}

std::string_view console::name() const {
    return "Console";
}

bool console::process(hal::keyboard::key k, const hal::proxy::video& vid) {
    switch (k) {
        using enum hal::keyboard::key;

    case F1:
        return true;

    default: {
        const field::op op { m_field.process(k, vid) };

        if (m_field.text.size() > m_maxChars) {
            m_field.trim(m_maxChars);
        }

        switch (op) {
            using enum field::op;

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

void console::process(std::string_view inp) {
    m_repaint = m_field.process(inp);

    if (m_field.text.size() > m_maxChars) {
        m_field.trim(m_maxChars);
    }

    m_repaint = true;

    set_cursor();
}

bool console::active() {
    return m_active;
}

void console::repaint(hal::renderer& rnd) {
    hal::surface text;

    if (m_field.text.empty()) {
        text = m_font.render(m_placeholders.next())
                   .fg(consts::placeholder_color)(consts::text_render_type);
    } else {
        text = m_font.render(m_field.text)
                   .fg(consts::input_color)(consts::text_render_type);

        if (text.size().x > rnd.info().max_texture_size().x) {
        }
    }

    m_tex = rnd.make_texture(text);
}

void console::set_cursor() {
    m_outline.pos.x = m_texBegin + (m_field.cursor % m_lineChars) * m_outline.size.x;
    m_outline.pos.y = consts::text_offset.y + m_outline.size.y * static_cast<hal::u8>(m_field.cursor / m_lineChars);

    m_cursorTime = consts::cursor_blink_time / 2.0;
    m_cursorVis  = true;
}