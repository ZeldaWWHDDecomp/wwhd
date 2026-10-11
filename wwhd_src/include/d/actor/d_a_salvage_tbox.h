/* Salvaged treasure chest, WWHD layout.
 * Actor members follow tww at +0x11C (HD fopAc_ac_c is 0x3AC).
 * The shadow callback is unchanged in size; HD adds GX2 rendering/setup methods.
 */
#pragma once
#include "bindings.h"
struct daSTBox_shadowEcallBack_c {
    void getMaxWaterY(cXyz *p);
    void setup(void *emitter, void *pos, void *angle);
    void executeAfter();
    void execute(void *emitter);
    void draw(void *emitter);
    void drawAfter(void *emitter);
    be<u32> __vtbl;
    be<s16> field_0x4;
    u8 pad6[2];
    be<f32> mWaterY, mWaterFlatY, mDepth;
    cXyz field_0x14[3], mPos;
    gptr<csXyz> mpAngle;
    be<f32> mExTransY, mExScaleY, field_0x50;
    gptr<void> mpEmitter;
};
WWHD_SIZE(daSTBox_shadowEcallBack_c, 0x58);
struct daSTBox_c : fopAc_ac_c {
    s32 CreateHeap();
    BOOL actWaitDummy(int i);
    BOOL actWaitGetItem(int i);
    void initWaitDummy();
    void initWait();
    void set_mtx();
    // HD: actor entry returns preserve the full result register.
    s32 _delete();
    s32 _draw();
    void initWait02(s32 staff);
    void initWaitGetItem(s32 staff);
    void initDrop(s32 staff);
    s32 actWait02(s32 staff);
    s32 actDrop(s32 staff);
    void CreateInit();
    s32 _create();
    s32 _execute();
    s32 actWait(s32 staff);
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<void> field_0x29C[3]; // tww unknown name, HD emitter pointers
    u8 pad3c4[4], mRippleCallBack[0x14];
    /* 0x3DC */ daSTBox_shadowEcallBack_c mShadowCallback;
    cXyz mParticlePos, field_0x324;
    be<u8> field_0x330, field_0x331;
    be<s16> field_0x332;
    be<u8> field_0x334, field_0x335, field_0x336;
    u8 pad453;
    be<u32> mItemPID;
};
WWHD_OFFSET(daSTBox_c, mShadowCallback, 0x3DC);
WWHD_OFFSET(daSTBox_c, field_0x331, 0x44D);
WWHD_SIZE(daSTBox_c, 0x458);

void stboxChaseS(void *, s32, s32);
s32 stboxSeaCheckArea(f32, f32);
f32 stboxSeaCalcWave(f32, f32);
f32 stboxWaterGroundCheck(void *);
f32 stboxGetWaterHeight(void *);
u32 stboxGetPlay();
s32 stboxResLoad(void *, const char *);
void stboxResDelete(void *, const char *);
s32 stboxGetReverb(s32);
s32 stboxGetStaffId(void *, const char *, u32, u32);
s32 stboxGetActionIndex(void *, s32, void *, u32, u32, u32);
void stboxCutEnd(void *, s32);
s32 stboxGetAdvance(void *, s32);
u32 stboxGetEnvironment();
void stboxGetSeaColor(void *, void *);
void stboxSettingTev(void *, u32, void *, void *);
void stboxSetLightTevColor(void *, void *, void *);
void *stboxParticleSet(void *, u32, u32, void *, void *, void *, u32, void *, s32, u32, u32, u32);
void stboxRippleConstruct(void *);
void stboxRippleEnd(void *);
void stboxSetEventReg(void *, u32, u32);
u32 stboxGetEventReg(void *, u32);
void stboxActorConstruct(void *);
void stboxActorDestroy(u32, u32);
s32 stboxEntrySolidHeap(void *, void *, s32);
void stboxSetCullBox(void *, f32, f32, f32, f32, f32, f32);
void stboxPosMove(void *, u32);
u32 stboxCreateItem(void *, u32, s32, s32, u32, u32);
void stboxOffDraw(void *);
void stboxCreateWaterPillar(void *, u32, f32, f32);
void stboxSubBgmStart(u32);
void stboxSoundStart(u32, void *, u32, s32);
void stboxModelUpdate(void *, u32);
J3DModel *stboxModelCreate(void *, u32, u32);
void stboxMtxYrot(void *, s32);
J3DModelData *stboxGetResource(void *, void *, s32);
void stboxAssertion(const char *, u32, const char *);
cXyz *stboxVectorAllocate(u32);
void stboxDeallocate(void *);
void stboxDepthConstruct(void *);
void stboxDepthApply(void *);
void stboxBufferFlush(void *, u32, u32);
s32 stboxFindShader(u32, void *);
void stboxVertexBufferBind(void *);
void stboxMaterialFinish(void *);
void stboxSetTextureMatrix(void *, void *, u32);
void stboxShaderBind(void *, void *);
u32 stboxGetShaderArchive();
void stboxProgramSetup(u32, u32, u32);
void stboxEmitterShaderSetup(u32);
void stboxRotationMatrix(s32, s32, s32, void *);
void stboxParticleMaterialSetup(void *, void *);
void stboxMtxIdentity(void *);
void stboxMtxTranslation(void *, f32, f32, f32);
void stboxCircleConstruct(void *);
void stboxAngleConstruct(void *);
void stboxConstructArray(void *, u32, u32, void *);
void stboxRegisterGlobal(void *);
void stboxDrawStrip(u32, u32, u32, u32, u32, u32);

