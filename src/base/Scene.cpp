//
// Created by Aier on 2026/5/31.
//

#include "azpch.h"
#include "Scene.h"
#include <algorithm>

namespace azer
{
    GameObject& Scene::CreateObject(const std::string& name)
    {
        auto obj = CreateScope<GameObject>(name);
        GameObject& ref = *obj;
        m_Objects.push_back(std::move(obj));
        return ref;
    }

    void Scene::AddObject(Scope<GameObject> obj)
    {
        m_Objects.push_back(std::move(obj));
    }

    void Scene::RemoveObject(GameObjectID id)
    {
        std::erase_if(m_Objects, [id](const Scope<GameObject>& obj) {
            return obj->GetID() == id;
        });
    }

    GameObject* Scene::FindObject(GameObjectID id)
    {
        for (auto& obj : m_Objects)
        {
            if (obj->GetID() == id)
                return obj.get();
        }
        return nullptr;
    }
}
