//
// Created by Trallkong on 2026/4/18.
//

#ifndef AZER_LAYER_H
#define AZER_LAYER_H

#include <string>

#include "Event.h"

namespace azer
{
    class Layer {
    public:
        explicit Layer(const std::string& name = "New Layer")
            : m_Name(name)
        {
        }
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate(float delta) {}
        virtual void OnDraw() {}
        virtual void OnEvent(Event& event) {}
        virtual void OnImGuiRender() {}

        const std::string& GetName() const { return m_Name; }
    private:
        std::string m_Name;
    };
}

#endif //AZER_LAYER_H
