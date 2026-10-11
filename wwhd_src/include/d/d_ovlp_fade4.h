/** Fade4 packets and transition state. */
#pragma once
#include "gc_api/gc_api.h"
#include "f_op/f_op_overlap.h"

struct dDlst_base_c { be<u32> __vtbl; };
struct dDlst_snapShot2_c : dDlst_base_c {
    gptr<void> mImagePtr;
    be<u16> mWidth, mHeight;
};
// HD: The native texture object grows from 0x20 to 0x40 bytes.
struct dDlst_2Dt_Sp_c : dDlst_base_c {
    u8 mTexObj[0x40];
    gptr<void> mTimg;
    gptr<void> mImagePtr;
    be<u32> mImageWidth, mImageHeight;
    be<f32> mPosX, mPosY, mWidth, mHeight;
    be<u32> mColor;
    be<f32> mPercent;
};
struct dDlst_2DtEff1_c : dDlst_base_c {
    be<f32> timer;
    be<u8> first;
    u8 padding[3];
    dDlst_snapShot2_c snap_dlst;
    dDlst_2Dt_Sp_c redraw_dlst;
    dDlst_snapShot2_c save_dlst;
    dDlst_2Dt_Sp_c blur0_dlst;
    dDlst_snapShot2_c blur0Snap_dlst;
    dDlst_2Dt_Sp_c composite_dlst;
    be<u16> curWidth, curHeight;
};
struct dOvlpFd4_c : overlap_task_class {
    using Phase = void (dOvlpFd4_c::*)();
    gabi::ptmf<Phase> mExecuteFunc;
    gabi::ptmf<Phase> mDrawFunc;
    dDlst_snapShot2_c fadeOutBlur0_dlst;
    dDlst_snapShot2_c fadeOutSnap_dlst;
    dDlst_2Dt_Sp_c fadeOutComposite_dlst;
    dDlst_2Dt_Sp_c fadeOutBlur1_dlst;
    dDlst_2DtEff1_c fadeIn_dlst;
    be<u8> first, fadeOutStatus;
    be<s8> totalTime;
    u8 padding34B;
    be<s16> timer;
    u8 padding34E[2];
    be<f32> delayPercent;
    be<f32> field_0x2b8;
    be<f32> field_0x2bc;
    be<u8> sizeStep;
    be<s8> startTime;
    be<s16> field_0x2c2;
    be<s16> field_0x2c4;
    u8 padding362[2];
    be<f32> timerStep;
    be<u8> field_0x2cc;
    u8 padding369[3];
    // HD: Native blur passes use an integer counter, independent of the packet dimensions.
    be<u32> mBlurCounter;
};
WWHD_SIZE(dDlst_snapShot2_c, 0xC);
WWHD_SIZE(dDlst_2Dt_Sp_c, 0x6C);
WWHD_SIZE(dDlst_2DtEff1_c, 0x178);
WWHD_OFFSET(dDlst_2Dt_Sp_c, mTimg, 0x44);
WWHD_OFFSET(dDlst_2Dt_Sp_c, mPercent, 0x68);
WWHD_OFFSET(dDlst_2DtEff1_c, curWidth, 0x174);
WWHD_OFFSET(dOvlpFd4_c, mExecuteFunc, 0xD0);
WWHD_OFFSET(dOvlpFd4_c, mDrawFunc, 0xD8);
WWHD_OFFSET(dOvlpFd4_c, fadeIn_dlst, 0x1D0);
WWHD_OFFSET(dOvlpFd4_c, first, 0x348);
WWHD_OFFSET(dOvlpFd4_c, timer, 0x34C);
WWHD_OFFSET(dOvlpFd4_c, mBlurCounter, 0x36C);
WWHD_SIZE(dOvlpFd4_c, 0x370);

// HD: Native renderer context and texture fields used by the replacement blur path.
struct dOvlpFd4_play_c {
    u8 padding[0x5AC9];
    be<u8> mWindowNum;
    u8 padding5ACA[0x5C22 - 0x5ACA];
    be<u8> m2DShow;
};
struct dOvlpFd4_size_c { u8 padding[0x10]; be<f32> mWidth, mHeight; };
struct dOvlpFd4_context_c {
    u8 padding[8];
    gptr<dOvlpFd4_size_c> mSize;
    u8 paddingC[0x10];
    gptr<void> mTarget;
};
struct dOvlpFd4_texture_c {
    u8 padding[0x150];
    be<u32> mFilter[3];
    u8 padding15C[0x34];
    be<u8> mDirty;
    u8 padding191[7];
};
struct dOvlpFd4_scale_c { be<f32> x, y; };
struct dOvlpFd4_output_c { be<u32> mTexture, mFlags; };
struct dOvlpFd4_renderer_c {};
struct dOvlpFd4_registry_c {};
struct dOvlpFd4_blur_c {};
WWHD_SIZE(dOvlpFd4_texture_c, 0x198);
WWHD_OFFSET(dOvlpFd4_context_c, mTarget, 0x1C);

