//
// Created by Trallkong on 2026/5/31.
//

#pragma once

#include "Base.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

namespace Azer
{

    struct Basis
    {
        glm::vec3 x = { 1.0, 0.0, 0.0 };
        glm::vec3 y = { 0.0, 1.0, 0.0 };
        glm::vec3 z = { 0.0, 0.0, 1.0 };
    };

    struct Rotation
    {
        explicit Rotation(Basis* basis) : m_Basis(basis)
        {
            UpdateFromBasis();
        }

        operator glm::vec3() const
        {
            return m_Euler;
        }

        Rotation& operator=(const glm::vec3& euler)
        {
            m_Euler = euler;
            UpdateBasis();
            return *this;
        }

        [[nodiscard]] glm::vec3 value() const { return m_Euler; }

    private:
        Basis* m_Basis;
        glm::vec3 m_Euler{};

        void UpdateFromBasis()
        {
            const auto rotMat = glm::mat3(m_Basis->x, m_Basis->y, m_Basis->z);
            const auto rotMat4 = glm::mat4(rotMat);
            float yaw, pitch, roll;
            glm::extractEulerAngleYXZ(rotMat4, yaw, pitch, roll);
            m_Euler = glm::vec3(pitch, yaw, roll);
        }

        void UpdateBasis() const {
            auto rotMat = glm::eulerAngleYXZ(m_Euler.y, m_Euler.x, m_Euler.z);
            m_Basis->x = glm::vec3(rotMat[0]);
            m_Basis->y = glm::vec3(rotMat[1]);
            m_Basis->z = glm::vec3(rotMat[2]);
        }
    };

    struct Transform3D
    {
        glm::vec3 position{0.0f, 0.0f, 0.0f};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};

        Basis basis{};
        Rotation rotation;

        Transform3D() : rotation(&basis) { }

        // rotation 持有指向本对象 basis 的裸指针，必须显式拷贝/移动以重新指向自己的 basis，
        // 否则默认浅拷贝会让 rotation.m_Basis 悬垂，后续写 rotation 会破坏源对象/未定义行为。
        Transform3D(const Transform3D& other)
            : position(other.position), scale(other.scale),
              basis(other.basis), rotation(&basis)
        {
            rotation = other.rotation.value();
        }

        Transform3D(Transform3D&& other) noexcept
            : position(std::move(other.position)), scale(std::move(other.scale)),
              basis(std::move(other.basis)), rotation(&basis)
        {
            rotation = other.rotation.value();
        }

        Transform3D& operator=(const Transform3D& other)
        {
            if (this != &other)
            {
                position = other.position;
                scale = other.scale;
                basis = other.basis;
                rotation = other.rotation.value();
            }
            return *this;
        }

        Transform3D& operator=(Transform3D&& other) noexcept
        {
            if (this != &other)
            {
                position = std::move(other.position);
                scale = std::move(other.scale);
                basis = std::move(other.basis);
                rotation = other.rotation.value();
            }
            return *this;
        }

        [[nodiscard]] glm::mat4 GetMatrix() const
        {
            const glm::mat4 translate_mat = glm::translate(glm::mat4(1.0), position);
            const auto rot_mat = glm::mat4(glm::mat3(basis.x, basis.y, basis.z));
            const glm::mat4 scale_mat = glm::scale(glm::mat4(1.0), scale);
            return translate_mat * rot_mat * scale_mat;
        }

        void RotateX(const float radians)
        {
            const float c = glm::cos(radians);
            const float s = glm::sin(radians);
            const glm::vec3 newY = basis.y * c + basis.z * s;
            const glm::vec3 newZ = -basis.y * s + basis.z * c;
            basis.y = newY;
            basis.z = newZ;
        }

        void RotateY(const float radians)
        {
            const float c = glm::cos(radians);
            const float s = glm::sin(radians);
            const glm::vec3 newX = basis.x * c + basis.z * s;
            const glm::vec3 newZ = -basis.x * s + basis.z * c;
            basis.x = newX;
            basis.z = newZ;
        }

        void RotateZ(const float radians)
        {
            const float c = glm::cos(radians);
            const float s = glm::sin(radians);
            const glm::vec3 newX = basis.x * c - basis.y * s;
            const glm::vec3 newY = basis.x * s + basis.y * c;
            basis.x = newX;
            basis.y = newY;
        }
    };
}
