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
        float Time;
        Numeric Value;
    };

    class AnimationChannel
    {
        friend class AnimationObject;
    public:
        explicit AnimationChannel(Numeric* property)
            : m_Property(property)
        {

        }

        void AddKeyFrame(const KeyFrame& keyFrame);
        void RemoveKeyFrame(float time);
    private:
        Numeric* m_Property;
        std::vector<KeyFrame> m_KeyFrames;

        uint32_t m_FrameIndex = 0;

        void UpdateProperty(float delta, float time);
    };

    class AnimationObject
    {
    public:
        bool Loop = false;
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

    private:
        float m_AccumulatedTime = 0.0f;

        template<typename C, typename M>
        static void SetMember(C& object, M C::* member, const M& value) { object.*member = value; }
    };
}
