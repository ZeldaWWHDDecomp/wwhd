/* Forsaken Fortress warp portal. Private HD views. */
#pragma once
#include "gc_api/f_op/f_op_actor.h"
// HD: model, animation, material and emitter layouts differ from GameCube.
struct WarpgnColor {
    be<u8> r, g, b, a;
};
WWHD_SIZE(WarpgnColor, 4);
struct WarpgnColor4f {
    be<f32> r, g, b, a;
};
WWHD_SIZE(WarpgnColor4f, 16);
struct WarpgnAnimation {
    be<u32> mRate;
    be<f32> mFrame;
    be<u16> mStart;
    be<s16> mEnd;
};
WWHD_SIZE(WarpgnAnimation, 12);
WWHD_OFFSET(WarpgnAnimation, mFrame, 4);
WWHD_OFFSET(WarpgnAnimation, mEnd, 10);
struct WarpgnModel {
    u8 mPadding0[0xAC];
    be<u32> mpModelData;
    u8 mPaddingB0[12];
    cXyz mBaseScale;
    be<f32> mBaseTRMtx[12];
};
WWHD_OFFSET(WarpgnModel, mpModelData, 0xAC);
WWHD_OFFSET(WarpgnModel, mBaseScale, 0xBC);
WWHD_OFFSET(WarpgnModel, mBaseTRMtx, 0xC8);
WWHD_SIZE(WarpgnModel, 0xF8);
struct WarpgnEmitter {
    u8 mPadding0[0x5C];
    be<u32> mMaxFrame;
    u8 mPadding60[0x1F4];
    be<u32> mFlags;
};
WWHD_OFFSET(WarpgnEmitter, mMaxFrame, 0x5C);
WWHD_OFFSET(WarpgnEmitter, mFlags, 0x254);
WWHD_SIZE(WarpgnEmitter, 0x258);
struct WarpgnSave {
    u8 mPadding0[0x20];
    u8 mZone[0x624];
    u8 mEvent[1];
};
WWHD_OFFSET(WarpgnSave, mZone, 0x20);
WWHD_OFFSET(WarpgnSave, mEvent, 0x644);
struct WarpgnPlay {
    u8 mPadding0[0x5292];
    be<u8> mEventRunning;
    u8 mPadding5293[0x25];
    be<u16> mEventFlags;
    u8 mPadding52BA[10];
    u8 mEventManager[0x7EC];
    be<u32> mpParticle;
    u8 mPadding5AB4[0x78];
    be<u32> mpPlayer;
    u8 mPadding5B30[12];
    be<u32> mpShip;
    u8 mPadding5B40[0x198];
    be<u32> mPlayerStatus0;
};
WWHD_OFFSET(WarpgnPlay, mEventRunning, 0x5292);
WWHD_OFFSET(WarpgnPlay, mEventFlags, 0x52B8);
WWHD_OFFSET(WarpgnPlay, mEventManager, 0x52C4);
WWHD_OFFSET(WarpgnPlay, mpParticle, 0x5AB0);
WWHD_OFFSET(WarpgnPlay, mpPlayer, 0x5B2C);
WWHD_OFFSET(WarpgnPlay, mpShip, 0x5B3C);
WWHD_OFFSET(WarpgnPlay, mPlayerStatus0, 0x5CD8);
struct WarpgnDemoActor {
    u8 mPadding0[0x28];
    be<u32> mShapeId;
};
WWHD_OFFSET(WarpgnDemoActor, mShapeId, 0x28);
struct WarpgnMaterialTable {
    u8 mPadding0[0x24];
    be<u16> mMaterialNum;
};
WWHD_OFFSET(WarpgnMaterialTable, mMaterialNum, 0x24);
struct WarpgnModelData {
    u8 mPadding0[12];
    be<u32> mMaterialCount, mpMaterials;
};
WWHD_OFFSET(WarpgnModelData, mMaterialCount, 12);
WWHD_OFFSET(WarpgnModelData, mpMaterials, 16);
struct WarpgnMaterial {
    u8 mPadding0[0x18];
    be<u32> mpTevBlock;
    u8 mPadding1C[0x84];
    be<u32> mAttribute;
    u8 mTail[0x2F8];
};
WWHD_OFFSET(WarpgnMaterial, mpTevBlock, 0x18);
WWHD_OFFSET(WarpgnMaterial, mAttribute, 0xA0);
WWHD_SIZE(WarpgnMaterial, 0x39C);
struct WarpgnTevBlock {
    u8 mPadding0[4];
    be<u32> mVtable;
};
WWHD_OFFSET(WarpgnTevBlock, mVtable, 4);
struct WarpgnTevVtable {
    u8 mPadding0[0x3C];
    be<u32> mSetColor;
    u8 mPadding40[12];
    be<u32> mGetColor;
};
WWHD_OFFSET(WarpgnTevVtable, mSetColor, 0x3C);
WWHD_OFFSET(WarpgnTevVtable, mGetColor, 0x4C);
template<class T> inline T* Warpgn_view(u32 address) { return gabi::at<T>(address); }
struct WarpgnLight {
    cXyz mPosition, mDirection;
    WarpgnColor mColor;
    be<s16> mAngle[4];
    be<f32> mAttenuation[8];
};
WWHD_OFFSET(WarpgnLight, mColor, 0x18);
WWHD_OFFSET(WarpgnLight, mAngle, 0x1C);
WWHD_OFFSET(WarpgnLight, mAttenuation, 0x24);
WWHD_SIZE(WarpgnLight, 0x44);
struct WarpgnTevLight {
    WarpgnLight mLight;
    u8 mPadding44[0x7C];
    WarpgnLight mHdLight[1];
    u8 mPadding104[0x40];
    WarpgnLight mHdLight2;
    u8 mTail[0x40];
};
WWHD_OFFSET(WarpgnTevLight, mHdLight, 0xC0);
WWHD_OFFSET(WarpgnTevLight, mHdLight2, 0x144);
WWHD_SIZE(WarpgnTevLight, 0x1C8);
inline WarpgnTevLight* Warpgn_tev(dKy_tevstr_c* tev) {return reinterpret_cast<WarpgnTevLight*>(tev);}
struct daWarpgn_c : fopAc_ac_c {
    // HD: the verified entrypoints retain their 32-bit results and omit unused staff arguments.
 s32 _delete();
    s32 CreateHeap();
    void CreateInit();
    s32 _create();
    s32 _execute();
    s32 normal_execute();
    s32 demo_execute();
    void demo_proc();
    s32 actWait();
    void initStartWarp();
    s32 actStartWarp();
    s32 actWarp();
    void initWarpArrive();
    s32 actWarpArrive();
    s32 actWarpArriveEnd();
    void initAppear();
    s32 actAppear();
    void eventOrder();
    void checkOrder();
    void anim_play(s32 mode);
    void set_end_anim();
    s32 check_warp();
    s32 checkValidWarp();
    s32 _draw();
    request_of_phase_process_class mPhs;
    be<u32> mpModel, mpBtkAnm1, mpBtkAnm2, mpBrkAnm, mpBckAnm, mpEmitter1, mpEmitter2, mpEmitter3, mSwitchNo;
    be<u8> mIsSwitch;
    u8 pad[3];
    be<u32> mSceneNo;
    be<s32> m2C4;
    be<s16> mEvtToMajyuuWarpIdx, mEvtAppearWarpIdx;
    be<s32> mStaffId;
    be<u32> mDemoShape;
    be<s32> mTimer;
    dKy_tevstr_c mTevStr;
    be<u8> m388;
    u8 tail[3];
};
WWHD_OFFSET(daWarpgn_c, mPhs, 0x3AC);
WWHD_OFFSET(daWarpgn_c, mpModel, 0x3B4);
WWHD_OFFSET(daWarpgn_c, mpEmitter1, 0x3C8);
WWHD_OFFSET(daWarpgn_c, mSwitchNo, 0x3D4);
WWHD_OFFSET(daWarpgn_c, m2C4, 0x3E0);
WWHD_OFFSET(daWarpgn_c, mStaffId, 0x3E8);
WWHD_OFFSET(daWarpgn_c, mTimer, 0x3F0);
WWHD_OFFSET(daWarpgn_c, mTevStr, 0x3F4);
WWHD_OFFSET(daWarpgn_c, m388, 0x5BC);
WWHD_SIZE(daWarpgn_c, 0x5C0);
namespace Warpgn { using Actor = daWarpgn_c; }

