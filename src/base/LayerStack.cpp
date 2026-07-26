//
// Created by Trallkong on 2026/4/18.
//

#include "azpch.h"
#include "LayerStack.h"

#include <algorithm>

namespace Azer
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

    void LayerStack::PopLayer()
    {
        assert(pos > 0);
        m_Layers.erase(m_Layers.begin() + pos - 1);
        pos--;
    }

    void LayerStack::PushOverlay(Layer* overlay)
    {
        m_Layers.push_back(overlay);
    }

    void LayerStack::PopOverlay()
    {
        assert(pos < m_Layers.size());
        m_Layers.pop_back();
    }

    Layer* LayerStack::PeekLayer() const
    {
        assert(pos > 0);
        return m_Layers[pos - 1];
    }

    Layer* LayerStack::PeekOverlay() const
    {
        assert(pos < m_Layers.size());
        return m_Layers.back();
    }

    void LayerStack::Erase(Layer* layer)
    {
        if (const auto it = std::ranges::find(m_Layers, layer); it != m_Layers.end())
        {
            if (it - m_Layers.begin() < pos)
                pos--;
            m_Layers.erase(it);
        }
    }
}