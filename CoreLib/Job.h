#pragma once
#include <functional>
#include <utility>

#include "Pch.h"
using Func = std::function<void()>;

class Job
{
public:
    // no params
    template <typename T>
    Job(void (T::*funcPtr)(), std::shared_ptr<T> instance)
    {
        _func = [funcPtr, instance]
        {
            (instance.get()->*funcPtr)();
        };
    }

    template <typename T, typename... Args>
    Job(void (T::*funcPtr)(Args...), std::shared_ptr<T> instance, Args... args)
    {
        _func = [funcPtr, instance, args...]
        {
            (instance.get()->*funcPtr)(args...);
        };
    }

    void Execute()
    {
        _func();
    }

private:
    Func _func;
};