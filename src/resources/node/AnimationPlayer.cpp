//
// Created by csis on 2026/9/30.
//

#include "azpch.h"
#include "AnimationPlayer.h"

#include "Logger.h"

namespace Azer
{
    AnimationPlayer::AnimationPlayer(const std::string& name)
        : Node(name)
    {

    }

    AnimationPlayer::~AnimationPlayer()
    {

    }

    void AnimationPlayer::Process(float delta)
    {
        Node::Process(delta);

    }

    void AnimationChannel::UpdateProperty(float delta, float time)
    {
        const float frame_time = m_KeyFrames[m_FrameIndex].Time;
        const Numeric value = m_KeyFrames[m_FrameIndex].Value;
        if (time > frame_time)
        {
            m_FrameIndex++;
            m_FrameIndex %= m_KeyFrames.size();
        }

        if (auto* p = std::get_if<float>(m_Property))
        {
            if (const auto* t = std::get_if<float>(&value))
            {
                *p = Math::Lerp(*p, *t, delta);
            }
        }
    }

    void AnimationChannel::AddKeyFrame(const KeyFrame& keyFrame)
    {
        for (const auto& [Time, Value] : m_KeyFrames)
        {
            if (Time == keyFrame.Time)
            {
                AZ_CORE_INFO("关键帧位置重复，添加失败");
                return;
            }
        }

        m_KeyFrames.push_back(keyFrame);
    }

    void AnimationChannel::RemoveKeyFrame(float time)
    {
        int key_to_remove = -1;

        for (int i = 0; i < m_KeyFrames.size(); i++)
        {
            if (m_KeyFrames[i].Time == time)
            {
                key_to_remove = i;
            }
        }

        if (key_to_remove >= 0)
        {
            m_KeyFrames.erase(m_KeyFrames.begin() + key_to_remove);
        }
        else
        {
            AZ_CORE_INFO("无效的关键帧位置，删除失败");
        }
    }
}
