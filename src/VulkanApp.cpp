#include "VulkanApp.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <iomanip>

VulkanApp::VulkanApp()
    : window(nullptr), surface(VK_NULL_HANDLE), imguiDescriptorPool(VK_NULL_HANDLE), targetFPS(0) // 0 = sin límite
      ,
      pointX(0.0f), pointY(0.0f), frameCount(0), lastFPSTime(0.0)
{
}

VulkanApp::~VulkanApp()
{
    // cleanup();
}

void VulkanApp::run()
{
    initWindow();
    initVulkan();
    initImGui();
    mainLoop();
    cleanup(); // Se llama aquí, no en el destructor
}

void VulkanApp::initWindow()
{
    if (!glfwInit())
    {
        throw std::runtime_error("Error al inicializar GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "Vulkan Game", nullptr, nullptr);

    if (!window)
    {
        throw std::runtime_error("Error al crear ventana");
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);

    lastFPSTime = glfwGetTime();

    std::cout << "Ventana creada" << std::endl;
}

void VulkanApp::initVulkan()
{
    vulkanInstance.create();

    VkResult result = glfwCreateWindowSurface(vulkanInstance.getInstance(), window, nullptr, &surface);
    if (result != VK_SUCCESS)
    {
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

    // CARGAR MODELO <-- AGREGAR ESTO
    if (modelLoader.loadModel("ship.glb"))
    {
        renderer.loadModel(modelLoader.getVertices(), modelLoader.getIndices());
    }
    else
    {
        std::cout << "Advertencia: No se pudo cargar el modelo, usando buffer por defecto" << std::endl;
        renderer.createDefaultBuffer();
    }

    renderer.createFramebuffers(renderPass.getRenderPass(),
                                swapChain.getImageViews(),
                                swapChain.getExtent());

    std::cout << "Vulkan inicializado" << std::endl;
}

void VulkanApp::initImGui()
{
    // TODO: Implementar ImGui
}

void VulkanApp::mainLoop() {
    std::cout << "Entrando al main loop..." << std::endl;
    
    int frameNum = 0;
    
    // Variables en coordenadas lógicas (lo que el usuario espera)
    float vertical = 0.0f;      // Arriba/Abajo en pantalla
    float depth = -2.0f;        // Profundidad (alejamiento de cámara)
    float horizontal = 0.0f;    // Izquierda/Derecha en pantalla
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        try {
            // Actualizar movimiento con nombres lógicos
            inputManager.updateMovement(vertical, depth, horizontal, 0.05f);
            
            // Mapear coordenadas lógicas al sistema interno de Vulkan
            // Sistema interno: X=vertical, Y=profundidad, Z=horizontal
            float modelX = horizontal;  // CAMBIO: horizontal va a X
            float modelY = depth;
            float modelZ = vertical;    // CAMBIO: vertical va a Z
            
            renderer.setModelPosition(modelX, modelY, modelZ);
            
            renderer.drawFrame(vulkanDevice.getLogicalDevice(),
                              swapChain.getSwapChain(), 
                              vulkanDevice.getGraphicsQueue(), 
                              vulkanDevice.getPresentQueue(),
                              renderPass.getRenderPass(),
                              pipeline.getPipeline(),
                              pipeline.getPipelineLayout(),
                              swapChain.getExtent());
            
            updateFPS();
            frameNum++;
            
            if (frameNum == 1) {
                std::cout << "Primer frame renderizado exitosamente" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error en frame " << frameNum << ": " << e.what() << std::endl;
            break;
        }
    }
    
    std::cout << "Saliendo del main loop después de " << frameNum << " frames" << std::endl;
}

void VulkanApp::updateFPS()
{
    frameCount++;
    double currentTime = glfwGetTime();
    double elapsed = currentTime - lastFPSTime;

    // Actualizar título cada 0.5 segundos
    if (elapsed >= 0.5)
    {
        double fps = frameCount / elapsed;

        std::ostringstream title;
        title << "Vulkan Game - " << std::fixed << std::setprecision(1) << fps << " FPS";
        glfwSetWindowTitle(window, title.str().c_str());

        frameCount = 0;
        lastFPSTime = currentTime;
    }
}

void VulkanApp::cleanup()
{
    std::cout << "Cleanup iniciado..." << std::endl; // <-- AGREGAR

    if (vulkanDevice.getLogicalDevice() != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(vulkanDevice.getLogicalDevice());
    }

    renderer.destroy();
    pipeline.destroy();
    renderPass.destroy();
    swapChain.destroy();
    vulkanDevice.destroy();

    if (surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(vulkanInstance.getInstance(), surface, nullptr);
        surface = VK_NULL_HANDLE;
    }

    vulkanInstance.destroy();

    if (window)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    std::cout << "Cleanup completado" << std::endl; // <-- AGREGAR
}

void VulkanApp::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    auto app = reinterpret_cast<VulkanApp *>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }

    app->inputManager.handleKeyPress(key, action);
}

void VulkanApp::handleKeyPress(int key, int action)
{
    inputManager.handleKeyPress(key, action);
}