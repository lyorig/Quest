#include <quest/scene/manager.hpp>

#include <quest/game.hpp>

using namespace hq::scene;

namespace {
    template <typename Tuple, typename Func, std::size_t... Is>
    consteval bool tuple_iterable(std::index_sequence<Is...>) {
        return (std::is_invocable_r_v<void, Func, std::tuple_element_t<Is, Tuple>&> && ...);
    }

    template <typename F, typename Ret, typename Tuple>
    concept tuple_iter_func_r = tuple_iterable<Tuple, F>(std::make_index_sequence<std::tuple_size_v<Tuple>>());

    template <std::size_t N, typename T, tuple_iter_func_r<void, T> F>
    void forward_tuple_bounded_iter(T& tuple, std::uint8_t end, F func) {
        if (N >= end) {
            func(std::get<N>(tuple));
        }

        if constexpr (N != std::tuple_size_v<T> - 1) {
            forward_tuple_bounded_iter<N + 1>(tuple, end, func);
        }
    }

    // Call a function for every element in a tuple up until the `end` index.
    template <typename T, typename F>
    void forward_tuple_bounded(T& tuple, std::uint8_t end, F func) {
        forward_tuple_bounded_iter<0>(tuple, end, func);
    }

    template <std::size_t N, typename T, tuple_iter_func_r<bool, T> F>
    void reverse_tuple_bounded_iter(T& tuple, std::uint8_t end, F func) {
        if (N < end) {
            return;
        }

        if (!func(std::get<N>(tuple))) {
            return;
        }

        if constexpr (N != 0) {
            reverse_tuple_bounded_iter<N - 1>(tuple, end, func);
        }
    }

    // Call a function for every element in a tuple in reverse order
    // up until the `end` index.
    template <typename T, tuple_iter_func_r<bool, T> F>
    void reverse_tuple_bounded(T& tuple, std::uint8_t end, F func) {
        reverse_tuple_bounded_iter<std::tuple_size_v<std::remove_cvref_t<T>> - 1>(tuple, end, func);
    }

    template <std::size_t N, typename T, tuple_iter_func_r<bool, T> F>
    void reverse_tuple_cond_iter(T& tuple, F func) {
        if (!func(std::get<N>(tuple))) {
            return;
        }

        if constexpr (N != 0) {
            reverse_tuple_cond_iter<N - 1>(tuple, func);
        }
    }

    // Call a function for every element in a tuple in reverse order
    // until the end is reached or `func` returns `false`.
    template <typename T, tuple_iter_func_r<bool, T> F>
    void reverse_tuple_cond(T& tuple, F func) {
        reverse_tuple_cond_iter<std::tuple_size_v<std::remove_cvref_t<T>> - 1>(tuple, func);
    }
}

manager::manager(game& g)
    : m_tuple { g, g }
    , m_cProcess { 0 }
    , m_cUpdate { 0 }
    , m_cDraw { 0 } {
}

void manager::update(game& g) {
    for_each(flag::enable_process, m_cProcess, [&](interface auto& scene) { scene.process(g); });
    for_each(flag::enable_update, m_cUpdate, [&](interface auto& scene) { scene.update(g); });

    g.atlas_pack();
    for_each(flag::enable_draw, m_cDraw, [&](interface auto& scene) { scene.draw(g); });
}

template <typename F>
void manager::for_each(flag enabler, std::uint8_t end, F func) {
    forward_tuple_bounded(m_tuple, end, [&](interface auto& scene) {
        flag_bitmask flg { scene.flags };

        if (flg[enabler]) {
            func(scene);

            // Check which flags changed.
            flg ^= scene.flags;

            for (const flag f : { flag::block_process, flag::block_update, flag::block_draw }) {
                if (flg[f]) {
                    update_cached(f);
                }
            }
        }
    });
}

void manager::update_cached(flag f) {
    std::uint8_t iter { find_last_with_flag(f) };

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

std::uint8_t manager::find_last_with_flag(flag m) const {
    std::uint8_t ret { std::tuple_size_v<decltype(m_tuple)> };

    // This works for some reason. I don't mind.
    reverse_tuple_cond(m_tuple, [&](const interface auto& scene) {
        --ret;

        if (scene.flags[m]) {
            return false;
        }

        return true;
    });

    return ret;
}
