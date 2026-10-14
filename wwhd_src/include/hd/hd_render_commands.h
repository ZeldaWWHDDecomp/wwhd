/* HD render-command queue and scene-layer forwarding. Private decomp source. */
#pragma once
#include "hd/hd_layer_base.h"
struct hdRenderCommandMgr_c {
    u8 _00[0xC];
    be<u32> mVtable;
    be<u32> mDrawCount;
    be<u32> mExtraDrawCount;
    be<u32> mOverlayCount;
    be<u32> mReservedCount;
    void clear();
    void initialize();
    void enqueueDraw(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g);
    void enqueueExtra(u32 a, u32 b, u32 c, u32 d, u32 e);
};
WWHD_OFFSET(hdRenderCommandMgr_c,mVtable,0xC);
WWHD_OFFSET(hdRenderCommandMgr_c,mDrawCount,0x10);
WWHD_OFFSET(hdRenderCommandMgr_c,mExtraDrawCount,0x14);
WWHD_OFFSET(hdRenderCommandMgr_c,mOverlayCount,0x18);
WWHD_OFFSET(hdRenderCommandMgr_c,mReservedCount,0x1C);
WWHD_SIZE(hdRenderCommandMgr_c,0x20);
struct hdRenderPacket_l { u8 mData[0x18]; };
struct hdRenderCommandVtable_l { u8 _00[0x7C]; be<u32> mSetCount; };
struct hdRenderCommandList_l { gptr<hdRenderCommandVtable_l> mpVtable; };
struct hdRenderCommandLayer_l {
    hdLayerBase_c mBase;
    u8 _610[0x2E6C-0x610];
    hdRenderPacket_l mPacket;
    gptr<hdRenderCommandList_l> mpDrawList;
    gptr<hdRenderCommandList_l> mpExtraDrawList;
    gptr<hdRenderCommandList_l> mpOverlayList;
};
WWHD_OFFSET(hdRenderCommandLayer_l,mPacket,0x2E6C);
WWHD_OFFSET(hdRenderCommandLayer_l,mpDrawList,0x2E84);
WWHD_OFFSET(hdRenderCommandLayer_l,mpExtraDrawList,0x2E88);
WWHD_OFFSET(hdRenderCommandLayer_l,mpOverlayList,0x2E8C);
struct hdRenderSceneMgr_l {
    u8 _00[0x1020];
    be<u32> mLayerCount;
    gptr<gptr<hdRenderCommandLayer_l>> mpLayers;
};
WWHD_OFFSET(hdRenderSceneMgr_l,mLayerCount,0x1020);
WWHD_OFFSET(hdRenderSceneMgr_l,mpLayers,0x1024);
struct hdRenderPerspectiveVtable_l { u8 _00[0xC]; be<u32> mIsKindOf; };
struct hdRenderPerspective_l {
    u8 _00[0x90];
    gptr<hdRenderPerspectiveVtable_l> mpVtable;
    be<f32> mNearPlane;
    be<f32> mFarPlane;
    u8 _9C[0xAC-0x9C];
    be<f32> mAspect;
    u8 mFrustum[0x10];
    be<f32> mFieldOfView;
};
WWHD_OFFSET(hdRenderPerspective_l,mpVtable,0x90);
WWHD_OFFSET(hdRenderPerspective_l,mNearPlane,0x94);
WWHD_OFFSET(hdRenderPerspective_l,mFarPlane,0x98);
WWHD_OFFSET(hdRenderPerspective_l,mAspect,0xAC);
WWHD_OFFSET(hdRenderPerspective_l,mFrustum,0xB0);
WWHD_OFFSET(hdRenderPerspective_l,mFieldOfView,0xC0);

struct hdRenderCommandsHeaderZero_c { be<u32> mWords[4]; };
WWHD_SIZE(hdRenderCommandsHeaderZero_c,16);
struct hdRenderCommandsHeaderDescriptor_c { u8 mData[12]; };
WWHD_SIZE(hdRenderCommandsHeaderDescriptor_c,12);
