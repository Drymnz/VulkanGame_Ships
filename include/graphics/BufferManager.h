#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <vulkan/vulkan.h>
#include "Vertex.h"

class BufferManager {
public:
    BufferManager();
    ~BufferManager();

    void init(VkDevice device, VkPhysicalDevice physicalDevice);
    void createVertexBuffer();
    void updateVertexPosition(float x, float y);
    void destroy();

    VkBuffer getVertexBuffer() const { return vertexBuffer; }

private:
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
};

#endif