// ============================================
// core/GameLoop.cpp - CORREGIDO
// ============================================
#include "core/GameLoop.h"
#include <iostream>
#include <sstream>
#include <iomanip>

GameLoop::GameLoop()
    : running(false)
    , frameCount(0)
    , lastFPSTime(0.0)
    , vertical(0.0f)
    , depth(-2.0f)
    , horizontal(0.0f) {}

void GameLoop::run(GLFWwindow* window, 
                   VulkanRenderer& renderer,
                   VulkanDevice& vulkanDevice,
                   VulkanSwapChain& swapChain,
                   VulkanRenderPass& renderPass,
                   VulkanPipeline& pipeline,
                   InputManager& inputManager,
                   GameObject& playerShip) {
    
    running = true;
    lastFPSTime = glfwGetTime();
    int frameNum = 0;
    
    std::cout << "Entrando al game loop..." << std::endl;
    
    while (running && !glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        try {
            // Actualizar input y movimiento
            inputManager.updateMovement(vertical, depth, horizontal, 0.05f);
            
            // Mapear coordenadas lógicas al sistema interno
            float modelX = horizontal;
            float modelY = depth;
            float modelZ = vertical;
            
            // Actualizar transform del objeto
            playerShip.getTransform().setPosition(modelX, modelY, modelZ);
            
            // Renderizar
            renderer.drawFrame(vulkanDevice.getLogicalDevice(),
                             swapChain.getSwapChain(),
                             vulkanDevice.getGraphicsQueue(),
                             vulkanDevice.getPresentQueue(),
                             renderPass.getRenderPass(),
                             pipeline.getPipeline(),
                             pipeline.getPipelineLayout(),
                             swapChain.getExtent(),
                             playerShip.getTransform());
            
            updateFPS(window);
            frameNum++;
            
            if (frameNum == 1) {
                std::cout << "Primer frame renderizado exitosamente" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error en frame " << frameNum << ": " << e.what() << std::endl;
            stop();
        }
    }
    
    std::cout << "Saliendo del game loop después de " << frameNum << " frames" << std::endl;
}

void GameLoop::updateFPS(GLFWwindow* window) {
    frameCount++;
    double currentTime = glfwGetTime();
    double elapsed = currentTime - lastFPSTime;

    if (elapsed >= 0.5) {
        double fps = frameCount / elapsed;
        std::ostringstream title;
        title << "Vulkan Game - " << std::fixed << std::setprecision(1) << fps << " FPS";
        glfwSetWindowTitle(window, title.str().c_str());
        frameCount = 0;
        lastFPSTime = currentTime;
    }
}

void GameLoop::stop() {
    running = false;
}