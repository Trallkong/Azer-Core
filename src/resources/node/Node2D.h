//
// Created by csis on 2026/9/29.
//

#pragma once

#include "Base.h"
#include "Node.h"
#include "Transform2D.h"

namespace Azer
{

    class Node2D : public Node
    {
    public:
        Node2D() = delete;
        explicit Node2D(std::string name) : Node(std::move(name)) {}
        ~Node2D() override = default;

        void Init() override {}
        void Ready() override {}
        void Process(float delta) override {}
        void PhysicsProcess(float delta) override {}
        void OnEvent(Event& event) override {}
        void Draw() override {}
        void Exit() override {}

        Transform2D Transform;
    };
}


