/** Scripted demo model actor. HD keeps seven resource IDs, an eight-byte action
 * descriptor, and an additional animated model; GameCube shadow IDs are absent. */
#pragma once
#include "bindings.h"
#include "f_op/f_op_actor.h"

enum DemoDrawMode {
    DEMO_DRAW_NORMAL = 0,
    DEMO_DRAW_MASK_OFF = 2,
    DEMO_DRAW_INVISIBLE = 3,
    DEMO_EVENT_BIT = 4,
    DEMO_ITEM = 5,
    DEMO_MONOTONE = 6,
    DEMO_VIBRATION = 7,
    DEMO_DRAW_P1 = 8,
    DEMO_FADE_WHITE = 9,
    DEMO_FADE_BLACK = 10
};
enum DemoResourceSelector {
    DEMO_BTP = 0,
    DEMO_BTK = 1,
    DEMO_PLIGHT_RESOURCE = 2,
    DEMO_BRK = 4,
    DEMO_BTK_LOOP = 6,
    DEMO_BRK_LOOP = 7
};

struct DemoModelView;
struct DemoMorfView;
// HD: the global save pointer addresses a wrapper preceding dSv_info_c.
struct DemoSaveView {
    u8 _00[0x20];
    dSv_info_c mInfo;
};
WWHD_OFFSET(DemoSaveView, mInfo, 0x20);
struct DemoModelDataView;

struct DemoTextureDescriptor {
    be<u32> words[9];
};
WWHD_SIZE(DemoTextureDescriptor, 0x24);
struct DemoMetadataView {
    be<u32> mTag, mNameOffset;
    u8 _08[8];
    be<u32> mJointNamesOffset;
};
WWHD_SIZE(DemoMetadataView, 0x14);
struct DemoTextureView {
    be<u16> mCount;
    u8 _02[2];
    gptr<DemoTextureDescriptor> mpDescriptors;
    be<u32> mFlagsHigh, mFlagsLow, mExtendedHigh, mExtendedLow, mMaskHigh, mMaskLow;
};
WWHD_SIZE(DemoTextureView, 0x20);
struct DemoImageView {
    u8 _00[8];
    be<u32> mWidth, mHeight;
};
WWHD_SIZE(DemoImageView, 0x10);
struct DemoDescriptorView {
    u8 _00[2];
    be<u16> mWidth, mHeight;
    u8 _06[2];
    be<u8> mWrap;
    u8 _09[3];
    be<u32> mPaletteOffset;
    u8 _10[12];
    be<u32> mImageOffset, mImage;
};
WWHD_SIZE(DemoDescriptorView, 0x24);
struct DemoJointView {
    u8 _00[0x10];
    cXyz mTranslation;
    u8 _1C[8];
    be<f32> mYaw;
};
WWHD_SIZE(DemoJointView, 0x28);
struct DemoMaterialView {
    u8 _00[0x18];
    gptr<void> mpColorBlock;
    u8 _1C[0xA0 - 0x1C];
    be<u32> mFlags;
};
WWHD_SIZE(DemoMaterialView, 0xA4);
struct DemoExposureView {
    u8 _00[0x10B8];
    be<f32> mExposure;
};
WWHD_SIZE(DemoExposureView, 0x10BC);
struct DemoLightDefinition {
    be<u8> mUseJoint, mType;
    u8 _02[2];
    gptr<char> mJointName;
};
WWHD_SIZE(DemoLightDefinition, 8);
struct DemoOtherActor {
    u8 _00[0x3C4];
    gptr<DemoMorfView> mpMorf;
};
WWHD_SIZE(DemoOtherActor, 0x3C8);
struct DemoDrawListsView {
    u8 _00[0x5D58];
    be<u32> mP1Opa;
    u8 _5D5C[4];
    be<u32> mP1Xlu;
    u8 _5D64[0x14];
    be<u32> mOpa, mXlu;
    u8 _5D80[4];
    be<u32> mMaskOffOpa, mMaskOffXlu, mInvisibleOpa, mInvisibleXlu;
};
WWHD_SIZE(DemoDrawListsView, 0x5D94);
WWHD_OFFSET(DemoDrawListsView, mOpa, 0x5D78);
struct DemoColorBlockVtable {
    be<u32> mMethods[10];
};
struct DemoColorBlock {
    u8 _00[4];
    gptr<DemoColorBlockVtable> mpVtable;
};
WWHD_SIZE(DemoColorBlock, 8);
WWHD_SIZE(DemoColorBlockVtable, 0x28);
struct DemoLightView {
    u8 _00[0x28];
    be<f32> mPower;
};
WWHD_SIZE(DemoLightView, 0x2C);
struct DemoSafeStringVtable {
    be<u32> mMethods[6];
};
WWHD_SIZE(DemoSafeStringVtable, 0x18);
struct DemoAnimationVtable {
    be<u32> mMethods[4];
};
WWHD_SIZE(DemoAnimationVtable, 0x10);
struct DemoAnimationResource {
    u8 _00[4];
    gptr<DemoAnimationVtable> mpVtable;
};
WWHD_SIZE(DemoAnimationResource, 8);

