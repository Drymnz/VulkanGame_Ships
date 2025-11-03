// ============================================
// graphics/VulkanRenderer.cpp - CORREGIDO
// ============================================
#include "graphics/VulkanRenderer.h"
#include <stdexcept>
#include <iostream>

// Constructor: inicializa los valores por defecto
VulkanRenderer::VulkanRenderer()
    : device(VK_NULL_HANDLE)   // Sin dispositivo asignado todavía
    , currentFrame(0)          // Frame actual (para sincronización)
    , useIndexBuffer(false) {} // Por defecto no se usa índice

// Destructor: libera los recursos asociados al renderer
VulkanRenderer::~VulkanRenderer() {
    destroy();
}

// ----------------------------------------------------------
// Inicializa todos los subsistemas del renderer:
// - Command buffers
// - Sincronización (semáforos, fences)
// - Buffers (vertex, index)
// - Framebuffers
// ----------------------------------------------------------
void VulkanRenderer::init(VkDevice dev, VkPhysicalDevice physDev, uint32_t graphicsQueueFamily) {
    device = dev;
    commandManager.init(device, graphicsQueueFamily); // Comandos de dibujo
    syncManager.init(device);                         // Semáforos y fences
    bufferManager.init(device, physDev);              // Buffers de vértices/índices
    framebufferManager.init(device);                  // Framebuffers de la swap chain
    std::cout << "Renderer inicializado" << std::endl;
}

// ----------------------------------------------------------
// Carga un modelo 3D en memoria (vértices e índices)
// ----------------------------------------------------------
void VulkanRenderer::loadModel(const std::vector<Vertex3D>& vertices, 
                               const std::vector<uint32_t>& indices) {
    bufferManager.createVertexBuffer3D(vertices); // Crea vertex buffer con datos del modelo
    bufferManager.createIndexBuffer(indices);     // Crea index buffer
    useIndexBuffer = true;                        // Indica que hay un índice válido
    std::cout << "Modelo cargado en renderer" << std::endl;
}

// ----------------------------------------------------------
// Crea un buffer simple por defecto (por si no hay modelo)
// ----------------------------------------------------------
void VulkanRenderer::createDefaultBuffer() {
    bufferManager.createVertexBuffer(); // Crea un buffer con datos genéricos
    useIndexBuffer = false;
    std::cout << "Buffer por defecto creado" << std::endl;
}

// ----------------------------------------------------------
// Crea los framebuffers para cada imagen del swap chain
// ----------------------------------------------------------
void VulkanRenderer::createFramebuffers(VkRenderPass renderPass,
                                       const std::vector<VkImageView>& imageViews,
                                       VkExtent2D extent) {
    framebufferManager.createFramebuffers(renderPass, imageViews, extent);
}

