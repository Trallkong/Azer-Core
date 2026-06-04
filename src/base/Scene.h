//
// Created by Aier on 2026/5/31.
//

#pragma once
#include "Base.h"
#include "GameObject.h"
#include <vector>
#include <string>

namespace azer
{
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        // 创建并添加一个新对象
        GameObject& CreateObject(const std::string& name = "GameObject");

        // 添加已有对象
        void AddObject(Scope<GameObject> obj);

        // 删除对象
        void RemoveObject(GameObjectID id);

        // 取出对象（不销毁，用于 undo）
        Scope<GameObject> TakeObject(GameObjectID id);

        // 查找
        GameObject* FindObject(GameObjectID id);

        // 访问
        const std::vector<Scope<GameObject>>& GetObjects() const { return m_Objects; }
        size_t GetObjectCount() const { return m_Objects.size(); }

        // 清空
        void Clear() { m_Objects.clear(); }

    private:
        std::vector<Scope<GameObject>> m_Objects;
    };
}
