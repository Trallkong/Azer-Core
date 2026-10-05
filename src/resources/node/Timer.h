//
// Created by csis on 2026/9/30.
//
#pragma once

#include "Base.h"
#include "Node.h"

#include <functional>
#include <utility>

namespace Azer
{
    class Timer : public Node
    {
    public:
        explicit Timer(const std::string& name);
        ~Timer() override;

        bool AutoStart = false;
        bool OneShot = false;
        float WaitTime = 1.0f;

        void Ready() override;
        void Process(float delta) override;

        void Start();
        void SetCallback(std::function<void()> callback) { m_Callback = std::move(callback); }
    private:
        std::function<void()> m_Callback;

        float m_AccumulatedTime = 0.0f;
        bool m_Running = false;
    };
}
