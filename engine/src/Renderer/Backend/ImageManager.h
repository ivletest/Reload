//
// Created by ivan on 14.4.21.
//

#ifndef RELOAD_IMAGE_MANAGER_H
#define RELOAD_IMAGE_MANAGER_H

#include "precompiled.h"

#include <unordered_map>
#include "Image.h"

class ImageManager {
public:

    ImageManager();

    void				Init();
    void				Shutdown();

    // If the exact combination of parameters has been asked for already, an existing
    // image will be returned, otherwise a new image will be created.
    // Be careful not to use the same image file with different filter / repeat / etc parameters
    // if possible, because it will cause a second copy to be loaded.
    // If the load fails for any reason, the image will be filled in with the default
    // grid pattern.
    // Will automatically execute image programs if needed.
//    Image *			ImageFromFile( const char *name,
//                                        textureFilter_t filter, textureRepeat_t repeat, textureUsage_t usage, cubeFiles_t cubeMap = CF_2D );

    // These images are for internal renderer use.  Names should start with "_".
    Image *			ScratchImage(std::string name, const ImageOpts & opts );

    // look for a loaded image, whatever the parameters
    Image *			GetImage(std::string name) const;

    // The callback will be issued immediately, and later if images are reloaded or vid_restart
    // The callback function should call one of the idImage::Generate* functions to fill in the data
//    Image *			ImageFromFunction( const char *name, void (*generatorFunction)( idImage *image ));

    // purges all the images before a vid_restart
    void				PurgeAllImages();

    // reloads all apropriate images after a vid_restart
//    void				ReloadImages( bool all );

    // Called only by renderSystem::BeginLevelLoad
//    void				BeginLevelLoad();

    // Called only by renderSystem::EndLevelLoad
//    void				EndLevelLoad();

//    void				Preload( const idPreloadManifest &manifest, const bool & mapPreload );

    // Loads unloaded level images
//    int					LoadLevelImages( bool pacifier );

//    void				PrintMemInfo( MemInfo_t *mi );

    // built-in images
//    void				CreateIntrinsicImages();

    Image *			AllocImage(std::string name);

    // the image has to be already loaded ( most straightforward way would be through a FindMaterial )
    // texture filter / mipmapping / repeat won't be modified by the upload
    // returns false if the image wasn't found
//    bool				UploadImage( const char * imageName, const char * data, int width, int height );

//    bool				ExcludePreloadImage( const char *name );

public:
    bool				m_insideLevelLoad;			// don't actually load images now
    bool				m_preloadingMapImages;		// unless this is set

    Image *			m_defaultImage;
    Image *			m_flatNormalMap;			// 128 128 255 in all pixels
    Image *			m_alphaNotchImage;
    Image *			m_whiteImage;				// full of 0xff
    Image *			m_blackImage;				// full of 0x00
    Image *			m_fogImage;					// increasing alpha is denser fog
    Image *			m_fogEnterImage;			// adjust fogImage alpha based on terminator plane
    Image *			m_noFalloffImage;
    Image *			m_quadraticImage;
    Image *			m_scratchImage;
    Image *			m_scratchImage2;
    Image *			m_currentRenderImage;		// for SS_POST_PROCESS shaders
    Image *			m_currentDepthImage;		// for motion blur
    Image *			m_accumImage;
    Image *			m_loadingIconImage;
    Image *			m_hellLoadingIconImage;

    //--------------------------------------------------------

    std::unordered_map<std::string, Image *> m_images;
};

extern ImageManager	* globalImages;

#endif //RELOAD_IMAGE_MANAGER_H
