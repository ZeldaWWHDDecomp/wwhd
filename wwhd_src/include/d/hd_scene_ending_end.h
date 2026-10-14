/* HD ending end screen singleton. Private reconstruction. */
#pragma once
#include "d/hd_scene_boot_common.h"
struct hdSceneEndingEnd_c {
    hdSceneDisposer_c mDisposer;
    hdSceneScreen_c mScreen;
    gptr<hdParts_c> mpMainParts;
    gptr<hdParts_c> mpShortcut;
    static hdSceneEndingEnd_c* construct(hdSceneEndingEnd_c*);
    static void createInstance(void* heap);
    static void destroyInstance();
    static void destroy(hdSceneEndingEnd_c*, u32 flags);
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
};
WWHD_OFFSET(hdSceneEndingEnd_c, mScreen, 0x10);
WWHD_OFFSET(hdSceneEndingEnd_c, mpMainParts, 0x50);
WWHD_SIZE(hdSceneEndingEnd_c, 0x58);
WWHD_OFFSET(hdSceneEndingEnd_c, mpShortcut, 0x54);
struct hdSceneShortcutVtable_c {
    u8 mPrefix[0x414]; gfn mCreate;
};
struct hdSceneCameraDrawContext_l {
    u8 mPrefix[0x548]; u8 mDrawContext;
};
WWHD_OFFSET(hdSceneShortcutVtable_c, mCreate, 0x414);
WWHD_OFFSET(hdSceneCameraDrawContext_l, mDrawContext, 0x548);