void Warpgn_vecSubtract(const void*, void*, const void*);
s32 Warpgn_calcTimer(void*);
u32 Warpgn_getPlay();
s32 Warpgn_resLoad(void*, const char*);
void Warpgn_resDelete(void*, const char*);
s32 Warpgn_getReverb(s32);
u32 Warpgn_getDemoActor(void*, u32);
s32 Warpgn_getStaffId(void*, const char*, s32, s32);
s32 Warpgn_getAction(void*, s32, const void*, s32, s32, s32);
void Warpgn_cutEnd(void*, s32);
void Warpgn_setGoal(void*, const void*);
s32 Warpgn_getEventIndex(void*, void*, s32);
s32 Warpgn_startCheck(void*, s32);
s32 Warpgn_endCheck(void*, s32);
s32 Warpgn_getIsAdvance(void*, s32);
u32 Warpgn_getEnvLight();
void Warpgn_tevInit(void*, s32, s32);
void Warpgn_settingTev(void*, s32, const void*, void*);
void Warpgn_setLightTev(void*, void*, void*);
void Warpgn_nextStage(u32, s32);
u32 Warpgn_particleSet(void*, s32, u32, const void*, s32, s32, s32, s32, s32, s32, s32, s32);
void Warpgn_onEvent(void*, u32);
s32 Warpgn_isEvent(void*, u32);
s32 Warpgn_isSwitch(void*, u32, s32);
void Warpgn_actorConstruct(void*);
void Warpgn_actorDestruct(void*, s32);
s32 Warpgn_entryHeap(void*, void*, u32);
void Warpgn_cullBox(void*, f32, f32, f32, f32, f32, f32);
s32 Warpgn_orderEvent(void*, s32, s32, s32, s32, s32);
u32 Warpgn_soundStart(u32);
void Warpgn_soundLevel(u32, const void*, s32, s32);
void Warpgn_modelUpdate(void*, s32);
u32 Warpgn_modelCreate(void*, u32, u32);
s32 Warpgn_animationPlay(void*);
u32 Warpgn_btkConstruct(s32);
s32 Warpgn_btkInit(void*, void*, void*, s32, s32, s32, s32, s32, f32, s32);
void Warpgn_btkEntry(void*, void*, f32);
u32 Warpgn_brkConstruct(s32);
s32 Warpgn_brkInit(void*, void*, void*, s32, s32, s32, s32, s32, f32, s32);
void Warpgn_brkEntry(void*, void*, f32);
s32 Warpgn_bckInit(void*, void*, void*, s32, s32, s32, s32, s32, f32);
void Warpgn_bckEntry(void*, void*, f32);
u32 Warpgn_getResource(void*, void*, s32);
void Warpgn_assertion(const void*, s32, const void*);
u32 Warpgn_allocate(u32);
void Warpgn_deallocate(void*);
void Warpgn_scaleColor(void*, const void*, f32);
void Warpgn_bckMatrixConstruct(void*);
void Warpgn_frameConstruct(void*, s32);
u32 Warpgn_materialTable(void*);
u32 Warpgn_materialAttribute(void*, s32);
f32 Warpgn_squareMagnitude(const void*);
void Warpgn_translate(void*, f32, f32, f32);
void Warpgn_constructByte(void*);
void Warpgn_constructHeader(void*);
void Warpgn_registerGlobal(void*);
f32 Warpgn_squareRoot(f32);
s32 daWarpgn_Create(daWarpgn_c *actor);
s32 daWarpgn_Delete(daWarpgn_c *actor);
s32 daWarpgn_Draw(daWarpgn_c *actor);
s32 daWarpgn_Execute(daWarpgn_c *actor);
s32 CheckCreateHeap(daWarpgn_c *actor);
u32 daWarpgnInitWarp();
void Warpgn_globalDestructor1();
void Warpgn_globalDestructor2();
void Warpgn_emptyVirtual();
s32 daWarpgn_IsDelete();
void Warpgn_actorDestructor(daWarpgn_c *actor, s32 flags);
void Warpgn_emptyDestructor(void *object, s32 flags);
void Warpgn_staticInit();
void Warpgn_colorConvert(void *destination, void *source);
daWarpgn_c *daWarpgnConstruct(daWarpgn_c *actor);

