// ============================================
// src/core/WindowManager.cpp
// ============================================
// Esta clase se encarga de inicializar GLFW, crear la ventana
// principal del juego y asociarla con la API de Vulkan.
// También gestiona callbacks y la destrucción ordenada.
// ============================================

// Esta macro hace que GLFW incluya automáticamente las cabeceras
// necesarias para usar Vulkan (vk_platform.h, etc.)
#define GLFW_INCLUDE_VULKAN  
#include <GLFW/glfw3.h>

#include "core/WindowManager.h"
#include <stdexcept>
#include <iostream>

// ------------------------------------------------------------
// Constructor: inicializa puntero a ventana en nullptr
// ------------------------------------------------------------
WindowManager::WindowManager() 
    : window(nullptr) {}

// Destructor: destruye la ventana al finalizar el objeto
WindowManager::~WindowManager() {
    destroy();
}


// ------------------------------------------------------------
// Crea la ventana GLFW compatible con Vulkan
// ------------------------------------------------------------
void WindowManager::create(int width, int height, const char* title) {
    // Inicializar GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Error al inicializar GLFW");
    }

    // Indicar que no queremos usar OpenGL: Vulkan manejará el renderizado
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Crear ventana (sin contexto OpenGL)
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!window) {
        throw std::runtime_error("Error al crear ventana");
    }

    std::cout << "Ventana creada exitosamente (" 
              << width << "x" << height << ")" << std::endl;
}


// ------------------------------------------------------------
// Asigna un puntero de usuario a la ventana (útil para callbacks)
// ------------------------------------------------------------
void WindowManager::setUserPointer(void* pointer) {
    glfwSetWindowUserPointer(window, pointer);
}


// ------------------------------------------------------------
// Configura la función de callback para eventos de teclado
// ------------------------------------------------------------
void WindowManager::setKeyCallback(GLFWkeyfun callback) {
    glfwSetKeyCallback(window, callback);
}


// ------------------------------------------------------------
// Devuelve el puntero de la ventana GLFW
// ------------------------------------------------------------
GLFWwindow* WindowManager::getWindow() const {
    return window;
}


// ------------------------------------------------------------
// Libera los recursos asociados a la ventana y cierra GLFW
// ------------------------------------------------------------
void WindowManager::destroy() {
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
        std::cout << "Ventana destruida y GLFW terminado" << std::endl;
    }
}
