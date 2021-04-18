//
// Created by ivan on 14.4.21.
//

#include "ImageManager.h"
#include "ReloadLib/Extensions/Str.h"

ImageManager imageManager;
ImageManager * globalImages = &imageManager;

ImageManager::ImageManager() {
        m_insideLevelLoad = false;
        m_preloadingMapImages = false;
}

void ImageManager::Init() {
    m_images.reserve(1024);

    // CreateIntrinsicImages();
}

void ImageManager::Shutdown() {
    m_images.clear();
}

Image *ImageManager::ScratchImage(std::string name, const ImageOpts &opts) {
    if(name.empty()) {
        spdlog::error("Invalid scratch image name.");
    }

    Image * image = GetImage(name);
    if (image == nullptr) {
        image = AllocImage(name);
    } else {
        image->Purge();
    }

    image->m_imgOpts = opts;
    image->Alloc();

    return image;
}

Image *ImageManager::GetImage(std::string name) const {
    if (name.empty() || name == "default" || name == "_default") {
        return globalImages->m_defaultImage;
    }

    Str::TrimExtension(name);

    auto imageMap = m_images.find(name);

    if (imageMap != m_images.end() && Str::CompareI(name, imageMap->second->m_imgName)) {
        return imageMap->second;
    }

    return nullptr;
}

void ImageManager::PurgeAllImages() {
    for (auto imageMap : m_images) {
        imageMap.second->Purge();
    }
}

Image *ImageManager::AllocImage(std::string name) {
    if (name.length() >= MAX_PATH) {
        spdlog::error("Image name \"{}\" is too long\n", name);
    }

    Image * image = new Image(name);
    m_images.insert({ name, image });

    return image;
}

