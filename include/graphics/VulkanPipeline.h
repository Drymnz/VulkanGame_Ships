// ============================================
// VulkanPipeline.h - AGREGAR ESTOS MIEMBROS
// ============================================

#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

#include <vulkan/vulkan.h>
#include "graphics/ShaderManager.h"
#include "graphics/Vertex.h"

struct PushConstants {
    float modelMatrix[16];
};

class VulkanPipeline {
public:
    VulkanPipeline();
    ~VulkanPipeline();

    void createGraphicsPipeline(VkDevice dev, VkRenderPass renderPass, VkExtent2D swapChainExtent);
    void destroy();

    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }  // NUEVO

private:
    VkDevice device;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkDescriptorSetLayout descriptorSetLayout;  // NUEVO
    ShaderManager shaderManager;
    
    void createDescriptorSetLayout();  // NUEVO
};

#endif // VULKAN_PIPELINE_H