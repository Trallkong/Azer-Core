//
// Created by Aier on 2026/5/31.
//
// AABB 包围盒 + 碰撞检测（2D / 3D）

#pragma once
#include "Base.h"
#include "glm/glm.hpp"
#include <algorithm>
#include <cmath>

namespace azer
{
    // ==================== AABB2D ====================
    struct AABB2D
    {
        glm::vec2 min{0.0f};
        glm::vec2 max{0.0f};

        AABB2D() = default;
        AABB2D(const glm::vec2& min, const glm::vec2& max) : min(min), max(max) {}
        AABB2D(float x, float y, float w, float h) : min(x, y), max(x + w, y + h) {}

        // 从中心和半尺寸构造
        static AABB2D FromCenter(const glm::vec2& center, const glm::vec2& halfSize)
        {
            return {center - halfSize, center + halfSize};
        }

        glm::vec2 GetCenter() const { return (min + max) * 0.5f; }
        glm::vec2 GetSize() const { return max - min; }
        glm::vec2 GetHalfSize() const { return GetSize() * 0.5f; }
        float GetWidth() const { return max.x - min.x; }
        float GetHeight() const { return max.y - min.y; }
        float GetArea() const { return GetWidth() * GetHeight(); }

        bool Contains(const glm::vec2& point) const
        {
            return point.x >= min.x && point.x <= max.x
                && point.y >= min.y && point.y <= max.y;
        }

        bool Contains(const AABB2D& other) const
        {
            return other.min.x >= min.x && other.max.x <= max.x
                && other.min.y >= min.y && other.max.y <= max.y;
        }

        bool Intersects(const AABB2D& other) const
        {
            return min.x <= other.max.x && max.x >= other.min.x
                && min.y <= other.max.y && max.y >= other.min.y;
        }

        // 合并
        AABB2D Union(const AABB2D& other) const
        {
            return {
                {std::min(min.x, other.min.x), std::min(min.y, other.min.y)},
                {std::max(max.x, other.max.x), std::max(max.y, other.max.y)}
            };
        }

        // 扩展包含点
        AABB2D ExpandToInclude(const glm::vec2& point) const
        {
            return {
                {std::min(min.x, point.x), std::min(min.y, point.y)},
                {std::max(max.x, point.x), std::max(max.y, point.y)}
            };
        }

        // 偏移
        AABB2D Offset(const glm::vec2& offset) const
        {
            return {min + offset, max + offset};
        }
    };

    // ==================== AABB3D ====================
    struct AABB3D
    {
        glm::vec3 min{0.0f};
        glm::vec3 max{0.0f};

        AABB3D() = default;
        AABB3D(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

        // 从中心和半尺寸构造
        static AABB3D FromCenter(const glm::vec3& center, const glm::vec3& halfSize)
        {
            return {center - halfSize, center + halfSize};
        }

        glm::vec3 GetCenter() const { return (min + max) * 0.5f; }
        glm::vec3 GetSize() const { return max - min; }
        glm::vec3 GetHalfSize() const { return GetSize() * 0.5f; }
        float GetVolume() const
        {
            auto s = GetSize();
            return s.x * s.y * s.z;
        }

        bool Contains(const glm::vec3& point) const
        {
            return point.x >= min.x && point.x <= max.x
                && point.y >= min.y && point.y <= max.y
                && point.z >= min.z && point.z <= max.z;
        }

        bool Contains(const AABB3D& other) const
        {
            return other.min.x >= min.x && other.max.x <= max.x
                && other.min.y >= min.y && other.max.y <= max.y
                && other.min.z >= min.z && other.max.z <= max.z;
        }

        bool Intersects(const AABB3D& other) const
        {
            return min.x <= other.max.x && max.x >= other.min.x
                && min.y <= other.max.y && max.y >= other.min.y
                && min.z <= other.max.z && max.z >= other.min.z;
        }

        AABB3D Union(const AABB3D& other) const
        {
            return {
                {std::min(min.x, other.min.x), std::min(min.y, other.min.y), std::min(min.z, other.min.z)},
                {std::max(max.x, other.max.x), std::max(max.y, other.max.y), std::max(max.z, other.max.z)}
            };
        }

        AABB3D ExpandToInclude(const glm::vec3& point) const
        {
            return {
                {std::min(min.x, point.x), std::min(min.y, point.y), std::min(min.z, point.z)},
                {std::max(max.x, point.x), std::max(max.y, point.y), std::max(max.z, point.z)}
            };
        }

        AABB3D Offset(const glm::vec3& offset) const
        {
            return {min + offset, max + offset};
        }
    };

    // ==================== 碰撞检测 ====================
    namespace Collision
    {
        // --- 2D ---

        // AABB vs AABB
        inline bool Intersects(const AABB2D& a, const AABB2D& b) { return a.Intersects(b); }

