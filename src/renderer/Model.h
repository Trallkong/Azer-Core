#pragma once
#include "Base.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"

namespace azer { class Renderer; }

namespace azer
{
    struct ModelNode
    {
        std::string Name;
        glm::mat4 LocalTransform = glm::mat4(1.0f);
        int32_t MeshStart = -1;
        int32_t MeshCount = 0;
        std::vector<int32_t> Children;
    };

    class Model
    {
    public:
        static Scope<Model> LoadGLTF(const std::string& filepath, Renderer* renderer = nullptr);

        const std::vector<ModelNode>& GetNodes() const { return m_Nodes; }
        const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }
        const std::vector<Material>& GetMaterials() const { return m_Materials; }
        const std::vector<Ref<Texture>>& GetTextures() const { return m_Textures; }

        void Traverse(const std::function<void(const ModelNode&, uint32_t meshIdx, const Mesh&, const glm::mat4&)>& callback) const
        {
            if (m_Nodes.empty()) return;
            glm::mat4 identity(1.0f);

            if (m_RootNodes.empty())
            {
                for (int32_t i = 0; i < static_cast<int32_t>(m_Nodes.size()); ++i)
                    TraverseNode(i, identity, callback);
            }
            else
            {
                for (int32_t rootIdx : m_RootNodes)
                    TraverseNode(rootIdx, identity, callback);
            }
        }

    private:
        void TraverseNode(int32_t nodeIdx, const glm::mat4& parentTransform,
                          const std::function<void(const ModelNode&, uint32_t meshIdx, const Mesh&, const glm::mat4&)>& callback) const;

        std::vector<ModelNode> m_Nodes;
        std::vector<Mesh> m_Meshes;
        std::vector<Material> m_Materials;
        std::vector<Ref<Texture>> m_Textures;
        std::vector<int32_t> m_RootNodes;
    };
}

