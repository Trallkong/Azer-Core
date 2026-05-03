//
// Created by Trallkong on 2026/4/18.
//

#ifndef LEARNSDL_LAYERSTACK_H
#define LEARNSDL_LAYERSTACK_H

#include <vector>
#include "Layer.h"

namespace azer
{
    class LayerStack {
    public:
        LayerStack();
        ~LayerStack();

        void PushLayer(Layer* layer);
        Layer* PopLayer();
        void PushOverlay(Layer* overlay);
        Layer* PopOverlay();

        std::vector<Layer*> GetLayers() const { return m_Layers; }

        std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator end() { return m_Layers.end(); }
        std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
        std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

    private:
        int pos = 0;
        std::vector<Layer*> m_Layers;
    };
}

#endif //LEARNSDL_LAYERSTACK_H
