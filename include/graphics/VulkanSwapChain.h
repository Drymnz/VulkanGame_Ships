// VulkanSwapChain.h
#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>

class VulkanSwapChain {
public:
    VulkanSwapChain();
    ~VulkanSwapChain();

    // Crear el swapchain
    void create(VkPhysicalDevice physicalDevice, VkDevice device,
                VkSurfaceKHR surface, GLFWwindow* window);

    // Crear vistas de imagen (Image Views)
    void createImageViews();

    // Liberar recursos
    void destroy();

    // Getters
    VkFormat getImageFormat() const { return swapChainImageFormat; }
    VkExtent2D getExtent() const { return swapChainExtent; }
    const std::vector<VkImageView>& getImageViews() const { return swapChainImageViews; }
    VkSwapchainKHR getSwapChain() const { return swapChain; }

private:
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    // Métodos auxiliares
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

private:
    VkDevice device;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
};
