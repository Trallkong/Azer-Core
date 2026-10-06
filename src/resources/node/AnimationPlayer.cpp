//
// Created by csis on 2026/9/30.
//

#include "azpch.h"
#include "AnimationPlayer.h"

#include "Logger.h"

namespace Azer
{
    void AnimationObject::Process(float delta)
    {
        for (auto& channel : m_Channels)
        {
            channel.UpdateProperty(delta, m_Time);
        }
    }

    void AnimationObject::Play()
    {

    }

    void AnimationObject::Stop()
    {
    }

    AnimationPlayer::AnimationPlayer(const std::string& name)
        : Node(name)
    {

    }

    AnimationPlayer::~AnimationPlayer()
    {

    }

    void AnimationPlayer::Process(const float delta)
    {
        Node::Process(delta);

        if (m_PlayingAnimation)
        {
            m_PlayingAnimation->Process(delta);
        }
    }

    void AnimationPlayer::Play(const std::string& name)
    {
        auto result = m_Animations | std::views::filter([name](const Ref<AnimationObject>& animation) { return animation->Name == name; });
        if (result.empty())
        {
            AZ_CORE_INFO("No animation found!");
            return;
        }
        result.begin()->get()->Play();
        m_PlayingAnimation.reset(result.begin()->get());
    }

    void AnimationPlayer::Stop()
    {
        if (m_PlayingAnimation)
        {
            m_PlayingAnimation->Stop();
            m_PlayingAnimation = nullptr;
            return;
        }
        AZ_CORE_WARN("No animation playing!");
    }

    void AnimationPlayer::AddAnimation(std::string name)
    {
        for (const auto& animation : m_Animations)
        {
            if (animation->Name == name)
            {
                AZ_CORE_WARN("Animation already exists!");
                return;
            }
        }

        m_Animations.push_back(CreateRef<AnimationObject>(std::move(name)));
    }

    void AnimationPlayer::RemoveAnimation(const std::string& name)
    {
        int remove_index = -1;
        for (int i = 0; i < m_Animations.size(); i++)
        {
            if (m_Animations[i]->Name == name)
            {
                remove_index = i;
                break;
            }
        }
        if (remove_index >= 0)
        {
            m_Animations.erase(m_Animations.begin() + remove_index);
        }
    }

    const Ref<AnimationObject>& AnimationPlayer::GetAnimation(const std::string& name) const
    {
        for (auto& animation : m_Animations)
        {
            if (animation->Name == name)
            {
                return animation;
            }
        }

        AZ_CORE_WARN("No animation found!");
        return nullptr;
    }

    void AnimationChannel::UpdateProperty(float delta, float time)
    {
        if (const float frame_time = m_KeyFrames[m_FrameIndex].Time; time > frame_time)
        {
            m_FrameIndex++;
            m_FrameIndex %= m_KeyFrames.size();
        }

        const VariantValue value = m_KeyFrames[m_FrameIndex].Value;
        const VariantValue v = m_Property.Lerp(value, delta);

        if (Variant::IsInt(v) && Variant::AsInt(v) == -1)
        {
            AZ_CORE_ERROR("Wrong interpolate type!");
            return;
        }

        m_Property.Set(v);
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
