#include "graphics/BufferManager.h"
#include <stdexcept>
#include <vector>
#include <cstring>

BufferManager::BufferManager()
    : device(VK_NULL_HANDLE)
    , physicalDevice(VK_NULL_HANDLE)
    , vertexBuffer(VK_NULL_HANDLE)
    , vertexBufferMemory(VK_NULL_HANDLE) {}

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

    if (vertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, vertexBuffer, nullptr);
        vertexBuffer = VK_NULL_HANDLE;
    }
    if (vertexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device, vertexBufferMemory, nullptr);
        vertexBufferMemory = VK_NULL_HANDLE;
    }
}