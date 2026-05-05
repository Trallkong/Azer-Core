//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "LayerStack.h"

namespace azer
{
    LayerStack::LayerStack()
    {
        m_Layers.reserve(10);
    }

    LayerStack::~LayerStack()
    {
        m_Layers.clear();
    }

    void LayerStack::PushLayer(Layer* layer)
    {
        m_Layers.insert(m_Layers.begin() + pos, layer);
        pos++;
    }

    Layer* LayerStack::PopLayer()
    {
        assert(pos > 0);
        Layer* layer = m_Layers[pos - 1];
        m_Layers.erase(m_Layers.begin() + pos - 1);
        pos--;
        return layer;
    }

    void LayerStack::PushOverlay(Layer* overlay)
    {
        m_Layers.push_back(overlay);
    }

    Layer* LayerStack::PopOverlay()
    {
        assert(pos < m_Layers.size());
        Layer* layer = m_Layers.back();
        m_Layers.pop_back();
        return layer;
    }
}