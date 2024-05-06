#include <halcyon/utility/strutil.hpp>
#include <quest/arguments.hpp>

using namespace HQ;

Arguments::Arguments(int argc, char** argv)
    : m_span { argv, static_cast<std::size_t>(argc) } {
}

bool Arguments::operator[](std::string_view what) const {
    for (auto arg : m_span)
        if (hal::streq(arg, what.data()))
            return true;

    return false;
}