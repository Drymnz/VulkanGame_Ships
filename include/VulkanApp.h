#ifndef VULKAN_APP_H
#define VULKAN_APP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/VulkanInstance.h"
#include "core/VulkanDevice.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanPipeline.h"
#include "graphics/VulkanRenderer.h"
#include "input/InputManager.h"

class VulkanApp {
public:
    VulkanApp();
    ~VulkanApp();
    
    void run();

private:
    void initWindow();
    void initVulkan();
    void initImGui();
    void mainLoop();
    void cleanup();
    
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void handleKeyPress(int key, int action);

    GLFWwindow* window;
    VkSurfaceKHR surface;
    VkDescriptorPool imguiDescriptorPool;
    
    VulkanInstance vulkanInstance;
    VulkanDevice vulkanDevice;
    VulkanSwapChain swapChain;
    VulkanRenderPass renderPass;
    VulkanPipeline pipeline;
    VulkanRenderer renderer;
    InputManager inputManager;
    
    int targetFPS;
    float pointX;
    float pointY;
};

#endif