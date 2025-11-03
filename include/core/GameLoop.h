// ============================================
// core/GameLoop.h
// ============================================
#pragma once

#include <GLFW/glfw3.h>
#include <vector>

#include "graphics/VulkanRenderer.h"
#include "core/VulkanDevice.h"
#include "graphics/VulkanSwapChain.h"
#include "graphics/VulkanRenderPass.h"
#include "graphics/VulkanPipeline.h"
#include "input/InputManager.h"
#include "game/GameObject.h"

class GameLoop {
public:
    GameLoop();

    // Inicia el bucle principal del juego
    void run(GLFWwindow* window,
             VulkanRenderer& renderer,
             VulkanDevice& vulkanDevice,
             VulkanSwapChain& swapChain,
             VulkanRenderPass& renderPass,
             VulkanPipeline& pipeline,
             InputManager& inputManager,
             GameObject* playerShip,
             GameObject* floor);

    // Detiene el bucle principal
    void stop();

private:
    // Actualiza el contador de FPS y el título de la ventana
    void updateFPS(GLFWwindow* window);

private:
    bool running;

    // Variables para cálculo de FPS
    int frameCount;
    double lastFPSTime;

    // Variables para movimiento del jugador
    float vertical;
    float depth;
    float horizontal;
};
