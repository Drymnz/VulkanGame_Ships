#ifndef VULKAN_RENDER_PASS_H
#define VULKAN_RENDER_PASS_H

#include <vulkan/vulkan.h>

class VulkanRenderPass {
public:
    VulkanRenderPass();
    ~VulkanRenderPass();

    void create(VkDevice device, VkFormat swapChainImageFormat);
    void destroy();

    VkRenderPass getRenderPass() const { return renderPass; }

private:
    VkDevice device;
    VkRenderPass renderPass;
};

#endif