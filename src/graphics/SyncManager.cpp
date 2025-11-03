#include "graphics/SyncManager.h"
#include <stdexcept>
#include <iostream>

SyncManager::SyncManager()
    : device(VK_NULL_HANDLE) {}

SyncManager::~SyncManager() {
    destroy();
}

// Inicializa el administrador de sincronización
// Guarda el dispositivo lógico y crea los semáforos y fences
void SyncManager::init(VkDevice dev) {
    device = dev;
    createSyncObjects();
}

// Crea los objetos de sincronización necesarios para el renderizado en paralelo
// Incluye semáforos y fences para manejar múltiples frames en vuelo
void SyncManager::createSyncObjects() {
    // Reservar espacio para cada frame en vuelo (triple buffering típico)
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    // Información base para crear un semáforo
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // Información base para crear un fence (inicialmente señalado)
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Permite que el primer frame no bloquee

    // Crear los objetos de sincronización para cada frame
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("Error al crear objetos de sincronización");
        }
    }
    
    std::cout << "Objetos de sincronización creados" << std::endl;
}

// Libera todos los recursos de sincronización asociados al dispositivo
void SyncManager::destroy() {
    if (device == VK_NULL_HANDLE) return;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (i < imageAvailableSemaphores.size() && imageAvailableSemaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        if (i < renderFinishedSemaphores.size() && renderFinishedSemaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        if (i < inFlightFences.size() && inFlightFences[i] != VK_NULL_HANDLE)
            vkDestroyFence(device, inFlightFences[i], nullptr);
    }

    // Limpia los vectores
    imageAvailableSemaphores.clear();
    renderFinishedSemaphores.clear();
    inFlightFences.clear();
}
