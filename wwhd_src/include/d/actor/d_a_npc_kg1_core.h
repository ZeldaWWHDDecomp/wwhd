// Salvatore's core layout. HD: the NPC base is 0x7DC; texture animators are 0x74,
// the action PTMF is eight bytes, and the later minigame fields move accordingly.
#pragma once
#include "bindings.h"
#include "d/d_camera.h"
#include "gc_api/tu.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
#include "gc_api/f_op/f_op_msg.h"
namespace kg1_core {
struct daNpc_Kg1_c : fopNpc_npc_c {
    /* 0x7DC */ gptr<J3DModel> m6C4;
    /* 0x7E0 */ mDoExt_btpAnm mBtpAnm;
    /* 0x854 */ request_of_phase_process_class mPhs;
    /* 0x85C */ gabi::ptmf<void (daNpc_Kg1_c::*)()> mAction;
    /* 0x864 */ u8 _864[4];
    /* 0x868 */ gptr<J3DAnmTexPattern> m_eye_tex_pattern;
    /* 0x86C */ mDoExt_btpAnm m6F8;
    /* 0x8E0 */ mDoExt_btpAnm m70C;
    /* 0x954 */ be<u8> m720;
    /* 0x955 */ u8 _955;
    /* 0x956 */ be<s16> m72C;
    /* 0x958 */ be<s16> m72E;
    /* 0x95A */ be<u8> m730;
    /* 0x95B */ be<u8> m731;
    /* 0x95C */ be<u8> m732;
    /* 0x95D */ u8 _95D[3];
    /* 0x960 */ cXyz mEyePos;
    /* 0x96C */ cXyz mHeadPos;
    /* 0x978 */ be<s8> mAnimation;
    /* 0x979 */ be<s8> m74D;
    /* 0x97A */ be<s8> mPreviousAnimation;
    /* 0x97B */ be<u8> mSpecialAnimation;
    /* 0x97C */ be<u8> m750;
    /* 0x97D */ be<u8> m751;
    /* 0x97E */ u8 _97E[0x99C - 0x97E];
    /* 0x99C */ be<u8> mbDidClearGame;
    /* 0x99D */ be<u8> mbGameEnd;
    /* 0x99E */ be<u8> mSequenceFlags[6];
    /* 0x9A4 */ be<u8> m778;
    /* 0x9A5 */ be<u8> m779;
    /* 0x9A6 */ be<u8> mGameBoardScore;
    /* 0x9A7 */ be<u8> mEndGameTimer;
    /* 0x9A8 */ be<u32> mPresentActor;
    /* 0x9AC */ be<s32> mWaitMode;
    /* 0x9B0 */ be<s16> mEventIds[3];
    /* 0x9B6 */ u8 _9B6[2];
    void lookBack();
    BOOL chkAttention();
    BOOL initTexPatternAnm(s32, u32);
    void playTexPatternAnm();
    void set_mtx();
    BOOL CreateHeap();
    s32 CreateInit();
    s32 _create();
    void eventOrder();
    void checkOrder();
    void kg1_talk_camera();
    void wait_action_init();
    void clr_seq_flag();
    u32 getMsg();
    u16 next_msgStatus(be<u32>*);
    void anmAtr(u16);
    void __dt(u32);
};
} // namespace kg1_core
WWHD_SIZE(kg1_core::daNpc_Kg1_c, 0x9B8);
WWHD_OFFSET(kg1_core::daNpc_Kg1_c, m6C4, 0x7DC);
WWHD_OFFSET(kg1_core::daNpc_Kg1_c, mBtpAnm, 0x7E0);
WWHD_OFFSET(kg1_core::daNpc_Kg1_c, mPhs, 0x854);
WWHD_OFFSET(kg1_core::daNpc_Kg1_c, mAction, 0x85C);
WWHD_OFFSET(kg1_core::daNpc_Kg1_c, m6F8, 0x86C);
WWHD_OFFSET(kg1_core::daNpc_Kg1_c, m70C, 0x8E0);
WWHD_OFFSET(kg1_core::daNpc_Kg1_c, m72E, 0x958);
WWHD_OFFSET(kg1_core::daNpc_Kg1_c, mEyePos, 0x960);
WWHD_OFFSET(kg1_core::daNpc_Kg1_c, m74D, 0x979);
WWHD_OFFSET(kg1_core::daNpc_Kg1_c, mbDidClearGame, 0x99C);
WWHD_OFFSET(kg1_core::daNpc_Kg1_c, mGameBoardScore, 0x9A6);
WWHD_OFFSET(kg1_core::daNpc_Kg1_c, mEventIds, 0x9B0);
struct Kg1HIO {
    /* 0x00 */ be<s8> mChild;
    u8 _1[3];
    /* 0x04 */ be<s32> mUsers;
    /* 0x08 */ be<u8> mDebug;
    u8 _9[3];
    /* 0x0C */ dNpc_HIO_c mNpc;
    /* 0x34 */ be<u8> mDraw;
    /* 0x35 */ be<u8> mEnable;
    u8 _36[2];
    /* 0x38 */ be<u32> __vtbl;
};
WWHD_SIZE(Kg1HIO, 0x3C);
WWHD_OFFSET(Kg1HIO, mNpc, 0xC);
struct Kg1VectorWords {
    be<u32> mWord[3];
};
struct Kg1ResourceName {
    u8 mCallFrame[8];
    SafeString name;
};
struct Kg1ResourceIndices {
    be<s32> mIndex[12];
};
struct Kg1RequestedAnimations {
    be<s8> mAnimation[9];
};
struct Kg1HeaderStatics {
    be<f32> mFirst, mSecond;
    be<u8> mAngle, mVector;
    u8 _A[2];
    be<u32> mZero[4];
};
struct Kg1InitialValues {
    be<f32> mFirst, mSecond;
    cXyz mCenter, mEye;
};
struct Kg1HeadOffsets {
    cXyz mHead, mEye;
    be<u32> mHeadInitialized, mEyeInitialized;
};
struct Kg1PatternVtable {
    u8 _0[0x14];
    gfn mGetFrameMax;
};
struct Kg1PatternView {
    u8 _0[4];
    gptr<Kg1PatternVtable> mpVtable;
};
struct Kg1PlayerVtable {
    u8 _0[0x114];
    gfn mSetPlayerPosAndAngle;
};
struct Kg1JointResource {
    u8 _0[4];
    be<u16> mIndex;
};
struct Kg1ModelDataView : J3DModelData {
    u8 _14[0x38 - 0x14];
    be<u32> mTextureAnimation;
};
WWHD_OFFSET(Kg1ModelDataView, mTextureAnimation, 0x38);
inline Kg1VectorWords* kg1Words(cXyz* v) {
    return reinterpret_cast<Kg1VectorWords*>(v);
}
inline dComIfG_inf_c* kg1Play() {
    return reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
}
inline Kg1HIO& kg1HIO() {
    return *gabi::at<Kg1HIO>(0x10467598);
}
inline Kg1HeaderStatics& kg1HeaderStatics() {
    return *gabi::at<Kg1HeaderStatics>(0x1046757C);
}
inline Kg1InitialValues& kg1InitialValues() {
    return *gabi::at<Kg1InitialValues>(0x1001BF28);
}
inline Kg1HeadOffsets& kg1HeadOffsets() {
    return *gabi::at<Kg1HeadOffsets>(0x104675D4);
}
inline cXyz& kg1CameraCenter() {
    return *gabi::at<cXyz>(0x104675F4);
}
inline cXyz& kg1CameraEye() {
    return *gabi::at<cXyz>(0x10467600);
}
inline Kg1ResourceIndices& kg1BtpIndices() {
    return *gabi::at<Kg1ResourceIndices>(0x1001BC8C);
}
inline Kg1RequestedAnimations& kg1RequestedAnimations() {
    return *gabi::at<Kg1RequestedAnimations>(0x1001BEB8);
}
inline void* kg1Resources() {
    return *gabi::at<gptr<void>>(0x101F4F28);
}
inline Kg1PatternView* kg1Pattern(J3DAnmTexPattern* p) {
    return reinterpret_cast<Kg1PatternView*>(p);
}
inline Kg1PlayerVtable* kg1PlayerVtable(fopAc_ac_c* p) {
    return gabi::at<Kg1PlayerVtable>(p->__vtbl);
}
inline kg1_core::daNpc_Kg1_c* kg1Actor(u32 p) {
    return gabi::at<kg1_core::daNpc_Kg1_c>(p);
}
inline J3DModel* kg1Model(u32 p) {
    return gabi::at<J3DModel>(p);
}
inline Mtx34* kg1Matrix() {
    return mDoMtx_stack_c::get();
}
inline dSv_event_c* kg1SaveEvent(bool temporary = false) {
    return gabi::at<dSv_event_c>(dComIfGs_ea() + (temporary ? 0x1178 : 0x644));
}
// HD: retain the original full-register event values and explicit eye-copy pointer.
inline u32 kg1GetEventReg(dSv_event_c* p, u32 flag) {
    return gabi::call<u32>(0x025B8BB0, p, flag);
}
inline void kg1SetEventReg(dSv_event_c* p, u32 flag, u32 v) {
    gabi::call(0x025B8AF4, p, flag, v);
}
inline void kg1LookAt(dNpc_JntCtrl_c* p, be<s16>* angle, cXyz* target, cXyz* eye, s16 facing,
                      s16 speed, u8 mode) {
    gabi::call(0x0259DED0, p, angle, target, eye, facing, speed, mode);
}
inline f32 kg1Sqrt(f32 value) {
    return gabi::call<f32>(0x028F4384, value);
}
inline s32 kg1Atan2(f32 x, f32 z) {
    return gabi::call<s32>(0x020195B0, x, z);
}

