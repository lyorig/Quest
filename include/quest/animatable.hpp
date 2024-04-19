#pragma once

#include <halcyon/debug.hpp>
#include <quest/types.hpp>
#include <vector>

namespace HQ
{
    namespace Easing
    {
        constexpr Delta Linear(Delta t) { return t; }

        namespace In // TODO
        {
            constexpr Delta Quint(Delta t)
            {
                return std::pow(t, 5);
            }
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
        using ValueType = T;
        using DiffType  = decltype(T {} - T {});

        Animatable() = default;

        Animatable(const ValueType& initial)
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

        void Start(const ValueType& val, Delta time)
        {
            HAL_ASSERT(time >= 0.0, "Negative animation duration");

            m_start = Value();
            Target(val);

            m_time    = time;
            m_elapsed = 0.0;
        }

        // "Teleport" instantly to a value.
        void Jump(const ValueType& val)
        {
            start(val, 0.0);
        }

        // Change the target.
        // This is only really useful when retargeting mid-animation.
        void Target(const ValueType& val)
        {
            m_dist = val - m_start;
        }

        ValueType Value() const
        {
            return m_start + m_dist * AnimFunc(m_time == 0.0 ? 1.0 : m_elapsed / m_time);
        }

        bool Done() const
        {
            return m_elapsed == m_time;
        }

    private:
        ValueType m_start {};

        DiffType m_dist;

        Delta m_time { 0.0 },
            m_elapsed { m_time };
    };

    template <typename T, auto Func>
    class AnimationQueue
    {
        struct Info
        {
            T     value;
            Delta time;
        };

    public:
        using Anim = Animatable<T, Func>;

        void Add(T&& value, Delta time)
        {
            m_vec.emplace_back(std::move(value), time);
        }

        // Returns false if there are no more animations.
        bool Update(Delta elapsed)
        {
            if (m_vec.empty())
                return false;

            if (!m_anim.Update(elapsed))
            {
                m_vec.pop_back();

                const Info& now { m_vec.back() };
                m_anim.Start(now.value, now.time);
            }

            return true;
        }

        const Anim& Get() const
        {
            return m_anim;
        }

    private:
        Anim m_anim;

        std::vector<Info> m_vec;
    };
}