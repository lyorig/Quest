#pragma once

#include <halcyon/filesystem.hpp>

namespace hq {
    class data_loader {
    public:
        data_loader();

        std::string resolve(std::string_view path) const;

    private:
        std::string m_base;
    };
}