        // 点 vs AABB
        inline bool PointInAABB(const glm::vec2& point, const AABB2D& box) { return box.Contains(point); }

        // 圆 vs 圆
        inline bool CircleVsCircle(const glm::vec2& centerA, float radiusA,
                                    const glm::vec2& centerB, float radiusB)
        {
            float distSq = glm::dot(centerA - centerB, centerA - centerB);
            float radiiSum = radiusA + radiusB;
            return distSq <= radiiSum * radiiSum;
        }

        // 圆 vs AABB
        inline bool CircleVsAABB(const glm::vec2& center, float radius, const AABB2D& box)
        {
            // 找 AABB 上离圆心最近的点
            float closestX = std::clamp(center.x, box.min.x, box.max.x);
            float closestY = std::clamp(center.y, box.min.y, box.max.y);
            float dx = center.x - closestX;
            float dy = center.y - closestY;
            return (dx * dx + dy * dy) <= radius * radius;
        }

        // --- 3D ---

        // AABB vs AABB
        inline bool Intersects(const AABB3D& a, const AABB3D& b) { return a.Intersects(b); }

        // 点 vs AABB
        inline bool PointInAABB(const glm::vec3& point, const AABB3D& box) { return box.Contains(point); }

        // 球 vs 球
        inline bool SphereVsSphere(const glm::vec3& centerA, float radiusA,
                                    const glm::vec3& centerB, float radiusB)
        {
            float distSq = glm::dot(centerA - centerB, centerA - centerB);
            float radiiSum = radiusA + radiusB;
            return distSq <= radiiSum * radiiSum;
        }

        // 球 vs AABB
        inline bool SphereVsAABB(const glm::vec3& center, float radius, const AABB3D& box)
        {
            float closestX = std::clamp(center.x, box.min.x, box.max.x);
            float closestY = std::clamp(center.y, box.min.y, box.max.y);
            float closestZ = std::clamp(center.z, box.min.z, box.max.z);
            float dx = center.x - closestX;
            float dy = center.y - closestY;
            float dz = center.z - closestZ;
            return (dx * dx + dy * dy + dz * dz) <= radius * radius;
        }

        // 射线 vs AABB（用于 3D 拾取）
        // origin: 射线起点, dir: 射线方向（需归一化）, tOut: 交点距离
        // 返回: 是否相交
        inline bool RayVsAABB(const glm::vec3& origin, const glm::vec3& dir,
                               const AABB3D& box, float& tOut)
        {
            float tmin = -std::numeric_limits<float>::max();
            float tmax = std::numeric_limits<float>::max();

            for (int i = 0; i < 3; ++i)
            {
                float o = (i == 0) ? origin.x : (i == 1) ? origin.y : origin.z;
                float d = (i == 0) ? dir.x : (i == 1) ? dir.y : dir.z;
                float bmin = (i == 0) ? box.min.x : (i == 1) ? box.min.y : box.min.z;
                float bmax = (i == 0) ? box.max.x : (i == 1) ? box.max.y : box.max.z;

                if (std::abs(d) < 1e-8f)
                {
                    // 射线平行于 slab
                    if (o < bmin || o > bmax)
                        return false;
                }
                else
                {
                    float invD = 1.0f / d;
                    float t0 = (bmin - o) * invD;
                    float t1 = (bmax - o) * invD;
                    if (t0 > t1) std::swap(t0, t1);
                    tmin = std::max(tmin, t0);
                    tmax = std::min(tmax, t1);
                    if (tmin > tmax)
                        return false;
                }
            }

            tOut = (tmin >= 0) ? tmin : tmax;
            return tOut >= 0;
        }

        // 2D 射线 vs AABB
        inline bool RayVsAABB2D(const glm::vec2& origin, const glm::vec2& dir,
                                 const AABB2D& box, float& tOut)
        {
            float tmin = -std::numeric_limits<float>::max();
            float tmax = std::numeric_limits<float>::max();

            for (int i = 0; i < 2; ++i)
            {
                float o = (i == 0) ? origin.x : origin.y;
                float d = (i == 0) ? dir.x : dir.y;
                float bmin = (i == 0) ? box.min.x : box.min.y;
                float bmax = (i == 0) ? box.max.x : box.max.y;

                if (std::abs(d) < 1e-8f)
                {
                    if (o < bmin || o > bmax)
                        return false;
                }
                else
                {
                    float invD = 1.0f / d;
                    float t0 = (bmin - o) * invD;
                    float t1 = (bmax - o) * invD;
                    if (t0 > t1) std::swap(t0, t1);
                    tmin = std::max(tmin, t0);
                    tmax = std::min(tmax, t1);
                    if (tmin > tmax)
                        return false;
                }
            }

            tOut = (tmin >= 0) ? tmin : tmax;
            return tOut >= 0;
        }
    }
}
