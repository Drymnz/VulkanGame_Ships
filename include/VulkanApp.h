// ============================================
// include/VulkanApp.h - CORREGIDO
// ============================================
#ifndef VULKAN_APP_H
#define VULKAN_APP_H

#define GLFW_INCLUDE_VULKAN  // <-- AGREGAR ESTO
#include <GLFW/glfw3.h>

// Core
#include "core/VulkanInstance.h"
#include "core/VulkanDevice.h"
#include "core/GameLoop.h"
#include "core/WindowManager.h"

// Graphics
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanPipeline.h"
#include "graphics/VulkanRenderer.h"
#include "graphics/ModelLoader.h"

// Game
#include "game/GameObject.h"

// Input
#include "input/InputManager.h"

class VulkanApp {
public:
    VulkanApp();
    ~VulkanApp();

    void run();

private:
    // Window
    WindowManager windowManager;

    // Vulkan Core
    VulkanInstance vulkanInstance;
    VulkanDevice vulkanDevice;
    VkSurfaceKHR surface;

    // Graphics
    VulkanSwapChain swapChain;
    VulkanRenderPass renderPass;
    VulkanPipeline pipeline;
    VulkanRenderer renderer;
    ModelLoader modelLoader;

    // Game Loop
    GameLoop gameLoop;

    // Input
    InputManager inputManager;

    // ImGui
    VkDescriptorPool imguiDescriptorPool;

    // Methods
    void initVulkan();
    void initImGui();
    void cleanup();

    // Callbacks
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif // VULKAN_APP_H