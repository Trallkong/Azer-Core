//
// Created by Trallkong on 2026/4/18.
//

#ifndef LEARNSDL_IMGUILAYER_H
#define LEARNSDL_IMGUILAYER_H

#include "Layer.h"

namespace azer
{
    class ImGuiLayer : public Layer {
    public:
        ImGuiLayer()
            : Layer("ImGuiLayer")
        {
        }
        ~ImGuiLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float deltaTime) override;
        void OnDraw() override;
        void OnEvent(Event& event) override;
        void OnImGuiRender() override;

        static void Begin();
        static void End();
    };
}

#endif //LEARNSDL_IMGUILAYER_H
