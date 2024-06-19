#include <quest/scene_manager.hpp>

using namespace HQ;

void scene_manager::pop_back() {
    m_stack.pop_back();
}

void scene_manager::update(game& g) {
    for (auto& obj : m_stack) {
        obj->update(g);
    }
}