#include <quest/atlas.hpp>

using namespace hq;

atlas::atlas(hal::clref<hal::renderer> rnd, hal::pixel::point size)
    : free { size }
    , fmt { rnd().info().formats().front() }
    , tex { rnd, size, fmt } {
    HAL_PRINT("<Atlas> Preferred format is ", fmt);
}

hal::pixel::rect atlas::add(hal::clref<hal::renderer> rnd, hal::ref<const hal::surface> surf) {
    // some dumbass algorithm here...
    (void)rnd, (void)surf;
    return {};
}
