//
// Created by ivan on 25.3.21.
//

#include "Image.h"
#include "RenderBackend.h"

//VkResult Image::Create(ImageOptions opts) {
//    VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
//    if (opts.format ==  vkContext.depthFormat) {
//        usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
//    } else {
//        usageFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
//    }
//
//    VkImageCreateInfo createInfo = {};
//    createInfo.sType            = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//    createInfo.flags            = opts.textureType == TEX_TYPE_CUBIC
//                                ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
//                                : 0;
//    createInfo.imageType        = VK_IMAGE_TYPE_2D;
//    createInfo.format           = opts.format;
//    createInfo.extent.width     = opts.width;
//    createInfo.extent.height    = opts.height;
//    createInfo.extent.depth     = 1;
//    createInfo.mipLevels        = static_cast<uint32_t>(opts.mipLevels);
//    createInfo.arrayLayers      = opts.textureType == TEX_TYPE_CUBIC ? 6 : 1;
//    createInfo.samples          = vkContext.sampleCount;
//    createInfo.tiling           = VK_IMAGE_TILING_OPTIMAL;
//    createInfo.usage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
//                                  | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT
//                                  | VK_IMAGE_USAGE_SAMPLED_BIT
//                                  | VK_IMAGE_USAGE_STORAGE_BIT;
//    createInfo.initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
//    createInfo.sharingMode      = VK_SHARING_MODE_EXCLUSIVE;
//}