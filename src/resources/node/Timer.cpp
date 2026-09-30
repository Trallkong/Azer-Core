//
// Created by csis on 2026/9/30.
//

#include "azpch.h"
#include "Timer.h"

#include "Logger.h"

namespace Azer
{
    Timer::Timer(const std::string& name)
        : Node(name)
    {

    }

    Timer::~Timer()
    {

    }

    void Timer::Ready()
    {
        Node::Ready();

        if (AutoStart)
        {
            Start();
        }
    }

    void Timer::Process(float delta)
    {
        Node::Process(delta);

        if (!m_Running) return;

        if (m_AccumulatedTime >= WaitTime)
        {
            m_Callback();

            if (OneShot)
            {
                m_Running = false;
            }
            else
            {
                m_AccumulatedTime = 0.0f;
            }
        }

        m_AccumulatedTime += delta;
    }

    void Timer::Start()
    {
        m_Running = true;
    }
}
