#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <iostream>

struct QueueFamilyIndices {
    uint32_t graphicsFamily = UINT32_MAX;
    uint32_t presentFamily = UINT32_MAX;
    
    bool isComplete() const {
        return graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX;
    }
};

class VulkanDevice {
private:
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    QueueFamilyIndices queueIndices;
    
public:
    VulkanDevice();
    ~VulkanDevice();
    
    void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surf);
    void createLogicalDevice();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void destroy();
    
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkDevice getLogicalDevice() const { return logicalDevice; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    QueueFamilyIndices getQueueIndices() const { return queueIndices; }
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};