#pragma once

#include <halcyon/utility/concepts.hpp>
#include <vector>

namespace HQ {
    template <hal::meta::bare T>
    class RFEMLeaf {
    protected:
        std::vector<T> vec;
    };

    // The Really Fast Entity Manager (RFEM).
    // Its key advantage is that it makes the compiler do a lot of work.
    // And disadvantages? 24 extra bytes per type.
    template <hal::meta::bare... Entities>
    class RFEMTemplate : RFEMLeaf<Entities>... {
    public:
        using Types = hal::meta::type_list<Entities...>;

        RFEMTemplate() = default;

        template <hal::meta::one_of<Entities...> T, typename... Args>
        void Spawn(Args&&... args) {
            RFEMLeaf<T>::vec.emplace_back(std::forward<Args>(args)...);
        }

        template <hal::meta::one_of<Entities...> T>
        const std::vector<T>& Get() const {
            return RFEMLeaf<T>::vec;
        }

        template <typename Functor> // clang-format turns this into cuneiform.
            requires hal::meta::any<std::is_invocable_v<Functor, Entities&>...>
        void Visit(Functor&& f) {
            ([&]() {
                for (auto& ent : RFEMLeaf<Entities>::vec)
                    f(ent);
            }(),
                ...);
        }
    };
}