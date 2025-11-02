#ifndef VULKAN_APP_H
#define VULKAN_APP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/VulkanInstance.h"
#include "core/VulkanDevice.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanPipeline.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanRenderer.h"
#include "input/InputManager.h"
#include "graphics/ModelLoader.h" 

class VulkanApp {
public:
    VulkanApp();
    ~VulkanApp();
    
    void run();
    
private:
    GLFWwindow* window;
    VkSurfaceKHR surface;
    VkDescriptorPool imguiDescriptorPool;
    
    VulkanInstance vulkanInstance;
    VulkanDevice vulkanDevice;
    VulkanSwapChain swapChain;
    VulkanPipeline pipeline;
    VulkanRenderPass renderPass;
    VulkanRenderer renderer;
    InputManager inputManager;
    ModelLoader modelLoader; 
    
    int targetFPS;
    float pointX, pointY;
    
    // FPS counter
    int frameCount;
    double lastFPSTime;
    
    void initWindow();
    void initVulkan();
    void initImGui();
    void mainLoop();
    void updateFPS();
    void cleanup();
    
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void handleKeyPress(int key, int action);
};

#endif // VULKAN_APP_H