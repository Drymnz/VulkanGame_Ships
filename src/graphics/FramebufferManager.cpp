#include "graphics/FramebufferManager.h"
#include <stdexcept>
#include <iostream>

// Constructor: inicializa el dispositivo Vulkan como nulo
FramebufferManager::FramebufferManager()
    : device(VK_NULL_HANDLE) {}

// Destructor: libera los recursos de framebuffers
FramebufferManager::~FramebufferManager() {
    destroy();
}

// Inicializa el manejador con el dispositivo Vulkan
void FramebufferManager::init(VkDevice dev) {
    device = dev;
}

// Crea los framebuffers asociados al render pass y a las imágenes del swapchain
void FramebufferManager::createFramebuffers(
    VkRenderPass renderPass,                    // Render pass con el que se usará el framebuffer
    const std::vector<VkImageView>& imageViews, // Vistas de las imágenes del swapchain
    VkExtent2D extent                           // Tamaño de las imágenes (ancho y alto)
) {
    // Reservar espacio en el vector para cada framebuffer
    framebuffers.resize(imageViews.size());

    // Crear un framebuffer por cada imagen del swapchain
    for (size_t i = 0; i < imageViews.size(); i++) {
        // Cada framebuffer solo necesita una attachment: la imagen de color
        VkImageView attachments[] = { imageViews[i] };

        // Estructura de creación del framebuffer
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;   // Asociado al render pass
        framebufferInfo.attachmentCount = 1;       // Una sola attachment (color)
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;      // Mismo tamaño que el swapchain
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;                // Solo una capa (no 3D ni cubemaps)

        // Crear el framebuffer
        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Error al crear framebuffer");
        }
    }

    std::cout << "Framebuffers creados (" << framebuffers.size() << ")" << std::endl;
}

// Destruye todos los framebuffers creados
void FramebufferManager::destroy() {
    if (device == VK_NULL_HANDLE) return; // Si no hay dispositivo válido, salir

    // Liberar cada framebuffer
    for (auto framebuffer : framebuffers) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    // Vaciar el vector para evitar referencias colgantes
    framebuffers.clear();
}
