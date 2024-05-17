#include <quest/console.hpp>

#include <random>

#include <halcyon/utility/locks.hpp>

#include <quest/constants.hpp>
#include <quest/helpers.hpp>

using namespace HQ;

namespace HQ::consts {
    constexpr std::string_view font_path { "assets/Ubuntu Mono.ttf" }, pfx_text { "root@Console ~ %" };

    constexpr hal::pixel_t padding_left { 20 }, padding_right { 20 };

    constexpr hal::color input_color { hal::palette::white },
        ph_color { 0x808080 },
        pfx_color { hal::palette::green };

    constexpr hal::coord_point offset { 10, 10 };

    constexpr hal::font::render_type text_render_type { hal::font::render_type::blended };

    constexpr bool clear_on_close { false };
}

shuffle_bag::shuffle_bag()
    : m_arr {
        "[enter command here]",
        "[be not afraid]",
        "[food for thought]",
        "[waiting for user input]",
        "[rm -rf / --no-preserve-root]",
        "[at your service]",
        "[not POSIX compliant]",
        "[made with Halcyon]",
        "[start typing, please]",
        "[may 5th, 2022]",
        "[commands not included]",
        "[who needs documentation]",
        "[your turn]",
        "[segfaulting since 2021]",
        "[quoth the raven, nevermore]",
        "[sudo pacman -S neofetch]",
        "[redacted]",
        "[is anyone there?]",
        "[licensed under the WTFPL]",
        "[openest source]",
        "[watch?v=lo5cG0FhWro]",
        "[no man page here, sorry]",
        "[womp womp]",
        "[40.7736N, 29.7564W]",
        "[you can sudo, trust me]"
    }
    , m_index { num_texts } {
}

const char* shuffle_bag::next() {
    if (m_index == num_texts) { // Need to refresh.
        HAL_PRINT("<Console> Shuffling placeholder text...");
        std::shuffle(std::begin(m_arr), std::end(m_arr), std::mt19937_64 { std::random_device {}() });
        m_index = 0;
    }

    return m_arr[m_index++];
}

console::console(hal::renderer& rnd, hal::ttf::context& ttf)
    : m_font { find_sized_font(ttf, consts::font_path, static_cast<hal::pixel_t>(rnd.size().y * 0.045)) }
    , m_glyphSize { static_cast<hal::coord_point>(m_font.size_text(" ")) }
    , m_texBegin { static_cast<hal::pixel_t>(consts::offset.x + m_font.size_text(consts::pfx_text).x + consts::padding_left) }
    , m_wrap { rnd.size().x - m_texBegin - consts::padding_right }
    , m_active { false }
    , m_repaint { false } {
}

void console::draw(hal::renderer& rnd) {
    hal::lock::color lock { rnd, { hal::palette::black, 128 } };
    rnd.fill();

    if (m_repaint) {
        m_repaint = false;
        repaint(rnd);
    }

    auto pos = consts::offset;

    rnd.render(m_pfx).to(pos)();

    pos.x += m_texBegin;
    rnd.render(m_tex).to(pos)();

    lock.set({ hal::palette::white, 128 });

    const hal::coord_rect r(
        pos.x + m_glyphSize.x * m_field.cursor, pos.y,
        m_glyphSize.x,
        m_glyphSize.y);

    rnd.fill(r);
}

bool console::process(hal::keyboard::key k, hal::keyboard::mod_state m, const hal::proxy::clipboard& c) {
    m_repaint = m_field.process(k, m, c) != 0;
    return k == hal::keyboard::key::F1;
}

void console::process(std::string_view inp) {
    m_field.process(inp); // changes are always made here
    m_repaint = true;
}

void console::show(hal::renderer& rnd) {
    m_repaint = true;

    m_pfx = rnd.make_texture(m_font.render(consts::pfx_text).fg(consts::pfx_color)(consts::text_render_type));

    m_active = true;
}

void console::hide() {
    m_pfx.reset();
    m_tex.reset();

    if constexpr (consts::clear_on_close) {
        m_field.text.clear();
    }

    m_active = false;
}

bool console::active() {
    return m_active;
}

void console::repaint(hal::renderer& rnd) {
    hal::surface text;

    if (m_field.text.empty()) {
        text = m_font.render(m_placeholders.next())
                   .fg(consts::ph_color)(consts::text_render_type);
    } else {
        text = m_font.render(m_field.text)
                   .wrap(m_wrap)
                   .fg(consts::input_color)(consts::text_render_type);
    }

    m_tex = rnd.make_texture(text);
}