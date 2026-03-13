#pragma once

#include <halcyon/filesystem.hpp>

namespace hq {
    class data_loader {
    public:
        data_loader() = default;

        std::string resolve(std::string_view path) const;

    private:
        hal::fs::resource_loader m_loader;
    };
}
