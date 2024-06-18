#include <quest/scene_manager.hpp>

using namespace HQ;

void scene_manager::pop_back() {
    m_stack.pop_back();
}

void scene_manager::process(const hal::event::handler& event) {
    for (auto& obj : m_stack) {
        obj->process(event);
    }
}

void scene_manager::update(delta_t time, hal::renderer& rnd) {
    for (auto& obj : m_stack) {
        obj->update(time, rnd);
    }
}