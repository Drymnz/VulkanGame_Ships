#include "graphics/VulkanRenderer.h"
#include <stdexcept>
#include <iostream>
#include <cstring>

VulkanRenderer::VulkanRenderer()
    : device(VK_NULL_HANDLE), physicalDevice(VK_NULL_HANDLE), descriptorPool(VK_NULL_HANDLE), currentFrame(0), useIndexBuffer(false) {}

VulkanRenderer::~VulkanRenderer()
{
    destroy();
}

void VulkanRenderer::init(VkDevice dev, VkPhysicalDevice physDev, uint32_t graphicsQueueFamily)
{
    device = dev;
    physicalDevice = physDev;
    commandManager.init(device, graphicsQueueFamily);
    syncManager.init(device);
    bufferManager.init(device, physDev);
    framebufferManager.init(device);
    std::cout << "Renderer inicializado" << std::endl;
}

void VulkanRenderer::createFramebuffers(VkRenderPass renderPass,
                                        const std::vector<VkImageView> &imageViews,
                                        VkExtent2D extent)
{
    framebufferManager.createFramebuffers(renderPass, imageViews, extent);
}

ModelBuffers VulkanRenderer::loadNamedModel(const std::vector<Vertex3D> &vertices,
                                            const std::vector<uint32_t> &indices,
                                            const std::string &name)
{
    return bufferManager.createModelBuffers(vertices, indices, name);
}

// ============================================
// UNIFORM BUFFERS PARA ILUMINACIÓN
// ============================================

uint32_t VulkanRenderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("No se encontró tipo de memoria adecuado");
}

void VulkanRenderer::createUniformBuffers(VkPhysicalDevice physicalDevice)
{
    VkDeviceSize bufferSize = sizeof(LightUBO);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &uniformBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Error al crear uniform buffer");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, uniformBuffers[i], &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &uniformBuffersMemory[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Error al asignar memoria uniform buffer");
        }

        vkBindBufferMemory(device, uniformBuffers[i], uniformBuffersMemory[i], 0);
    }

    // Inicializar con datos por defecto
    LightUBO defaultLight;
    updateLightData(defaultLight);

    std::cout << "Uniform buffers creados (iluminación)" << std::endl;
}

void VulkanRenderer::createDescriptorPool(VkDevice device)
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Error al crear descriptor pool");
    }

    std::cout << "Descriptor pool creado" << std::endl;
}

void VulkanRenderer::createDescriptorSets(VkDevice device, VkDescriptorSetLayout descriptorSetLayout)
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Error al asignar descriptor sets");
    }

    // Configurar los descriptor sets
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(LightUBO);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }

    std::cout << "Descriptor sets creados y configurados" << std::endl;
}

void VulkanRenderer::updateLightData(const LightUBO &lightData)
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        void *data;
        vkMapMemory(device, uniformBuffersMemory[i], 0, sizeof(LightUBO), 0, &data);
        memcpy(data, &lightData, sizeof(LightUBO));
        vkUnmapMemory(device, uniformBuffersMemory[i]);
    }
}

// ============================================
// RENDERIZADO
// ============================================

void VulkanRenderer::drawFrame(VkDevice device,
                               VkSwapchainKHR swapChain,
                               VkQueue graphicsQueue,
                               VkQueue presentQueue,
                               VkRenderPass renderPass,
                               VkPipeline pipeline,
                               VkPipelineLayout pipelineLayout,
                               VkDescriptorSetLayout descriptorSetLayout,
                               VkExtent2D extent,
                               const std::vector<GameObject *> &gameObjects)
{

    vkWaitForFences(device, 1, &syncManager.getInFlightFences()[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
                                            syncManager.getImageAvailableSemaphores()[currentFrame],
                                            VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Error al adquirir imagen del swap chain");
    }

    vkResetFences(device, 1, &syncManager.getInFlightFences()[currentFrame]);

    VkCommandBuffer commandBuffer = commandManager.getCommandBuffers()[currentFrame];

    recordCommandBuffer(commandBuffer, imageIndex, renderPass, pipeline,
                        pipelineLayout, extent, gameObjects);

    submitCommandBuffer(commandBuffer, graphicsQueue, presentQueue, swapChain, imageIndex);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// En la función recordCommandBuffer, comenta esta línea:
void VulkanRenderer::recordCommandBuffer(VkCommandBuffer commandBuffer,
                                         uint32_t imageIndex,
                                         VkRenderPass renderPass,
                                         VkPipeline pipeline,
                                         VkPipelineLayout pipelineLayout,
                                         VkExtent2D extent,
                                         const std::vector<GameObject*>& gameObjects) {
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

    // 🌤️ COLOR DE CIELO CLARO (azul cielo)
    VkClearValue clearColor = {{{0.53f, 0.81f, 0.92f, 1.0f}}};  // Azul cielo claro
    
    // Otras opciones:
    // {{{0.7f, 0.85f, 1.0f, 1.0f}}}     // Azul cielo muy claro
    // {{{0.9f, 0.95f, 1.0f, 1.0f}}}     // Casi blanco (nublado)
    // {{{0.4f, 0.6f, 0.8f, 1.0f}}}      // Azul más oscuro
    
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    // Enlazar el descriptor set con los datos de iluminación
    if (!descriptorSets.empty()) {
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                               pipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
    }

    // Renderizar cada objeto
    for (const auto* gameObject : gameObjects) {
        if (!gameObject || !gameObject->hasBuffers()) continue;

        // Enviar matriz de transformación
        float modelMatrix[16];
        gameObject->getTransform().getModelMatrix(modelMatrix);
        vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT,
                          0, sizeof(modelMatrix), modelMatrix);

        // Enlazar buffers del objeto
        VkBuffer vertexBuffers[] = {gameObject->getVertexBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, gameObject->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

        // Dibujar
        vkCmdDrawIndexed(commandBuffer, gameObject->getIndexCount(), 1, 0, 0, 0);
    }

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error al grabar command buffer");
    }
}

void VulkanRenderer::submitCommandBuffer(VkCommandBuffer commandBuffer,
                                         VkQueue graphicsQueue,
                                         VkQueue presentQueue,
                                         VkSwapchainKHR swapChain,
                                         uint32_t imageIndex)
{
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

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, syncManager.getInFlightFences()[currentFrame]) != VK_SUCCESS)
    {
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

    VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        // Recreación del swapchain (manejado externamente)
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Error al presentar imagen");
    }
}

void VulkanRenderer::destroy()
{
    if (device == VK_NULL_HANDLE)
        return;

    // Destruir uniform buffers
    for (size_t i = 0; i < uniformBuffers.size(); i++)
    {
        if (uniformBuffers[i] != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        }
        if (uniformBuffersMemory[i] != VK_NULL_HANDLE)
        {
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }
    }

    // Destruir descriptor pool (automáticamente libera los descriptor sets)
    if (descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }

    framebufferManager.destroy();
    syncManager.destroy();
    bufferManager.destroy();
    commandManager.destroy();

    std::cout << "Renderer destruido" << std::endl;
}