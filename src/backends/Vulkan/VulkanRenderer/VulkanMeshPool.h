#pragma once

#include "Base.h"
#include "Mesh2D.h"

#include <unordered_map>
#include <string>

namespace Azer {

    class VulkanVertexBuffer;
    class VulkanIndexBuffer;

    struct MeshRenderData
    {
        Ref<VulkanVertexBuffer> Vbo;
        Ref<VulkanIndexBuffer> Ibo;

        ~MeshRenderData()
        {
            Vbo.reset();
            Ibo.reset();
        }
    };

    class VulkanMeshPool
    {
    public:
        VulkanMeshPool() = default;
        ~VulkanMeshPool();

        MeshRenderData& GetRenderData(const MeshType2D& type);
        MeshRenderData& GetRenderData(const std::string& filePath, uint32_t width, uint32_t height);

        void CleanUp();
    private:
        std::unordered_map<MeshType2D, MeshRenderData, MeshType2DHash> m_Map;
        std::unordered_map<std::string, MeshRenderData> m_TextureMeshMap;

        std::unordered_map<std::string, bool> m_UseState;
    };
}