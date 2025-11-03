#pragma once
#include <vulkan/vulkan.h>
#include "ShaderManager.h"
#include "Vertex.h"

struct PushConstants {
    float modelMatrix[16];  // Matriz 4x4
};

class VulkanPipeline {
public:
    VulkanPipeline();
    ~VulkanPipeline();

    void createGraphicsPipeline(VkDevice dev, VkRenderPass renderPass, VkExtent2D swapChainExtent);
    void destroy();

    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

private:
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkDevice device;
    ShaderManager shaderManager;
};