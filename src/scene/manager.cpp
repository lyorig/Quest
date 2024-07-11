#include <quest/scene/manager.hpp>

#include <quest/game.hpp>

using namespace hq::scene;

// Process -> update -> draw.
void manager::update(game& g) {
    for (auto& scn : m_scenes) {
        scene::base& obj { *scn };

        // Disable status flags beforehand.
        obj.flags -= flags::all_status;

        // Process:
        if (obj.flags[flags::process]) {
            obj.process(g);
        }

        // Update:
        if (obj.flags[flags::update]) {
            obj.update(g);
        }

        // Draw:
        if (obj.flags[flags::draw]) {
            obj.draw(g.renderer);
        }

        // Check status flags:
        // if (obj.flags[flags::remove_me]) {
        //     it = m_scenes.erase(it);
        // }
    }
}

void manager::add(base_up&& scn) {
    m_scenes.push_back(std::move(scn));
}
