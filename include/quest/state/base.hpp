#pragma once

#include <quest/app.hpp>

namespace HQ::State
{
    enum class Type
    {
        None,
        Quit,
        MainMenu,
    };

    // Base state class.
    class Base
    {
    public:
        virtual Type Update(App& app, hal::f64 elapsed)    = 0;
        virtual void Draw(hal::video::renderer& rnd) const = 0;

        virtual ~Base() = default;
    };
}