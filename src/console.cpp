#include <quest/console.hpp>

#include <halcyon/utility/locks.hpp>

using namespace HQ;

namespace consts {
    constexpr std::string_view font_path { "assets/pixelmix.ttf" }, pfx_text { "root@Console ~ %" };
    constexpr hal::font::pt_t  font_size { 32 };

    constexpr hal::pixel_t padding_left { 20 }, padding_right { 20 };

    constexpr hal::color input_color { hal::palette::white },
        ph_color { 0x808080 },
        bg_color { hal::palette::black, 128 },
        pfx_color { hal::palette::green };

    constexpr hal::coord_point offset { 10, 10 };

    constexpr hal::font::render_type text_render_type { hal::font::render_type::solid };

    constexpr bool clear_on_close { false };
}

shuffle_bag::shuffle_bag()
    : m_arr {
        "[enter command here]",
        "[be not afraid]",
        "[food for thought]",
        "[waiting for user input]",
        "rm -rf / --no-preserve-root",
        "[at your service]",
        "[not POSIX compliant]",
        "[made with Halcyon]",
        "[start typing, please]",
        "[waiting for 5/5/2022]",
        "[commands not included]",
        "[who needs documentation]",
        "[your turn]",
        "[segfaulting since 2021]",
        "[quoth the raven, nevermore]",
        "[sudo pacman -S neofetch]",
        "[rand() is a bad RNG]"
    }
    , m_index { num_texts } {
}

const char* shuffle_bag::next() {
    if (m_index == num_texts) { // Need to refresh.
        HAL_PRINT("Shuffling console splash texts...");
        std::shuffle(std::begin(m_arr), std::end(m_arr), std::mt19937_64 { std::random_device {}() });
        m_index = 0;
    }

    return m_arr[m_index++];
}

console::console(hal::renderer& rnd, hal::ttf::context& ttf)
    : m_font { ttf.load(hal::access(consts::font_path), consts::font_size) }
    , m_texBegin { static_cast<hal::pixel_t>(consts::offset.x + m_font.size_text(consts::pfx_text).x + consts::padding_left) }
    , m_wrap { rnd.size().x - m_texBegin - consts::padding_right }
    , m_repaint { false } {
}

void console::draw(hal::renderer& rnd) {
    hal::lock::color lock { rnd, consts::bg_color };
    rnd.fill_target();

    if (m_repaint) {
        m_repaint = false;
        repaint(rnd);
    }

    auto pos = consts::offset;

    rnd.draw(m_pfx).to(pos)();

    pos.x += m_texBegin;
    rnd.draw(m_tex).to(pos)();
}

bool console::process(hal::keyboard::key k, hal::keyboard::mod_state m, const hal::proxy::clipboard& c) {
    m_repaint = m_field.process(k, m, c) != 0;
    return k == hal::keyboard::key::F1;
}

void console::process(char ch) {
    m_field.process(ch);
    m_repaint = true;
}

void console::show(hal::renderer& rnd) {
    m_repaint = true;

    m_pfx = rnd.make_texture(m_font.render(consts::pfx_text).fg(consts::pfx_color)(consts::text_render_type));

    m_field.show();
}

void console::hide() {
    m_pfx.reset();
    m_tex.reset();

    if constexpr (consts::clear_on_close) {
        m_field.text.clear();
    }

    m_field.hide();
}

bool console::active() {
    return m_field.has_focus();
}

bool console::toggle(hal::renderer& rnd) {
    if (m_field.toggle()) {
        show(rnd);
        return true;
    } else {
        hide();
        return false;
    }
}

void console::repaint(hal::renderer& rnd) {
    hal::surface text;

    if (m_field.text.empty()) {
        text = m_font.render(m_splash.next())
                   .fg(consts::ph_color)(consts::text_render_type);
    } else {
        text = m_font.render(m_field.text + '|')
                   .wrap(m_wrap)
                   .fg(consts::input_color)(consts::text_render_type);
    }

    m_tex = rnd.make_texture(text);
}