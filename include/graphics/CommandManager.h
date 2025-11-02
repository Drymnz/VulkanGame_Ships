#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include <vulkan/vulkan.h>
#include <vector>

class CommandManager {
public:
    CommandManager();
    ~CommandManager();

    void init(VkDevice device, uint32_t graphicsQueueFamily);
    void destroy();

    VkCommandPool getCommandPool() const { return commandPool; }
    const std::vector<VkCommandBuffer>& getCommandBuffers() const { return commandBuffers; }
    VkCommandBuffer getCurrentCommandBuffer(size_t frame) const { return commandBuffers[frame]; }

private:
    void createCommandPool(uint32_t graphicsQueueFamily);
    void createCommandBuffers();

    static const int MAX_FRAMES_IN_FLIGHT = 2;

    VkDevice device;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};

#endif