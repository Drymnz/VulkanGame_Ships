// ============================================
// src/VulkanApp.cpp - CORREGIDO
// ============================================
#define GLFW_INCLUDE_VULKAN  // <-- AGREGAR ESTO PRIMERO
#include <GLFW/glfw3.h>

#include "VulkanApp.h"
#include <stdexcept>
#include <iostream>

VulkanApp::VulkanApp()
    : surface(VK_NULL_HANDLE)
    , imguiDescriptorPool(VK_NULL_HANDLE) {}

VulkanApp::~VulkanApp() {}

void VulkanApp::run() {
    windowManager.create(800, 600, "Vulkan Game");
    windowManager.setUserPointer(this);
    windowManager.setKeyCallback(keyCallback);
    
    initVulkan();
    initImGui();
    
    // Crear objeto del juego
    GameObject playerShip;
    
    // Ejecutar game loop con TODOS los parámetros necesarios
    gameLoop.run(windowManager.getWindow(), 
                 renderer,
                 vulkanDevice,
                 swapChain,
                 renderPass,
                 pipeline,
                 inputManager,
                 playerShip);
    
    cleanup();
}

void VulkanApp::initVulkan() {
    vulkanInstance.create();

    VkResult result = glfwCreateWindowSurface(vulkanInstance.getInstance(), 
                                              windowManager.getWindow(), nullptr, &surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Error al crear surface");
    }

    vulkanDevice.pickPhysicalDevice(vulkanInstance.getInstance(), surface);
    vulkanDevice.createLogicalDevice();

    swapChain.create(vulkanDevice.getPhysicalDevice(), vulkanDevice.getLogicalDevice(), 
                     surface, windowManager.getWindow());
    swapChain.createImageViews();

    renderPass.create(vulkanDevice.getLogicalDevice(), swapChain.getImageFormat());

    pipeline.createGraphicsPipeline(vulkanDevice.getLogicalDevice(),
                                    renderPass.getRenderPass(),
                                    swapChain.getExtent());

    renderer.init(vulkanDevice.getLogicalDevice(),
                  vulkanDevice.getPhysicalDevice(),
                  vulkanDevice.getQueueIndices().graphicsFamily);

    // Cargar modelo
    if (modelLoader.loadModel("ship.glb")) {
        renderer.loadModel(modelLoader.getVertices(), modelLoader.getIndices());
    } else {
        std::cout << "Advertencia: No se pudo cargar el modelo" << std::endl;
        renderer.createDefaultBuffer();
    }

    renderer.createFramebuffers(renderPass.getRenderPass(),
                               swapChain.getImageViews(),
                               swapChain.getExtent());

    std::cout << "Vulkan inicializado" << std::endl;
}

void VulkanApp::initImGui() {
    // TODO: Implementar ImGui
}

void VulkanApp::cleanup() {
    std::cout << "Cleanup iniciado..." << std::endl;

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
    windowManager.destroy();

    std::cout << "Cleanup completado" << std::endl;
}

void VulkanApp::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto app = reinterpret_cast<VulkanApp*>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }

    app->inputManager.handleKeyPress(key, action);
}