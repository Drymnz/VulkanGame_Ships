#include "graphics/VulkanRenderer.h"
#include <stdexcept>
#include <array>

VulkanRenderer::VulkanRenderer()
    : device(VK_NULL_HANDLE)
    , physicalDevice(VK_NULL_HANDLE)
    , currentFrame(0) {}

VulkanRenderer::~VulkanRenderer() {
    destroy();
}

void VulkanRenderer::init(VkDevice dev, VkPhysicalDevice physDev, uint32_t graphicsQueueFamily) {
    device = dev;
    physicalDevice = physDev;
    
    commandManager.init(device, graphicsQueueFamily);
    syncManager.init(device);
    bufferManager.init(device, physicalDevice);
    bufferManager.createVertexBuffer();
    framebufferManager.init(device);
}

void VulkanRenderer::createFramebuffers(VkRenderPass renderPass, const std::vector<VkImageView>& imageViews, VkExtent2D extent) {
    framebufferManager.createFramebuffers(renderPass, imageViews, extent);
}

void VulkanRenderer::updateVertexPosition(float x, float y) {
    bufferManager.updateVertexPosition(x, y);
}

void VulkanRenderer::drawFrame(VkDevice dev, VkSwapchainKHR swapChain, VkQueue graphicsQueue,
                               VkQueue presentQueue, VkRenderPass renderPass, VkPipeline pipeline, 
                               VkExtent2D extent) {
    VkFence fence = syncManager.getInFlightFence(currentFrame);
    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &fence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, 
                         syncManager.getImageAvailableSemaphore(currentFrame), VK_NULL_HANDLE, &imageIndex);

    if (imageIndex >= framebufferManager.getFramebuffers().size()) {
        throw std::runtime_error("imageIndex fuera de rango");
    }

    VkCommandBuffer commandBuffer = commandManager.getCurrentCommandBuffer(currentFrame);
    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Error al comenzar grabación de command buffer");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebufferManager.getFramebuffer(imageIndex);
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

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {syncManager.getImageAvailableSemaphore(currentFrame)};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = {syncManager.getRenderFinishedSemaphore(currentFrame)};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, syncManager.getInFlightFence(currentFrame)) != VK_SUCCESS) {
        throw std::runtime_error("Error al enviar command buffer");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(presentQueue, &presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
void VulkanRenderer::destroy() {
    framebufferManager.destroy();
    syncManager.destroy();
    bufferManager.destroy();
    commandManager.destroy();
    
    device = VK_NULL_HANDLE;
}