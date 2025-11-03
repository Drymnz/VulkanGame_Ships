#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <GLFW/glfw3.h>
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

    void run(GLFWwindow* window,
             VulkanRenderer& renderer,
             VulkanDevice& vulkanDevice,
             VulkanSwapChain& swapChain,
             VulkanRenderPass& renderPass,
             VulkanPipeline& pipeline,
             InputManager& inputManager,
             GameObject& playerShip);

    void stop();

private:
    bool running;
    int frameCount;
    double lastFPSTime;

    // Variables de movimiento
    float vertical;
    float depth;
    float horizontal;

    void updateFPS(GLFWwindow* window);
};

#endif // GAME_LOOP_H