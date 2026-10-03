module;

#include <string>
#include <string_view>

#include <halcyon/filesystem.hpp>

export module quest.data_loader;

namespace {
    std::string data_path() {
        std::string s { hal::fs::base_path() };
        s.resize(s.find_last_of('/', s.find_last_of('/') - 1) + 1);
        s += "data/";
        return s;
    }
}

namespace hq {
    export class data_loader {
    public:
        data_loader()
            : m_base { data_path() } {
        }

        std::string resolve(std::string_view path) const {
            return hal::fs::resource_loader::resolve(m_base, path);
        }

    private:
        std::string m_base;
    };
}
