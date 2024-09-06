#include <quest/scene/manager.hpp>

#include <quest/game.hpp>

using namespace hq::scene;

namespace {
    template <std::size_t N, typename T, typename F>
    void forward_tuple_iter(T& tuple, F func) {
        func(std::get<N>(tuple));
        if constexpr (N != std::tuple_size_v<T> - 1) {
            forward_tuple_iter<N + 1>(tuple, func);
        }
    }

    template <typename T, typename F>
    void forward_tuple(T& tuple, F func) {
        forward_tuple_iter<0>(tuple, func);
    }

    template <std::size_t N, typename T, typename F>
    void reverse_tuple_iter(T& tuple, F func) {
        func(std::get<N>(tuple));
        if constexpr (N != 0) {
            reverse_tuple_iter<N - 1>(tuple, func);
        }
    }

    template <typename T, typename F>
    void reverse_tuple(T& tuple, F func) {
        reverse_tuple_iter<std::tuple_size_v<T> - 1>(tuple, func);
    }
}

manager::manager(game& g)
    : m_tuple {}
    , m_curr { 0 }
    , m_cProcess { 0 }
    , m_cUpdate { 0 }
    , m_cDraw { 0 } {
}
// Process -> update -> draw.
void manager::update(game& g) {
    m_curr = 0;
    forward_tuple(m_tuple, [&](auto&& obj) { update_one(obj, g); });
}

template <typename T>
void manager::update_one(T& obj, game& g) {
    flag_bitmask flg { obj.flags };

    // Process:
    if (obj.flags[flag::enable_process] && m_curr >= m_cProcess) {
        obj.process(g);
    }

    // Update:
    if (obj.flags[flag::enable_update] && m_curr >= m_cUpdate) {
        obj.update(g);
    }

    // Draw:
    if (obj.flags[flag::enable_draw] && m_curr >= m_cDraw) {
        obj.draw(g);
    }

    // Get flag diff
    flg ^= obj.flags;

    for (const flag f : { flag::block_process, flag::block_update, flag::block_draw }) {
        if (flg[f]) {
            update_cached(f);
        }
    }

    ++m_curr;
}

void manager::update_cached(flag f) {
    hal::u8 iter { find_last_with_flag(f) };

    switch (f) {
    case flag::block_process:
        m_cProcess = iter;
        break;

    case flag::block_update:
        m_cUpdate = iter;
        break;

    case flag::block_draw:
        m_cDraw = iter;
        break;

    default:
        std::unreachable();
    }
}

hal::u8 manager::find_last_with_flag(flag m) const {
    hal::u8 ret { 0 };

    reverse_tuple(m_tuple, [&](const auto& obj) {if (obj.flags[m]) {++ret;} });

    return ret;
}