// HD: additional renderer fields are views over the verified J3D objects.
struct DemoModelView {
    u8 _00[0x10];
    be<u8> mFramebufferTexture;
    u8 _11[3];
    gptr<DemoMetadataView> mpMetadata;
    u8 _18[0x2A - 0x18];
    be<u16> mMaterialCount;
    gptr<J3DMtxBuffer_hd> mpMtxBuffer;
    u8 _30[4];
    gptr<void> mpMaterialPackets;
    u8 _38[0x74 - 0x38];
    be<u32> mFlags;
    u8 _78[0xAC - 0x78];
    gptr<DemoModelDataView> mpModelData;
    u8 _B0[0xBC - 0xB0];
    cXyz mBaseScale;
    Mtx34 mBaseTransformMtx;
};
WWHD_SIZE(DemoModelView, 0xF8);
WWHD_OFFSET(DemoModelView, mpModelData, 0xAC);
WWHD_OFFSET(DemoModelView, mBaseTransformMtx, 0xC8);
struct DemoMorfView {
    u8 _00[0x30];
    gptr<void> mpJoint;
    u8 _34[0x54 - 0x34];
    be<u32> mFlags;
    u8 _58[0x90 - 0x58];
    gptr<DemoModelView> mpModel;
    u8 _94[4];
    J3DFrameCtrl mFrameCtrl;
};
WWHD_SIZE(DemoMorfView, 0xA8);
WWHD_OFFSET(DemoMorfView, mFlags, 0x54);
WWHD_OFFSET(DemoMorfView, mFrameCtrl, 0x98);
struct DemoModelDataView {
    u8 _00[0x10];
    gptr<DemoMaterialView> mpMaterials;
    u8 _14[0x30 - 0x14];
    gptr<DemoTextureView> mpTextures;
    gptr<void> mpTextureNames;
    be<u32> mBtpAnimation;
    u8 _3C[8];
    be<u32> mBtkAnimation, mBrkAnimation;
};
WWHD_SIZE(DemoModelDataView, 0x4C);
WWHD_OFFSET(DemoModelDataView, mpTextures, 0x30);
struct DemoAuxiliaryView {
    gptr<DemoModelView> mpModel;
    J3DFrameCtrl mFrameCtrl;
};
WWHD_SIZE(DemoAuxiliaryView, 0x14);
struct daDemo00_bgc_c {
    dBgS_GndChk mGndChk;
    be<f32> mGroundY;
};
WWHD_SIZE(daDemo00_bgc_c, 0x58);
WWHD_OFFSET(daDemo00_bgc_c, mGroundY, 0x54);

struct daDemo00_resID_c {
    /* 0x00 */ be<u32> mShapeID;
    /* 0x04 */ be<u32> mBckID;
    /* 0x08 */ be<u32> mAuxiliaryID; // HD: additional animated model resource.
    /* 0x0C */ be<u32> mBtpID;
    /* 0x10 */ be<u32> mBtkID;
    /* 0x14 */ be<u32> mBrkID;
    /* 0x18 */ be<u32> mPlightID;
    void reset();
};
WWHD_SIZE(daDemo00_resID_c, 0x1C);

struct daDemo00_model_c {
    /* 0x00 */ daDemo00_resID_c mID;
    /* 0x1C */ gptr<DemoMorfView> mpMorf;
    /* 0x20 */ gptr<DemoModelView> mpModel;
    /* 0x24 */ gptr<void> mpInvisibleModel;
    /* 0x28 */ gptr<DemoAuxiliaryView> mpAuxiliary; // HD: model and texture animation.
    /* 0x2C */ gptr<J3DFrameCtrl> mpBtpAnm;
    /* 0x30 */ gptr<J3DFrameCtrl> mpBtkAnm;
    /* 0x34 */ gptr<J3DFrameCtrl> mpBrkAnm;
    /* 0x38 */ gptr<void> mpUnused;
    /* 0x3C */ gptr<DemoLightView> mpPlight;
    /* 0x40 */ gptr<daDemo00_bgc_c> mBgc;
    void reset();
};
WWHD_SIZE(daDemo00_model_c, 0x44);
WWHD_OFFSET(daDemo00_model_c, mpMorf, 0x1C);
WWHD_OFFSET(daDemo00_model_c, mpAuxiliary, 0x28);
WWHD_OFFSET(daDemo00_model_c, mpPlight, 0x3C);
WWHD_OFFSET(daDemo00_model_c, mBgc, 0x40);

struct daDemo00_c : fopAc_ac_c {
    // HD: compact action descriptor; the shared GameCube PTMF is twelve bytes.
    /* 0x3AC */ be<s16> actionAdjustment, actionIndex;
    /* 0x3B0 */ be<u32> actionTarget;
    /* 0x3B4 */ be<u8> mDrawMode;
    /* 0x3B5 */ be<s8> mPreviousDrawMode;
    /* 0x3B6 */ be<u8> mGroundValid, mFlags;
    /* 0x3B8 */ daDemo00_resID_c mNextID;
    /* 0x3D4 */ daDemo00_model_c mModel;
    s32 create();
    void setBaseMtx();
    s32 createHeap();
    s32 actStandby(void* demoActor);
    s32 actPerformance(void* demoActor);
    s32 actLeaving(void* demoActor);
    s32 draw();
    s32 execute();
    void adjustPatternFrame(); // HD: auxiliary model's animation follows displacement.
    void drawAuxiliary();      // HD: auxiliary model follows sea height.
};
WWHD_SIZE(daDemo00_c, 0x418);
WWHD_OFFSET(daDemo00_c, actionAdjustment, 0x3AC);
WWHD_OFFSET(daDemo00_c, mDrawMode, 0x3B4);
WWHD_OFFSET(daDemo00_c, mNextID, 0x3B8);
WWHD_OFFSET(daDemo00_c, mModel, 0x3D4);
