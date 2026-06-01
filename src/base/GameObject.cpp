//
// Created by Aier on 2026/5/31.
//

#include "azpch.h"
#include "GameObject.h"

namespace azer
{
    GameObjectID GameObject::s_NextID = 1;

    GameObject::GameObject()
        : m_ID(s_NextID++), m_Name("GameObject")
    {
    }

    GameObject::GameObject(const std::string& name)
        : m_ID(s_NextID++), m_Name(name)
    {
    }
}
