#ifndef SYNC_MANAGER_H
#define SYNC_MANAGER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanConfig.h"

class SyncManager {
public:
    SyncManager();
    ~SyncManager();

    void init(VkDevice device);
    void destroy();

    // Getters que devuelven los vectores completos
    const std::vector<VkSemaphore>& getImageAvailableSemaphores() const { return imageAvailableSemaphores; }
    const std::vector<VkSemaphore>& getRenderFinishedSemaphores() const { return renderFinishedSemaphores; }
    const std::vector<VkFence>& getInFlightFences() const { return inFlightFences; }

private:
    VkDevice device;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    void createSyncObjects();
};

#endif // SYNC_MANAGER_H