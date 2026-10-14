/* HD baton-sign list screen. Private binary reconstruction; no GameCube counterpart. */
#pragma once
#include "d/hd_parts.h"
#include "d/d_particle.h"

struct hdBatonListEmitter_l {
    u8 _00[0x34];
    be<f32> mEmissionRate; /* probably: set from the screen's effect rate */
    u8 _38[0x24];
    be<s32> mMaxFrame;
    u8 _60[0x1F4];
    be<u32> mFlags;
};
WWHD_OFFSET(hdBatonListEmitter_l, mEmissionRate, 0x34);
WWHD_OFFSET(hdBatonListEmitter_l, mMaxFrame, 0x5C);
WWHD_OFFSET(hdBatonListEmitter_l, mFlags, 0x254);
struct hdBatonListOffset_c { be<f32> x, y; };
WWHD_SIZE(hdBatonListOffset_c, 8);

struct hdBatonSignList_c : hdParts_c {
    gptr<hdPartsPaneSRT_c> mPanes[9];
    dPa_followEcallBack mEffectCallback;
    cXyz mEffectPosition;
    be<u8> mListEnabled;
    be<u8> mInputEnabled;
    u8 _96[2];
    hdBatonListOffset_c mEffectOffset;
    be<f32> mEffectRate;
    gptr<hdBatonListEmitter_l> mpEmitter;

    static void destroy(hdBatonSignList_c* self, s32 flags);
    static hdBatonSignList_c* construct(hdBatonSignList_c* self);
    u32 inputAllowed();
    void inputEnter();
    void updateInputEffect();
    void inputExecute();
    void slideEnter();
    void slideExecute();
    void setInputEnabled(u32 enabled);
    void setListEnabled(u32 enabled);
    void hideElementsForMode();
    void calculateLayout();
    void showElementsForMode();
    void allEnter();
    void allExecute();
    void outEnter();
    void outExecute();
    void bindPanes();
    u32 initialize(u32 first, u32 second, void* descriptor);
    void finalize();
    void updatePaneVisibility();
    void executeState();
    void requestInAll();
    void requestOutAll();
    void requestDisplay();
    void requestPause();
    void reset();
    u32 isPauseFinished();
    u32 createChildren();
    u32 createLayout(void* resource, u32 first, u32 second, u32 third, void* descriptor);
};
WWHD_OFFSET(hdBatonSignList_c, mPanes, 0x50);
WWHD_OFFSET(hdBatonSignList_c, mEffectCallback, 0x74);
WWHD_OFFSET(hdBatonSignList_c, mEffectPosition, 0x88);
WWHD_OFFSET(hdBatonSignList_c, mListEnabled, 0x94);
WWHD_OFFSET(hdBatonSignList_c, mInputEnabled, 0x95);
WWHD_OFFSET(hdBatonSignList_c, mEffectOffset, 0x98);
WWHD_OFFSET(hdBatonSignList_c, mEffectRate, 0xA0);
WWHD_OFFSET(hdBatonSignList_c, mpEmitter, 0xA4);
WWHD_SIZE(hdBatonSignList_c, 0xA8);
