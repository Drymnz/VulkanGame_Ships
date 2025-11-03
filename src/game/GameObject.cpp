#include "game/GameObject.h"

// Constructor por defecto
GameObject::GameObject() 
    : vertexBuffer(VK_NULL_HANDLE)
    , indexBuffer(VK_NULL_HANDLE)
    , indexCount(0) {}

// Método update: se llama cada frame.
void GameObject::update(float deltaTime) {
    // Lógica específica del objeto (vacía por ahora)
}

// Devuelve una referencia al transform del objeto
Transform& GameObject::getTransform() {
    return transform;
}

// Devuelve una referencia constante al transform
const Transform& GameObject::getTransform() const {
    return transform;
}

// Establece los buffers de vértices e índices para este objeto
void GameObject::setBuffers(VkBuffer vb, VkBuffer ib, uint32_t count) {
    vertexBuffer = vb;
    indexBuffer = ib;
    indexCount = count;
}

// Getters para los buffers
VkBuffer GameObject::getVertexBuffer() const {
    return vertexBuffer;
}

VkBuffer GameObject::getIndexBuffer() const {
    return indexBuffer;
}

uint32_t GameObject::getIndexCount() const {
    return indexCount;
}

bool GameObject::hasBuffers() const {
    return vertexBuffer != VK_NULL_HANDLE && indexBuffer != VK_NULL_HANDLE && indexCount > 0;
}