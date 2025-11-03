#include "graphics/VulkanRenderPass.h"
#include <stdexcept>
#include <iostream>

// Constructor: inicializa los valores del render pass y del dispositivo a nulos
VulkanRenderPass::VulkanRenderPass()
    : device(VK_NULL_HANDLE)
    , renderPass(VK_NULL_HANDLE) {}

// Destructor: destruye el render pass si existe
VulkanRenderPass::~VulkanRenderPass() {
    destroy();
}

// Crea el render pass de Vulkan, que define cómo se procesarán las imágenes
void VulkanRenderPass::create(VkDevice dev, VkFormat swapChainImageFormat) {
    // Guarda el dispositivo lógico de Vulkan
    device = dev;
    
    // Descripción del attachment de color (imagen que se va a renderizar)
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapChainImageFormat;                 // Formato del color (igual al de la swap chain)
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;               // Sin multisampling (1 muestra por pixel)
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;          // Limpiar el attachment al iniciar el render pass
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;        // Guardar el resultado al final del render pass
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;   // Ignorar stencil
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // Ignorar stencil
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;     // Layout inicial desconocido
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Layout final (listo para mostrarse en pantalla)

    // Referencia al attachment de color
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;                             // Índice del attachment (solo uno)
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // Layout óptimo para color

    // Descripción del subpass (etapa del pipeline donde se usará el attachment)
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;   // Subpass de tipo gráfico
    subpass.colorAttachmentCount = 1;                              // Número de attachments de color
    subpass.pColorAttachments = &colorAttachmentRef;               // Referencia al attachment

    // Información de creación del render pass
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO; // Tipo de estructura
    renderPassInfo.attachmentCount = 1;                               // Solo un attachment (color)
    renderPassInfo.pAttachments = &colorAttachment;                    // Puntero al attachment
    renderPassInfo.subpassCount = 1;                                  // Solo un subpass
    renderPassInfo.pSubpasses = &subpass;                             // Puntero al subpass

    // Crea el render pass con la información configurada
    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear render pass"); // Lanza error si falla
    }

    // Mensaje de confirmación
    std::cout << "Render pass creado" << std::endl;
}

// Destruye el render pass (libera recursos de Vulkan)
void VulkanRenderPass::destroy() {
    // Si no hay dispositivo, no hacer nada
    if (device == VK_NULL_HANDLE) return;
    
    // Si el render pass está creado, destruirlo
    if (renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device, renderPass, nullptr);
        renderPass = VK_NULL_HANDLE;
    }
}
