#pragma once

#include <limits>

namespace hq {
    // Shorthand for std::numeric_limits.
    template <typename T>
    using lim = std::numeric_limits<T>;

    // So that I can change it to a `float` if I ever feel like it.
    using delta_t = double;
}
