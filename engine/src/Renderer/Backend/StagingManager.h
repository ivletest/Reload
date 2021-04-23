//
// Created by ivan on 23.3.21.
//

#ifndef RELOAD_STAGING_MANAGER_H
#define RELOAD_STAGING_MANAGER_H

#include "RenderCommon.h"
#include "VulkanCommon.h"

class StagingManager {
public:
                    StagingManager();
                    ~StagingManager();

    void			Init();
    void			Shutdown();

    char *			Stage(uint32_t size, uint32_t alignment, VkCommandBuffer & commandBuffer, VkBuffer & buffer, VkDeviceSize & bufferOffset);
    void			Flush();

private:
    void			Wait(RVkStagingBuffer & stage);

private:
    uint32_t	    m_maxBufferSize;
    uint32_t		m_currentBuffer;
    char *			m_mappedData;
    VkDeviceMemory	m_memory;
    VkCommandPool	m_commandPool;

    RVkStagingBuffer   m_buffers[MAX_FRAMES_IN_FLIGHT];
};

extern StagingManager stagingManager;

#endif //RELOAD_STAGING_MANAGER_H
