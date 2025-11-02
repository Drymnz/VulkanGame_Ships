#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <vulkan/vulkan.h>
#include "BufferManager.h"
#include "CommandManager.h"
#include "SyncManager.h"
#include "FramebufferManager.h"

class VulkanRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void init(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily);
    void createFramebuffers(VkRenderPass renderPass, const std::vector<VkImageView>& imageViews, VkExtent2D extent);
    void updateVertexPosition(float x, float y);
    void drawFrame(VkDevice device, VkSwapchainKHR swapChain, VkQueue graphicsQueue,
                   VkQueue presentQueue, VkRenderPass renderPass, VkPipeline pipeline, 
                   VkExtent2D extent);
    void destroy();

private:
    static const int MAX_FRAMES_IN_FLIGHT = 2;

    VkDevice device;
    VkPhysicalDevice physicalDevice;
    
    BufferManager bufferManager;
    CommandManager commandManager;
    SyncManager syncManager;
    FramebufferManager framebufferManager;
    
    size_t currentFrame;
};

#endif