#pragma once

// The types passed to Quest console commands.
// Intended to be used in function declarations or as type parameters.
#define HQ_CMD_PARAM_TYPES game&

// The parameters passed to Quest console commands.
// Intended to be used in function definitions.
#define HQ_CMD_PARAMS game& g

namespace hq::cmd {
    enum class status {
        ok
    };
}
