#include "graphics/VulkanRenderer.h"
#include <stdexcept>
#include <iostream>

VulkanRenderer::VulkanRenderer()
    : device(VK_NULL_HANDLE)
    , currentFrame(0) {}

VulkanRenderer::~VulkanRenderer() {
    destroy();
}

void VulkanRenderer::init(VkDevice dev, VkPhysicalDevice physDev, uint32_t graphicsQueueFamily) {
    device = dev;
    
    commandManager.init(device, graphicsQueueFamily);
    syncManager.init(device);
    bufferManager.init(device, physDev);
    bufferManager.createVertexBuffer();
    framebufferManager.init(device);
    
    std::cout << "Renderer inicializado" << std::endl;
}

void VulkanRenderer::createFramebuffers(VkRenderPass renderPass, 
                                       const std::vector<VkImageView>& imageViews, 
                                       VkExtent2D extent) {
    framebufferManager.createFramebuffers(renderPass, imageViews, extent);
}

void VulkanRenderer::updateVertexPosition(float x, float y) {
    bufferManager.updateVertexPosition(x, y);
}

void VulkanRenderer::drawFrame(VkDevice device, VkSwapchainKHR swapChain, 
                              VkQueue graphicsQueue, VkQueue presentQueue,
                              VkRenderPass renderPass, VkPipeline pipeline, 
                              VkExtent2D extent) {
    
    // Esperar a que el frame anterior termine
    vkWaitForFences(device, 1, &syncManager.getInFlightFences()[currentFrame], VK_TRUE, UINT64_MAX);
    
    // Adquirir imagen del swap chain
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, 
                                           syncManager.getImageAvailableSemaphores()[currentFrame], 
                                           VK_NULL_HANDLE, &imageIndex);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return; // Swap chain necesita recrearse
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Error al adquirir imagen del swap chain");
    }
    
    // Reset fence solo si vamos a enviar trabajo
    vkResetFences(device, 1, &syncManager.getInFlightFences()[currentFrame]);
    
    // Grabar command buffer
    VkCommandBuffer commandBuffer = commandManager.getCommandBuffers()[currentFrame];
    vkResetCommandBuffer(commandBuffer, 0);
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Error al comenzar command buffer");
    }
    
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebufferManager.getFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = extent;
    
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    
    VkBuffer vertexBuffers[] = {bufferManager.getVertexBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    
    vkCmdDraw(commandBuffer, 1, 1, 0, 0);
    
    vkCmdEndRenderPass(commandBuffer);
    
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error al grabar command buffer");
    }
    
    // Submit command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore waitSemaphores[] = {syncManager.getImageAvailableSemaphores()[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    VkSemaphore signalSemaphores[] = {syncManager.getRenderFinishedSemaphores()[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, syncManager.getInFlightFences()[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Error al enviar command buffer");
    }
    
    // Presentar
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    
    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    
    result = vkQueuePresentKHR(presentQueue, &presentInfo);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Swap chain necesita recrearse
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Error al presentar imagen");
    }
    
    // Avanzar al siguiente frame
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanRenderer::destroy() {
    if (device == VK_NULL_HANDLE) return;
    
    framebufferManager.destroy();
    syncManager.destroy();
    bufferManager.destroy();
    commandManager.destroy();
    
    std::cout << "Renderer destruido" << std::endl;
}