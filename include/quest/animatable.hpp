#pragma once

#include <algorithm>

#include <quest/types.hpp>

namespace HQ {
    namespace Easing {
        constexpr Delta Linear(Delta t) { return t; }

        namespace In // TODO
        {
            template <std::size_t N = 2>
            constexpr Delta Parabola(Delta t) {
                return std::pow(t, N);
            }
        }

        namespace Out // TODO
        {

        }

        namespace InOut {
            constexpr Delta Bezier(Delta t) { return t * t * (3.0 - 2.0 * t); }

            constexpr Delta Parametric(Delta t) {
                const Delta sqr { t * t };
                return sqr / (2.0 * (sqr - t) + 1.0);
            }
        }
    }

    template <typename T, auto AnimFunc>
        requires std::is_invocable_r_v<Delta, decltype(AnimFunc), Delta>
    class Animatable {
    public:
        using ValueType = T;
        using DiffType  = decltype(T {} - T {});

        Animatable(const ValueType& initial)
            : m_start { initial } {
        }

        bool Update(Delta elapsed) {
            if (m_elapsed == m_time) {
                return false;
            }

            else {
                m_elapsed = std::min(m_elapsed + elapsed, m_time);
                return true;
            }
        }

        void Start(const ValueType& val, Delta time) {
            m_start = Value();
            Target(val);

            m_time    = time - (m_time - m_elapsed);
            m_elapsed = 0.0;
        }

        // Change the target.
        // This is only really useful when retargeting mid-animation.
        void Target(const ValueType& val) {
            m_dist = val - m_start;
        }

        ValueType At(Delta time) const {
            return m_start + m_dist * AnimFunc(time == 0.0 ? 1.0 : m_elapsed / time);
        }

        ValueType Value() const {
            return At(m_time);
        }

        bool Done() const {
            return m_elapsed == m_time;
        }

    private:
        ValueType m_start {};

        DiffType m_dist {};

        Delta m_time { 0.0 },
            m_elapsed { m_time };
    };
}