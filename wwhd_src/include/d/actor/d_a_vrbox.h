#pragma once
#include "bindings.h"

// HD: actor base expands by 0x11C; member names follow tww.
struct vrbox_class : fopAc_ac_c {
    /* 0x3AC */ u8 m290[4];
    /* 0x3B0 */ gptr<J3DModel> mpModel;
    /* 0x3B4 */ u8 m298[4];
    /* 0x3B8 */ be<u8> m29C;
};
WWHD_OFFSET(vrbox_class, mpModel, 0x3B0);
WWHD_OFFSET(vrbox_class, m29C, 0x3B8);

// HD: private views of the renderer and sky-light members used by this actor.
struct VrboxEnvironment {
    u8 padding0[0xAB4];
    struct Thunder {
        be<s32> mMode;
    } mThunderEff;
    u8 paddingAB8[0xB90 - 0xAB8];
    GXColor mVrSkyColor;
    u8 paddingB94[4];
    GXColor mVrkumoColor;
    u8 paddingB9C[4];
    GXColor mVrKasumiMaeColor;
    u8 paddingBA4[0x106C - 0xBA4];
    be<s32> mRainCountOrig;
    u8 padding1070[0x109E - 0x1070];
    be<u8> mbVrboxInvisible;
    u8 padding109F[0x10D4 - 0x109F];
    be<f32> mVrSkyColorScale;
    u8 padding10D8[0x10E4 - 0x10D8];
    be<f32> mVrKasumiMaeColorScale;
};
struct VrboxColor4f {
    union {
        struct {
            be<f32> r, g, b, a;
        };
        be<u32> mChannelBits[4];
    };
};
struct VrboxTevVtable {
    u8 padding0[0x3C];
    gfn setTevKColor;
};
struct VrboxTevBlock {
    u8 padding0[4];
    gptr<VrboxTevVtable> __vtbl;
};
struct VrboxMaterial {
    u8 padding0[0x18];
    gptr<VrboxTevBlock> mpTevBlock;
    u8 padding1C[0xA0 - 0x1C];
    be<u32> mConstantColorFlags;
};
struct VrboxProjectionVtable {
    u8 padding0[0x0C];
    gfn checkDerived;
};
struct VrboxProjection {
    be<u8> mDirty;
    u8 padding1[0x90 - 1];
    gptr<VrboxProjectionVtable> __vtbl;
    be<f32> mNear, mFar, mFovy;
    u8 paddingA0[0x0C];
    be<f32> mAspect;
    union {
        struct {
            be<f32> mOffsetX, mOffsetY;
        };
        be<u32> mOffsetBits[2];
    };
    be<u32> mFrustum[3];
};
struct VrboxLayer {
    u8 padding0[0x4C];
    gptr<VrboxProjection> mpProjection;
    be<u32> mFlags;
    u8 padding54[0x164 - 0x54];
    gptr<VrboxProjection> mpOverrideProjection;
};
struct VrboxRenderer {
    u8 padding0[0x1020];
    be<u32> mLayerCount;
    gptr<gptr<VrboxLayer>> mpLayers;
};
struct VrboxView {
    u8 padding0[0x174];
    Mtx34 mInvViewMtx;
};
struct VrboxGameView {
    u8 padding0[0x5FA4];
    gptr<VrboxView> mpView;
};
struct VrboxFileList {
    u8 padding0[4];
    be<f32> mSeaLevel;
};
struct VrboxRoomVtable {
    u8 padding0[0x1DC];
    gfn getFileListInfo;
};
struct VrboxRoom {
    gptr<VrboxRoomVtable> __vtbl;
};
struct VrboxStringVtable {
    u8 padding0[0x14];
    gfn assureTermination;
};
WWHD_OFFSET(VrboxEnvironment, mThunderEff, 0xAB4);
WWHD_OFFSET(VrboxEnvironment, mVrSkyColor, 0xB90);
WWHD_OFFSET(VrboxEnvironment, mVrkumoColor, 0xB98);
WWHD_OFFSET(VrboxEnvironment, mVrKasumiMaeColor, 0xBA0);
WWHD_OFFSET(VrboxEnvironment, mRainCountOrig, 0x106C);
WWHD_OFFSET(VrboxEnvironment, mbVrboxInvisible, 0x109E);
WWHD_OFFSET(VrboxEnvironment, mVrSkyColorScale, 0x10D4);
WWHD_OFFSET(VrboxEnvironment, mVrKasumiMaeColorScale, 0x10E4);
WWHD_SIZE(VrboxColor4f, 0x10);
WWHD_OFFSET(VrboxTevBlock, __vtbl, 4);
WWHD_OFFSET(VrboxTevVtable, setTevKColor, 0x3C);
WWHD_OFFSET(VrboxMaterial, mpTevBlock, 0x18);
WWHD_OFFSET(VrboxMaterial, mConstantColorFlags, 0xA0);
WWHD_OFFSET(VrboxProjection, __vtbl, 0x90);
WWHD_OFFSET(VrboxProjection, mNear, 0x94);
WWHD_OFFSET(VrboxProjection, mFar, 0x98);
WWHD_OFFSET(VrboxProjection, mFovy, 0x9C);
WWHD_OFFSET(VrboxProjection, mAspect, 0xAC);
WWHD_OFFSET(VrboxProjection, mOffsetX, 0xB0);
WWHD_OFFSET(VrboxProjection, mFrustum, 0xB8);
WWHD_SIZE(VrboxProjection, 0xC4);
WWHD_OFFSET(VrboxProjectionVtable, checkDerived, 0x0C);
WWHD_OFFSET(VrboxLayer, mpProjection, 0x4C);
WWHD_OFFSET(VrboxLayer, mFlags, 0x50);
WWHD_OFFSET(VrboxLayer, mpOverrideProjection, 0x164);
WWHD_OFFSET(VrboxRenderer, mLayerCount, 0x1020);
WWHD_OFFSET(VrboxRenderer, mpLayers, 0x1024);
WWHD_OFFSET(VrboxView, mInvViewMtx, 0x174);
WWHD_OFFSET(VrboxGameView, mpView, 0x5FA4);
WWHD_OFFSET(VrboxFileList, mSeaLevel, 4);
WWHD_OFFSET(VrboxRoomVtable, getFileListInfo, 0x1DC);
WWHD_OFFSET(VrboxStringVtable, assureTermination, 0x14);