// ----------------------------------------------------------
// Función principal de dibujo de un frame
// ----------------------------------------------------------
void VulkanRenderer::drawFrame(VkDevice device, 
                               VkSwapchainKHR swapChain,
                               VkQueue graphicsQueue, 
                               VkQueue presentQueue,
                               VkRenderPass renderPass, 
                               VkPipeline pipeline,
                               VkPipelineLayout pipelineLayout, 
                               VkExtent2D extent,
                               const Transform& transform) {
    
    // Espera a que el frame anterior haya terminado
    vkWaitForFences(device, 1, &syncManager.getInFlightFences()[currentFrame], VK_TRUE, UINT64_MAX);

    // Adquiere la próxima imagen del swap chain para renderizar
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
                                            syncManager.getImageAvailableSemaphores()[currentFrame],
                                            VK_NULL_HANDLE, &imageIndex);

    // Si el swap chain está desactualizado, se debe recrear
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Error al adquirir imagen del swap chain");
    }

    // Reinicia el fence actual (para usarlo en el siguiente frame)
    vkResetFences(device, 1, &syncManager.getInFlightFences()[currentFrame]);

    // Obtiene el command buffer del frame actual
    VkCommandBuffer commandBuffer = commandManager.getCommandBuffers()[currentFrame];

    // Graba los comandos de renderizado en el command buffer
    recordCommandBuffer(commandBuffer, imageIndex, renderPass, pipeline, 
                       pipelineLayout, extent, transform);

    // Envía el command buffer a la GPU y presenta la imagen
    submitCommandBuffer(commandBuffer, graphicsQueue, presentQueue, swapChain, imageIndex);

    // Pasa al siguiente frame (para sincronización tipo doble/triple buffer)
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// ----------------------------------------------------------
// Graba todos los comandos de render en un command buffer
// ----------------------------------------------------------
void VulkanRenderer::recordCommandBuffer(VkCommandBuffer commandBuffer, 
                                         uint32_t imageIndex,
                                         VkRenderPass renderPass,
                                         VkPipeline pipeline,
                                         VkPipelineLayout pipelineLayout,
                                         VkExtent2D extent,
                                         const Transform& transform) {
    // Limpia cualquier estado previo del command buffer
    vkResetCommandBuffer(commandBuffer, 0);

    // Información inicial para comenzar a grabar
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Error al comenzar command buffer");
    }

    // Configura el render pass (qué framebuffer usar, área y color de limpieza)
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebufferManager.getFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;

    // Color de limpieza (fondo negro)
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    // Comienza el render pass
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Enlaza el pipeline gráfico
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    // Envía la matriz de transformación al shader (como push constant)
    float modelMatrix[16];
    transform.getModelMatrix(modelMatrix);
    vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT,
                       0, sizeof(modelMatrix), modelMatrix);

    // Enlaza los vertex buffers
    VkBuffer vertexBuffers[] = {bufferManager.getVertexBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    // Si hay índice, dibuja con él; si no, usa un solo vértice
    if (useIndexBuffer && bufferManager.getIndexCount() > 0) {
        vkCmdBindIndexBuffer(commandBuffer, bufferManager.getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(commandBuffer, bufferManager.getIndexCount(), 1, 0, 0, 0);
    } else {
        vkCmdDraw(commandBuffer, 1, 1, 0, 0);
    }

    // Finaliza el render pass
    vkCmdEndRenderPass(commandBuffer);

    // Termina la grabación del command buffer
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error al grabar command buffer");
    }
}

// ----------------------------------------------------------
// Envía el command buffer a la GPU y presenta el resultado
// ----------------------------------------------------------
void VulkanRenderer::submitCommandBuffer(VkCommandBuffer commandBuffer,
                                         VkQueue graphicsQueue,
                                         VkQueue presentQueue,
                                         VkSwapchainKHR swapChain,
                                         uint32_t imageIndex) {
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Esperar hasta que la imagen esté disponible para dibujar
    VkSemaphore waitSemaphores[] = {syncManager.getImageAvailableSemaphores()[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    // Señal para indicar que el render ha terminado
    VkSemaphore signalSemaphores[] = {syncManager.getRenderFinishedSemaphores()[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    // Envía el command buffer a la cola gráfica
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, syncManager.getInFlightFences()[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Error al enviar command buffer");
    }

    // Información para presentar la imagen en pantalla
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    // Presenta la imagen renderizada
    VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);

    // Si el swap chain está desactualizado, deberá recrearse
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Se omite recreación aquí (debe manejarse en VulkanApp)
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Error al presentar imagen");
    }
}

// ----------------------------------------------------------
// Libera todos los recursos asociados al renderer
// ----------------------------------------------------------
void VulkanRenderer::destroy() {
    // Si no hay dispositivo válido, no hace nada
    if (device == VK_NULL_HANDLE) return;
    
    framebufferManager.destroy(); // Libera framebuffers
    syncManager.destroy();        // Libera semáforos y fences
    bufferManager.destroy();      // Libera buffers
    commandManager.destroy();     // Libera command pools y buffers
    
    std::cout << "Renderer destruido" << std::endl;
}