inline be<s16>* kg1JointParams() {
    return gabi::at<be<s16>>(0x104675A8);
}
inline Mtx34* kg1Matrices(u32 p) {
    return gabi::at<Mtx34>(p);
}
inline dCcD_SrcCyl* kg1CylinderSource() {
    return gabi::at<dCcD_SrcCyl>(0x101BF094);
}
inline void* kg1DestructorRecords(int index) {
    return gabi::at<void>(0x101BF0D8 + 12 * index);
}
enum Kg1CoreBindings {
    Kg1ActorVtable = 0x1001BF50,
    Kg1HioVtable = 0x1001BC7C,
    Kg1WaitAction = 0x02266D60,
    Kg1HeapCallback = 0x02265CC8,
    Kg1NodeCallback = 0x0226559C,
    Kg1NpcHioCtor = 0x0259DA18
};
s16 Kg1Core_calcTimer(be<s16>*);
void Kg1Core_destroyCylinder(dCcD_Cyl*, s32);
void Kg1Core_destroyStts(dCcD_Stts*, s32);
void Kg1Core_destroyCircle(cM3dGCir_l*, s32);
void Kg1Core_destroyAcch(dBgS_ObjAcch*, s32);
void Kg1Core_destroyActor(fopAc_ac_c*, s32);
s32 Kg1Core_startCheck(dEvent_manager_c*, s16);
s32 Kg1Core_endCheck(dEvent_manager_c*, s16);
void* Kg1Core_getResource(void*, SafeString*, s32);
void Kg1Core_assert(const char*, u32, const char*);
s32 Kg1Core_initBtp(mDoExt_btpAnm*, J3DModelData*, J3DAnmTexPattern*, u32, s32, f32, s32, s32, u32,
                    s32);
