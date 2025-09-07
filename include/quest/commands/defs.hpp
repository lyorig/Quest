#pragma once

#include <span> // IWYU pragma: export

// The types passed to Quest console commands.
// Intended to be used in function declarations or as type parameters.
#define HQ_CMD_PARAM_TYPES game&, std::span<const std::string_view>

// The parameters passed to Quest console commands.
// Intended to be used in function definitions.
#define HQ_CMD_PARAMS game &g, std::span<const std::string_view> args

namespace hq::cmd {
    enum class status {
        ok
    };
}
