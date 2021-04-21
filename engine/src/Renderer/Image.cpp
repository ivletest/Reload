//
// Created by ivan on 25.3.21.
//

#include "Image.h"
#include "VulkanCommon.h"
#include "VulkanHelpers.h"
#include "StagingManager.h"

int						Image::m_garbageIndex = 0;
List<VmaAllocation>     Image::m_allocationGarbage[MAX_FRAMES_IN_FLIGHT];
List<VkImage>		    Image::m_imageGarbage[MAX_FRAMES_IN_FLIGHT];
List<VkImageView>	    Image::m_viewGarbage[MAX_FRAMES_IN_FLIGHT];
List<VkSampler>		    Image::m_samplerGarbage[MAX_FRAMES_IN_FLIGHT];

[[maybe_unused]] VkFormat RVk_GetFormatFromTextureFormat(const TextureFormat format) {
    switch ( format ) {
        case FMT_RGBA8: return VK_FORMAT_R8G8B8A8_UNORM;
        case FMT_XRGB8: return VK_FORMAT_R8G8B8_UNORM;
        case FMT_ALPHA: return VK_FORMAT_R8_UNORM;
        case FMT_L8A8: return VK_FORMAT_R8G8_UNORM;
        case FMT_LUM8:
        case FMT_INT8: return VK_FORMAT_R8_UNORM;
        case FMT_DXT1: return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
        case FMT_DXT5: return VK_FORMAT_BC3_UNORM_BLOCK;
        case FMT_DEPTH: return vkContext.depthFormat;
        case FMT_X16: return VK_FORMAT_R16_UNORM;
        case FMT_Y16_X16: return VK_FORMAT_R16G16_UNORM;
        case FMT_RGB565: return VK_FORMAT_R5G6B5_UNORM_PACK16;
        default:
            return VK_FORMAT_UNDEFINED;
    }
}

/*
====================
VK_GetComponentMappingFromTextureFormat
====================
*/
[[maybe_unused]]  VkComponentMapping VK_GetComponentMappingFromTextureFormat(const TextureFormat format, TextureColor color) {
    VkComponentMapping componentMapping = {
            VK_COMPONENT_SWIZZLE_ZERO,
            VK_COMPONENT_SWIZZLE_ZERO,
            VK_COMPONENT_SWIZZLE_ZERO,
            VK_COMPONENT_SWIZZLE_ZERO
    };

    if (color == COLOR_FMT_GREEN_ALPHA) {
        componentMapping.r = VK_COMPONENT_SWIZZLE_ONE;
        componentMapping.g = VK_COMPONENT_SWIZZLE_ONE;
        componentMapping.b = VK_COMPONENT_SWIZZLE_ONE;
        componentMapping.a = VK_COMPONENT_SWIZZLE_G;
        return componentMapping;
    }

    switch (format) {
        case FMT_LUM8:
            componentMapping.r = VK_COMPONENT_SWIZZLE_R;
            componentMapping.g = VK_COMPONENT_SWIZZLE_R;
            componentMapping.b = VK_COMPONENT_SWIZZLE_R;
            componentMapping.a = VK_COMPONENT_SWIZZLE_ONE;
            break;
        case FMT_L8A8:
            componentMapping.r = VK_COMPONENT_SWIZZLE_R;
            componentMapping.g = VK_COMPONENT_SWIZZLE_R;
            componentMapping.b = VK_COMPONENT_SWIZZLE_R;
            componentMapping.a = VK_COMPONENT_SWIZZLE_G;
            break;
        case FMT_ALPHA:
            componentMapping.r = VK_COMPONENT_SWIZZLE_ONE;
            componentMapping.g = VK_COMPONENT_SWIZZLE_ONE;
            componentMapping.b = VK_COMPONENT_SWIZZLE_ONE;
            componentMapping.a = VK_COMPONENT_SWIZZLE_R;
            break;
        case FMT_INT8:
            componentMapping.r = VK_COMPONENT_SWIZZLE_R;
            componentMapping.g = VK_COMPONENT_SWIZZLE_R;
            componentMapping.b = VK_COMPONENT_SWIZZLE_R;
            componentMapping.a = VK_COMPONENT_SWIZZLE_R;
            break;
        default:
            componentMapping.r = VK_COMPONENT_SWIZZLE_R;
            componentMapping.g = VK_COMPONENT_SWIZZLE_G;
            componentMapping.b = VK_COMPONENT_SWIZZLE_B;
            componentMapping.a = VK_COMPONENT_SWIZZLE_A;
            break;
    }

    return componentMapping;
}

