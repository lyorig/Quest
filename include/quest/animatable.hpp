#pragma once

#include <algorithm>
#include <halcyon/debug.hpp>
#include <quest/types.hpp>

namespace HQ
{
    namespace Easing
    {
        constexpr Delta Linear(Delta t) { return t; }

        namespace In // TODO
        {

        }

        namespace Out // TODO
        {

        }

        namespace InOut
        {
            constexpr Delta Bezier(Delta t) { return t * t * (3.0 - 2.0 * t); }

            constexpr Delta Parametric(Delta t)
            {
                const Delta sqr { t * t };
                return sqr / (2.0 * (sqr - t) + 1.0);
            }
        }
    }

    template <typename T, auto AnimFunc>
        requires std::is_invocable_r_v<Delta, decltype(AnimFunc), Delta>
    class Animatable
    {
    public:
        Animatable() = default;

        Animatable(const T& initial)
            : m_start { initial }
        {
        }

        bool Update(Delta elapsed)
        {
            if (m_elapsed == m_time)
            {
                return false;
            }

            else
            {
                m_elapsed = std::min(m_elapsed + elapsed, m_time);
                return true;
            }
        }

        void Start(const T& val, Delta time)
        {
            HAL_ASSERT(time >= 0.0, "Negative animation duration");

            m_start = Value();
            Target(val);

            m_time    = time;
            m_elapsed = 0.0;
        }

        // "Teleport" instantly to a value.
        void Jump(const T& val)
        {
            start(val, 0.0);
        }

        // Change the target.
        // This is only really useful when retargeting mid-animation.
        void Target(const T& val)
        {
            m_dist = val - m_start;
        }

        T Value() const
        {
            return m_start + m_dist * AnimFunc(m_time == 0.0 ? 1.0 : m_elapsed / m_time);
        }

    private:
        T m_start {},
            m_dist {};

        Delta m_time { 0.0 },
            m_elapsed { m_time };
    };
}