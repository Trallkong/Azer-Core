//
// Created by Trallkong on 2026/4/18.
//

#pragma once

#include <algorithm>
#include <vector>
#include "Layer.h"

namespace azer
{
    class LayerStack {
    public:
        LayerStack();
        ~LayerStack();

        void PushLayer(Layer* layer);
        void PopLayer();
        void PushOverlay(Layer* overlay);
        void PopOverlay();
        Layer* PeekLayer() const;
        Layer* PeekOverlay() const;
        void Erase(Layer* layer);

        const std::vector<Layer*>& GetLayers() const { return m_Layers; }

        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }
        std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
        std::vector<Layer*>::const_iterator end() const { return m_Layers.end(); }
        std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
        std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }
    private:
        int pos = 0;
        std::vector<Layer*> m_Layers;
    };
}