inline be<u32>& Warpgn_word(void* p) {return *reinterpret_cast<be<u32>*>(p);}
inline const be<f32>& Warpgn_float(const void* p) {return *reinterpret_cast<const be<f32>*>(p);}

struct WarpgnDefaultLight {
    be<f32> mPositionX;
    be<f32> mPositionY;
    be<f32> mPositionZ;
    be<f32> mDirectionX;
    be<f32> mDirectionY;
    be<f32> mDirectionZ;
    be<u8> mColorR;
    be<u8> mColorG;
    be<u8> mColorB;
    be<u8> mColorA;
    be<s16> mAngle0;
    be<s16> mAngle1;
    be<s16> mAngle2;
    be<s16> mAngle3;
    be<f32> mAttenuation0;
    be<f32> mAttenuation1;
    be<f32> mAttenuation2;
    be<f32> mAttenuation3;
    be<f32> mAttenuation4;
    be<f32> mAttenuation5;
    be<f32> mAttenuation6;
    be<f32> mAttenuation7;
};
WWHD_SIZE(WarpgnDefaultLight, 0x44);
struct WarpgnTU {
    u32 mSourceName;
    u32 mModelAssertion;
    u32 mBtkAssertion;
    u32 mBrkAssertion;
    u32 mBckAssertion;
    u32 mDemoSourceName;
    u32 mDemoAssertion;
    u32 mHeapCallback;
    u32 mActorVtable;
    u32 mSafeStringVtable;
    u32 mBaseAnimationVtable;
    u32 mMatrixVtable;
    u32 mAnimationVtable;
    u32 mHeaderRegistration0;
    u32 mHeaderRegistration1;
    u32 mHeaderRegistration2;
    u32 mHeaderGuard;
    u32 mHeaderByte;
};

