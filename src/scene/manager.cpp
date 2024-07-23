#include <quest/scene/manager.hpp>

#include <quest/game.hpp>

using namespace hq::scene;

manager::manager()
    : m_begin { m_scenes.begin() }
    , m_cachedProcess { m_scenes.begin() }
    , m_cachedUpdate { m_scenes.begin() }
    , m_cachedDraw { m_scenes.begin() } {
}
// Process -> update -> draw.
void manager::update(game& g) {
    for (auto it = m_begin; it != m_scenes.end(); ++it) {
        scene::base& obj { **it };

        flag_bitmask flg { obj.flags };

        // Process:
        if (obj.flags[flag::enable_process] && it >= m_cachedProcess) {
            obj.process(g);
        }

        // Update:
        if (obj.flags[flag::enable_update] && it >= m_cachedUpdate) {
            obj.update(g);
        }

        // Draw:
        if (obj.flags[flag::enable_draw] && it >= m_cachedDraw) {
            obj.draw(g);
        }

        flg ^= obj.flags;

        // TODO: This doesn't work, like, at all. I should rethink the architecture of this whole thing.
        for (const auto f : { flag::block_process, flag::block_update, flag::block_draw }) {
            if (flg[f]) {
                update_cached(f);
            }
        }
    }
}

void manager::add(base_up&& scn) {
    m_scenes.push_back(std::move(scn));

    const flag_bitmask f { m_scenes.back()->flags };

    if (f[flag::block_process]) {
        m_cachedProcess = m_scenes.end() - 1;
    }

    if (f[flag::block_update]) {
        m_cachedUpdate = m_scenes.end() - 1;
    }

    if (f[flag::block_draw]) {
        m_cachedDraw = m_scenes.end() - 1;
    }
}

manager::const_iterator manager::find_last_with_flags(flag_bitmask m) const {
    const_iterator it { m_scenes.end() - 1 };

    for (; it != m_scenes.begin(); --it) {
        if ((*it)->flags[m]) {
            break;
        }
    }

    return it;
}

void manager::update_cached(flag f) {
    const_iterator iter { find_last_with_flags(f) };

    switch (f) {
    case flag::block_process:
        m_cachedProcess = iter;
        break;

    case flag::block_update:
        m_cachedUpdate = iter;
        break;

    case flag::block_draw:
        m_cachedDraw = iter;
        break;

    default:
        std::unreachable();
    }

    m_begin = std::min({ m_cachedProcess, m_cachedUpdate, m_cachedDraw });
}
