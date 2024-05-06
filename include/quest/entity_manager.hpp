#pragma once

#include <halcyon/utility/concepts.hpp>
#include <vector>

namespace HQ {
    template <typename T>
    struct RFEMLeaf {
        std::vector<T> vec;
    };

    template <typename... Entities>
    class RFEMTemplate : RFEMLeaf<Entities>... {
    public:
        RFEMTemplate() = default;

        constexpr static std::size_t Types() {
            return sizeof...(Entities);
        }

        template <hal::one_of<Entities...> T, typename... Args>
        void Spawn(Args&&... args) {
            RFEMLeaf<T>::vec.emplace_back(std::forward<Args>(args)...);
        }

        template <hal::one_of<Entities...> T>
        const std::vector<T>& Get() {
            return RFEMLeaf<T>::vec;
        }

        template <typename Functor> // This is pure cancer.
        void Visit(Functor&& f) {
            ([&]() {
                for (auto& ent : RFEMLeaf<Entities>::vec)
                    f(ent);
            }(),
                ...);
        }
    };

    using RFEM = RFEMTemplate<int, double, short>;
}