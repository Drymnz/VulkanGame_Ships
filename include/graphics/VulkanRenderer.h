#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanConfig.h"
#include "BufferManager.h"
#include "CommandManager.h"
#include "SyncManager.h"
#include "FramebufferManager.h"

class VulkanRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void init(VkDevice dev, VkPhysicalDevice physDev, uint32_t graphicsQueueFamily);

    void loadModel(const std::vector<Vertex3D>& vertices, const std::vector<uint32_t>& indices);
    void createDefaultBuffer();

    void createFramebuffers(VkRenderPass renderPass,
                            const std::vector<VkImageView>& imageViews,
                            VkExtent2D extent);

    // ────────────────────────────────────────────────
    // Posición del modelo 3D
    // x = vertical (arriba / abajo en pantalla)
    // y = profundidad (alejamiento de cámara, negativo = más lejos)
    // z = horizontal (izquierda / derecha en pantalla)
    // ────────────────────────────────────────────────
    void setModelPosition(float x, float y, float z);

    void drawFrame(VkDevice device, VkSwapchainKHR swapChain,
                   VkQueue graphicsQueue, VkQueue presentQueue,
                   VkRenderPass renderPass, VkPipeline pipeline,
                   VkPipelineLayout pipelineLayout, VkExtent2D extent);

    void destroy();

private:
    VkDevice device{};
    uint32_t currentFrame{0};

    BufferManager bufferManager;
    CommandManager commandManager;
    SyncManager syncManager;
    FramebufferManager framebufferManager;

    bool useIndexBuffer{false};

    float modelX{0.0f};
    float modelY{0.0f};
    float modelZ{0.0f};

    void createModelMatrix(float* matrix, float x, float y, float z);
};
