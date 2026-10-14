/** HD pause panel. */
#pragma once
#include "d/hd_parts.h"
struct hdPauseTexture_l {
    u8 _00[0xC];
    be<u32> mExtentX, mExtentY;
    u8 _14[0x74 - 0x14];
    be<u32> mImageOffset, mImageExtentY, mMipOffset, mImageExtentX, mComponentOrder;
    u8 _88[0x9C - 0x88];
};
WWHD_SIZE(hdPauseTexture_l, 0x9C);
WWHD_OFFSET(hdPauseTexture_l, mExtentX, 0xC);
WWHD_OFFSET(hdPauseTexture_l, mExtentY, 0x10);
WWHD_OFFSET(hdPauseTexture_l, mImageOffset, 0x74);
WWHD_OFFSET(hdPauseTexture_l, mImageExtentY, 0x78);
WWHD_OFFSET(hdPauseTexture_l, mMipOffset, 0x7C);
WWHD_OFFSET(hdPauseTexture_l, mImageExtentX, 0x80);
WWHD_OFFSET(hdPauseTexture_l, mComponentOrder, 0x84);
struct hdPausePictureTexture_l {
    u8 _00[4];
    be<s32> mWidth, mHeight;
    u8 _0C[0x74 - 0xC];
};
WWHD_SIZE(hdPausePictureTexture_l, 0x74);
WWHD_OFFSET(hdPausePictureTexture_l, mWidth, 4);
WWHD_OFFSET(hdPausePictureTexture_l, mHeight, 8);
struct hdPausePictureEntry_l {
    u8 _00[4];
    hdPausePictureTexture_l mTextureData;
    be<u8> mMinimumWidth, mMinimumHeight;
};
WWHD_OFFSET(hdPausePictureEntry_l, mTextureData, 4);
WWHD_OFFSET(hdPausePictureEntry_l, mMinimumWidth, 0x78);
WWHD_OFFSET(hdPausePictureEntry_l, mMinimumHeight, 0x79);
struct hdPause_c : hdParts_c {
    be<u32> mPictureActive;
    be<u16> mPictureWidth;
    be<u16> mPictureHeight;
    be<u8> mPictureFormat;
    u8 _59[3];
    hdPauseTexture_l mTexture;
    be<u8> mInputMode;
    be<u8> mAutomatic;
    u8 _FA[2];
    u32 loadScreen(void* name, u32 archive, u32 resource, u32 flags, void* descriptor);
    void setTexture();
    void inAllEnter();
    void setAutomatic(u32 enabled);
    u32 setup(u32 archive, u32 resource, u32 flags);
    void requestOut();
    void reset();
    void inAllExecute();
    void outAllEnter();
    void outAllExecute();
    void release();
    void stateExecute();
};
WWHD_SIZE(hdPause_c, 0xFC);
WWHD_OFFSET(hdPause_c, mPictureActive, 0x50);
WWHD_OFFSET(hdPause_c, mPictureWidth, 0x54);
WWHD_OFFSET(hdPause_c, mPictureHeight, 0x56);
WWHD_OFFSET(hdPause_c, mInputMode, 0xF8);
WWHD_OFFSET(hdPause_c, mAutomatic, 0xF9);
bool pauseAnimationStopped(hdAnimSet* animation, u32 track);
void pauseDetachArchives(hdPause_c* panel);
void pauseExecuteState(hdStateMachine* machine);

bool pauseCanBegin(hdPause_c* panel);

void pauseDestroyBase(hdPause_c* panel, u32 flags);
hdPause_c* pauseConstructBase(hdPause_c* panel);
void pauseAttachArchives(hdPause_c* panel, hdUiSafeString* names, u32 count);
void pauseSetAutomatic(hdPause_c* panel, u32 enabled);
void pauseSetAnimationRate(hdAnimSet* animation, u32 entry, u32 slot, f32 rate);

hdPartsLayout_c* pauseAllocateLayout(u32 size, void* heap, u32 alignment);
void pauseBindAnimation(hdAnimSet* animation, u32 track, hdUiSafeString* name, hdUiSafeString* pane);

hdPausePictureEntry_l* pauseGetPicture(void* manager, u32 slot);
void pauseLockPictures(void* manager);
void pauseUnlockPictures(void* manager);
void pauseCopyTexture(hdPauseTexture_l* destination, void* source, u32 size, u32 flags);
void pauseInitTextureRegisters(hdPauseTexture_l* texture);
void pauseGetTextureFormat(be<u32>* format, u32 kind, u32 flags);
void pauseInstallPicture(hdPartsPaneSRT_c* pane, void* picture, u32 flags, u32 extra);
