#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "Vertex.h"  // <-- Incluir en lugar de redefinir

class BufferManager {
public:
    BufferManager();
    ~BufferManager();

    void init(VkDevice dev, VkPhysicalDevice physDev);
    void createVertexBuffer();
    void createVertexBuffer3D(const std::vector<Vertex3D>& vertices);
    void createIndexBuffer(const std::vector<uint32_t>& indices);
    void updateVertexPosition(float x, float y);
    void destroy();

    VkBuffer getVertexBuffer() const { return vertexBuffer; }
    VkBuffer getIndexBuffer() const { return indexBuffer; }
    uint32_t getIndexCount() const { return indexCount; }

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};