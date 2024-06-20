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
    for (auto it = begin; it != end;) {
        switch ((*it)->process(g)) {
            using enum scene::action;

        case switch_state: // The scene requests to be made active / "unparked".
            (*it)->activate(g);
            add_active(std::move(*it));

            // ↓↓ Intentional fallthrough! ↓↓

        case kill: // The scene wantes to be removed.
            it = m_parked.erase(it);

            --end;
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

    for (auto it = m_cachedLastProcess; it != end;) {
        switch ((*it)->process(g)) {
            using enum scene::action;

        case switch_state: // The state requests to be made inactive / "parked".
            (*it)->deactivate();
            add_parked(std::move(*it));

            // ↓↓ Intentional fallthrough! ↓↓

        case kill: // The scene wantes to be removed.
            it = m_active.erase(it);

            // Reset iterators.
            --end;
            update_cached();

            break;

        default:
            ++it;
            break;
        };
    }

    // Update everything.
    for (auto it = begin; it != end; ++it) {
        (*it)->update(g);
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
    update_cached();
}

void scene_manager::add_parked(base_up&& scn) {
    m_parked.push_back(std::move(scn));
}

scene_manager::scene_vector::iterator scene_manager::find_last_with_flag(scene::flags f) {
    auto it = m_active.end() - 1;

    for (; it != m_active.begin(); --it) {
        if ((*it)->flags[f]) {
            break;
        }
    }

    return it;
}

void scene_manager::update_cached() {
    m_cachedLastOpaque  = find_last_with_flag(scene::flags::opaque);
    m_cachedLastProcess = find_last_with_flag(scene::flags::block_further_processing);
}