// HD: particle/material rendering views used by the salvage ribbon.
struct STBoxColor {
    be<u8> r, g, b, a;
};
WWHD_SIZE(STBoxColor, 4);
struct STBoxEmitter {
    u8 pad_0[0x8];
    /* 0x008 */ cXyz mEmitterScale;
    /* 0x014 */ cXyz mEmitterTranslation;
    u8 pad_20[0x3C];
    /* 0x05C */ be<s32> mFrameCount;
    u8 pad_60[0x134];
    /* 0x194 */ be<f32> mFrame;
    u8 pad_198[0x14];
    /* 0x1AC */ be<u32> mParticleFirst;
    u8 pad_1b0[0x4];
    /* 0x1B4 */ be<u32> mParticleCount;
    u8 pad_1b8[0x2C];
    /* 0x1E4 */ be<u32> mpCallBack;
    u8 pad_1e8[0x8];
    /* 0x1F0 */ Mtx34 mGlobalRotation;
    u8 pad_220[0xC];
    /* 0x22C */ cXyz mGlobalTranslation;
    /* 0x238 */ cXyz mGlobalParticleScale;
    /* 0x244 */ STBoxColor mGlobalPrmColor;
    /* 0x248 */ STBoxColor mGlobalEnvColor;
    u8 pad_24c[0x8];
    /* 0x254 */ be<u32> mStatus;
    u8 pad_258[0xA];
    /* 0x262 */ be<u8> mEmitterKind;
    u8 pad_263[0x9];
    /* 0x26C */ be<u32> mpShaderProgram;
    u8 pad_270[0xA0];
    /* 0x310 */ be<u32> mpRibbonBuffers;
};
WWHD_SIZE(STBoxEmitter, 0x314);
WWHD_OFFSET(STBoxEmitter, mEmitterScale, 0x8);
WWHD_OFFSET(STBoxEmitter, mEmitterTranslation, 0x14);
WWHD_OFFSET(STBoxEmitter, mFrameCount, 0x5C);
WWHD_OFFSET(STBoxEmitter, mFrame, 0x194);
WWHD_OFFSET(STBoxEmitter, mParticleFirst, 0x1AC);
WWHD_OFFSET(STBoxEmitter, mParticleCount, 0x1B4);
WWHD_OFFSET(STBoxEmitter, mpCallBack, 0x1E4);
WWHD_OFFSET(STBoxEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(STBoxEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(STBoxEmitter, mGlobalParticleScale, 0x238);
WWHD_OFFSET(STBoxEmitter, mGlobalPrmColor, 0x244);
WWHD_OFFSET(STBoxEmitter, mGlobalEnvColor, 0x248);
WWHD_OFFSET(STBoxEmitter, mStatus, 0x254);
WWHD_OFFSET(STBoxEmitter, mEmitterKind, 0x262);
WWHD_OFFSET(STBoxEmitter, mpShaderProgram, 0x26C);
WWHD_OFFSET(STBoxEmitter, mpRibbonBuffers, 0x310);
struct STBoxParticle {
    u8 pad_0[0x10];
    /* 0x010 */ cXyz mOffsetPosition;
    u8 pad_1c[0xC];
    /* 0x028 */ cXyz mGlobalPosition;
    u8 pad_34[0xB8];
    /* 0x0EC */ be<u32> mpMaterial;
    /* 0x0F0 */ be<u32> mpShader;
    u8 pad_f4[0x18];
    /* 0x10C */ be<u8> mHidden;
    u8 pad_10d[0x13];
    /* 0x120 */ be<u8> mDrawDone;
};
WWHD_SIZE(STBoxParticle, 0x121);
WWHD_OFFSET(STBoxParticle, mOffsetPosition, 0x10);
WWHD_OFFSET(STBoxParticle, mGlobalPosition, 0x28);
WWHD_OFFSET(STBoxParticle, mpMaterial, 0xEC);
WWHD_OFFSET(STBoxParticle, mpShader, 0xF0);
WWHD_OFFSET(STBoxParticle, mHidden, 0x10C);
WWHD_OFFSET(STBoxParticle, mDrawDone, 0x120);
struct STBoxParticleLink {
    /* 0x000 */ be<u32> mpObject;
    u8 pad_4[0x8];
    /* 0x00C */ be<u32> mpNext;
};
WWHD_SIZE(STBoxParticleLink, 0x10);
WWHD_OFFSET(STBoxParticleLink, mpObject, 0x0);
WWHD_OFFSET(STBoxParticleLink, mpNext, 0xC);
struct STBoxShip {
    u8 pad_0[0x32A];
    /* 0x32A */ be<s16> mCraneHookAngleY;
    u8 pad_32c[0x3F0];
    /* 0x71C */ be<u32> mpCraneTop;
    u8 pad_720[0xBE8];
    /* 0x1308 */ cXyz mCraneRipplePos;
};
WWHD_SIZE(STBoxShip, 0x1314);
WWHD_OFFSET(STBoxShip, mCraneHookAngleY, 0x32A);
WWHD_OFFSET(STBoxShip, mpCraneTop, 0x71C);
WWHD_OFFSET(STBoxShip, mCraneRipplePos, 0x1308);
struct STBoxPlay {
    u8 pad_0[0x5292];
    /* 0x5292 */ be<u8> mEventRunning;
    u8 pad_5293[0xD];
    /* 0x52A0 */ be<u32> mItemPartnerId;
    u8 pad_52a4[0x20];
    /* 0x52C4 */ be<u8> mEventManager;
    u8 pad_52c5[0x7EB];
    /* 0x5AB0 */ be<u32> mpParticleControl;
    u8 pad_5ab4[0x88];
    /* 0x5B3C */ be<u32> mpShipActor;
};
WWHD_SIZE(STBoxPlay, 0x5B40);
WWHD_OFFSET(STBoxPlay, mEventRunning, 0x5292);
WWHD_OFFSET(STBoxPlay, mItemPartnerId, 0x52A0);
WWHD_OFFSET(STBoxPlay, mEventManager, 0x52C4);
WWHD_OFFSET(STBoxPlay, mpParticleControl, 0x5AB0);
WWHD_OFFSET(STBoxPlay, mpShipActor, 0x5B3C);
struct STBoxModel {
    u8 pad_0[0xBC];
    /* 0x0BC */ cXyz mBaseScale;
    /* 0x0C8 */ Mtx34 mBaseTRMtx;
};
WWHD_SIZE(STBoxModel, 0xF8);
WWHD_OFFSET(STBoxModel, mBaseScale, 0xBC);
WWHD_OFFSET(STBoxModel, mBaseTRMtx, 0xC8);
struct STBoxRibbonBuffers {
    u8 pad_0[0x2BA70];
    /* 0x2BA70 */ be<u32> mActiveBank;
    u8 pad_2ba74[0xC];
    /* 0x2BA80 */ be<u32> mNeedsBind;
};
WWHD_SIZE(STBoxRibbonBuffers, 0x2BA84);
WWHD_OFFSET(STBoxRibbonBuffers, mActiveBank, 0x2BA70);
WWHD_OFFSET(STBoxRibbonBuffers, mNeedsBind, 0x2BA80);
struct STBoxRibbonRecord {
    /* 0x000 */ be<u32> mpVertices;
    u8 pad_4[0x14C];
    /* 0x150 */ be<u32> mFlushSize;
    u8 pad_154[0x4];
    /* 0x158 */ be<u8> mVertexBuffer;
    u8 pad_159[0xFB];
};
WWHD_SIZE(STBoxRibbonRecord, 0x254);
WWHD_OFFSET(STBoxRibbonRecord, mpVertices, 0x0);
WWHD_OFFSET(STBoxRibbonRecord, mFlushSize, 0x150);
WWHD_OFFSET(STBoxRibbonRecord, mVertexBuffer, 0x158);
struct STBoxRibbonVertex {
    /* 0x000 */ cXyz mPosition;
    /* 0x00C */ be<f32> mTexX;
    /* 0x010 */ be<f32> mTexY;
    /* 0x014 */ cXyz mPreviousPosition;
    /* 0x020 */ be<f32> mPreviousTexX;
    /* 0x024 */ be<f32> mPreviousTexY;
};
WWHD_SIZE(STBoxRibbonVertex, 0x28);
WWHD_OFFSET(STBoxRibbonVertex, mPosition, 0x0);
WWHD_OFFSET(STBoxRibbonVertex, mTexX, 0xC);
WWHD_OFFSET(STBoxRibbonVertex, mTexY, 0x10);
WWHD_OFFSET(STBoxRibbonVertex, mPreviousPosition, 0x14);
WWHD_OFFSET(STBoxRibbonVertex, mPreviousTexX, 0x20);
WWHD_OFFSET(STBoxRibbonVertex, mPreviousTexY, 0x24);

struct STBoxMember {
    be<s16> mThisDelta, mVirtualSlot;
    be<u32> mTarget;
};
WWHD_SIZE(STBoxMember, 8);
struct STBoxBufferDescriptor {
    u8 pad0[0x14C];
    be<u32> mFlushSize;
};
WWHD_SIZE(STBoxBufferDescriptor, 0x150);
struct STBoxMaterial {
    u8 pad0[12];
    be<u32> __vtbl;
};
WWHD_OFFSET(STBoxMaterial, __vtbl, 12);
struct STBoxMaterialVtable {
    u8 pad0[0x2C];
    be<u32> mpApply;
};
WWHD_OFFSET(STBoxMaterialVtable, mpApply, 0x2C);
struct STBoxShaderArchive {
    be<u32> __vtbl, mpArchive, mProgramCount, mpPrograms;
};
WWHD_SIZE(STBoxShaderArchive, 0x10);
struct STBoxShaderProgram {
    u8 pad0[0x20];
    be<u8> mReady;
    u8 pad21[3];
};
WWHD_SIZE(STBoxShaderProgram, 0x24);
struct STBoxShaderArchiveData {
    u8 pad0[0x1C];
    be<u32> mCount, mpParameters;
};
WWHD_SIZE(STBoxShaderArchiveData, 0x24);
struct STBoxHeaderZeros {
    be<u32> mWords[4];
};
WWHD_SIZE(STBoxHeaderZeros, 16);
struct STBoxDestructorRecord {
    u8 mBytes[12];
};
WWHD_SIZE(STBoxDestructorRecord, 12);
struct STBoxShadowTexture {
    be<u32> __vtbl;
    be<u8> mAlpha;
    u8 pad5[3];
    be<u32> mFlags;
    be<u8> mShadowAlpha;
    u8 padd[3];
    be<f32> mDepth, mScroll, mScale;
};
WWHD_SIZE(STBoxShadowTexture, 0x1C);
struct STBoxDepthState {
    be<u8> mDepthTest, mDepthWrite;
    u8 pad2[2];
    be<u32> mDepthFunction;
    u8 pad8[4];
    be<u32> mFlags;
    u8 pad10[0x10C];
};
WWHD_SIZE(STBoxDepthState, 0x11C);
enum STBoxEmitterFlags { STBOX_INVALID = 1, STBOX_IMMORTAL = 0x40 };
enum STBoxParticleId { STBOX_WATER_DROP = 0x38, STBOX_SALVAGE_SHADOW = 0x53, STBOX_RIPPLE = 0x35C };

namespace stbox_tu {
constexpr u32 safeString = 0x10039AD4, actor = 0x10039B0C, shadow = 0x10039C4C;
}
constexpr u32 STBOX_SHADOW_TEXTURE_VTABLE = 0x10039C8C;

struct STBoxTable_m_bdlidx {
    be<s16> mEntries[3];
};

struct STBoxTable_m_heapsize {
    be<s16> mEntries[3];
};

struct STBoxTable_crane_offset {
    be<f32> mEntries[3];
};

struct STBoxTable_action_table {
    be<u32> mEntries[5];
};

struct STBoxTable_event_init_tbl {
    STBoxMember mEntries[5];
};

struct STBoxTable_event_action_tbl {
    STBoxMember mEntries[5];
};

struct STBoxTable_l_headerDtor {
    STBoxDestructorRecord mEntries[3];
};

struct STBoxRippleCallback {
    be<u32> __vtbl;
    u8 pad4[12];
    be<f32> mRate;
};
WWHD_SIZE(STBoxRippleCallback, 0x14);
WWHD_OFFSET(daSTBox_c, mPhase, 0x3AC);
WWHD_OFFSET(daSTBox_c, mpModel, 0x3B4);
WWHD_OFFSET(daSTBox_c, field_0x29C, 0x3B8);
WWHD_OFFSET(daSTBox_c, mRippleCallBack, 0x3C8);
WWHD_OFFSET(daSTBox_c, mParticlePos, 0x434);
WWHD_OFFSET(daSTBox_c, field_0x324, 0x440);
WWHD_OFFSET(daSTBox_c, field_0x330, 0x44C);
WWHD_OFFSET(daSTBox_c, field_0x332, 0x44E);
WWHD_OFFSET(daSTBox_c, field_0x334, 0x450);
WWHD_OFFSET(daSTBox_c, field_0x335, 0x451);
WWHD_OFFSET(daSTBox_c, field_0x336, 0x452);
WWHD_OFFSET(daSTBox_c, mItemPID, 0x454);
WWHD_OFFSET(daSTBox_shadowEcallBack_c, field_0x4, 0x4);
WWHD_OFFSET(daSTBox_shadowEcallBack_c, mWaterY, 0x8);
WWHD_OFFSET(daSTBox_shadowEcallBack_c, mWaterFlatY, 0xC);
WWHD_OFFSET(daSTBox_shadowEcallBack_c, mDepth, 0x10);
WWHD_OFFSET(daSTBox_shadowEcallBack_c, field_0x14, 0x14);
WWHD_OFFSET(daSTBox_shadowEcallBack_c, mPos, 0x38);
WWHD_OFFSET(daSTBox_shadowEcallBack_c, mpAngle, 0x44);
WWHD_OFFSET(daSTBox_shadowEcallBack_c, mExTransY, 0x48);
WWHD_OFFSET(daSTBox_shadowEcallBack_c, mExScaleY, 0x4C);
WWHD_OFFSET(daSTBox_shadowEcallBack_c, field_0x50, 0x50);
WWHD_OFFSET(daSTBox_shadowEcallBack_c, mpEmitter, 0x54);

struct SafeName {
    be<u32> text, vt;
};
WWHD_SIZE(SafeName, 8);
WWHD_SIZE(STBoxTable_m_bdlidx, 0x6);
WWHD_SIZE(STBoxTable_m_heapsize, 0x6);
WWHD_SIZE(STBoxTable_crane_offset, 0xC);
WWHD_SIZE(STBoxTable_action_table, 0x14);
WWHD_SIZE(STBoxTable_event_init_tbl, 0x28);
WWHD_SIZE(STBoxTable_event_action_tbl, 0x28);
WWHD_SIZE(STBoxTable_l_headerDtor, 0x24);
