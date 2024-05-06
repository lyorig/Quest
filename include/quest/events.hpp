#pragma once

#include <halcyon/types/numeric.hpp>

namespace HQ {
    class App;

    namespace Event {
        struct Quit { };

        struct AppCommand {
            hal::func_ptr<void, App&> command;
        };
    }
}