int BitsForFormat(TextureFormat format ) {
    switch ( format ) {
        case FMT_NONE:		return 0;
        case FMT_RGBA8:
        case FMT_XRGB8:		return 32;
        case FMT_RGB565:
        case FMT_L8A8:		return 16;
        case FMT_ALPHA:
        case FMT_LUM8:
        case FMT_INT8:		return 8;
        case FMT_DXT1:		return 4;
        case FMT_DXT5:		return 8;
        case FMT_DEPTH:		return 32;
        case FMT_X16:		return 16;
        case FMT_Y16_X16:	return 32;
        default:
            assert(0);
    }
}

Image::Image(std::string name)
        : m_refCount(0)
        , m_imgName(name)
        , m_cubeFiles(CF_2D)
        , m_generatorFunction(nullptr)
        , m_usage(TD_DEFAULT)
        , m_filter(TF_DEFAULT)
        , m_repeat(TR_REPEAT)
        , m_referencedOutsideLevelLoad(false)
        , m_levelLoadReferenced(false)
        , m_bIsSwapChainImage(false)
        , m_internalFormat(VK_FORMAT_UNDEFINED)
        , m_sampler(VK_NULL_HANDLE)
        , m_image(VK_NULL_HANDLE)
        , m_view(VK_NULL_HANDLE)
        , m_layout(VK_IMAGE_LAYOUT_GENERAL){}

Image::~Image() {

}

void Image::Alloc() {
    Purge();
    m_internalFormat = RVk_GetFormatFromTextureFormat(m_imgOpts.format);

    VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
    usageFlags |= (m_imgOpts.format == FMT_DEPTH)
                  ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
                  : VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    VkImageCreateInfo imgInfo = {};
    imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imgInfo.imageType = VK_IMAGE_TYPE_2D;
    imgInfo.format = m_internalFormat;
    imgInfo.extent.width = m_imgOpts.width;
    imgInfo.extent.height = m_imgOpts.height;
    imgInfo.extent.depth = 1;
    imgInfo.mipLevels = static_cast<uint32_t>(m_imgOpts.numLevels);
    imgInfo.arrayLayers = (m_imgOpts.texType == TEX_TYPE_CUBIC) ? 6 : 1;
//    imgInfo.samples = m_imgOpts.samples;
    imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imgInfo.usage = usageFlags;
    imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imgInfo.flags = (m_imgOpts.texType == TEX_TYPE_CUBIC) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

    VmaAllocationCreateInfo vmaAllocCreateInfo = {};
    vmaAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    // IF depth
    // vmaAllocCreateInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vmaCreateImage(vmaAllocator, &imgInfo, &vmaAllocCreateInfo, &m_image, &m_allocation, nullptr))

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_image;
    viewInfo.viewType = (m_imgOpts.texType == TEX_TYPE_CUBIC) ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = m_internalFormat;
    viewInfo.components = VK_GetComponentMappingFromTextureFormat(m_imgOpts.format, m_imgOpts.colorFormat);
    viewInfo.subresourceRange.aspectMask = (m_imgOpts.format == FMT_DEPTH)
//            ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT
                                           ? VK_IMAGE_ASPECT_DEPTH_BIT
                                           : VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = static_cast<uint32_t>(m_imgOpts.numLevels);
    viewInfo.subresourceRange.layerCount = m_imgOpts.texType == TEX_TYPE_CUBIC ? 6 : 1;
    viewInfo.subresourceRange.baseMipLevel = 0;

    VK_CHECK(vkCreateImageView(vkContext.device, &viewInfo, nullptr, &m_view))
}

