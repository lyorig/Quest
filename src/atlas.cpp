#include "halcyon/video/texture.hpp"
#include <quest/atlas.hpp>

#include <halcyon/utility/guard.hpp>

using namespace hq;

namespace {
    constexpr hal::f64 resize_multiplier { 1.5 };

    static_assert(resize_multiplier > 1.0);
}

atlas::atlas(hal::lref<hal::renderer> rnd, hal::pixel::point size)
    : free { { hal::tag::as_size, size } }
    , fmt { rnd().info().get().formats().front() }
    , tex { make_texture(rnd, size) } {
    HAL_PRINT("<Atlas> Preferred format is ", fmt);
}

hal::pixel::rect atlas::add(hal::lref<hal::renderer> rnd, hal::ref<const hal::surface> surf) {
    const hal::pixel::point sz { surf->size() };

    for (auto it = free.begin(); it != free.end(); ++it) {
        const hal::pixel::rect rect { *it };

        if (sz.x <= rect.size.x && sz.y <= rect.size.y) {
            free.erase(it);

            hal::pixel::rect r { rect.pos.x, rect.pos.y + sz.y, rect.size.x, rect.size.y - sz.y };

            if (!(r.size.x == 0 || r.size.y == 0))
                free.push_back(r);

            r = { rect.pos.x + sz.x, rect.pos.y, rect.size.x - sz.x, sz.y };

            if (!(r.size.x == 0 || r.size.y == 0))
                free.push_back(r);

            hal::guard::target _ { rnd, tex };
            rnd->draw(hal::static_texture { rnd, surf }).to(rect.pos)();

            return { rect.pos, sz };
        }
    }

    // Not enough space. Resize!
    hal::target_texture new_tex { make_texture(rnd, tex.size() * resize_multiplier) };

    const hal::pixel::point old_sz { tex.size() }, new_sz { new_tex.size() };

    free.emplace_back(0, old_sz.y, new_sz.x, new_sz.y - old_sz.y);
    free.emplace_back(old_sz.x, 0, new_sz.x - old_sz.x, old_sz.y);

    {
        hal::guard::target _ { rnd, new_tex };
        hal::guard::blend  __ { tex, hal::blend_mode::none };

        rnd->draw(tex)();
    }

    tex = std::move(new_tex);

    return add(rnd, surf);
}

hal::target_texture atlas::make_texture(hal::lref<hal::renderer> rnd, hal::pixel::point size) {
    hal::target_texture tx { rnd, size, fmt };
    tx.blend(hal::blend_mode::alpha);
    return tx;
}
