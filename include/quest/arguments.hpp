#pragma once

#include <span>
#include <string_view>

namespace HQ {
    class Arguments {
    public:
        Arguments(int argc, char** argv);

        bool operator[](std::string_view what) const;

    private:
        std::span<char*> m_span;
    };
}