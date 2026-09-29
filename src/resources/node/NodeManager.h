//
// Created by csis on 2026/9/29.
//

#pragma once

#include "Base.h"
#include <vector>

namespace Azer
{
    class Event;
    class Node;

    class NodeManager
    {
    public:
        NodeManager() = default;
        ~NodeManager() = default;

        void init() const;
        void ready() const;
        void process(float delta) const;
        void physics_process(float delta) const;
        void input(Event& event) const;
        void draw() const;
        void exit() const;

        void add_node(const Ref<Node>& node);
        Ref<Node> get_node(const std::string& name) const;

    private:
        std::vector<Ref<Node>> m_Nodes;
    };
}




