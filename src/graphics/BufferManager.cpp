#include "graphics/BufferManager.h"
#include <stdexcept>
#include <vector>
#include <cstring>
#include <iostream>

// Constructor: inicializa todos los handles de Vulkan como nulos
BufferManager::BufferManager()
    : device(VK_NULL_HANDLE)
    , physicalDevice(VK_NULL_HANDLE)
    , vertexBuffer(VK_NULL_HANDLE)
    , vertexBufferMemory(VK_NULL_HANDLE)
    , indexBuffer(VK_NULL_HANDLE)
    , indexBufferMemory(VK_NULL_HANDLE)
    , indexCount(0) {}

// Destructor: destruye los recursos asignados
BufferManager::~BufferManager() {
    destroy();
}

// Inicializa el administrador de buffers con los dispositivos Vulkan
void BufferManager::init(VkDevice dev, VkPhysicalDevice physDev) {
    device = dev;
    physicalDevice = physDev;
}

// Crea un vertex buffer 2D simple con un solo vértice (0,0)
void BufferManager::createVertexBuffer() {
    std::vector<Vertex> vertices = {
        {{0.0f, 0.0f}}
    };

    // Descripción del buffer que se va a crear
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();          // Tamaño total en bytes
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;             // Usado como vertex buffer
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;               // Solo una cola lo usa

    // Crear el buffer
    if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear vertex buffer");
    }

    // Obtener los requisitos de memoria del buffer
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

    // Información de asignación de memoria
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    // HOST_VISIBLE → CPU puede acceder
    // HOST_COHERENT → Sin necesidad de flush manual

    // Reservar memoria
    if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Error al asignar memoria del vertex buffer");
    }

    // Asociar el buffer con su bloque de memoria
    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

    // Copiar los datos desde la CPU al buffer
    void* data;
    vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferInfo.size);
    vkUnmapMemory(device, vertexBufferMemory);
}

// Actualiza la posición del único vértice (útil para animaciones o transformaciones simples)
void BufferManager::updateVertexPosition(float x, float y) {
    if (device == VK_NULL_HANDLE || vertexBuffer == VK_NULL_HANDLE) return;
    
    Vertex vertex = {{x, y}};
    
    void* data;
    vkMapMemory(device, vertexBufferMemory, 0, sizeof(Vertex), 0, &data);
    memcpy(data, &vertex, sizeof(vertex));
    vkUnmapMemory(device, vertexBufferMemory);
}

// Encuentra un tipo de memoria compatible con los requisitos del buffer
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

// Libera todos los recursos Vulkan (buffers y memoria)
void BufferManager::destroy() {
    if (device == VK_NULL_HANDLE) return;

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

// Crea un vertex buffer 3D genérico (usa la misma lógica pero con más vértices)
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

    // Copiar los vértices al buffer
    void* data;
    vkMapMemory(device, vertexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, vertexBufferMemory);
    
    std::cout << "Vertex buffer 3D creado con " << vertices.size() << " vértices" << std::endl;
}

// Crea un index buffer para dibujar con índices
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

    // Copiar los índices al buffer
    void* data;
    vkMapMemory(device, indexBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(device, indexBufferMemory);
    
    std::cout << "Index buffer creado con " << indices.size() << " índices" << std::endl;
}
