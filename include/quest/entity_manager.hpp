#pragma once

#include <halcyon/utility/concepts.hpp>
#include <vector>

namespace hq {
    template <hal::meta::bare T>
    class rfem_leaf {
    public:
        template <typename Functor>
        void apply(Functor&& f) {
            for (auto& ent : vec)
                f(ent);
        }

    protected:
        std::vector<T> vec;
    };

    // The Really Fast Entity Manager (RFEM).
    // Its key advantage is that it makes the compiler do a lot of work.
    // And disadvantages? 24 extra bytes per type.
    template <hal::meta::bare... Entities>
    class rfem_template : public rfem_leaf<Entities>... {
    public:
        using types = hal::meta::type_list<Entities...>;

        rfem_template() = default;

        template <hal::meta::one_of<Entities...> T, typename... Args>
        void spawn(Args&&... args) {
            rfem_leaf<T>::vec.emplace_back(std::forward<Args>(args)...);
        }

        template <hal::meta::one_of<Entities...> T>
        rfem_leaf<T>& get() const {
            return static_cast<rfem_leaf<T>&>(*this);
        }

        template <typename Functor> // clang-format turns this into cuneiform.
            requires hal::meta::all<std::is_invocable_v<Functor, Entities&>...>
        void Visit(Functor&& f) {
            (rfem_leaf<Entities>::apply(f), ...);
        }
    };
}