// ============================================
// src/core/WindowManager.cpp
// ============================================
#define GLFW_INCLUDE_VULKAN  // <-- AGREGAR
#include <GLFW/glfw3.h>

#include "core/WindowManager.h"
#include <stdexcept>
#include <iostream>

WindowManager::WindowManager() 
    : window(nullptr) {}

WindowManager::~WindowManager() {
    destroy();
}

void WindowManager::create(int width, int height, const char* title) {
    if (!glfwInit()) {
        throw std::runtime_error("Error al inicializar GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!window) {
        throw std::runtime_error("Error al crear ventana");
    }

    std::cout << "Ventana creada" << std::endl;
}

void WindowManager::setUserPointer(void* pointer) {
    glfwSetWindowUserPointer(window, pointer);
}

void WindowManager::setKeyCallback(GLFWkeyfun callback) {
    glfwSetKeyCallback(window, callback);
}

GLFWwindow* WindowManager::getWindow() const {
    return window;
}

void WindowManager::destroy() {
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}