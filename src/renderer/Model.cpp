#include "azpch.h"
#include "Model.h"
#include "Texture.h"
#include "Renderer.h"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include "stb_image.h"

namespace Azer
{
    static glm::mat4 Mat4FromCgltf(const cgltf_float* data)
    {
        // cgltf stores matrices in column-major order (same as GLM)
        glm::mat4 result;
        for (int c = 0; c < 4; ++c)
            for (int r = 0; r < 4; ++r)
                result[c][r] = data[c * 4 + r];
        return result;
    }

    static std::string GetGLTFDir(const std::string& filepath)
    {
        auto pos = filepath.find_last_of("/\\");
        if (pos == std::string::npos)
            return "./";
        return filepath.substr(0, pos + 1);
    }

    Scope<Model> Model::LoadGLTF(const std::string& filepath, Renderer* renderer)
    {
        cgltf_options options{};
        cgltf_data* data = nullptr;

        cgltf_result result = cgltf_parse_file(&options, filepath.c_str(), &data);
        if (result != cgltf_result_success)
        {
            AZ_CORE_ERROR("Failed to parse glTF file: {0}", filepath);
            return nullptr;
        }

        result = cgltf_load_buffers(&options, data, filepath.c_str());
        if (result != cgltf_result_success)
        {
            AZ_CORE_ERROR("Failed to load glTF buffers: {0}", filepath);
            cgltf_free(data);
            return nullptr;
        }

        auto model = CreateScope<Model>();

        // Load textures
        if (data->textures_count > 0)
        {
            model->m_Textures.resize(data->textures_count);

            if (renderer)
            {
                std::string baseDir = GetGLTFDir(filepath);

                for (cgltf_size i = 0; i < data->textures_count; ++i)
                {
                    const auto& srcTex = data->textures[i];
                    if (!srcTex.image || !srcTex.image->uri)
                        continue;

                    std::string imgPath = baseDir + srcTex.image->uri;

                    int w, h, channels;
                    unsigned char* pixels = stbi_load(imgPath.c_str(), &w, &h, &channels, 4);
                    if (!pixels)
                    {
                        AZ_CORE_WARN("Failed to load texture: {0}", imgPath);
                        continue;
                    }

                    model->m_Textures[i] = Texture::Create(*renderer, pixels,
                        static_cast<uint32_t>(w), static_cast<uint32_t>(h));
                    stbi_image_free(pixels);
                }
            }
        }

        // Load materials
        if (data->materials_count > 0)
        {
            model->m_Materials.reserve(data->materials_count);
            for (cgltf_size i = 0; i < data->materials_count; ++i)
            {
                const auto& src = data->materials[i];
                Material mat{};
                mat.BaseColorFactor = glm::vec4(
                    src.pbr_metallic_roughness.base_color_factor[0],
                    src.pbr_metallic_roughness.base_color_factor[1],
                    src.pbr_metallic_roughness.base_color_factor[2],
                    src.pbr_metallic_roughness.base_color_factor[3]
                );
                mat.MetallicFactor = src.pbr_metallic_roughness.metallic_factor;
                mat.RoughnessFactor = src.pbr_metallic_roughness.roughness_factor;

                if (src.pbr_metallic_roughness.base_color_texture.texture)
                {
                    cgltf_size texIdx = src.pbr_metallic_roughness.base_color_texture.texture -
                                        data->textures;
                    mat.BaseColorTexIndex = static_cast<int32_t>(texIdx);
                }
                if (src.pbr_metallic_roughness.metallic_roughness_texture.texture)
                {
                    cgltf_size texIdx = src.pbr_metallic_roughness.metallic_roughness_texture.texture -
                                        data->textures;
                    mat.MetallicRoughnessTexIndex = static_cast<int32_t>(texIdx);
                }

                model->m_Materials.push_back(mat);
            }
        }

        // Load meshes
        std::unordered_map<const cgltf_mesh*, uint32_t> meshToStart;

        if (data->meshes_count > 0)
        {
            for (cgltf_size mi = 0; mi < data->meshes_count; ++mi)
            {
                const auto& srcMesh = data->meshes[mi];
                meshToStart[&srcMesh] = static_cast<uint32_t>(model->m_Meshes.size());

                // Each primitive becomes a separate Mesh
                for (cgltf_size pi = 0; pi < srcMesh.primitives_count; ++pi)
                {
                    const auto& prim = srcMesh.primitives[pi];

                    Mesh engineMesh{};
                    engineMesh.MaterialIndex = static_cast<uint32_t>(prim.material
                        ? (prim.material - data->materials)
                        : 0);

                    // Find required attributes
                    const cgltf_accessor* posAccessor = nullptr;
                    const cgltf_accessor* normalAccessor = nullptr;
                    const cgltf_accessor* texCoordAccessor = nullptr;

                    for (cgltf_size ai = 0; ai < prim.attributes_count; ++ai)
                    {
                        const auto& attr = prim.attributes[ai];
                        if (attr.type == cgltf_attribute_type_position)
                            posAccessor = attr.data;
                        else if (attr.type == cgltf_attribute_type_normal)
                            normalAccessor = attr.data;
                        else if (attr.type == cgltf_attribute_type_texcoord)
                            texCoordAccessor = attr.data;
                    }

                    if (!posAccessor)
                    {
                        AZ_CORE_WARN("Mesh primitive has no position attribute, skipping");
                        continue;
                    }

                    cgltf_size vertexCount = posAccessor->count;
                    engineMesh.Vertices.resize(vertexCount);

                    // Read positions
                    for (cgltf_size vi = 0; vi < vertexCount; ++vi)
                    {
                        cgltf_float pos[3];
                        cgltf_accessor_read_float(posAccessor, vi, pos, 3);
                        engineMesh.Vertices[vi].Position = glm::vec3(pos[0], pos[1], pos[2]);
                    }

                    // Read normals
                    if (normalAccessor)
                    {
                        for (cgltf_size vi = 0; vi < vertexCount; ++vi)
                        {
                            cgltf_float n[3];
                            cgltf_accessor_read_float(normalAccessor, vi, n, 3);
                            engineMesh.Vertices[vi].Normal = glm::vec3(n[0], n[1], n[2]);
                        }
                    }

                    // Read texcoords
                    if (texCoordAccessor)
                    {
                        for (cgltf_size vi = 0; vi < vertexCount; ++vi)
                        {
                            cgltf_float uv[2];
                            cgltf_accessor_read_float(texCoordAccessor, vi, uv, 2);
                            engineMesh.Vertices[vi].TexCoord = glm::vec2(uv[0], uv[1]);
                        }
                    }

                    // Read indices
                    if (prim.indices)
                    {
                        cgltf_size indexCount = prim.indices->count;
                        engineMesh.Indices.resize(indexCount);
                        for (cgltf_size ii = 0; ii < indexCount; ++ii)
                        {
                            cgltf_uint idx;
                            cgltf_accessor_read_uint(prim.indices, ii, &idx, 1);
                            engineMesh.Indices[ii] = static_cast<uint32_t>(idx);
                        }
                    }

                    model->m_Meshes.push_back(std::move(engineMesh));
                }
            }
        }

        // Build root nodes from scene
        if (data->scene)
        {
            model->m_RootNodes.reserve(data->scene->nodes_count);
            for (cgltf_size i = 0; i < data->scene->nodes_count; ++i)
            {
                cgltf_size nodeIdx = data->scene->nodes[i] - data->nodes;
                model->m_RootNodes.push_back(static_cast<int32_t>(nodeIdx));
            }
        }
        else if (data->scenes_count > 0 && data->scenes[0].nodes_count > 0)
        {
            model->m_RootNodes.reserve(data->scenes[0].nodes_count);
            for (cgltf_size i = 0; i < data->scenes[0].nodes_count; ++i)
            {
                cgltf_size nodeIdx = data->scenes[0].nodes[i] - data->nodes;
                model->m_RootNodes.push_back(static_cast<int32_t>(nodeIdx));
            }
        }

        // Build node hierarchy
        if (data->nodes_count > 0)
        {
            model->m_Nodes.resize(data->nodes_count);
            for (cgltf_size ni = 0; ni < data->nodes_count; ++ni)
            {
                const auto& srcNode = data->nodes[ni];
                auto& node = model->m_Nodes[ni];

                if (srcNode.name)
                    node.Name = srcNode.name;

                // Compute local transform
                if (srcNode.has_matrix)
                {
                    node.LocalTransform = Mat4FromCgltf(srcNode.matrix);
                }
                else
                {
                    glm::mat4 t(1.0f), r(1.0f), s(1.0f);
                    if (srcNode.has_translation)
                        t = glm::translate(glm::mat4(1.0f),
                            glm::vec3(srcNode.translation[0], srcNode.translation[1], srcNode.translation[2]));
                    if (srcNode.has_rotation)
                    {
                        glm::quat q(srcNode.rotation[3], srcNode.rotation[0],
                                    srcNode.rotation[1], srcNode.rotation[2]);
                        r = glm::mat4_cast(q);
                    }
                    if (srcNode.has_scale)
                        s = glm::scale(glm::mat4(1.0f),
                            glm::vec3(srcNode.scale[0], srcNode.scale[1], srcNode.scale[2]));
                    node.LocalTransform = t * r * s;
                }

                if (srcNode.mesh)
                {
                    auto it = meshToStart.find(srcNode.mesh);
                    if (it != meshToStart.end())
                    {
                        node.MeshStart = static_cast<int32_t>(it->second);
                        // Count how many primitives this mesh has
                        auto* nextMesh = srcNode.mesh + 1;
                        auto nextIt = meshToStart.find(nextMesh);
                        int32_t endIdx = nextIt != meshToStart.end()
                            ? static_cast<int32_t>(nextIt->second)
                            : static_cast<int32_t>(model->m_Meshes.size());
                        node.MeshCount = endIdx - node.MeshStart;
                    }
                }

                if (srcNode.children_count > 0)
                {
                    node.Children.reserve(srcNode.children_count);
                    for (cgltf_size ci = 0; ci < srcNode.children_count; ++ci)
                    {
                        cgltf_size childIdx = srcNode.children[ci] - data->nodes;
                        node.Children.push_back(static_cast<int32_t>(childIdx));
                    }
                }
            }
        }

        cgltf_free(data);

        AZ_CORE_INFO("Loaded glTF model: {0} (meshes={1}, nodes={2}, materials={3})",
                      filepath, model->m_Meshes.size(), model->m_Nodes.size(), model->m_Materials.size());
        return model;
    }

    void Model::TraverseNode(int32_t nodeIdx, const glm::mat4& parentTransform,
                             const std::function<void(const ModelNode&, uint32_t meshIdx, const Mesh&, const glm::mat4&)>& callback) const
    {
        if (nodeIdx < 0 || nodeIdx >= static_cast<int32_t>(m_Nodes.size()))
            return;

        const auto& node = m_Nodes[nodeIdx];
        glm::mat4 world = parentTransform * node.LocalTransform;

        if (node.MeshStart >= 0)
        {
            int32_t end = node.MeshStart + node.MeshCount;
            for (int32_t i = node.MeshStart; i < end && i < static_cast<int32_t>(m_Meshes.size()); ++i)
                callback(node, static_cast<uint32_t>(i), m_Meshes[i], world);
        }

        for (int32_t childIdx : node.Children)
            TraverseNode(childIdx, world, callback);
    }
}
