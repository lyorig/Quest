module;

#include <cstdint>

export module quest.commands.defs;

export namespace hq::cmd {
    enum class status : std::uint8_t {
        ok,
        err
    };
}
