#include <quest/scene_manager.hpp>

#include <quest/game.hpp>

using namespace HQ;

void scene_manager::update(game& g) {
    std::size_t num_activated { 0 };

    // Parked scenes can still listen for events to determine whether they want to be pulled up.
    // The "block_further_processing" flag is not relevant here.
    for (auto it = parked.begin(); it != parked.end();) {
        switch ((*it)->process(g.polled(), g.video)) {
            using enum scene::action;

        case switch_state: // The scene requests to be made active / "unparked".
            HAL_PRINT("<SceneMgr> Activating ", (*it)->name());
            (*it)->activate(g.renderer);
            active.push_back(std::move(*it));
            it = parked.erase(it);
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
    for (auto it = active.rbegin() + num_activated; it != active.rend();) {
        const bool block { (*it)->flags[scene::flags::block_further_processing] };

        switch ((*it)->process(g.polled(), g.video)) {
            using enum scene::action;

        case switch_state: // The state requests to be made inactive / "parked".
            HAL_PRINT("<SceneMgr> Parking ", (*it)->name());
            (*it)->deactivate();
            parked.push_back(std::move(*it));
            it = static_cast<decltype(it)>(active.erase((it + 1).base()));
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
    for (auto it = active.rbegin() + num_activated; it != active.rend(); ++it) {
        (*it)->update(g.delta());
    }

    // Draw everything.
    // A scene can specify that it's opaque, which will block
    // further scenes from being drawn.
    for (auto it = active.rbegin() + num_activated; it != active.rend(); ++it) {
        (*it)->draw(g.renderer);

        if ((*it)->flags[scene::flags::opaque]) {
            break;
        }
    }
}