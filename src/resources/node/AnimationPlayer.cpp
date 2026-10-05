//
// Created by csis on 2026/9/30.
//

#include "azpch.h"
#include "AnimationPlayer.h"

#include "Logger.h"

namespace Azer
{
    void AnimationObject::Play()
    {

    }

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

        for (auto& animation : m_Animations)
        {

        }
    }

    void AnimationPlayer::Play(const std::string& name)
    {
        auto result = m_Animations | std::views::filter([name](const AnimationObject& animation) { return animation.Name == name; });
        if (result.empty())
        {
            AZ_CORE_INFO("No animation found!");
            return;
        }
        result.begin()->Play();
    }

    void AnimationChannel::UpdateProperty(float delta, float time)
    {
        const float frame_time = m_KeyFrames[m_FrameIndex].Time;
        const Variant value = m_KeyFrames[m_FrameIndex].Value;
        if (time > frame_time)
        {
            m_FrameIndex++;
            m_FrameIndex %= m_KeyFrames.size();
        }

        const Variant v = m_Property.Lerp(value, delta);

        if (v.IsInt() && v.AsInt() == -1)
        {
            AZ_CORE_ERROR("Wrong interpolate type!");
            return;
        }

        m_Property.Set(v.Get());
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
