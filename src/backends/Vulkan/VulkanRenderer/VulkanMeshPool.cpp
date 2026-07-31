#include "azpch.h"
#include "VulkanMeshPool.h"

#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"

namespace Azer {

    VulkanMeshPool::~VulkanMeshPool()
    {
        m_Map.clear();
        m_TextureMeshMap.clear();
        m_UseState.clear();
    }

    MeshRenderData& Azer::VulkanMeshPool::GetRenderData(const MeshType2D& type)
    {
        if (m_Map.count(type))
        {
            return m_Map.at(type);
        }
        else
        {
            switch (type)
            {
            case MeshType2D::QuadMesh:
                QuadMesh mesh;
                mesh.SetSize({1, 1});
                MeshRenderData data = {
                    CreateRef<VulkanVertexBuffer>(QuadMesh::QUAD_MESH_VERICES_SIZE),
                    CreateRef<VulkanIndexBuffer>(QuadMesh::QUAD_MESH_INDICES_SIZE)
                };
                data.Vbo->Upload(mesh.GetVertices());
                data.Ibo->Upload(mesh.GetIndices());
                m_Map[type] = data;
                break;
            }

            return m_Map.at(type);
        }
    }

    MeshRenderData &VulkanMeshPool::GetRenderData(const std::string &filePath, uint32_t width, uint32_t height)
    {
        if (m_TextureMeshMap.count(filePath))
        {
            m_UseState[filePath] = true;
            return m_TextureMeshMap[filePath];
        }
        else
        {
            QuadMesh mesh;
            mesh.SetSize({width, height});
            MeshRenderData data = {
                CreateRef<VulkanVertexBuffer>(QuadMesh::QUAD_MESH_VERICES_SIZE),
                CreateRef<VulkanIndexBuffer>(QuadMesh::QUAD_MESH_INDICES_SIZE)
            };
            data.Vbo->Upload(mesh.GetVertices());
            data.Ibo->Upload(mesh.GetIndices());
            m_TextureMeshMap.emplace(filePath, data);

            m_UseState[filePath] = true;

            return m_TextureMeshMap[filePath];
        }
    }

    void VulkanMeshPool::CleanUp()
    {
        std::vector<std::string> toClean;

        for (auto& state: m_UseState)
        {
            if (state.second == false) {
                toClean.push_back(state.first);
            }
        }

        for (auto& s : toClean)
        {
            m_TextureMeshMap.erase(s);
            m_UseState.erase(s);
        }

        for (auto& state: m_UseState)
        {
            state.second = false;
        }
    }
}
