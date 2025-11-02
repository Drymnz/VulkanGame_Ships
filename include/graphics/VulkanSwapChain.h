#ifndef VULKAN_SWAP_CHAIN_H
#define VULKAN_SWAP_CHAIN_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanSwapChain {
public:
    VulkanSwapChain();
    ~VulkanSwapChain();

    void create(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, GLFWwindow* window);
    void createImageViews();
    void destroy();

    VkSwapchainKHR getSwapChain() const { return swapChain; }
    VkFormat getImageFormat() const { return swapChainImageFormat; }
    VkExtent2D getExtent() const { return swapChainExtent; }
    const std::vector<VkImageView>& getImageViews() const { return swapChainImageViews; }

private:
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface;
};

#endif