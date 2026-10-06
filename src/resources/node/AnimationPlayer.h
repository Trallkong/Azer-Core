//
// Created by csis on 2026/9/30.
//

#pragma once
#include "Node.h"
#include "Type.h"

namespace Azer
{
    struct KeyFrame
    {
        float Time = 0.0f;
        VariantValue Value;
    };

    class AnimationChannel
    {
        friend class AnimationObject;
    public:
        explicit AnimationChannel(VariantValue* property)
            : m_Property(property)
        {

        }

        void AddKeyFrame(const KeyFrame& keyFrame);
        void RemoveKeyFrame(float time);
    private:
        Variant m_Property;
        std::vector<KeyFrame> m_KeyFrames;

        uint32_t m_FrameIndex = 0;

        void UpdateProperty(float delta, float time);
    };

    class AnimationObject
    {
    public:
        explicit AnimationObject(std::string name)
            : Name(std::move(name)) {}

        bool Loop = false;
        std::string Name;

        void Process(float delta);

        void Play();
        void Stop();

        void AddChannel();
    private:
        std::vector<AnimationChannel> m_Channels;
        bool m_IsPlaying = false;
        float m_Time = 0.0f;
        float m_Length = 1.0f;
    };

    class AnimationPlayer : public Node
    {
    public:
        explicit AnimationPlayer(const std::string& name);
        ~AnimationPlayer() override;

        void Process(float delta) override;

        void Play(const std::string& name);
        void Stop();

        void AddAnimation(std::string name);
        void RemoveAnimation(const std::string& name);

        [[nodiscard]] const Ref<AnimationObject>& GetAnimation(const std::string& name) const;
        [[nodiscard]] bool IsPlaying() const { return m_PlayingAnimation != nullptr; }
    private:
        std::vector<Ref<AnimationObject>> m_Animations;
        Ref<AnimationObject> m_PlayingAnimation;
    };
}
