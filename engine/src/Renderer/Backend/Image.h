//
// Created by ivan on 25.3.21.
//

#ifndef RELOAD_IMAGE_H
#define RELOAD_IMAGE_H

#include "precompiled.h"

#include "ReloadLib/RldLib.h"
#include "ReloadLib/Containers/List.h"
#include "VulkanCommon.h"
#include "RenderCommon.h"
#include "VulkanMemory.h"

class ImageManager;

class Image{
public:
    explicit        Image(std::string name);
                    ~Image();

    void            Alloc();
    void            Purge();
    void            SubImageUpload(int mipLevel, int x, int y, int z, int width, int height, const void * pic, int pixelPitch);
    void		    CreateFromSwapImage( VkImage image, VkImageView imageView, VkFormat format, const VkExtent2D & extent );
    void            CreateSampler();

    static void     EmptyGarbage();

    [[nodiscard]]
    bool		    IsCompressed() const { return (m_imgOpts.format == FMT_DXT1 || m_imgOpts.format == FMT_DXT5 ); }

    [[nodiscard]]
    VkImage		    GetImage() const { return m_image; }

    [[nodiscard]]
    VkImageView	    GetView() const { return m_view; }

    [[nodiscard]]
    VkImageLayout   GetLayout() const { return m_layout; }

    [[nodiscard]]
    VkSampler	    GetSampler() const { return m_sampler; }
private:
    friend class ImageManager;

    int					m_refCount;
    std::string		    m_imgName;				                                // game path, including extension (except for cube maps), may be an image program
    CubeFiles			m_cubeFiles;			                                // If this is a cube map, and if so, what kind
    void				(*m_generatorFunction)(Image *image);	        // NULL for files
    TextureUsage		m_usage;				                                // Used to determine the type of compression to use

    // Sampler settings
    TextureFilter		m_filter;
    TextureRepeat		m_repeat;

    bool				m_referencedOutsideLevelLoad;
    bool				m_levelLoadReferenced;	                                // for determining if it needs to be purged

    ImageOpts           m_imgOpts;

    bool				m_bIsSwapChainImage;
    VkFormat			m_internalFormat;
    VkSampler			m_sampler;
    VkImage				m_image;
    VkImageView			m_view;
    VkImageLayout		m_layout;

    static int				    m_garbageIndex;
    static List<VkImage>		m_imageGarbage[MAX_FRAMES_IN_FLIGHT];
    static List<VkImageView>	m_viewGarbage[MAX_FRAMES_IN_FLIGHT];
    static List<VkSampler>	    m_samplerGarbage[MAX_FRAMES_IN_FLIGHT];

    VmaAllocation		        m_allocation;
    static List<VmaAllocation>  m_allocationGarbage[MAX_FRAMES_IN_FLIGHT];

};

#endif //RELOAD_IMAGE_H
