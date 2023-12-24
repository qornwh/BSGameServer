#pragma once
#include <functional>
#include <utility>

#include "Pch.h"

class Job
{
public:
    using Func = std::function<void()>;

    // Job(Func &&orgFunc) : _func(std::move(orgFunc)) {}

    // no params
    template <typename T>
    Job(void (T::*funcPtr)(), std::shared_ptr<T> instance)
    {
        _func = [funcPtr, instance]
        {
            (instance.get()->*funcPtr)();
        };
    }

    // 여러 파라미터 처리
    // template<typename T, typename... Args>
    // Job(void (T::*funcPtr)(Args...), shared_ptr<T> owner, Args... args)
    //{
    //    _func = [funcPtr, owner, args...]
    //    {
    //        (owner.get()->*funcPtr)(args...);
    //    };
    //}
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