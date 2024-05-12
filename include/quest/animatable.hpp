#pragma once

#include <algorithm>

#include <quest/types.hpp>

namespace HQ {
    namespace easing {
        constexpr delta_t linear(delta_t t) { return t; }

        namespace in // TODO
        {
            template <std::size_t N = 2>
            constexpr delta_t Parabola(delta_t t) {
                return std::pow(t, N);
            }
        }

        namespace out // TODO
        {

        }

        namespace in_out {
            constexpr delta_t bezier(delta_t t) { return t * t * (3.0 - 2.0 * t); }

            constexpr delta_t parametric(delta_t t) {
                const delta_t sqr { t * t };
                return sqr / (2.0 * (sqr - t) + 1.0);
            }
        }
    }

    template <typename T, auto AnimFunc>
        requires std::is_invocable_r_v<delta_t, decltype(AnimFunc), delta_t>
    class animatable {
    public:
        using value_t = T;
        using diff_t  = decltype(T {} - T {});

        animatable(const value_t& initial)
            : m_start { initial } {
        }

        bool update(delta_t elapsed) {
            if (m_elapsed == m_time) {
                return false;
            }

            else {
                m_elapsed = std::min(m_elapsed + elapsed, m_time);
                return true;
            }
        }

        void start(const value_t& val, delta_t time) {
            m_start = value();
            target(val);

            m_time    = time - (m_time - m_elapsed);
            m_elapsed = 0.0;
        }

        // Change the target.
        // This is only really useful when retargeting mid-animation.
        void target(const value_t& val) {
            m_dist = val - m_start;
        }

        value_t at(delta_t time) const {
            return m_start + m_dist * AnimFunc(time == 0.0 ? 1.0 : m_elapsed / time);
        }

        value_t value() const {
            return at(m_time);
        }

        bool done() const {
            return m_elapsed == m_time;
        }

    private:
        value_t m_start {};

        diff_t m_dist {};

        delta_t m_time { 0.0 },
            m_elapsed { m_time };
    };
}