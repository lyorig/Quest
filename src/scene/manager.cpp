#include <quest/scene/manager.hpp>

#include <quest/game.hpp>

using namespace hq::scene;

// Process -> update -> draw.
void manager::update(game& g) {
    for (auto it = m_scenes.begin(); it != m_scenes.end(); ++it) {
        scene::base& obj { **it };

        // Process:
        if (obj.flags[flags::enable_process]) {
            obj.process(g);
        }

        // Update:
        if (obj.flags[flags::enable_update]) {
            obj.update(g);
        }

        // Draw:
        if (obj.flags[flags::enable_draw]) {
            obj.draw(g.renderer);
        }
    }
}

void manager::add(base_up&& scn) {
    m_scenes.push_back(std::move(scn));
    reset_cached();
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

void manager::reset_cached() {
    m_cachedProcess = find_last_with_flags(flags::block_process);
    m_cachedUpdate  = find_last_with_flags(flags::block_update);
    m_cachedDraw    = find_last_with_flags(flags::block_draw);
}
