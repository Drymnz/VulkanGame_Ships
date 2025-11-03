#include "graphics/ShaderManager.h"
#include <fstream>
#include <stdexcept>

ShaderManager::ShaderManager()
    : device(VK_NULL_HANDLE) {}

ShaderManager::~ShaderManager() {
    destroy();
}

// Inicializa el administrador de shaders con el dispositivo lógico de Vulkan
void ShaderManager::init(VkDevice dev) {
    device = dev;
}

// Lee un archivo binario (por ejemplo, un .spv compilado con glslc) y devuelve su contenido en un vector de bytes
std::vector<char> ShaderManager::readFile(const std::string& filename) {
    // Abre el archivo al final (ate) en modo binario
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Error al abrir archivo: " + filename);
    }

    // Obtiene el tamaño del archivo
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    // Regresa al inicio y lee todo el contenido
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

// Crea un módulo de shader (VkShaderModule) a partir del código SPIR-V cargado en memoria
VkShaderModule ShaderManager::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); // SPIR-V se trata como uint32_t

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear shader module");
    }
    
    // Guarda el módulo para destruirlo más tarde
    shaderModules.push_back(shaderModule);
    return shaderModule;
}

// Destruye todos los módulos de shader creados por el administrador
void ShaderManager::destroy() {
    if (device == VK_NULL_HANDLE) return;

    for (auto module : shaderModules) {
        vkDestroyShaderModule(device, module, nullptr);
    }
    shaderModules.clear();
}
