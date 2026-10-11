/* HD animation backends for GameCube-style mDoExt wrappers. */
#pragma once
#include "bindings.h"

struct HdAnmResource {
    u8 padding[0xC];
    be<u32> mFlags;
    u8 padding2[4];
    be<u16> mMaterialCount;
    u8 padding3[2];
    be<s32> mFirstCount;
    be<s32> mSecondCount;
};
struct HdAnmObject {
    be<f32> mFrame;
    gptr<gfn> mVtable;
    gptr<HdAnmResource> mBckResource;
    gptr<HdAnmResource> mResource;
};
struct HdAnmController {
    be<f32> mFrame, mStart, mEnd;
    u8 padding[4];
    gfn mCalc;
    gptr<void> mCalcData;
};
struct HdAnmBackend {
    gptr<HdAnmController> mController;
    u8 padding[0x30];
    gptr<void> __vtbl;
    u8 padding2[0xC];
    be<u32> mBuffer;
    u8 padding3[0x10];
};
struct HdBckBackend {
    u8 padding[4];
    gptr<HdAnmController> mController;
    u8 padding2[0x70];
};
struct HdAnmModelBinding { u8 padding[0x14]; gptr<HdBckBackend> mBck; };
struct HdAnmModel {
    u8 padding[8];
    gptr<HdAnmModelBinding> mJointBinding;
    u8 padding2[8];
    u8 mMaterialTarget;
    u8 padding3[0x2F];
    gptr<HdAnmBackend> mBtk;
    gptr<HdAnmBackend> mBrk;
};
struct HdAnmModelData { u8 padding[0x24]; be<u16> mMaterialCount; };
struct HdAnmDescriptor {
    be<s32> mModelCount, mMaterialCount, mFirstCount, mSecondCount;
    be<u8> mEnabled, mFlags;
    u8 padding[2];
    be<u32> mBufferSize, mBufferCount;
    u8 padding2[0x24];
};
// HD: local layouts expose the enlarged backends without editing shared opaque/padded wrappers.
struct HdBtkAnm : mDoExt_baseAnm {
    HdAnmBackend mBackend;
    gptr<HdAnmObject> mpAnm;
    be<u32> mpTexMtxAnm;
    be<u16> mUpdateMaterialNum;
    u8 padding[2];
    u32 init(HdAnmModel*, HdAnmObject*, u32, s32, s32, s32, u32, f32);
    void entry(HdAnmModel*, f32);
    void calc(HdAnmModel*, f32);
};
struct HdBrkAnm : mDoExt_baseAnm {
    gptr<HdAnmObject> mpAnm;
    be<u32> mpCRegAnm, mpKRegAnm;
    be<u16> mCRegUpdateMaterialNum, mKRegUpdateMaterialNum;
    HdAnmBackend mBackend;
    u32 init(HdAnmModel*, HdAnmObject*, u32, s32, s32, s32, u32, f32);
    void entry(HdAnmModel*, f32);
    void calc(HdAnmModel*, f32);
};
struct HdBckAnm : mDoExt_baseAnm {
    HdBckBackend mBackend;
    gptr<HdAnmObject> mpAnm;
    u32 init(HdAnmModel*, HdAnmObject*, u32, s32, s32, s32, u32, f32);
    void entry(HdAnmModel*, f32);
    void changeBckOnly(HdAnmObject*);
};
struct HdAnmDrawState {
    u8 padding[0xC];
    be<u32> mFlags;
    u8 padding2[0xDC];
    be<u32> mMode;
    u8 padding3[0x2C];
};
WWHD_SIZE(HdAnmDescriptor, 0x40);
WWHD_SIZE(HdAnmBackend, 0x58);
WWHD_SIZE(HdBtkAnm, 0x74);
WWHD_SIZE(HdBrkAnm, 0x78);
WWHD_SIZE(HdBckAnm, 0x8C);
WWHD_OFFSET(HdBtkAnm, mpAnm, 0x68);
WWHD_OFFSET(HdBrkAnm, mBackend, 0x20);
WWHD_OFFSET(HdBckAnm, mpAnm, 0x88);
WWHD_SIZE(HdAnmDrawState, 284);
enum HdAnmFlags : u32 { HdAnmResource_Enabled = 1, HdAnmResource_ExcludeLast = 4, HdAnm_LengthSlot = 5 };
enum HdAnmDrawMode : u32 {
    HdAnm_ModeLowMask = 0xF,
    HdAnm_ModeHighMask = 0xF0,
    HdAnm_ModeEight = 8,
    HdAnm_ModeSeven = 7,
    HdAnm_ModeHighValue = 0x10,
    HdAnm_DrawFlags = 3,
};
void* hdAnmAllocate(u32);
void hdAnmFrameConstruct(mDoExt_baseAnm*, u32);
void hdAnmBackendConstruct(HdAnmBackend*);
HdAnmModelData* hdAnmGetModelData(HdAnmModel*);
u32 hdAnmDescriptorSize(HdAnmDescriptor*);
void* hdAnmBufferAllocate(u32, u32);
void hdAnmBackendInit(HdAnmBackend*, HdAnmDescriptor*, void*, u32);
void hdAnmSetResource(HdAnmBackend*, HdAnmResource*);
void hdAnmBindModel(HdAnmBackend*, HdAnmModelData*);
u32 hdAnmInitPlay(mDoExt_baseAnm*, s32, s32, s32, s32, u32, f32);
void hdAnmBackendCalc(HdAnmBackend*);
void hdAnmBackendApply(HdAnmBackend*, void*);
u32 hdAnmIsCurrentSolidHeap();
void hdBckBackendInit(HdBckBackend*, HdAnmModel*, HdAnmObject*);
void hdBckBackendSet(HdBckBackend*, HdAnmObject*);
void hdAnmAssert(const char*, u32, const char*);
void hdAnmDrawStateConstruct(HdAnmDrawState*);
void hdAnmDrawStateApply(HdAnmDrawState*);
