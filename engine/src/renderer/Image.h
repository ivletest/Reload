//
// Created by ivan on 25.3.21.
//

#ifndef RELOAD_IMAGE_H
#define RELOAD_IMAGE_H

#include "../lib/RldLib.h"
#include "../Common.h"

using TextureSamples = VkSampleCountFlagBits;

enum TextureType {
    TEX_TYPE_DISABLED,
    TEX_TYPE_2D,
    TEX_TYPE_CUBIC
};

/*
================================================
DXT5 color formats
================================================
*/
enum TextureColor {
    COLOR_FMT_DEFAULT,			// RGBA
    COLOR_FMT_NORMAL_DXT5,		// XY format and use the fast DXT5 compressor
    COLOR_FMT_YCOCG_DXT5,			// convert RGBA to CoCg_Y format
    COLOR_FMT_GREEN_ALPHA			// Copy the alpha channel to green
};

struct ImageOptions {
public:
    ImageOptions()
        : textureType(TEX_TYPE_2D)
        , format(VK_FORMAT_UNDEFINED)
        , width(0)
        , height(0)
        , colorFormat(COLOR_FMT_DEFAULT)
        , samples(VK_SAMPLE_COUNT_1_BIT)
        , usage(VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
        , mipLevels(0)
        , gammaMips(false)
        , readback(false)
    {}

    bool operator==( const ImageOptions & opts ) {
        return (memcmp(this, &opts, sizeof( *this )) == 0);
    }

    TextureType	    	    textureType;
    VkFormat		        format;
    uint32_t			    width;
    uint32_t			    height;			                                    // not needed for cube maps
    TextureColor		    colorFormat;
    TextureSamples	        samples;
    VkImageUsageFlags       usage;
    VkMemoryPropertyFlags   properties;
    VkImage *               image;
    VkDeviceMemory *        imageMem;
    int					    mipLevels;		                                    // if 0, will be 1 for NEAREST / LINEAR filters, otherwise based on size
    bool				    gammaMips;		                                    // if true, mips will be generated with gamma correction
    bool				    readback;		                                    // 360 specific - cpu reads back from this texture, so allocate with cached memory
};

class Image{
public:
//    static VkResult     Create(ImageOptions opts);
};


#endif //RELOAD_IMAGE_H
