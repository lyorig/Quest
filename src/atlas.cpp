#include <quest/atlas.hpp>

using namespace hq;

atlas::atlas(hal::clref<hal::renderer> rnd, hal::pixel::point size)
    : m_fmt { rnd->info().formats().front() }
    , m_tex { rnd, size, m_fmt } {
}

hal::pixel::rect atlas::add(hal::clref<hal::renderer> rnd, hal::ref<const hal::surface> surf) {
    (void)rnd, (void)surf;
    return {};
}
