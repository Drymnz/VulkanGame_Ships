#ifndef VULKAN_APP_H
#define VULKAN_APP_H

#include <vulkan/vulkan.h>
#include "core/WindowManager.h"
#include "core/VulkanInstance.h"
#include "core/VulkanDevice.h"
#include "core/GameLoop.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanPipeline.h"
#include "graphics/VulkanRenderer.h"
#include "graphics/ModelLoader.h"
#include "input/InputManager.h"
#include "game/GameObject.h"

struct GLFWwindow;

class VulkanApp {
public:
    VulkanApp();
    ~VulkanApp();

    void run();

private:
    void initVulkan();
    void initImGui();
    void cleanup();

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    // Core
    WindowManager windowManager;
    VulkanInstance vulkanInstance;
    VulkanDevice vulkanDevice;
    GameLoop gameLoop;

    // Graphics
    VulkanSwapChain swapChain;
    VulkanRenderPass renderPass;
    VulkanPipeline pipeline;
    VulkanRenderer renderer;
    ModelLoader modelLoader;

    // Input
    InputManager inputManager;

    // Vulkan resources
    VkSurfaceKHR surface;
    VkDescriptorPool imguiDescriptorPool;

    // Game objects
    GameObject* playerShip;
    GameObject* floor;
};

#endif // VULKAN_APP_H