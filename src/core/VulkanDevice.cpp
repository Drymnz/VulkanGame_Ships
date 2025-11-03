// ============================================
// core/VulkanDevice.cpp
// Maneja la selección de GPU, creación de colas
// y configuración del dispositivo lógico Vulkan.
// ============================================

#include "core/VulkanDevice.h"
#include <vector>
#include <stdexcept>
#include <iostream>

// ------------------------------------------------------------
// Constructor: inicializa todos los handles a NULL
// ------------------------------------------------------------
VulkanDevice::VulkanDevice() 
    : physicalDevice(VK_NULL_HANDLE)
    , logicalDevice(VK_NULL_HANDLE)
    , graphicsQueue(VK_NULL_HANDLE)
    , presentQueue(VK_NULL_HANDLE)
    , surface(VK_NULL_HANDLE) {}

// Destructor: libera recursos Vulkan asociados
VulkanDevice::~VulkanDevice() {
    destroy();
}


// ------------------------------------------------------------
// Selecciona la GPU física (Physical Device) disponible
// y obtiene los índices de las colas (gráfica y presentación)
// ------------------------------------------------------------
void VulkanDevice::pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surf) {
    surface = surf; // Guarda la superficie donde se renderizará
    
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("No hay GPU con Vulkan");
    }

    // Lista todas las GPUs disponibles
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Por simplicidad, se elige la primera GPU disponible
    physicalDevice = devices[0];

    // Obtener propiedades de la GPU (nombre, tipo, etc.)
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(physicalDevice, &props);
    std::cout << "GPU: " << props.deviceName << std::endl;
    
    // Encuentra los índices de las colas (gráfica/presentación)
    queueIndices = findQueueFamilies(physicalDevice);
}


// ------------------------------------------------------------
// Busca las familias de colas necesarias para renderizar y presentar
// (una para gráficos, otra para mostrar en pantalla)
// ------------------------------------------------------------
QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // Itera sobre cada familia de colas disponibles
    for (uint32_t i = 0; i < queueFamilies.size(); i++) {
        // Verifica si esta cola soporta comandos gráficos
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        // Verifica si esta cola puede presentar imágenes a la superficie
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        // Si ya encontró ambas colas, sale del bucle
        if (indices.isComplete()) break;
    }

    return indices;
}


// ------------------------------------------------------------
// Crea el dispositivo lógico y obtiene las colas de gráficos/presentación
// ------------------------------------------------------------
void VulkanDevice::createLogicalDevice() {
    float queuePriority = 1.0f; // Prioridad de la cola (1.0 = máxima)

    // Información de creación de la cola de comandos
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueIndices.graphicsFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    // Características del dispositivo (sin extras por ahora)
    VkPhysicalDeviceFeatures deviceFeatures{};

    // Extensiones necesarias para el swapchain
    const char *extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // Información general para crear el dispositivo lógico
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 1;
    createInfo.ppEnabledExtensionNames = extensions;

    // Crear el dispositivo lógico (interfaz con la GPU)
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear dispositivo lógico");
    }

    // Obtener los handles de las colas de GPU
    vkGetDeviceQueue(logicalDevice, queueIndices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, queueIndices.presentFamily, 0, &presentQueue);

    std::cout << "Dispositivo lógico creado" << std::endl;
}


// ------------------------------------------------------------
// Busca un tipo de memoria compatible con los requisitos solicitados
// (usado al crear buffers, imágenes o memoria de GPU)
// ------------------------------------------------------------
uint32_t VulkanDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    // Recorre los tipos de memoria del dispositivo físico
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        // Verifica compatibilidad con el filtro y las propiedades deseadas
        if ((typeFilter & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i; // Devuelve el índice del tipo de memoria válido
        }
    }

    throw std::runtime_error("No se encontró tipo de memoria adecuado");
}


// ------------------------------------------------------------
// Libera el dispositivo lógico Vulkan (si existe)
// ------------------------------------------------------------
void VulkanDevice::destroy() {
    if (logicalDevice != VK_NULL_HANDLE) {
        vkDestroyDevice(logicalDevice, nullptr);
        logicalDevice = VK_NULL_HANDLE;
    }
}
