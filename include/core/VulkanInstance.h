#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class VulkanInstance
{
public:
    VulkanInstance();
    ~VulkanInstance();

    void create();
    void destroy();
    
    VkInstance getInstance() const { return instance; }

private:
    VkInstance instance;
};

#endif // VULKAN_INSTANCE_H