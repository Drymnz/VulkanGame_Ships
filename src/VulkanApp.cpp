#define GLFW_INCLUDE_VULKAN  
#include <GLFW/glfw3.h>
#include "VulkanApp.h"
#include <stdexcept>
#include <iostream>

VulkanApp::VulkanApp()
    : surface(VK_NULL_HANDLE)
    , imguiDescriptorPool(VK_NULL_HANDLE) 
    , playerShip(nullptr)
    , floor(nullptr) {}

VulkanApp::~VulkanApp() {
    // Limpiar GameObjects
    delete playerShip;
    delete floor;
}

void VulkanApp::run() {
    windowManager.create(800, 600, "Vulkan Game - Ships");
    windowManager.setUserPointer(this);
    windowManager.setKeyCallback(keyCallback);
    
    // ✅ PASO 1: CREAR OBJETOS PRIMERO (antes de initVulkan)
    std::cout << "Creando objetos del juego..." << std::endl;
    playerShip = new GameObject();
    floor = new GameObject();
    
    // Configurar posición inicial del suelo (debajo de la nave)
    floor->getTransform().setPosition(0.0f, 2.0f, 0.0f);   // Y positivo = abajo en pantalla
    floor->getTransform().setScale(10.0f, 0.5f, 10.0f);    // Más grande en X y Z
    std::cout << "✓ Objetos creados" << std::endl;
    
    // ✅ PASO 2: Ahora sí podemos inicializar Vulkan y cargar los modelos
    initVulkan();
    initImGui();
    
    // Ejecutar el bucle principal del juego
    gameLoop.run(windowManager.getWindow(), 
                 renderer,
                 vulkanDevice,
                 swapChain,
                 renderPass,
                 pipeline,
                 inputManager,
                 playerShip,
                 floor);
    
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

    // 🔥 CRÍTICO: Crear uniform buffers y descriptor sets
    std::cout << "Inicializando sistema de iluminación..." << std::endl;
    renderer.createUniformBuffers(vulkanDevice.getPhysicalDevice());
    renderer.createDescriptorPool(vulkanDevice.getLogicalDevice());
    renderer.createDescriptorSets(vulkanDevice.getLogicalDevice(), 
                                  pipeline.getDescriptorSetLayout());
    std::cout << "✓ Sistema de iluminación inicializado" << std::endl;

    // Cargar el modelo de la nave
    if (modelLoader.loadModel("ship.glb")) {
        auto shipBuffers = renderer.loadNamedModel(
            modelLoader.getVertices(), 
            modelLoader.getIndices(),
            "ship"
        );
        playerShip->setBuffers(shipBuffers.vertexBuffer, 
                              shipBuffers.indexBuffer, 
                              shipBuffers.indexCount);
        std::cout << "✓ Nave cargada" << std::endl;
    } else {
        std::cout << "✗ Error: No se pudo cargar ship.glb" << std::endl;
    }

    // Cargar el modelo del suelo
    if (modelLoader.loadModel("floor.glb")) {
        auto floorBuffers = renderer.loadNamedModel(
            modelLoader.getVertices(),
            modelLoader.getIndices(),
            "floor"
        );
        floor->setBuffers(floorBuffers.vertexBuffer,
                         floorBuffers.indexBuffer,
                         floorBuffers.indexCount);
        std::cout << "✓ Suelo cargado" << std::endl;
    } else {
        std::cout << "✗ Advertencia: No se pudo cargar floor.glb" << std::endl;
    }

    renderer.createFramebuffers(renderPass.getRenderPass(),
                               swapChain.getImageViews(),
                               swapChain.getExtent());

    std::cout << "Vulkan inicializado" << std::endl;
}

void VulkanApp::initImGui() {
    // TODO: Implementar integración de ImGui
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