void Image::Purge() {
    if (m_sampler != VK_NULL_HANDLE) {
        m_samplerGarbage[m_garbageIndex].Add(m_sampler);
        m_sampler = VK_NULL_HANDLE;
    }

    if ( m_image != VK_NULL_HANDLE ) {
        m_allocationGarbage[m_garbageIndex].Add(m_allocation);
        m_viewGarbage[m_garbageIndex].Add(m_view);
        m_imageGarbage[m_garbageIndex].Add(m_image);

        m_allocation = NULL;

        m_view = VK_NULL_HANDLE;
        m_image = VK_NULL_HANDLE;
    }
}

void Image::SubImageUpload(int mipLevel, int x, int y, int z, int width, int height, const void * pic, int pixelPitch ) {
    assert( x >= 0 && y >= 0 && mipLevel >= 0 && width >= 0 && height >= 0 && mipLevel < m_imgOpts.numLevels);

    if (IsCompressed()) {
        width = (width + 3) & ~3;
        height = (height + 3) & ~3;
    }

    uint32_t size = static_cast<uint32_t>(width * height * BitsForFormat(m_imgOpts.format) / 8);

    VkBuffer buffer;
    VkCommandBuffer commandBuffer;
    VkDeviceSize offset = 0;
    char * data = stagingManager.Stage(size, 16, commandBuffer, buffer, offset);

    if (m_imgOpts.format == FMT_RGB565 ) {
        char * imgData = (char *)pic;
        for (uint32_t i = 0; i < size; i += 2 ) {
            data[ i ] = imgData[i + 1];
            data[ i + 1 ] = imgData[i];
        }
    } else {
        memcpy(data, pic, size);
    }

    VkBufferImageCopy imgCopy = {};
    imgCopy.bufferOffset = offset;
    imgCopy.bufferRowLength = static_cast<uint32_t>(pixelPitch);
    imgCopy.bufferImageHeight = static_cast<uint32_t>(height);
    imgCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imgCopy.imageSubresource.layerCount = 1;
    imgCopy.imageSubresource.mipLevel = static_cast<uint32_t>(mipLevel);
    imgCopy.imageSubresource.baseArrayLayer = static_cast<uint32_t>(z);
    imgCopy.imageOffset.x = x;
    imgCopy.imageOffset.y = y;
    imgCopy.imageOffset.z = 0;
    imgCopy.imageExtent.width = static_cast<uint32_t>(width);
    imgCopy.imageExtent.height = static_cast<uint32_t>(height);
    imgCopy.imageExtent.depth = 1;

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = static_cast<uint32_t>(m_imgOpts.numLevels);
    barrier.subresourceRange.baseArrayLayer = static_cast<uint32_t>(z);
    barrier.subresourceRange.layerCount = 1;

    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);

    vkCmdCopyBufferToImage(commandBuffer, buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imgCopy);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    m_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void Image::CreateFromSwapImage(VkImage image, VkImageView imageView,
                                VkFormat format, const VkExtent2D &extent) {

}


void Image::CreateSampler() {

}

void Image::EmptyGarbage() {
    m_garbageIndex = (m_garbageIndex + 1) % static_cast<int>(MAX_FRAMES_IN_FLIGHT);

    List<VmaAllocation> & allocationsToFree = m_allocationGarbage[m_garbageIndex];
    List<VkImage> & imagesToFree = m_imageGarbage[m_garbageIndex];
    List<VkImageView> & viewsToFree = m_viewGarbage[m_garbageIndex];
    List<VkSampler> & samplersToFree = m_samplerGarbage[m_garbageIndex];

    const int numAllocations = allocationsToFree.Size();
    for ( int i = 0; i < numAllocations; ++i ) {
        vmaDestroyImage(vmaAllocator, imagesToFree[i], allocationsToFree[i]);
    }

    const int numViews = viewsToFree.Size();
    for ( int i = 0; i < numViews; ++i ) {
        vkDestroyImageView(vkContext.device, viewsToFree[i], nullptr);
    }

    const int numSamplers = samplersToFree.Size();
    for ( int i = 0; i < numSamplers; ++i ) {
        vkDestroySampler(vkContext.device, samplersToFree[ i ], nullptr);
    }

    allocationsToFree.Clear();
    imagesToFree.Clear();
    viewsToFree.Clear();
    samplersToFree.Clear();
}
