#include "graphics/CommandManager.h"
#include <stdexcept>
#include <iostream>

// Constructor: inicializa los handles de Vulkan como nulos
CommandManager::CommandManager()
    : device(VK_NULL_HANDLE)
    , commandPool(VK_NULL_HANDLE) {}

// Destructor: libera los recursos Vulkan usados
CommandManager::~CommandManager() {
    destroy();
}

// Inicializa el administrador de comandos
// Recibe el dispositivo lógico y el índice de la cola de gráficos
void CommandManager::init(VkDevice dev, uint32_t graphicsQueueFamily) {
    device = dev;
    createCommandPool(graphicsQueueFamily); // Crear el pool donde se gestionan los command buffers
    createCommandBuffers();                 // Crear los buffers de comando
}

// Crea el command pool desde el cual se asignarán los command buffers
void CommandManager::createCommandPool(uint32_t graphicsQueueFamily) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamily; // Familia de colas que usará este pool
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    // Permite reiniciar individualmente los command buffers

    // Crear el command pool
    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear command pool");
    }
}

// Crea los command buffers que se usarán en cada frame
void CommandManager::createCommandBuffers() {
    // Reservar espacio para los command buffers (uno por frame)
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool; // Pool desde el que se asignan
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; 
    // Nivel primario: puede ser enviado directamente a la GPU
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    // Asignar los command buffers desde el pool
    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Error al crear command buffers");
    }
    
    std::cout << "Command buffers creados (" << commandBuffers.size() << ")" << std::endl;
}

// Libera los recursos Vulkan asociados al administrador de comandos
void CommandManager::destroy() {
    if (device == VK_NULL_HANDLE) return; // Si no hay dispositivo válido, salir

    // Destruir el pool (esto también destruye automáticamente todos los command buffers asociados)
    if (commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device, commandPool, nullptr);
        commandPool = VK_NULL_HANDLE;
    }
}
