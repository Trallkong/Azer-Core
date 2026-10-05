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
        Variant Value;
    };

    class AnimationChannel
    {
        friend class AnimationObject;
    public:
        explicit AnimationChannel(const VariantValue& property)
            : m_Property(property)
        {

        }

        void AddKeyFrame(const KeyFrame& keyFrame);
        void RemoveKeyFrame(float time);

        [[nodiscard]] const Variant& GetCurrentProperty() const { return m_Property; }
    private:
        Variant m_Property;
        std::vector<KeyFrame> m_KeyFrames;

        uint32_t m_FrameIndex = 0;

        void UpdateProperty(float delta, float time);
    };

    class AnimationObject
    {
    public:
        explicit AnimationObject(std::string name, const bool isLoop = false)
            : Loop(isLoop), Name(std::move(name)) {}

        bool Loop = false;
        std::string Name;

        void Play();
    private:
        std::vector<AnimationChannel> m_Channels;
    };

    // 论文点
    class AnimationPlayer : public Node
    {
    public:
        explicit AnimationPlayer(const std::string& name);
        ~AnimationPlayer() override;

        void Process(float delta) override;

        void Play(const std::string& name);
        void CreateAnimation(std::string name);
    private:
        std::vector<AnimationObject> m_Animations;
    };
}
