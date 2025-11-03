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
#include "game/GameObject.h"

class VulkanRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void init(VkDevice dev, VkPhysicalDevice physDev, uint32_t graphicsQueueFamily);
    void loadModel(const std::vector<Vertex3D>& vertices, const std::vector<uint32_t>& indices);
    void createDefaultBuffer();
    void createFramebuffers(VkRenderPass renderPass,
                           const std::vector<VkImageView>& imageViews,
                           VkExtent2D extent);

    // Nuevo: Cargar modelo con nombre
    ModelBuffers loadNamedModel(const std::vector<Vertex3D>& vertices,
                               const std::vector<uint32_t>& indices,
                               const std::string& name);

    // Nuevo: Renderizar múltiples objetos
    void drawFrame(VkDevice device,
                   VkSwapchainKHR swapChain,
                   VkQueue graphicsQueue,
                   VkQueue presentQueue,
                   VkRenderPass renderPass,
                   VkPipeline pipeline,
                   VkPipelineLayout pipelineLayout,
                   VkExtent2D extent,
                   const std::vector<GameObject*>& gameObjects);

    void destroy();

private:
    VkDevice device;
    CommandManager commandManager;
    SyncManager syncManager;
    BufferManager bufferManager;
    FramebufferManager framebufferManager;
    
    size_t currentFrame;
    bool useIndexBuffer;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

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