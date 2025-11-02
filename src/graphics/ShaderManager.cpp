#include "graphics/ShaderManager.h"
#include <fstream>
#include <stdexcept>

ShaderManager::ShaderManager()
    : device(VK_NULL_HANDLE) {}

ShaderManager::~ShaderManager() {
    destroy();
}

void ShaderManager::init(VkDevice dev) {
    device = dev;
}

std::vector<char> ShaderManager::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Error al abrir archivo: " + filename);
    }
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

VkShaderModule ShaderManager::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear shader module");
    }
    
    shaderModules.push_back(shaderModule);
    return shaderModule;
}

void ShaderManager::destroy() {
    if (device == VK_NULL_HANDLE) return;

    for (auto module : shaderModules) {
        vkDestroyShaderModule(device, module, nullptr);
    }
    shaderModules.clear();
}