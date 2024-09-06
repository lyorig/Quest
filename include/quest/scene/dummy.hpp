#pragma once

#include <quest/scene/base.hpp>

namespace hq {
    namespace scene {
        class dummy : public base {
        public:
            void process(game& g);
            void update(game& g);
            void draw(game& g);
        };

        static_assert(interface<dummy>);
    }
}
