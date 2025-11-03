#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <map>
#include "Vertex.h"

// Estructura para almacenar buffers de un modelo
struct ModelBuffers {
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
    uint32_t indexCount = 0;
};

class BufferManager {
public:
    BufferManager();
    ~BufferManager();

    void init(VkDevice dev, VkPhysicalDevice physDev);
    
    // Métodos originales (para compatibilidad)
    void createVertexBuffer();
    void createVertexBuffer3D(const std::vector<Vertex3D>& vertices);
    void createIndexBuffer(const std::vector<uint32_t>& indices);
    void updateVertexPosition(float x, float y);
    
    // Nuevos métodos para múltiples modelos
    ModelBuffers createModelBuffers(const std::vector<Vertex3D>& vertices,
                                   const std::vector<uint32_t>& indices,
                                   const std::string& name);
    const ModelBuffers* getModelBuffers(const std::string& name) const;

    void destroy();

    VkBuffer getVertexBuffer() const { return vertexBuffer; }
    VkBuffer getIndexBuffer() const { return indexBuffer; }
    uint32_t getIndexCount() const { return indexCount; }

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    
    // Buffers genéricos (compatibilidad con código anterior)
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;
    
    // Mapa de buffers por modelo
    std::map<std::string, ModelBuffers> modelBuffers;

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

#endif // BUFFER_MANAGER_H