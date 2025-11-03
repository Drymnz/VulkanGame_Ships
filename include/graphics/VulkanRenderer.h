#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "graphics/CommandManager.h"
#include "graphics/SyncManager.h"
#include "graphics/BufferManager.h"
#include "graphics/FramebufferManager.h"
#include "graphics/Vertex.h"
#include "graphics/Transform.h"
#include "graphics/LightData.h"
#include "game/GameObject.h"

class VulkanRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void init(VkDevice dev, VkPhysicalDevice physDev, uint32_t graphicsQueueFamily);
    
    // AGREGADO: Declaración del método createFramebuffers que faltaba
    void createFramebuffers(VkRenderPass renderPass,
                           const std::vector<VkImageView>& imageViews,
                           VkExtent2D extent);
    
    // Nuevo: crear uniform buffers y descriptor sets
    void createUniformBuffers(VkPhysicalDevice physicalDevice);
    void createDescriptorPool(VkDevice device);
    void createDescriptorSets(VkDevice device, VkDescriptorSetLayout descriptorSetLayout);
    void updateLightData(const LightUBO& lightData);  
    
    ModelBuffers loadNamedModel(const std::vector<Vertex3D>& vertices,
                               const std::vector<uint32_t>& indices,
                               const std::string& name);

void drawFrame(VkDevice device,
               VkSwapchainKHR swapChain,
               VkQueue graphicsQueue,
               VkQueue presentQueue,
               VkRenderPass renderPass,
               VkPipeline pipeline,
               VkPipelineLayout pipelineLayout,
               VkDescriptorSetLayout descriptorSetLayout,  // ← FALTA ESTE
               VkExtent2D extent,
               const std::vector<GameObject*>& gameObjects);

    void destroy();
    
    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }
    const std::vector<VkDescriptorSet>& getDescriptorSets() const { return descriptorSets; }

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice; 
    CommandManager commandManager;
    SyncManager syncManager;
    BufferManager bufferManager;
    FramebufferManager framebufferManager;
    
    // Uniform buffers para iluminación
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    
    size_t currentFrame;
    bool useIndexBuffer;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void recordCommandBuffer(VkCommandBuffer commandBuffer,
                            uint32_t imageIndex,
                            VkRenderPass renderPass,
                            VkPipeline pipeline,
                            VkPipelineLayout pipelineLayout,
                            VkExtent2D extent,
                            const std::vector<GameObject*>& gameObjects);

    void submitCommandBuffer(VkCommandBuffer commandBuffer,
                            VkQueue graphicsQueue,
                            VkQueue presentQueue,
                            VkSwapchainKHR swapChain,
                            uint32_t imageIndex);
};

#endif // VULKAN_RENDERER_H