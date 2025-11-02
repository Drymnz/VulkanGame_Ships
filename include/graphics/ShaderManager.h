#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class ShaderManager {
public:
    ShaderManager();
    ~ShaderManager();

    void init(VkDevice device);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    std::vector<char> readFile(const std::string& filename);
    void destroy();

private:
    VkDevice device;
    std::vector<VkShaderModule> shaderModules;
};

#endif