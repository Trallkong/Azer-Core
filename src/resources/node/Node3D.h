//
// Created by csis on 2026/9/29.
//

#pragma once

#include "Base.h"
#include "Node.h"

namespace Azer
{
    class Node3D : public Node
    {
    public:
        Node3D() = delete;
        explicit Node3D(std::string name) : Node(std::move(name)) {}
        ~Node3D() override = default;

        void Init() override {}
        void Ready() override {}
        void Process(float delta) override {}
        void PhysicsProcess(float delta) override {}
        void Draw() override {}
        void Exit() override {}

        Transform3D Transform;
    };
}


