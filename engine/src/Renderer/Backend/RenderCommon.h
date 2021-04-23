//
// Created by ivan on 20.3.21.
//

#ifndef RELOAD_RENDER_COMMON_H
#define RELOAD_RENDER_COMMON_H

#include "Common.h"

// everything that is needed by the backend needs
// to be double buffered to allow it to run in
// parallel on a dual cpu machine
static const uint32_t MAX_FRAMES_IN_FLIGHT	= 2;

static const int MAX_DESC_SETS				= 16384;
static const int MAX_DESC_UNIFORM_BUFFERS	= 8192;
static const int MAX_DESC_IMAGE_SAMPLERS	= 12384;
static const int MAX_DESC_SET_WRITES		= 32;
static const int MAX_DESC_SET_UNIFORMS		= 48;
static const int MAX_IMAGE_PARMS			= 16;
static const int MAX_UBO_PARMS				= 2;
static const int NUM_TIMESTAMP_QUERIES		= 16;

typedef enum {
    TEX_TYPE_DISABLED,
    TEX_TYPE_2D,
    TEX_TYPE_CUBIC
} TextureType;

typedef enum {
    COLOR_FMT_DEFAULT,			// RGBA
    COLOR_FMT_NORMAL_DXT5,		// XY format and use the fast DXT5 compressor
    COLOR_FMT_YCOCG_DXT5,			// convert RGBA to CoCg_Y format
    COLOR_FMT_GREEN_ALPHA			// Copy the alpha channel to green
} TextureColor;

typedef enum {
    FMT_NONE,

    //------------------------
    // Standard color image formats
    //------------------------

    FMT_RGBA8,			// 32 bpp
    FMT_XRGB8,			// 32 bpp

    //------------------------
    // Alpha channel only
    //------------------------

    // Alpha ends up being the same as L8A8 in our current implementation, because straight
    // alpha gives 0 for color, but we want 1.
    FMT_ALPHA,

    //------------------------
    // Luminance replicates the value across RGB with a constant A of 255
    // Intensity replicates the value across RGBA
    //------------------------

    FMT_L8A8,			// 16 bpp
    FMT_LUM8,			//  8 bpp
    FMT_INT8,			//  8 bpp

    //------------------------
    // Compressed texture formats
    //------------------------

    FMT_DXT1,			// 4 bpp
    FMT_DXT5,			// 8 bpp

    //------------------------
    // Depth buffer formats
    //------------------------

    FMT_DEPTH,			// 24 bpp

    //------------------------
    //
    //------------------------

    FMT_X16,			// 16 bpp
    FMT_Y16_X16,		// 32 bpp
    FMT_RGB565,			// 16 bpp
} TextureFormat;

// How is this texture used?  Determines the storage and color format
typedef enum {
    TD_SPECULAR,			// may be compressed, and always zeros the alpha channel
    TD_DIFFUSE,				// may be compressed
    TD_DEFAULT,				// generic RGBA texture (particles, etc...)
    TD_BUMP,				// may be compressed with 8 bit lookup
    TD_FONT,				// Font image
    TD_LIGHT,				// Light image
    TD_LOOKUP_TABLE_MONO,	// Mono lookup table (including alpha)
    TD_LOOKUP_TABLE_ALPHA,	// Alpha lookup table with a white color channel
    TD_LOOKUP_TABLE_RGB1,	// RGB lookup table with a solid white alpha
    TD_LOOKUP_TABLE_RGBA,	// RGBA lookup table
    TD_COVERAGE,			// coverage map for fill depth pass when YCoCG is used
    TD_DEPTH,				// depth buffer copy for motion blur
} TextureUsage;

typedef enum {
    CF_2D,			// not a cube map
    CF_NATIVE,
    CF_CAMERA
} CubeFiles;

typedef enum {
    TF_LINEAR,
    TF_NEAREST,
    TF_DEFAULT				// use the user-specified r_textureFilter
} TextureFilter;

typedef enum {
    TR_REPEAT,
    TR_CLAMP,
    TR_CLAMP_TO_ZERO,		// guarantee 0,0,0,255 edge for projected textures
    TR_CLAMP_TO_ZERO_ALPHA	// guarantee 0 alpha edge for projected textures
} TextureRepeat;

