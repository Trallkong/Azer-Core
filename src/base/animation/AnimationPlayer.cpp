//
// Created by Trallkong on 2026/5/30.
//

#include "azpch.h"
#include "AnimationPlayer.h"

namespace Azer
{
    void AnimationPlayer::AddAnimation(Animation animation)
    {
        m_Animations.push_back(std::move(animation));
    }

    Animation* AnimationPlayer::GetAnimation(const std::string& name)
    {
        auto it = std::ranges::find(m_Animations, name, &Animation::name);
        if (it == m_Animations.end())
            return nullptr;
        return &*it;
    }

    void AnimationPlayer::Play(const std::string& name)
    {
        Animation* anim = GetAnimation(name);
        if (!anim)
        {
            AZ_CORE_WARN("AnimationPlayer: animation \"{0}\" not found", name);
            return;
        }

        m_Current = anim;
        m_CurrentTime = 0.0f;
        m_Playing = true;
    }

    void AnimationPlayer::Stop()
    {
        m_Playing = false;
        m_CurrentTime = 0.0f;
    }

    void AnimationPlayer::Pause()
    {
        m_Playing = false;
    }

    void AnimationPlayer::Resume()
    {
        if (m_Current)
            m_Playing = true;
    }

    void AnimationPlayer::Update(float delta)
    {
        if (!m_Playing || !m_Current)
            return;

        m_CurrentTime += delta * m_Speed;

        // Find animation duration (max time across all channels)
        float duration = 0.0f;
        for (const auto& channel : m_Current->channels)
        {
            if (!channel.keyFrames.empty())
            {
                float last = channel.keyFrames.back().time;
                if (last > duration)
                    duration = last;
            }
        }

        if (duration <= 0.0f)
            return;

        if (m_Looping)
        {
            m_CurrentTime = std::fmod(m_CurrentTime, duration);
            if (m_CurrentTime < 0.0f)
                m_CurrentTime += duration;
        }
        else
        {
            if (m_CurrentTime >= duration)
            {
                m_CurrentTime = duration;
                ApplyAnimation();
                m_Playing = false;
                return;
            }
        }

        ApplyAnimation();
    }

    void AnimationPlayer::SetSpeed(float speed)
    {
        m_Speed = speed;
    }

    float AnimationPlayer::GetSpeed() const
    {
        return m_Speed;
    }

    void AnimationPlayer::SetLooping(bool looping)
    {
        m_Looping = looping;
    }

    bool AnimationPlayer::IsLooping() const
    {
        return m_Looping;
    }

    bool AnimationPlayer::IsPlaying() const
    {
        return m_Playing;
    }

    float AnimationPlayer::GetDuration() const
    {
        if (!m_Current)
            return 0.0f;

        float duration = 0.0f;
        for (const auto& channel : m_Current->channels)
        {
            if (!channel.keyFrames.empty())
            {
                float last = channel.keyFrames.back().time;
                if (last > duration)
                    duration = last;
            }
        }
        return duration;
    }

    float AnimationPlayer::GetCurrentTime() const
    {
        return m_CurrentTime;
    }

    void AnimationPlayer::ApplyAnimation()
    {
        for (const auto& channel : m_Current->channels)
        {
            if (channel.keyFrames.empty())
                continue;

            const auto& keyframes = channel.keyFrames;

            // Only one keyframe — apply directly
            if (keyframes.size() == 1)
            {
                channel.propertyAccessor.Apply(keyframes[0].value);
                continue;
            }

            // Find the pair of keyframes surrounding m_CurrentTime
            size_t i = 0;
            for (size_t j = 1; j < keyframes.size(); ++j)
            {
                if (keyframes[j].time >= m_CurrentTime)
                {
                    i = j - 1;
                    break;
                }
                i = j;
            }

            size_t j = i + 1;
            if (j >= keyframes.size())
            {
                // Past last keyframe — apply last value
                channel.propertyAccessor.Apply(keyframes.back().value);
                continue;
            }

            float t0 = keyframes[i].time;
            float t1 = keyframes[j].time;
            float alpha = (t1 > t0) ? (m_CurrentTime - t0) / (t1 - t0) : 0.0f;

            Variant result = Interpolate(keyframes[i].value, keyframes[j].value, alpha);
            channel.propertyAccessor.Apply(result);
        }
    }

} // azer
