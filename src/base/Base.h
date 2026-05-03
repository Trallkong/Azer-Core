//
// Created by Trallkong on 2026/4/18.
//

#ifndef LEARNSDL_BASE_H
#define LEARNSDL_BASE_H
#include <memory>

namespace azer
{
    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T>
    using Scope = std::unique_ptr<T>;

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
}

#endif //LEARNSDL_BASE_H