struct WarpgnBckAnimation {
    WarpgnAnimation mFrame;
    u8 mPaddingC[4];
    be<u32> mVtable;
    u8 mMatrix[0x34];
    be<u32> mMatrixVtable;
    u8 mPadding4C[12];
    be<u32> mMatrixAnm;
    u8 mPadding5C[0x20];
    be<u32> mFrameData, mBlend, mJoint, mModel;
};
WWHD_OFFSET(WarpgnBckAnimation, mVtable, 0x10);
WWHD_OFFSET(WarpgnBckAnimation, mMatrix, 0x14);
WWHD_OFFSET(WarpgnBckAnimation, mMatrixVtable, 0x48);
WWHD_OFFSET(WarpgnBckAnimation, mMatrixAnm, 0x58);
WWHD_OFFSET(WarpgnBckAnimation, mFrameData, 0x7C);
WWHD_OFFSET(WarpgnBckAnimation, mBlend, 0x80);
WWHD_OFFSET(WarpgnBckAnimation, mJoint, 0x84);
WWHD_OFFSET(WarpgnBckAnimation, mModel, 0x88);
WWHD_SIZE(WarpgnBckAnimation, 0x8C);
struct WarpgnEventMethod {
    be<s16> mAdjustment, mIndex;
    be<u32> mTarget;
};
WWHD_SIZE(WarpgnEventMethod, 8);
inline s32 Warpgn_eventMethod(daWarpgn_c* actor, const WarpgnEventMethod& method, s32 staff) {
 const auto& callback=*reinterpret_cast<const gabi::ptmf<s32(daWarpgn_c::*)(s32)>*>(&method);
 return (actor->*callback)(staff);
}
inline be<u32>& Warpgn_rate(u32 address) {return Warpgn_view<WarpgnAnimation>(address)->mRate;}
inline be<f32>& Warpgn_floatRate(u32 address) {return *reinterpret_cast<be<f32>*>(&Warpgn_rate(address));}

enum WarpgnOrder { WarpgnOrder_NONE, WarpgnOrder_APPEAR, WarpgnOrder_WARP };
enum WarpgnAnimationMode { WarpgnAnim_WAIT, WarpgnAnim_APPEAR, WarpgnAnim_OPEN, WarpgnAnim_FIRST, WarpgnAnim_REVERSE };
enum WarpgnResource { dRes_INDEX_GMJWP_BCK_GMJWP01_e=6, dRes_INDEX_GMJWP_BDL_GMJWP00_e=9, dRes_INDEX_GMJWP_BRK_GMJWP01_e=12, dRes_INDEX_GMJWP_BTK_GMJWP00_e=15, dRes_INDEX_GMJWP_BTK_GMJWP02_e=16 };

WWHD_OFFSET(daWarpgn_c, mpBtkAnm1, 0x3B8);
WWHD_OFFSET(daWarpgn_c, mpBtkAnm2, 0x3BC);
WWHD_OFFSET(daWarpgn_c, mpBrkAnm, 0x3C0);
WWHD_OFFSET(daWarpgn_c, mpBckAnm, 0x3C4);
WWHD_OFFSET(daWarpgn_c, mpEmitter2, 0x3CC);
WWHD_OFFSET(daWarpgn_c, mpEmitter3, 0x3D0);
WWHD_OFFSET(daWarpgn_c, mIsSwitch, 0x3D8);
WWHD_OFFSET(daWarpgn_c, mSceneNo, 0x3DC);
WWHD_OFFSET(daWarpgn_c, mEvtToMajyuuWarpIdx, 0x3E4);
WWHD_OFFSET(daWarpgn_c, mEvtAppearWarpIdx, 0x3E6);
WWHD_OFFSET(daWarpgn_c, mDemoShape, 0x3EC);
