//
// Created by csis on 2026/9/29.
//

#include "azpch.h"
#include "Node.h"
#include "NodeManager.h"
#include "UUID.h"

Azer::Node::Node(std::string name)
    : Name(std::move(name))
{
    UUID = generate_uuid();
    m_NodeManager = CreateScope<NodeManager>();
}

Azer::Node::~Node()
{

}

void Azer::Node::Init()
{
    m_NodeManager->init();
}

void Azer::Node::Ready()
{
    m_NodeManager->ready();
}

void Azer::Node::Process(float delta)
{
    m_NodeManager->process(delta);
}

void Azer::Node::PhysicsProcess(float delta)
{
    m_NodeManager->physics_process(delta);
}

void Azer::Node::OnEvent(Event& event)
{
    m_NodeManager->input(event);
}

void Azer::Node::Draw()
{
    m_NodeManager->draw();
}

void Azer::Node::Exit()
{
    m_NodeManager->exit();
}

void Azer::Node::AddChild(const Ref<Node>& child) const
{
    m_NodeManager->add_node(child);
}

Azer::Ref<Azer::Node> Azer::Node::GetChild(const std::string& name) const
{
    return std::move(m_NodeManager->get_node(name));
}
