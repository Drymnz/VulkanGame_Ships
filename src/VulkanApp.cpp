// ============================================
// src/VulkanApp.cpp - CORREGIDO
// ============================================

// Esta macro le indica a GLFW que incluya automáticamente los encabezados de Vulkan
// antes de incluir <GLFW/glfw3.h>. Así no es necesario incluir <vulkan/vulkan.h> manualmente.
#define GLFW_INCLUDE_VULKAN  
#include <GLFW/glfw3.h>  // Librería para manejo de ventanas y entrada (keyboard/mouse)

// Incluye la declaración de la clase VulkanApp y sus miembros
#include "VulkanApp.h"

#include <stdexcept>     // Para lanzar excepciones (std::runtime_error)
#include <iostream>      // Para imprimir mensajes en consola

// -------------------------------------------------------
// Constructor: inicializa miembros con valores nulos de Vulkan
// -------------------------------------------------------
VulkanApp::VulkanApp()
    : surface(VK_NULL_HANDLE)
    , imguiDescriptorPool(VK_NULL_HANDLE) {}  // Inicializa el pool de descriptores de ImGui como vacío

// Destructor vacío — el cleanup se maneja manualmente
VulkanApp::~VulkanApp() {}

// -------------------------------------------------------
// Método principal: ejecuta el flujo completo de la app
// -------------------------------------------------------
void VulkanApp::run() {
    // Crear una ventana GLFW de 800x600 píxeles
    windowManager.create(800, 600, "Vulkan Game");

    // Asignar un puntero de usuario (this) a la ventana, para acceder a VulkanApp desde callbacks
    windowManager.setUserPointer(this);

    // Registrar el callback de teclado
    windowManager.setKeyCallback(keyCallback);
    
    // Inicializa todos los componentes Vulkan
    initVulkan();

    // Inicializa ImGui (pendiente de implementación)
    initImGui();
    
    // Crear un objeto del juego (por ejemplo, la nave del jugador)
    GameObject playerShip;
    
    // Ejecutar el bucle principal del juego, pasando todos los módulos necesarios
    gameLoop.run(windowManager.getWindow(), 
                 renderer,
                 vulkanDevice,
                 swapChain,
                 renderPass,
                 pipeline,
                 inputManager,
                 playerShip);
    
    // Liberar recursos al salir
    cleanup();
}

// -------------------------------------------------------
// Inicializa Vulkan y todos los objetos relacionados
// -------------------------------------------------------
void VulkanApp::initVulkan() {
    // Crear la instancia principal de Vulkan (VkInstance)
    vulkanInstance.create();

    // Crear el "surface" que conecta Vulkan con la ventana GLFW
    VkResult result = glfwCreateWindowSurface(vulkanInstance.getInstance(), 
                                              windowManager.getWindow(), nullptr, &surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Error al crear surface");
    }

    // Seleccionar el dispositivo físico (GPU) que soporta Vulkan
    vulkanDevice.pickPhysicalDevice(vulkanInstance.getInstance(), surface);

    // Crear el dispositivo lógico y las colas (graphics/present)
    vulkanDevice.createLogicalDevice();

    // Crear el swapchain (intercambio de imágenes para el renderizado)
    swapChain.create(vulkanDevice.getPhysicalDevice(), vulkanDevice.getLogicalDevice(), 
                     surface, windowManager.getWindow());
    swapChain.createImageViews();  // Crear las vistas de imagen del swapchain

    // Crear el render pass (define los attachments y cómo se dibuja en ellos)
    renderPass.create(vulkanDevice.getLogicalDevice(), swapChain.getImageFormat());

    // Crear el pipeline gráfico (shaders, configuración de rasterización, etc.)
    pipeline.createGraphicsPipeline(vulkanDevice.getLogicalDevice(),
                                    renderPass.getRenderPass(),
                                    swapChain.getExtent());

    // Inicializar el renderer (maneja buffers, draw calls, etc.)
    renderer.init(vulkanDevice.getLogicalDevice(),
                  vulkanDevice.getPhysicalDevice(),
                  vulkanDevice.getQueueIndices().graphicsFamily);

    // Intentar cargar un modelo 3D ("ship.glb")
    if (modelLoader.loadModel("ship.glb")) {
        // Cargar los vértices e índices en GPU
        renderer.loadModel(modelLoader.getVertices(), modelLoader.getIndices());
    } else {
        // Si no se encuentra el modelo, crear un buffer básico por defecto
        std::cout << "Advertencia: No se pudo cargar el modelo" << std::endl;
        renderer.createDefaultBuffer();
    }

    // Crear los framebuffers (una por imagen del swapchain)
    renderer.createFramebuffers(renderPass.getRenderPass(),
                               swapChain.getImageViews(),
                               swapChain.getExtent());

    std::cout << "Vulkan inicializado" << std::endl;
}

// -------------------------------------------------------
// Inicializa la interfaz ImGui (aún no implementado)
// -------------------------------------------------------
void VulkanApp::initImGui() {
    // TODO: Implementar integración de ImGui
}

// -------------------------------------------------------
// Limpia y destruye todos los recursos Vulkan/GLFW
// -------------------------------------------------------
void VulkanApp::cleanup() {
    std::cout << "Cleanup iniciado..." << std::endl;

    // Asegura que el dispositivo termine cualquier tarea antes de destruir objetos
    if (vulkanDevice.getLogicalDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(vulkanDevice.getLogicalDevice());
    }

    // Destruir en orden inverso de creación
    renderer.destroy();
    pipeline.destroy();
    renderPass.destroy();
    swapChain.destroy();
    vulkanDevice.destroy();

    // Destruir el surface de la ventana
    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(vulkanInstance.getInstance(), surface, nullptr);
        surface = VK_NULL_HANDLE;
    }

    // Destruir la instancia de Vulkan y la ventana GLFW
    vulkanInstance.destroy();
    windowManager.destroy();

    std::cout << "Cleanup completado" << std::endl;
}

// -------------------------------------------------------
// Callback de teclado estático
// -------------------------------------------------------
void VulkanApp::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Recuperar el puntero a la aplicación desde la ventana
    auto app = reinterpret_cast<VulkanApp*>(glfwGetWindowUserPointer(window));

    // Si se presiona ESC, cerrar la ventana
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }

    // Pasar el evento al gestor de entrada
    app->inputManager.handleKeyPress(key, action);
}
