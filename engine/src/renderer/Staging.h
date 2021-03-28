//
// Created by ivan on 23.3.21.
//

#ifndef RELOAD_STAGING_H
#define RELOAD_STAGING_H

#include "RenderBackend.h"

struct StagingBuffer {
    StagingBuffer() :
            submitted( false ),
            commandBuffer( VK_NULL_HANDLE ),
            buffer( VK_NULL_HANDLE ),
            fence( VK_NULL_HANDLE ),
            offset( 0 ),
            data(nullptr) {}

    bool				submitted;
    VkCommandBuffer		commandBuffer;
    VkBuffer			buffer;
    VkFence				fence;
    VkDeviceSize		offset;
    char *				data;
};

class Staging {
public:
                    Staging();
                    ~Staging();

    void			Init();
    void			Shutdown();

    char *			Stage(uint32_t size, uint32_t alignment, VkCommandBuffer & commandBuffer, VkBuffer & buffer, int & bufferOffset);
    void			Flush();

private:
    void			Wait(StagingBuffer & stage);

private:
    uint32_t	    m_maxBufferSize;
    uint32_t		m_currentBuffer;
    char *			m_mappedData;
    VkDeviceMemory	m_memory;
    VkCommandPool	m_commandPool;

    StagingBuffer   m_buffers[ NUM_FRAME_DATA ];
};

extern Staging staging;

#endif //RELOAD_STAGING_H
