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
                   GameObject* playerShip,
                   GameObject* floor) {
    
    running = true;
    lastFPSTime = glfwGetTime();
    int frameNum = 0;

    std::cout << "Entrando al game loop..." << std::endl;

    while (running && !glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        try {
            // Actualizar movimiento del jugador
            inputManager.updateMovement(vertical, depth, horizontal, 0.05f);
            
            float modelX = horizontal;
            float modelY = depth;
            float modelZ = vertical;
            
            // Actualizar posición de la nave
            playerShip->getTransform().setPosition(modelX, modelY, modelZ);
            
            // El suelo permanece estático (ya configurado en VulkanApp)
            
            // Crear lista de objetos a renderizar
            std::vector<GameObject*> gameObjects;
            gameObjects.push_back(floor);      // Primero el suelo
            gameObjects.push_back(playerShip); // Luego la nave
            
            // Renderizar todos los objetos
            renderer.drawFrame(
                vulkanDevice.getLogicalDevice(),
                swapChain.getSwapChain(),
                vulkanDevice.getGraphicsQueue(),
                vulkanDevice.getPresentQueue(),
                renderPass.getRenderPass(),
                pipeline.getPipeline(),
                pipeline.getPipelineLayout(),
                swapChain.getExtent(),
                gameObjects  // Lista de objetos
            );
            
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
        title << "Vulkan Game - Ships - " << std::fixed << std::setprecision(1) << fps << " FPS";

        glfwSetWindowTitle(window, title.str().c_str());

        frameCount = 0;
        lastFPSTime = currentTime;
    }
}

void GameLoop::stop() {
    running = false;
}