// ============================================
// core/GameLoop.cpp - CORREGIDO
// ============================================

#include "core/GameLoop.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// Constructor: inicializa variables del bucle principal
GameLoop::GameLoop()
    : running(false)       // Indica si el juego está corriendo
    , frameCount(0)        // Contador de frames renderizados
    , lastFPSTime(0.0)     // Último tiempo en el que se calculó el FPS
    , vertical(0.0f)       // Movimiento en eje vertical (Y)
    , depth(-2.0f)         // Movimiento en eje profundidad (Z)
    , horizontal(0.0f) {}  // Movimiento en eje horizontal (X)


// ------------------------------------------------------------
// Método principal del juego (main loop)
// Encargado de manejar input, actualizar estado y renderizar.
// ------------------------------------------------------------
void GameLoop::run(GLFWwindow* window, 
                   VulkanRenderer& renderer,
                   VulkanDevice& vulkanDevice,
                   VulkanSwapChain& swapChain,
                   VulkanRenderPass& renderPass,
                   VulkanPipeline& pipeline,
                   InputManager& inputManager,
                   GameObject& playerShip) {
    
    running = true;                   // Activa el bucle
    lastFPSTime = glfwGetTime();      // Guarda el tiempo inicial para medir FPS
    int frameNum = 0;                 // Lleva la cuenta del número de frames

    std::cout << "Entrando al game loop..." << std::endl;

    // ========================================================
    // Bucle principal del juego
    // ========================================================
    while (running && !glfwWindowShouldClose(window)) {
        // Procesa eventos de ventana y teclado (GLFW)
        glfwPollEvents();
        
        try {
            // -----------------------------------------------
            // 1. Actualizar entrada y movimiento del jugador
            // -----------------------------------------------
            // El InputManager ajusta los valores de movimiento
            // (vertical, depth, horizontal) según teclas presionadas.
            inputManager.updateMovement(vertical, depth, horizontal, 0.05f);
            
            // -----------------------------------------------
            // 2. Convertir movimiento lógico a coordenadas 3D
            // -----------------------------------------------
            float modelX = horizontal;
            float modelY = depth;
            float modelZ = vertical;
            
            // -----------------------------------------------
            // 3. Actualizar la posición del objeto (jugador)
            // -----------------------------------------------
            playerShip.getTransform().setPosition(modelX, modelY, modelZ);
            
            // -----------------------------------------------
            // 4. Renderizar el frame actual
            // -----------------------------------------------
            renderer.drawFrame(
                vulkanDevice.getLogicalDevice(),  // Dispositivo lógico
                swapChain.getSwapChain(),         // Swapchain activo
                vulkanDevice.getGraphicsQueue(),  // Cola de render
                vulkanDevice.getPresentQueue(),   // Cola de presentación
                renderPass.getRenderPass(),       // Render pass configurado
                pipeline.getPipeline(),           // Pipeline gráfico
                pipeline.getPipelineLayout(),     // Layout de pipeline
                swapChain.getExtent(),            // Tamaño de ventana
                playerShip.getTransform()         // Transform actual del objeto
            );
            
            // -----------------------------------------------
            // 5. Actualizar y mostrar FPS en la ventana
            // -----------------------------------------------
            updateFPS(window);
            frameNum++;
            
            // Imprime mensaje de confirmación al renderizar el primer frame
            if (frameNum == 1) {
                std::cout << "Primer frame renderizado exitosamente" << std::endl;
            }

        } catch (const std::exception& e) {
            // Captura errores de Vulkan o lógica y detiene el loop
            std::cerr << "Error en frame " << frameNum << ": " << e.what() << std::endl;
            stop();
        }
    }

    std::cout << "Saliendo del game loop después de " << frameNum << " frames" << std::endl;
}


// ------------------------------------------------------------
// Actualiza el título de la ventana con los FPS en tiempo real
// Se ejecuta aproximadamente cada 0.5 segundos.
// ------------------------------------------------------------
void GameLoop::updateFPS(GLFWwindow* window) {
    frameCount++;
    double currentTime = glfwGetTime();
    double elapsed = currentTime - lastFPSTime;

    if (elapsed >= 0.5) { // Cada medio segundo recalcula FPS
        double fps = frameCount / elapsed;

        // Crea un string con precisión de un decimal (ej: "Vulkan Game - 59.8 FPS")
        std::ostringstream title;
        title << "Vulkan Game - " << std::fixed << std::setprecision(1) << fps << " FPS";

        // Cambia el título de la ventana
        glfwSetWindowTitle(window, title.str().c_str());

        // Reinicia contadores
        frameCount = 0;
        lastFPSTime = currentTime;
    }
}


// ------------------------------------------------------------
// Detiene el bucle principal del juego
// ------------------------------------------------------------
void GameLoop::stop() {
    running = false;
}
