#include "VulkanApp.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <iomanip>

VulkanApp::VulkanApp() 
    : window(nullptr)
    , surface(VK_NULL_HANDLE)
    , imguiDescriptorPool(VK_NULL_HANDLE)
    , targetFPS(0)  // 0 = sin límite
    , pointX(0.0f)
    , pointY(0.0f)
    , frameCount(0)
    , lastFPSTime(0.0) {}

VulkanApp::~VulkanApp() {
    cleanup();
}

void VulkanApp::run() {
    initWindow();
    initVulkan();
    initImGui();
    mainLoop();
    cleanup();
}

void VulkanApp::initWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("Error al inicializar GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "Vulkan Game", nullptr, nullptr);
    
    if (!window) {
        throw std::runtime_error("Error al crear ventana");
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    
    lastFPSTime = glfwGetTime();
    
    std::cout << "Ventana creada" << std::endl;
}

void VulkanApp::initVulkan() {
    vulkanInstance.create();
    
    VkResult result = glfwCreateWindowSurface(vulkanInstance.getInstance(), window, nullptr, &surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Error al crear surface");
    }
    
    vulkanDevice.pickPhysicalDevice(vulkanInstance.getInstance(), surface);
    vulkanDevice.createLogicalDevice();
    
    swapChain.create(vulkanDevice.getPhysicalDevice(), vulkanDevice.getLogicalDevice(), surface, window);
    swapChain.createImageViews();
    
    renderPass.create(vulkanDevice.getLogicalDevice(), swapChain.getImageFormat());
    
    pipeline.createGraphicsPipeline(vulkanDevice.getLogicalDevice(), 
                                   renderPass.getRenderPass(), 
                                   swapChain.getExtent());
    
    renderer.init(vulkanDevice.getLogicalDevice(), 
                 vulkanDevice.getPhysicalDevice(), 
                 vulkanDevice.getQueueIndices().graphicsFamily);
    
    renderer.createFramebuffers(renderPass.getRenderPass(), 
                               swapChain.getImageViews(), 
                               swapChain.getExtent());
    
    std::cout << "Vulkan inicializado" << std::endl;
}

void VulkanApp::initImGui() {
    // TODO: Implementar ImGui
}

void VulkanApp::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        inputManager.updateMovement(pointX, pointY, 0.01f);
        renderer.updateVertexPosition(pointX, pointY);
        
        renderer.drawFrame(vulkanDevice.getLogicalDevice(),
                          swapChain.getSwapChain(), 
                          vulkanDevice.getGraphicsQueue(), 
                          vulkanDevice.getPresentQueue(),
                          renderPass.getRenderPass(),
                          pipeline.getPipeline(), 
                          swapChain.getExtent());
        
        updateFPS();
    }
}

void VulkanApp::updateFPS() {
    frameCount++;
    double currentTime = glfwGetTime();
    double elapsed = currentTime - lastFPSTime;
    
    // Actualizar título cada 0.5 segundos
    if (elapsed >= 0.5) {
        double fps = frameCount / elapsed;
        
        std::ostringstream title;
        title << "Vulkan Game - " << std::fixed << std::setprecision(1) << fps << " FPS";
        glfwSetWindowTitle(window, title.str().c_str());
        
        frameCount = 0;
        lastFPSTime = currentTime;
    }
}

void VulkanApp::cleanup() {
    if (vulkanDevice.getLogicalDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(vulkanDevice.getLogicalDevice());
    }
    
    renderer.destroy();
    pipeline.destroy();
    renderPass.destroy();
    swapChain.destroy();
    vulkanDevice.destroy();
    
    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(vulkanInstance.getInstance(), surface, nullptr);
        surface = VK_NULL_HANDLE;
    }
    
    vulkanInstance.destroy();
    
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

void VulkanApp::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = reinterpret_cast<VulkanApp*>(glfwGetWindowUserPointer(window));
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }
    
    app->inputManager.handleKeyPress(key, action);
}

void VulkanApp::handleKeyPress(int key, int action) {
    inputManager.handleKeyPress(key, action);
}