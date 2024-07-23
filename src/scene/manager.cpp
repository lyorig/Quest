#include <quest/scene/manager.hpp>

#include <quest/game.hpp>

using namespace hq::scene;

manager::manager()
    : m_cachedProcess { m_scenes.begin() }
    , m_cachedUpdate { m_scenes.begin() }
    , m_cachedDraw { m_scenes.begin() } {
}
// Process -> update -> draw.
void manager::update(game& g) {
    for (auto it = m_scenes.begin(); it != m_scenes.end(); ++it) {
        scene::base& obj { **it };

        const flag_bitmask oldf { obj.flags };

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

        const flag_bitmask diff { oldf ^ obj.flags };

        if (diff[flag::enable_process]) {
            m_cachedProcess = find_last_with_flags(flag::block_process);
        }

        if (diff[flag::enable_update]) {
            m_cachedUpdate = find_last_with_flags(flag::block_update);
        }

        if (diff[flag::enable_draw]) {
            m_cachedDraw = find_last_with_flags(flag::block_draw);
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
