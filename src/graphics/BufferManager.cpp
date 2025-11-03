#include "graphics/BufferManager.h"
#include <stdexcept>
#include <vector>
#include <cstring>
#include <iostream>

BufferManager::BufferManager()
    : device(VK_NULL_HANDLE)
    , physicalDevice(VK_NULL_HANDLE)
    , vertexBuffer(VK_NULL_HANDLE)
    , vertexBufferMemory(VK_NULL_HANDLE)
    , indexBuffer(VK_NULL_HANDLE)
    , indexBufferMemory(VK_NULL_HANDLE)
    , indexCount(0) {}

BufferManager::~BufferManager() {
    destroy();
}

void BufferManager::init(VkDevice dev, VkPhysicalDevice physDev) {
    device = dev;
    physicalDevice = physDev;
}

void BufferManager::createVertexBuffer() {
    std::vector<Vertex> vertices = {
        {{0.0f, 0.0f}}
    };

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear vertex buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Error al asignar memoria del vertex buffer");
    }

    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

    void* data;
    vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferInfo.size);
    vkUnmapMemory(device, vertexBufferMemory);
}

void BufferManager::updateVertexPosition(float x, float y) {
    if (device == VK_NULL_HANDLE || vertexBuffer == VK_NULL_HANDLE) return;
    
    Vertex vertex = {{x, y}};
    
    void* data;
    vkMapMemory(device, vertexBufferMemory, 0, sizeof(Vertex), 0, &data);
    memcpy(data, &vertex, sizeof(vertex));
    vkUnmapMemory(device, vertexBufferMemory);
}

uint32_t BufferManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("No se encontró tipo de memoria adecuado");
}

void BufferManager::destroy() {
    if (device == VK_NULL_HANDLE) return;

    // Destruir buffers individuales
    for (auto& pair : modelBuffers) {
        if (pair.second.indexBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(device, pair.second.indexBuffer, nullptr);
        }
        if (pair.second.indexBufferMemory != VK_NULL_HANDLE) {
            vkFreeMemory(device, pair.second.indexBufferMemory, nullptr);
        }
        if (pair.second.vertexBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(device, pair.second.vertexBuffer, nullptr);
        }
        if (pair.second.vertexBufferMemory != VK_NULL_HANDLE) {
            vkFreeMemory(device, pair.second.vertexBufferMemory, nullptr);
        }
    }
    modelBuffers.clear();

    // Destruir buffer genérico si existe
    if (indexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, indexBuffer, nullptr);
        indexBuffer = VK_NULL_HANDLE;
    }
    if (indexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, indexBufferMemory, nullptr);
        indexBufferMemory = VK_NULL_HANDLE;
    }
    if (vertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, vertexBuffer, nullptr);
        vertexBuffer = VK_NULL_HANDLE;
    }
    if (vertexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, vertexBufferMemory, nullptr);
        vertexBufferMemory = VK_NULL_HANDLE;
    }
}

void BufferManager::createVertexBuffer3D(const std::vector<Vertex3D>& vertices) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear vertex buffer 3D");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Error al asignar memoria del vertex buffer 3D");
    }

    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

    void* data;
    vkMapMemory(device, vertexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, vertexBufferMemory);
    
    std::cout << "Vertex buffer 3D creado con " << vertices.size() << " vértices" << std::endl;
}

void BufferManager::createIndexBuffer(const std::vector<uint32_t>& indices) {
    indexCount = static_cast<uint32_t>(indices.size());
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &indexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear index buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, indexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &indexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Error al asignar memoria del index buffer");
    }

    vkBindBufferMemory(device, indexBuffer, indexBufferMemory, 0);

    void* data;
    vkMapMemory(device, indexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, indexBufferMemory);
    
    std::cout << "Index buffer creado con " << indices.size() << " índices" << std::endl;
}

// NUEVAS FUNCIONES PARA MÚLTIPLES MODELOS

ModelBuffers BufferManager::createModelBuffers(const std::vector<Vertex3D>& vertices, 
                                               const std::vector<uint32_t>& indices,
                                               const std::string& name) {
    ModelBuffers buffers{};
    
    // Crear vertex buffer
    VkDeviceSize vBufferSize = sizeof(vertices[0]) * vertices.size();
    
    VkBufferCreateInfo vBufferInfo{};
    vBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vBufferInfo.size = vBufferSize;
    vBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &vBufferInfo, nullptr, &buffers.vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear vertex buffer para " + name);
    }

    VkMemoryRequirements vMemReq;
    vkGetBufferMemoryRequirements(device, buffers.vertexBuffer, &vMemReq);

    VkMemoryAllocateInfo vAllocInfo{};
    vAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vAllocInfo.allocationSize = vMemReq.size;
    vAllocInfo.memoryTypeIndex = findMemoryType(vMemReq.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(device, &vAllocInfo, nullptr, &buffers.vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Error al asignar memoria vertex para " + name);
    }

    vkBindBufferMemory(device, buffers.vertexBuffer, buffers.vertexBufferMemory, 0);

    void* vData;
    vkMapMemory(device, buffers.vertexBufferMemory, 0, vBufferSize, 0, &vData);
    memcpy(vData, vertices.data(), (size_t)vBufferSize);
    vkUnmapMemory(device, buffers.vertexBufferMemory);

    // Crear index buffer
    buffers.indexCount = static_cast<uint32_t>(indices.size());
    VkDeviceSize iBufferSize = sizeof(indices[0]) * indices.size();

    VkBufferCreateInfo iBufferInfo{};
    iBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    iBufferInfo.size = iBufferSize;
    iBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    iBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &iBufferInfo, nullptr, &buffers.indexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear index buffer para " + name);
    }

    VkMemoryRequirements iMemReq;
    vkGetBufferMemoryRequirements(device, buffers.indexBuffer, &iMemReq);

    VkMemoryAllocateInfo iAllocInfo{};
    iAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    iAllocInfo.allocationSize = iMemReq.size;
    iAllocInfo.memoryTypeIndex = findMemoryType(iMemReq.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(device, &iAllocInfo, nullptr, &buffers.indexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Error al asignar memoria index para " + name);
    }

    vkBindBufferMemory(device, buffers.indexBuffer, buffers.indexBufferMemory, 0);

    void* iData;
    vkMapMemory(device, buffers.indexBufferMemory, 0, iBufferSize, 0, &iData);
    memcpy(iData, indices.data(), (size_t)iBufferSize);
    vkUnmapMemory(device, buffers.indexBufferMemory);

    // Guardar en el mapa
    modelBuffers[name] = buffers;

    std::cout << "Modelo '" << name << "' cargado: " 
              << vertices.size() << " vértices, " 
              << indices.size() << " índices" << std::endl;

    return buffers;
}

const ModelBuffers* BufferManager::getModelBuffers(const std::string& name) const {
    auto it = modelBuffers.find(name);
    if (it != modelBuffers.end()) {
        return &it->second;
    }
    return nullptr;
}