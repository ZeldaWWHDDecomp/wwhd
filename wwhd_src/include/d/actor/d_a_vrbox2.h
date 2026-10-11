#pragma once
#include "bindings.h"

// HD: actor base expands by 0x11C; GameCube field names retained.
struct vrbox2_class : fopAc_ac_c {
    /* 0x3AC */ u8 m290[4];
    /* 0x3B0 */ gptr<J3DModel> mpBackCloud;
    /* 0x3B4 */ u8 m298[4];
    /* 0x3B8 */ gptr<J3DModel> mpKasumiMae;
    /* 0x3BC */ u8 m2A0[4];
    /* 0x3C0 */ gptr<J3DModel> mpUsoUmi;
    /* 0x3C4 */ u8 m2A8[4];
    /* 0x3C8 */ be<f32> mScrollSpeed; /* HD: computed in Execute, applied (and cleared) in Draw */
};
WWHD_OFFSET(vrbox2_class, mpBackCloud, 0x3B0);
WWHD_OFFSET(vrbox2_class, mpKasumiMae, 0x3B8);
WWHD_OFFSET(vrbox2_class, mpUsoUmi, 0x3C0);
WWHD_OFFSET(vrbox2_class, mScrollSpeed, 0x3C8);

// HD: private views of the renderer and sky-light members used by this actor.
struct Vrbox2Environment {
    u8 padding0[0xA24];
    struct Wind {
        be<u16> mTactWindAngleX;
        be<s16> mTactWindAngleY;
    } mWind;
    u8 paddingA28[0xB90 - 0xA28];
    GXColor mVrSkyColor;
    GXColor mVrUsoUmiColor;
    GXColor mVrkumoColor;
    u8 paddingB9C[4];
    GXColor mVrKasumiMaeColor;
    u8 paddingBA4[0x10D8 - 0xBA4];
    be<f32> mVrUsoUmiColorScale;
    be<f32> mVrkumoColorScale;
    u8 padding10E0[4];
    be<f32> mVrKasumiMaeColorScale;
};
struct Vrbox2Color4f {
    union {
        struct {
            be<f32> r, g, b, a;
        };
        be<u32> mChannelBits[4];
    };
};
struct Vrbox2TevVtable {
    u8 padding0[0x24];
    gfn setTevColor;
    u8 padding28[0x3C - 0x28];
    gfn setTevKColor;
};
struct Vrbox2TevBlock {
    u8 padding0[4];
    gptr<Vrbox2TevVtable> __vtbl;
};
struct Vrbox2Material {
    u8 padding0[0x18];
    gptr<Vrbox2TevBlock> mpTevBlock;
    u8 padding1C[0xA0 - 0x1C];
    be<u32> mConstantColorFlags;
};
struct Vrbox2ProjectionVtable {
    u8 padding0[0x0C];
    gfn checkDerived;
};
struct Vrbox2Projection {
    be<u8> mDirty;
    u8 padding1[0x90 - 1];
    gptr<Vrbox2ProjectionVtable> __vtbl;
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
struct Vrbox2Layer {
    u8 padding0[0x4C];
    gptr<Vrbox2Projection> mpProjection;
    be<u32> mFlags;
    u8 padding54[0x164 - 0x54];
    gptr<Vrbox2Projection> mpOverrideProjection;
};
struct Vrbox2Renderer {
    u8 padding0[0x1020];
    be<u32> mLayerCount;
    gptr<gptr<Vrbox2Layer>> mpLayers;
};
struct Vrbox2View {
    u8 padding0[0xD4];
    be<f32> mFovy;
    u8 paddingD8[0x174 - 0xD8];
    Mtx34 mInvViewMtx;
};
struct Vrbox2GameView {
    u8 padding0[0x5FA4];
    gptr<Vrbox2View> mpView;
};
struct Vrbox2FileList {
    u8 padding0[4];
    be<f32> mSeaLevel;
};
struct Vrbox2RoomVtable {
    u8 padding0[0x1DC];
    gfn getFileListInfo;
};
struct Vrbox2Room {
    gptr<Vrbox2RoomVtable> __vtbl;
};
struct Vrbox2StringVtable {
    u8 padding0[0x14];
    gfn assureTermination;
};
WWHD_OFFSET(Vrbox2Environment, mVrSkyColor, 0xB90);
WWHD_OFFSET(Vrbox2Environment, mVrkumoColor, 0xB98);
WWHD_OFFSET(Vrbox2Environment, mVrKasumiMaeColor, 0xBA0);
WWHD_OFFSET(Vrbox2Environment, mVrKasumiMaeColorScale, 0x10E4);
WWHD_SIZE(Vrbox2Color4f, 0x10);
WWHD_OFFSET(Vrbox2TevBlock, __vtbl, 4);
WWHD_OFFSET(Vrbox2TevVtable, setTevKColor, 0x3C);
WWHD_OFFSET(Vrbox2Material, mpTevBlock, 0x18);
WWHD_OFFSET(Vrbox2Material, mConstantColorFlags, 0xA0);
WWHD_OFFSET(Vrbox2Projection, __vtbl, 0x90);
WWHD_OFFSET(Vrbox2Projection, mNear, 0x94);
WWHD_OFFSET(Vrbox2Projection, mFar, 0x98);
WWHD_OFFSET(Vrbox2Projection, mFovy, 0x9C);
WWHD_OFFSET(Vrbox2Projection, mAspect, 0xAC);
WWHD_OFFSET(Vrbox2Projection, mOffsetX, 0xB0);
WWHD_OFFSET(Vrbox2Projection, mFrustum, 0xB8);
WWHD_SIZE(Vrbox2Projection, 0xC4);
WWHD_OFFSET(Vrbox2ProjectionVtable, checkDerived, 0x0C);
WWHD_OFFSET(Vrbox2Layer, mpProjection, 0x4C);
WWHD_OFFSET(Vrbox2Layer, mFlags, 0x50);
WWHD_OFFSET(Vrbox2Layer, mpOverrideProjection, 0x164);
WWHD_OFFSET(Vrbox2Renderer, mLayerCount, 0x1020);
WWHD_OFFSET(Vrbox2Renderer, mpLayers, 0x1024);
WWHD_OFFSET(Vrbox2View, mInvViewMtx, 0x174);
WWHD_OFFSET(Vrbox2GameView, mpView, 0x5FA4);
WWHD_OFFSET(Vrbox2FileList, mSeaLevel, 4);
WWHD_OFFSET(Vrbox2RoomVtable, getFileListInfo, 0x1DC);
WWHD_OFFSET(Vrbox2StringVtable, assureTermination, 0x14);

// HD: material animations are fixed-size records, selected by a relative name table.
struct Vrbox2NameResource {
    u8 padding0[0x18];
    be<s32> mNameTableOffset;
};
struct Vrbox2MaterialAnimation {
    u8 padding0[0x3C];
};
struct Vrbox2ModelAnimations {
    u8 padding0[0x14];
    gptr<Vrbox2NameResource> mpNameResource;
    u8 padding18[0x34 - 0x18];
    gptr<Vrbox2MaterialAnimation> mpMaterialAnimations;
};
struct Vrbox2TexSrt {
    u8 padding0[0x10];
    be<f32> mTranslationX;
};
struct Vrbox2ColorS10 {
    be<s16> r, g, b, a;
};
struct Vrbox2LayerVtable {
    u8 padding0[0x44];
    gfn checkDerived;
};
struct Vrbox2ClearLayer {
    u8 padding0[0x58];
    gptr<Vrbox2LayerVtable> __vtbl;
    u8 padding5C[0x114 - 0x5C];
    Vrbox2Color4f mClearColor;
};
struct Vrbox2LayerHandle {
    be<u16> mIndex, mGeneration;
};
struct Vrbox2LayerTable {
    u8 padding0[8];
    be<u32> mHandleCount;
    gptr<Vrbox2LayerHandle> mpHandles;
    be<u32> mLayerCount;
    gptr<gptr<Vrbox2ClearLayer>> mpLayers;
};
struct Vrbox2LayerRegistry {
    u8 padding0[0x148];
    gptr<Vrbox2LayerTable> mpTable;
};
struct Vrbox2Camera {
    u8 padding0[0xDC];
    cXyz mEye, mCenter;
};
struct Vrbox2StageInfo {
    u8 padding0[0xC];
    be<u32> mType;
};
struct Vrbox2SaveWind {
    u8 padding0[0x4A];
    be<s16> mWindX, mWindY;
};
WWHD_OFFSET(Vrbox2Environment, mWind, 0xA24);
WWHD_OFFSET(Vrbox2Environment, mVrUsoUmiColor, 0xB94);
WWHD_OFFSET(Vrbox2Environment, mVrUsoUmiColorScale, 0x10D8);
WWHD_OFFSET(Vrbox2Environment, mVrkumoColorScale, 0x10DC);
WWHD_OFFSET(Vrbox2TevVtable, setTevColor, 0x24);
WWHD_OFFSET(Vrbox2View, mFovy, 0xD4);
WWHD_OFFSET(Vrbox2NameResource, mNameTableOffset, 0x18);
WWHD_SIZE(Vrbox2MaterialAnimation, 0x3C);
WWHD_OFFSET(Vrbox2ModelAnimations, mpNameResource, 0x14);
WWHD_OFFSET(Vrbox2ModelAnimations, mpMaterialAnimations, 0x34);
WWHD_OFFSET(Vrbox2TexSrt, mTranslationX, 0x10);
WWHD_SIZE(Vrbox2ColorS10, 8);
WWHD_OFFSET(Vrbox2LayerVtable, checkDerived, 0x44);
WWHD_OFFSET(Vrbox2ClearLayer, __vtbl, 0x58);
WWHD_OFFSET(Vrbox2ClearLayer, mClearColor, 0x114);
WWHD_SIZE(Vrbox2LayerHandle, 4);
WWHD_OFFSET(Vrbox2LayerTable, mHandleCount, 8);
WWHD_OFFSET(Vrbox2LayerTable, mpHandles, 0xC);
WWHD_OFFSET(Vrbox2LayerTable, mLayerCount, 0x10);
WWHD_OFFSET(Vrbox2LayerTable, mpLayers, 0x14);
WWHD_OFFSET(Vrbox2LayerRegistry, mpTable, 0x148);
WWHD_OFFSET(Vrbox2Camera, mEye, 0xDC);
WWHD_OFFSET(Vrbox2Camera, mCenter, 0xE8);
WWHD_OFFSET(Vrbox2StageInfo, mType, 0xC);
WWHD_OFFSET(Vrbox2SaveWind, mWindX, 0x4A);
WWHD_OFFSET(Vrbox2SaveWind, mWindY, 0x4C);
