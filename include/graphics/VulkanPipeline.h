#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include "Vertex.h"
#include "ShaderManager.h"

class VulkanPipeline {
public:
    VulkanPipeline();
    ~VulkanPipeline();

    void createGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VkExtent2D swapChainExtent);
    void destroy();

    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

private:
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkDevice device;
    
    ShaderManager shaderManager;
};

#endif