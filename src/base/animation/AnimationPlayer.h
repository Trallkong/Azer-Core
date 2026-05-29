//
// Created by Trallkong on 2026/5/30.
//

#pragma once

#include "Base.h"
#include "Animation.h"

#include <string>
#include <vector>

namespace azer
{
    class AnimationPlayer
    {
    public:
        AnimationPlayer() = default;
        ~AnimationPlayer() = default;

        void AddAnimation(Animation animation);
        Animation* GetAnimation(const std::string& name);

        void Play(const std::string& name);
        void Stop();
        void Pause();
        void Resume();

        void Update(float delta);

        void SetSpeed(float speed);
        float GetSpeed() const;

        void SetLooping(bool looping);
        bool IsLooping() const;

        bool IsPlaying() const;
        float GetDuration() const;
        float GetCurrentTime() const;

    private:
        std::vector<Animation> m_Animations;

        Animation* m_Current = nullptr;
        float m_CurrentTime = 0.0f;
        float m_Speed = 1.0f;
        bool m_Looping = true;
        bool m_Playing = false;

        void ApplyAnimation();
    };
} // azer