typedef struct {
    int		stayTime;		// msec for no change
    int		fadeTime;		// msec to fade vertex colors over
    float	start[4];		// vertex color at spawn (possibly out of 0.0 - 1.0 range, will clamp after calc)
    float	end[4];			// vertex color at fade-out (possibly out of 0.0 - 1.0 range, will clamp after calc)
} DecalInfo;

struct ImageOpts {
    bool operator==( const ImageOpts & opts ) {
        return (memcmp(this, &opts, sizeof( *this )) == 0);
    }

    TextureType	    	    texType = TEX_TYPE_2D;
    TextureFormat		    format = FMT_NONE;
    uint32_t			    width = 0;
    uint32_t			    height = 0;			                                    // not needed for cube maps
    TextureColor		    colorFormat = COLOR_FMT_DEFAULT;
    VkSampleCountFlagBits	samples = VK_SAMPLE_COUNT_1_BIT;
    VkImageUsageFlags       usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags   properties{};
    VkImage *               image = nullptr;
    VkDeviceMemory *        imageMem = nullptr;
    VkImageView *           imageView = nullptr;
    int					    numLevels = 0;		                                // if 0, will be 1 for NEAREST / LINEAR filters, otherwise based on size
    bool				    gammaMips = false;		                            // if true, mips will be generated with gamma correction
    bool				    readback = false;		                            // 360 specific - cpu reads back from this texture, so allocate with cached memory
};

struct ViewDefiniton {
    // specified in the call to DrawScene()
//    renderView_t		renderView;
//
//    float				projectionMatrix[16];
//    idRenderMatrix		projectionRenderMatrix;	// tech5 version of projectionMatrix
//    viewEntity_t		worldSpace;

//    idRenderWorld *renderWorld;
//
//    idVec3				initialViewAreaOrigin;
//    // Used to find the portalArea that view flooding will take place from.
//    // for a normal view, the initialViewOrigin will be renderView.viewOrg,
//    // but a mirror may put the projection origin outside
//    // of any valid area, or in an unconnected area of the map, so the view
//    // area must be based on a point just off the surface of the mirror / subview.
//    // It may be possible to get a failed portal pass if the plane of the
//    // mirror intersects a portal, and the initialViewAreaOrigin is on
//    // a different side than the renderView.viewOrg is.
//
//    bool				isSubview;				// true if this view is not the main view
//    bool				isMirror;				// the portal is a mirror, invert the face culling
//    bool				isXraySubview;
//
//    bool				isEditor;
//    bool				is2Dgui;
//
//    int					numClipPlanes;			// mirrors will often use a single clip plane
//    idPlane				clipPlanes[MAX_CLIP_PLANES];		// in world space, the positive side
//    // of the plane is the visible side
//    idScreenRect		viewport;				// in real pixels and proper Y flip
//
//    idScreenRect		scissor;
//    // for scissor clipping, local inside renderView viewport
//    // subviews may only be rendering part of the main view
//    // these are real physical pixel values, possibly scaled and offset from the
//    // renderView x/y/width/height
//
//    viewDef_t *			superView;				// never go into an infinite subview loop
//    const drawSurf_t *	subviewSurface;
//
//    // drawSurfs are the visible surfaces of the viewEntities, sorted
//    // by the material sort parameter
//    drawSurf_t **		drawSurfs;				// we don't use an idList for this, because
//    int					numDrawSurfs;			// it is allocated in frame temporary memory
//    int					maxDrawSurfs;			// may be resized
//
//    viewLight_t	*		viewLights;			// chain of all viewLights effecting view
//    viewEntity_t *		viewEntitys;			// chain of all viewEntities effecting view, including off screen ones casting shadows
//    // we use viewEntities as a check to see if a given view consists solely
//    // of 2D rendering, which we can optimize in certain ways.  A 2D view will
//    // not have any viewEntities
//
//    idPlane				frustum[6];				// positive sides face outward, [4] is the front clip plane
//
//    int					areaNum;				// -1 = not in a valid area
//
//    // An array in frame temporary memory that lists if an area can be reached without
//    // crossing a closed door.  This is used to avoid drawing interactions
//    // when the light is behind a closed door.
//    bool *				connectedAreas;
};

#endif //RELOAD_RENDERCOMMON_H
