//
// Created by ivan on 20.3.21.
//

#ifndef RELOAD_RENDERCOMMON_H
#define RELOAD_RENDERCOMMON_H

#define VK_CHECK(x) {                                                          \
	VkResult ret = x;                                                          \
	if (ret != VK_SUCCESS) {                                                   \
        SDL_LogCritical(LOG_SYSTEM, "VK: %s - %s", Vk_ErrorToString(ret), #x); \
        exit(1);                                                               \
    }                                                                          \
}

#define VK_VALIDATE(x, msg)                                                    \
	if (!(x)) {                                                                \
        SDL_LogCritical(LOG_SYSTEM, "VK: %s - %s", msg, #x);                   \
        exit(1);                                                               \
    }

const char * Vk_ErrorToString(VkResult result);

// everything that is needed by the backend needs
// to be double buffered to allow it to run in
// parallel on a dual cpu machine
static const uint32_t NUM_FRAME_DATA	= 2;

static const int MAX_DESC_SETS				= 16384;
static const int MAX_DESC_UNIFORM_BUFFERS	= 8192;
static const int MAX_DESC_IMAGE_SAMPLERS	= 12384;
static const int MAX_DESC_SET_WRITES		= 32;
static const int MAX_DESC_SET_UNIFORMS		= 48;
static const int MAX_IMAGE_PARMS			= 16;
static const int MAX_UBO_PARMS				= 2;
static const int NUM_TIMESTAMP_QUERIES		= 16;

#endif //RELOAD_RENDERCOMMON_H
