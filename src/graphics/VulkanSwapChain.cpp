#include "graphics/VulkanSwapChain.h"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <algorithm>
#include <iostream>

// -------------------------------------------------------
// Constructor: inicializa punteros de Vulkan en estado nulo
// -------------------------------------------------------
VulkanSwapChain::VulkanSwapChain()
    : device(VK_NULL_HANDLE)
    , swapChain(VK_NULL_HANDLE) {}

// Destructor: destruye automáticamente los recursos del swap chain
VulkanSwapChain::~VulkanSwapChain() {
    destroy();
}

// -------------------------------------------------------
// Crea el swap chain (cadena de intercambio de imágenes de pantalla)
// -------------------------------------------------------
void VulkanSwapChain::create(VkPhysicalDevice physicalDevice, VkDevice dev, 
                             VkSurfaceKHR surface, GLFWwindow* window) {
    device = dev; // Guardar el dispositivo lógico

    // 1️⃣ Consultar las capacidades del swap chain soportadas por la GPU y la superficie
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
    
    // 2️⃣ Elegir las configuraciones óptimas según las capacidades
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);
    
    // 3️⃣ Elegir el número de imágenes en el swap chain
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    // Limitar si excede el máximo permitido
    if (swapChainSupport.capabilities.maxImageCount > 0 && 
        imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    
    // 4️⃣ Rellenar la estructura de creación del swap chain
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; // 1 por imagen (sin estereoscopía)
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // Se usará como render target
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;     // Una sola cola (más eficiente)
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform; // Sin rotaciones
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Sin transparencia de ventana
    createInfo.presentMode = presentMode; // Modo de presentación elegido
    createInfo.clipped = VK_TRUE;         // Ignorar píxeles ocultos por otras ventanas
    createInfo.oldSwapchain = VK_NULL_HANDLE; // No reemplaza uno anterior (por ahora)
    
    // 5️⃣ Crear el swap chain
    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear swap chain");
    }
    
    // 6️⃣ Obtener las imágenes del swap chain (las texturas reales en GPU)
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
    
    // 7️⃣ Guardar formato y dimensiones para otros componentes
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
    
    // Log informativo sobre el modo de presentación elegido
    std::cout << "Swap chain creado con modo: ";
    if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
        std::cout << "IMMEDIATE (sin VSync)" << std::endl;
    } else if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
        std::cout << "MAILBOX (triple buffering)" << std::endl;
    } else {
        std::cout << "FIFO (VSync)" << std::endl;
    }
}

// -------------------------------------------------------
// Consulta las capacidades, formatos y modos soportados por la GPU
// -------------------------------------------------------
VulkanSwapChain::SwapChainSupportDetails VulkanSwapChain::querySwapChainSupport(
    VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;
    
    // Capabilities: límites, número de imágenes, tamaños posibles
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    
    // Formatos soportados (colores, espacio SRGB, etc.)
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }
    
    // Modos de presentación soportados (FIFO, MAILBOX, IMMEDIATE)
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, 
                                                  details.presentModes.data());
    }
    
    return details;
}

// -------------------------------------------------------
// Elige el mejor formato de superficie disponible
// -------------------------------------------------------
VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    // Buscar un formato SRGB con 8 bits por canal (común y recomendado)
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && 
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    // Si no se encuentra, tomar el primero disponible
    return availableFormats[0];
}

// -------------------------------------------------------
// Elige el modo de presentación (sincronización de refresco)
// -------------------------------------------------------
VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR>& availablePresentModes) {
    
    // Prioridad: IMMEDIATE > MAILBOX > FIFO
    // IMMEDIATE = sin VSync, máximo rendimiento, puede causar tearing
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }
    
    // MAILBOX = triple buffering, buena latencia y sin tearing
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return VK_PRESENT_MODE_MAILBOX_KHR;
        }
    }
    
    // FIFO = siempre disponible (modo VSync)
    return VK_PRESENT_MODE_FIFO_KHR;
}

// -------------------------------------------------------
// Determina la resolución del swap chain (ancho y alto)
// -------------------------------------------------------
VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                                             GLFWwindow* window) {
    // Si el valor está definido por el SO, usarlo directamente
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        // Si no, obtener el tamaño actual del framebuffer de GLFW
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        
        // Limitar dentro de los valores mínimos/máximos soportados
        actualExtent.width = std::clamp(actualExtent.width, 
                                       capabilities.minImageExtent.width,
                                       capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
                                        capabilities.minImageExtent.height,
                                        capabilities.maxImageExtent.height);
        
        return actualExtent;
    }
}

// -------------------------------------------------------
// Crea vistas (image views) para cada imagen del swap chain
// -------------------------------------------------------
void VulkanSwapChain::createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());
    
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;

        // Componentes RGBA sin modificaciones
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // Definir qué parte del recurso se usará (color, sin mipmaps)
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        
        // Crear el ImageView en GPU
        if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("Error al crear image view");
        }
    }
    
    std::cout << "Image views creados (" << swapChainImageViews.size() << ")" << std::endl;
}

// -------------------------------------------------------
// Libera todos los recursos asociados al swap chain
// -------------------------------------------------------
void VulkanSwapChain::destroy() {
    if (device == VK_NULL_HANDLE) return;
    
    // Destruir todas las vistas de imagen
    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }
    swapChainImageViews.clear();
    
    // Destruir el swap chain principal
    if (swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        swapChain = VK_NULL_HANDLE;
    }
}
