#pragma once

#include <variant>
#include <vector>

#include <halcyon/utility/concepts.hpp>
#include <quest/events.hpp>

namespace HQ {

    template <typename... Events>
    class EventManagerTemplate {
    public:
        using variant = std::variant<Events...>;

        EventManagerTemplate() = default;

        template <hal::one_of<Events...> T, typename... Args>
        void Send(Args&&... args) {
            m_variant.emplace_back(std::in_place_type<T>, std::forward<Args>(args)...);
        }

        void Clear() {
            m_variant.clear();
        }

        const variant* begin() const {
            return m_variant.begin();
        }

        const variant* end() const {
            return m_variant.end();
        }

    private:
        std::vector<variant> m_variant;
    };

    using EventManager = EventManagerTemplate<Event::Quit, Event::AppCommand>;
}