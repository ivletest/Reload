//
// Created by ivan on 23.3.21.
//

#include "StagingManager.h"

#include "../ConfigManager.h"
#include "VulkanCommon.h"
#include "VulkanHelpers.h"
#include "VulkanMemory.h"

StagingManager stagingManager;

/*
================================================================================
StagingManager::StagingManager

DESCRIPTION:
The default constructor.
================================================================================
*/
StagingManager::StagingManager() {
    m_maxBufferSize = 0;
    m_currentBuffer = 0;
    m_mappedData    = nullptr;
    m_memory        = VK_NULL_HANDLE;
    m_commandPool   = VK_NULL_HANDLE;
}

/*
================================================================================
StagingManager::StagingManagerManager

DESCRIPTION:
The default destructor.
================================================================================
*/
StagingManager::~StagingManager() = default;

/*
================================================================================
StagingManager::Init

DESCRIPTION:
Initializes the staging manager.
================================================================================
*/
void StagingManager::Init() {
    m_maxBufferSize = vkConfig.uploadBufferSizeMB * 1024 * 1024;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType  = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size   = (VkDeviceSize)m_maxBufferSize;
    bufferCreateInfo.usage  = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    for (auto & m_buffer : m_buffers) {
        m_buffer.offset = 0;

        VK_CHECK(vkCreateBuffer(vkContext.device, &bufferCreateInfo, nullptr, &m_buffer.buffer))
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(vkContext.device, m_buffers[0].buffer, &memoryRequirements);

    const VkDeviceSize alignMod = memoryRequirements.size % memoryRequirements.alignment;
    const VkDeviceSize alignedSize = (alignMod == 0)
            ? memoryRequirements.size
            : memoryRequirements.size + memoryRequirements.alignment - alignMod;

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = alignedSize * MAX_FRAMES_IN_FLIGHT;
    memoryAllocateInfo.memoryTypeIndex = FindMemoryTypeIndex(memoryRequirements.memoryTypeBits, VK_MEMORY_USAGE_CPU_TO_GPU);

    VK_CHECK(vkAllocateMemory(vkContext.device, &memoryAllocateInfo, nullptr, &m_memory))

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
        VK_CHECK(vkBindBufferMemory(vkContext.device, m_buffers[i].buffer, m_memory, i * alignedSize))
    }

    VK_CHECK(vkMapMemory(vkContext.device, m_memory, 0, alignedSize * MAX_FRAMES_IN_FLIGHT, 0, reinterpret_cast< void ** >( &m_mappedData )))

    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = static_cast<uint32_t>(vkContext.graphicsFamilyIdx);
    VK_CHECK(vkCreateCommandPool(vkContext.device, &commandPoolCreateInfo, nullptr, &m_commandPool))

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = m_commandPool;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i ) {
        VK_CHECK(vkAllocateCommandBuffers(vkContext.device, &commandBufferAllocateInfo, &m_buffers[i].commandBuffer))
        VK_CHECK(vkCreateFence(vkContext.device, &fenceCreateInfo, nullptr, &m_buffers[i].fence))
        VK_CHECK(vkBeginCommandBuffer(m_buffers[ i ].commandBuffer, &commandBufferBeginInfo))

        m_buffers[i].data = (char *)m_mappedData + (i * alignedSize);
    }
}

/*
================================================================================
StagingManager::Shutdown

DESCRIPTION:
Shuts down the stagingManager manager.
================================================================================
*/
void StagingManager::Shutdown() {
    vkUnmapMemory(vkContext.device, m_memory);
    m_memory = VK_NULL_HANDLE;
    m_mappedData = nullptr;

    for (auto & buffer : m_buffers) {
        vkDestroyFence(vkContext.device, buffer.fence, nullptr);
        vkDestroyBuffer(vkContext.device, buffer.buffer, nullptr);
        vkFreeCommandBuffers(vkContext.device, m_commandPool, 1, &buffer.commandBuffer);
    }
    memset(m_buffers, 0, sizeof(m_buffers));

    m_maxBufferSize = 0;
    m_currentBuffer = 0;
}

/*
================================================================================
Class::Method

DESCRIPTION:
RETURNS:
NOTE:
================================================================================
*/
char *StagingManager::Stage(uint32_t size, uint32_t alignment,
                            VkCommandBuffer &commandBuffer, VkBuffer &buffer,
                            VkDeviceSize &bufferOffset) {
    if (size > m_maxBufferSize) {
        SDL_LogCritical(
                LOG_RENDER,
                "Can't allocate %d MB in gpu transfer buffer",
                (int)(size / 1024 / 1024 ));
    }

    StagingBuffer * stage = &m_buffers[m_currentBuffer];
    uint32_t alignMod = (uint32_t)stage->offset % alignment;
    stage->offset = ((stage->offset % alignment) == 0)
            ? stage->offset
            : (stage->offset + alignment - alignMod);

    if ((stage->offset + size) >= (m_maxBufferSize) && !stage->submitted) {
        Flush();
    }

    stage = &m_buffers[m_currentBuffer];
    if (stage->submitted) {
        Wait(*stage);
    }

    commandBuffer = stage->commandBuffer;
    buffer = stage->buffer;
    bufferOffset = stage->offset;

    char * data = stage->data + stage->offset;
    stage->offset += size;

    return data;
}

/*
================================================================================
Class::Method

DESCRIPTION:
RETURNS:
NOTE:
================================================================================
*/
void StagingManager::Flush() {
    StagingBuffer & stage = m_buffers[m_currentBuffer];
    if (stage.submitted || stage.offset == 0) {
        return;
    }

    VkMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT;
    vkCmdPipelineBarrier(
            stage.commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
            0, 1, &barrier, 0, nullptr, 0, nullptr);

    vkEndCommandBuffer(stage.commandBuffer);

    VkMappedMemoryRange memoryRange = {};
    memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    memoryRange.memory = m_memory;
    memoryRange.size = VK_WHOLE_SIZE;
    vkFlushMappedMemoryRanges(vkContext.device, 1, &memoryRange);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &stage.commandBuffer;

    vkQueueSubmit(vkContext.graphicsQueue, 1, &submitInfo, stage.fence);

    stage.submitted = true;

    m_currentBuffer = (m_currentBuffer + 1) % MAX_FRAMES_IN_FLIGHT;
}

void StagingManager::Wait(StagingBuffer &stage) {
    if (!stage.submitted) {
        return;
    }

    VK_CHECK(vkWaitForFences(vkContext.device, 1, &stage.fence, VK_TRUE, UINT64_MAX))
    VK_CHECK(vkResetFences(vkContext.device, 1, &stage.fence))

    stage.offset = 0;
    stage.submitted = false;

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VK_CHECK(vkBeginCommandBuffer(stage.commandBuffer, &commandBufferBeginInfo))
}