void* dOvlpFd4_alloc(u32);
void dDlst_base_construct(dDlst_base_c*);
void dDlst_base_destroy(dDlst_base_c*, s32);
void dOvlpFd4_clear(void*, u32);
void fpcBs_construct(base_process_class*);
void fpcBs_destruct(base_process_class*, s32);
dOvlpFd4_play_c* dOvlpFd4_getPlay();
u32 dOvlpFd4_calcTimer(be<s16>*);
void fopOvlpM_Done(dOvlpFd4_c*);
u32 fopOvlpM_IsOutReq(dOvlpFd4_c*);
void fopOvlpM_SceneIsStart();
void fopOvlpM_SceneIsStop();
u32 dOvlpFd4_register(dOvlpFd4_registry_c*, dOvlpFd4_c*);
void dOvlpFd4_unregister(dOvlpFd4_registry_c*);
void dOvlpFd4_capture(dOvlpFd4_renderer_c*, s32, u32);
void dOvlpFd4_release(dOvlpFd4_renderer_c*, s32);
void dOvlpFd4_setSize(dOvlpFd4_size_c*);
void dOvlpFd4_setTarget(void*, dOvlpFd4_size_c*);
u32 dOvlpFd4_getImage(dOvlpFd4_renderer_c*, s32);
void dOvlpFd4_initTexture(dOvlpFd4_texture_c*, u32);
dOvlpFd4_output_c* dOvlpFd4_getOutput(u32);
u32 dOvlpFd4_blur(dOvlpFd4_blur_c*, dOvlpFd4_texture_c*, dOvlpFd4_scale_c*, u32, f32, f32);
void dOvlpFd4_destroyTexture(dOvlpFd4_texture_c*, s32);
void dOvlpFd4_registerStatic(u32);
void dOvlpFd4_initFlag(u32);
void dOvlpFd4_initByte(u32);

struct dOvlpFd4_data_c { u8 unused; };

struct dOvlpFd4_staticWords_c { be<u32> values[4]; };
struct dOvlpFd4_staticFloats_c { be<f32> values[2]; };

WWHD_OFFSET(dDlst_2Dt_Sp_c, mImagePtr, 0x48);
WWHD_OFFSET(dDlst_2Dt_Sp_c, mImageWidth, 0x4C);
WWHD_OFFSET(dDlst_2Dt_Sp_c, mPosX, 0x54);
WWHD_OFFSET(dDlst_2Dt_Sp_c, mWidth, 0x5C);
WWHD_OFFSET(dDlst_2Dt_Sp_c, mColor, 0x64);
WWHD_OFFSET(dDlst_2DtEff1_c, blur0_dlst, 0x90);
WWHD_OFFSET(dDlst_2DtEff1_c, composite_dlst, 0x108);
WWHD_OFFSET(dOvlpFd4_c, fadeOutBlur0_dlst, 0xE0);
WWHD_OFFSET(dOvlpFd4_c, fadeOutSnap_dlst, 0xEC);
WWHD_OFFSET(dOvlpFd4_c, fadeOutComposite_dlst, 0xF8);
WWHD_OFFSET(dOvlpFd4_c, fadeOutBlur1_dlst, 0x164);
WWHD_OFFSET(dOvlpFd4_c, totalTime, 0x34A);
WWHD_OFFSET(dOvlpFd4_c, delayPercent, 0x350);
WWHD_OFFSET(dOvlpFd4_c, timerStep, 0x364);
WWHD_OFFSET(dOvlpFd4_c, field_0x2cc, 0x368);
WWHD_OFFSET(dOvlpFd4_play_c, mWindowNum, 0x5AC9);
WWHD_OFFSET(dOvlpFd4_play_c, m2DShow, 0x5C22);
WWHD_OFFSET(dOvlpFd4_texture_c, mFilter, 0x150);
WWHD_OFFSET(dOvlpFd4_texture_c, mDirty, 0x190);
WWHD_SIZE(dOvlpFd4_staticWords_c, 0x10);
WWHD_SIZE(dOvlpFd4_staticFloats_c, 8);
