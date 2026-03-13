#include <quest/data_loader.hpp>

#include <format>

using namespace hq;

std::string data_loader::resolve(std::string_view path) const {
    return m_loader.resolve(std::format("../data/{}", path));
}
