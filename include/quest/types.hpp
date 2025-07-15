#pragma once

#include <limits>

namespace hq {
    // Shorthand for std::numeric_limits.
    template <typename T>
    using lim = std::numeric_limits<T>;

    using delta_t = double;
}
