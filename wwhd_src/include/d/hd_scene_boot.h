/* HD boot screen singleton. Private reconstruction. */
#pragma once
#include "d/hd_scene_boot_common.h"
struct hdSceneBoot_c {
    hdSceneDisposer_c mDisposer;
    hdSceneScreen_c mScreen;
    gptr<hdParts_c> mpMainParts;
    static hdSceneBoot_c* construct(hdSceneBoot_c*);
    static void createInstance(void* heap);
    static void destroyInstance();
    static void destroy(hdSceneBoot_c*, u32 flags);
    static void destroyDisposer(hdSceneDisposer_c*, u32 flags);
    static void staticInit();
    void unloadArchive();
    void createParts();
    void initialize();
    void executeParts();
    void execute();
    void releaseParts();
    void release();
    void draw(hdSceneDrawInfo_c*);
    void cacheArchive();
};
WWHD_OFFSET(hdSceneBoot_c, mScreen, 0x10);
WWHD_OFFSET(hdSceneBoot_c, mpMainParts, 0x50);
WWHD_SIZE(hdSceneBoot_c, 0x54);
struct hdBootCameraDrawContext_l { u8 mPrefix[0x548]; u8 mDrawContext; };
WWHD_OFFSET(hdBootCameraDrawContext_l, mDrawContext, 0x548);
