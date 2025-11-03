// ============================================
// game/GameObject.h
// ============================================
#pragma once

#include <vulkan/vulkan.h>
#include "graphics/Transform.h"

class GameObject {
public:
    GameObject();

    // Actualiza el estado del objeto (por frame)
    void update(float deltaTime);

    // Acceso al transform
    Transform& getTransform();
    const Transform& getTransform() const;

    // Asignar buffers de vértices e índices
    void setBuffers(VkBuffer vertexBuffer, VkBuffer indexBuffer, uint32_t indexCount);

    // Getters
    VkBuffer getVertexBuffer() const;
    VkBuffer getIndexBuffer() const;
    uint32_t getIndexCount() const;
    bool hasBuffers() const;

private:
    Transform transform;     // Posición, rotación, escala

    VkBuffer vertexBuffer;   // Buffer de vértices
    VkBuffer indexBuffer;    // Buffer de índices
    uint32_t indexCount;     // Número de índices
};
