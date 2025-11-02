#ifndef SYNC_MANAGER_H
#define SYNC_MANAGER_H

#include <vulkan/vulkan.h>
#include <vector>

class SyncManager {
public:
    SyncManager();
    ~SyncManager();

    void init(VkDevice device);
    void destroy();

    VkSemaphore getImageAvailableSemaphore(size_t frame) const { return imageAvailableSemaphores[frame]; }
    VkSemaphore getRenderFinishedSemaphore(size_t frame) const { return renderFinishedSemaphores[frame]; }
    VkFence getInFlightFence(size_t frame) const { return inFlightFences[frame]; }

private:
    void createSyncObjects();

    static const int MAX_FRAMES_IN_FLIGHT = 2;

    VkDevice device;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
};

#endif