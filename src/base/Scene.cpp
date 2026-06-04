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

    Scope<GameObject> Scene::TakeObject(GameObjectID id)
    {
        for (auto it = m_Objects.begin(); it != m_Objects.end(); ++it)
        {
            if ((*it)->GetID() == id)
            {
                Scope<GameObject> obj = std::move(*it);
                m_Objects.erase(it);
                return obj;
            }
        }
        return nullptr;
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
