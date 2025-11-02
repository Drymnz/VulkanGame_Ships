#ifndef FRAMEBUFFER_MANAGER_H
#define FRAMEBUFFER_MANAGER_H

#include <vulkan/vulkan.h>
#include <vector>

class FramebufferManager {
public:
    FramebufferManager();
    ~FramebufferManager();

    void init(VkDevice device);
    void createFramebuffers(VkRenderPass renderPass, const std::vector<VkImageView>& imageViews, VkExtent2D extent);
    void destroy();

    const std::vector<VkFramebuffer>& getFramebuffers() const { return framebuffers; }
    VkFramebuffer getFramebuffer(size_t index) const { return framebuffers[index]; }

private:
    VkDevice device;
    std::vector<VkFramebuffer> framebuffers;
};

#endif