//
// Created by csis on 2026/9/29.
//
#pragma once

#include "Base.h"
#include <string>
#include <utility>

#include "NodeManager.h"
#include "UUID.h"

namespace Azer
{
    class Node
    {
    public:
        explicit Node(std::string name)
            : Name(std::move(name))
        {
            UUID = generate_uuid();
        };
        virtual ~Node() = default;

        virtual void Init();
        virtual void Ready();
        virtual void Process(float delta);
        virtual void PhysicsProcess(float delta);
        virtual void OnEvent(Event& event);
        virtual void Draw();
        virtual void Exit();

        inline const std::string& GetUUID() const { return UUID; }
        inline const std::string& GetName() const { return Name; }

        inline void set_name(std::string name) { Name = std::move(name); }

        void AddChild(const Ref<Node>& child);
        Ref<Node> GetChild(const std::string& name) const;

    private:
        std::string UUID;
        std::string Name;

        NodeManager m_NodeManager;
    };
}



