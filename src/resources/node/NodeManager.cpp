//
// Created by csis on 2026/9/29.
//

#include "azpch.h"
#include "NodeManager.h"
#include "Node.h"

Azer::NodeManager::NodeManager()
{

}

Azer::NodeManager::~NodeManager()
{

}

void Azer::NodeManager::init() const
{
    for (const Ref<Node>& node : m_Nodes)
    {
        node->Init();
    }
}

void Azer::NodeManager::ready() const
{
    for (const Ref<Node>& node : m_Nodes)
    {
        node->Ready();
    }
}

void Azer::NodeManager::process(float delta) const
{
    for (const Ref<Node>& node : m_Nodes)
    {
        node->Process(delta);
    }
}

void Azer::NodeManager::physics_process(float delta) const
{
    for (const Ref<Node>& node : m_Nodes)
    {
        node->PhysicsProcess(delta);
    }
}

void Azer::NodeManager::input(Event& event) const
{
    for (const Ref<Node>& node : m_Nodes)
    {
        node->OnEvent(event);
    }
}

void Azer::NodeManager::draw() const
{
    for (const Ref<Node>& node : m_Nodes)
    {
        node->Draw();
    }
}

void Azer::NodeManager::exit() const
{
    for (const Ref<Node>& node : m_Nodes)
    {
        node->Exit();
    }
}

void Azer::NodeManager::add_node(const Ref<Node>& node)
{
    m_Nodes.push_back(node);
}

Azer::Ref<Azer::Node> Azer::NodeManager::get_node(const std::string& name) const
{
    for (const Ref<Node>& node : m_Nodes)
    {
        if (node->GetName() == name)
        {
            return node;
        }

        if (Ref<Node> n = node->GetChild(name))
        {
            return n;
        }
    }
    return nullptr;
}
