//
// Created by Trallkong on 2026/4/18.
//

#pragma once

#include <memory>

namespace Azer
{
    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Weak = std::weak_ptr<T>;

    template<typename T, typename ... Args>
    Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    Weak<T> CreateWeak(Ref<T> ref)
    {
        return std::weak_ptr<T>(ref);
    }

    template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;


    #define AZ_ASSERT(x, ...) { if(!(x)) { AZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
}
