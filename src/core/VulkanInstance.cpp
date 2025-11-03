// ============================================
// core/VulkanInstance.cpp
// Crea y gestiona la instancia principal de Vulkan.
// La instancia conecta la aplicación con la API de Vulkan.
// ============================================

#include "core/VulkanInstance.h"
#include <stdexcept>
#include <vector>

// ------------------------------------------------------------
// Constructor: inicializa la instancia en nulo
// ------------------------------------------------------------
VulkanInstance::VulkanInstance() : instance(VK_NULL_HANDLE) {}

// Destructor: destruye la instancia si está activa
VulkanInstance::~VulkanInstance() {
    destroy();
}


// ------------------------------------------------------------
// Crea una instancia de Vulkan que conecta la aplicación con la API.
// Esta es la primera llamada necesaria antes de usar cualquier función Vulkan.
// ------------------------------------------------------------
void VulkanInstance::create() {
    // ----------------------------------------
    // Información básica de la aplicación
    // (usada solo para depuración o herramientas externas)
    // ----------------------------------------
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Mi Juego";       // Nombre de la app
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Sin Motor";           // Nombre del "motor"
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;     // Versión mínima de Vulkan requerida

    // ----------------------------------------
    // Extensiones requeridas por GLFW para crear una ventana compatible con Vulkan
    // (por ejemplo, VK_KHR_surface, VK_KHR_xcb_surface, etc.)
    // ----------------------------------------
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // ----------------------------------------
    // Estructura principal de creación de la instancia
    // ----------------------------------------
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    // No habilitamos capas de validación en esta versión (pueden agregarse después)
    createInfo.enabledLayerCount = 0;

    // ----------------------------------------
    // Crear la instancia de Vulkan
    // ----------------------------------------
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear instancia Vulkan");
    }
}


// ------------------------------------------------------------
// Destruye la instancia Vulkan y libera los recursos asociados
// ------------------------------------------------------------
void VulkanInstance::destroy() {
    if (instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }
}
