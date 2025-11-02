#include "graphics/FramebufferManager.h"
#include <stdexcept>
#include <iostream>

FramebufferManager::FramebufferManager()
    : device(VK_NULL_HANDLE) {}

FramebufferManager::~FramebufferManager() {
    destroy();
}

void FramebufferManager::init(VkDevice dev) {
    device = dev;
}

void FramebufferManager::createFramebuffers(VkRenderPass renderPass, const std::vector<VkImageView>& imageViews, VkExtent2D extent) {
    framebuffers.resize(imageViews.size());

    for (size_t i = 0; i < imageViews.size(); i++) {
        VkImageView attachments[] = {imageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Error al crear framebuffer");
        }
    }

    std::cout << "Framebuffers creados (" << framebuffers.size() << ")" << std::endl;
}

void FramebufferManager::destroy() {
    if (device == VK_NULL_HANDLE) return;

    for (auto framebuffer : framebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    framebuffers.clear();
}