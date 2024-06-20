#include <quest/scene_manager.hpp>

#include <quest/game.hpp>

using namespace HQ;

void scene_manager::update(game& g) {
    // This variable exists to prevent activated states from immediately receiving the same events again.
    std::size_t num_activated { 0 };

    // Iterators can be cached. Be careful to reset them when modifying a vector.
    auto begin = m_parked.begin(), end = m_parked.end();

    // Parked scenes can still listen for events to determine whether they want to be pulled up.
    // The "block_further_processing" flag is not relevant here.
    for (auto it = m_parked.begin(); it != m_parked.end();) {
        switch ((*it)->process(g.polled(), g.video)) {
            using enum scene::action;

        case switch_state: // The scene requests to be made active / "unparked".
            HAL_PRINT("<SceneMgr> Activating ", (*it)->name());
            (*it)->activate(g);
            add_active(std::move(*it));
            it = m_parked.erase(it);

            begin = m_parked.begin(), end = m_parked.end();
            ++num_activated;

            break;

        default:
            ++it;
            break;
        }
    }

    // Give all scenes event updates.
    // A scene can block further scenes from receiving events
    // by having the "block_further_processing" flag.

    begin = m_active.begin(), end = m_active.end() - num_activated;

    for (auto it = begin; it != end;) {
        const bool block { (*it)->flags[scene::flags::block_further_processing] };

        switch ((*it)->process(g.polled(), g.video)) {
            using enum scene::action;

        case switch_state: // The state requests to be made inactive / "parked".
            HAL_PRINT("<SceneMgr> Parking ", (*it)->name());
            (*it)->deactivate();
            m_parked.push_back(std::move(*it));
            it = m_active.erase(it);

            // Reset iterators.
            begin = m_active.begin(), end = m_active.end() - num_activated;
            m_cachedLastOpaque = find_last_opaque();
            break;

        default:
            ++it;
            break;
        };

        if (block) {
            break;
        }
    }

    // Update everything.
    for (auto it = begin; it != end; ++it) {
        (*it)->update(g.delta());
    }

    // Draw everything.
    // A scene can specify that it's opaque, which will block
    // further scenes from being drawn. As such, the last opaque scene is found.
    for (auto it = m_cachedLastOpaque; it != end; ++it) {
        (*it)->draw(g.renderer);

        if ((*it)->flags[scene::flags::opaque]) {
            break;
        }
    }
}

void scene_manager::add_active(base_up&& scn) {
    m_active.push_back(std::move(scn));
    m_cachedLastOpaque = find_last_opaque();
}

void scene_manager::add_parked(base_up&& scn) {
    m_parked.push_back(std::move(scn));
}

scene_manager::scene_vector::iterator scene_manager::find_last_opaque() {
    if (auto draw_from = std::find_if(m_active.rbegin(), m_active.rend(), [](const auto& ptr) { return ptr->flags[scene::flags::opaque]; }); draw_from == m_active.rend()) {
        return m_active.begin(); // not found, draw everything
    } else {
        return std::next(draw_from).base(); // found, draw from here
    }
}