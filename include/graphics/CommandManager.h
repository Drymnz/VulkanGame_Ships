#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanConfig.h"

class CommandManager {
public:
    CommandManager();
    ~CommandManager();

    void init(VkDevice device, uint32_t graphicsQueueFamily);
    void destroy();

    const std::vector<VkCommandBuffer>& getCommandBuffers() const { return commandBuffers; }

private:
    VkDevice device;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    void createCommandPool(uint32_t graphicsQueueFamily);
    void createCommandBuffers();
};

#endif // COMMAND_MANAGER_H