void Kg1Core_cameraStart(dCamera_c*);
void Kg1Core_cameraSet(dCamera_c*, cXyz*, cXyz*, s32, f32);
void Kg1Core_cameraReset(dCamera_c*);
void Kg1Core_cameraTrim(dCamera_c*, s32);
void Kg1Core_entryBtp(mDoExt_btpAnm*, J3DModelData*, s32);
mDoExt_McaMorf* Kg1Core_newMorf(void*, J3DModelData*, void*, void*, J3DAnmTransform*, s32, s32, s32,
                                f32, u32, u32, u32, u32);
s8 Kg1Core_jointIndex(JUTNameTab*, const char*);
void Kg1Core_constructArray(void*, u32, u32, u32);
void Kg1Core_registerGlobal(void*);
void Kg1Core_initAngle(be<u8>*);
void Kg1Core_initVector(be<u8>*);
s32 Kg1Core_createChild(const char*, Kg1HIO*);
s16 Kg1Core_eventIndex(dEvent_manager_c*, const char*, u32);
void Kg1Core_setAnm(kg1_core::daNpc_Kg1_c*);
void Kg1Core_constructNpc(kg1_core::daNpc_Kg1_c*);
s32 Kg1Core_entryHeap(kg1_core::daNpc_Kg1_c*, u32, u32);
Kg1JointResource* Kg1Core_jointResource(J3DNode*);
void Kg1Core_XYZrotM(Mtx34*, s16, s16, s16);
void Kg1Core_onEventBit(dSv_event_c*, u32);
u32 Kg1Core_isEventBit(dSv_event_c*, u32);
s32 Kg1Core_getDays();

inline camera_class* kg1CameraProcess(u32 address) {
    return gabi::at<camera_class>(address);
}
inline dCamera_c* kg1CameraBody(camera_class* p) {
    return gabi::at<dCamera_c>(gabi::ea(p) + CAMERA_BODY);
}

WWHD_SIZE(Kg1VectorWords, 0xC);
WWHD_SIZE(Kg1ResourceName, 0x10);
WWHD_SIZE(Kg1ResourceIndices, 0x30);
WWHD_SIZE(Kg1RequestedAnimations, 9);
WWHD_SIZE(Kg1HeaderStatics, 0x1C);
WWHD_SIZE(Kg1InitialValues, 0x20);
WWHD_SIZE(Kg1HeadOffsets, 0x20);
WWHD_OFFSET(Kg1HeadOffsets, mHeadInitialized, 0x18);
WWHD_OFFSET(Kg1HeadOffsets, mEyeInitialized, 0x1C);
WWHD_OFFSET(Kg1PatternView, mpVtable, 4);
WWHD_OFFSET(Kg1PatternVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Kg1PlayerVtable, mSetPlayerPosAndAngle, 0x114);
WWHD_OFFSET(Kg1HIO, mUsers, 4);
WWHD_OFFSET(Kg1HIO, __vtbl, 0x38);
