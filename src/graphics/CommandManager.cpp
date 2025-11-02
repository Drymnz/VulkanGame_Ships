#include "graphics/CommandManager.h"
#include <stdexcept>
#include <iostream>

CommandManager::CommandManager()
    : device(VK_NULL_HANDLE)
    , commandPool(VK_NULL_HANDLE) {}

CommandManager::~CommandManager() {
    destroy();
}

void CommandManager::init(VkDevice dev, uint32_t graphicsQueueFamily) {
    device = dev;
    createCommandPool(graphicsQueueFamily);
    createCommandBuffers();
}

void CommandManager::createCommandPool(uint32_t graphicsQueueFamily) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear command pool");
    }
}

void CommandManager::createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear command buffers");
    }
    
    std::cout << "Command buffers creados (" << commandBuffers.size() << ")" << std::endl;
}

void CommandManager::destroy() {
    if (device == VK_NULL_HANDLE) return;

    if (commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device, commandPool, nullptr);
        commandPool = VK_NULL_HANDLE;
    }
}