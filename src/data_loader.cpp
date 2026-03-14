#include <quest/data_loader.hpp>

using namespace hq;

namespace {
    std::string data_path() {
        std::string s { hal::fs::base_path() };
        s.resize(s.find_last_of('/', s.find_last_of('/') - 1) + 1);
        s += "data/";
        return s;
    }
}

data_loader::data_loader()
    : m_base { data_path() } {
}

std::string data_loader::resolve(std::string_view path) const {
    return hal::fs::resource_loader::resolve(m_base, path);
}
