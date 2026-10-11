/* daNpc_Gk1_c (Mila's father, poor), WWHD layout. 
 *
 * The GameCube decompilation has no layout (all functions are "Nonmatching" stubs); the members
 * below are measured from the WWHD code of d_a_npc_gk1 and named after their use (the family of
 * d_a_npc_aj1). The class derives from fopNpc_npc_c (0x7DC, see d_a_npc_ba1.h). Members are read
 * up to 0x925. The archive name "Gk" is a member string (0x7F0) copied in decideType. */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* fopNpc_npc_l, ProcFunc_l (shared NPC base, measured) */

struct daNpc_Gk1_c : fopNpc_npc_l {
    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_Neck(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    bool init_GK1_0();
    bool createInit();
    void play_animation();
    void setMtx(u32);
    int bckResID(int);
    int btpResID(int);
    u32 setBtp(s8, u32);
    u32 init_texPttrnAnm(s8, u32);
    void play_btp_anm();
    void setAnm_anm(anm_prm_c*);
    void setAnm();
    void chngAnmAtr(u8);
    void setAnm_ATR();
    void anmAtr(u16);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_GK1_0();
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    u8 chk_talk();
    u8 chk_parts_notMov();
    fopAc_ac_c* searchByID(u32, be<s32>*);
    void lookBack();
    u8 chkAttention();
    void setAttention(u32);
    u8 decideType(int);
    void privateCut(int);
    void endEvent();
    s32 isEventEntry();
    void event_proc(int);
    BOOL set_action(ProcFunc_l*, void*);
    void setStt(s8);
    u8 chk_attn();
    BOOL wait_1();
    BOOL talk_1();
    BOOL wait_action1(void*);
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL bodyCreateHeap();
    BOOL itemCreateHeap();
    BOOL hat_CreateHeap();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_nck_jnt_num;
    /* 0x7E7 */ u8 _7E7;
    /* 0x7E8 */ gptr<J3DModel> mpItemModel; /* "Gk" res 5, on the head joint */
    /* 0x7EC */ gptr<J3DModel> mpHatModel;  /* "Gk" res 6, on the head joint */
    /* 0x7F0 */ char mArcName[4];           /* "Gk" (decideType) */
    /* 0x7F4 */ mDoExt_btpAnm mBtpAnm;      /* mDoExt_btpAnm (HD 0x74); the J3DAnmTexPattern* at +0x10 */
    /* 0x868 */ be<u8> mBtpFrame;
    /* 0x869 */ u8 _869;
    /* 0x86A */ be<s16> mBlinkTimer;
    /* 0x86C */ ProcFunc_l mCurrProcFunc;
    /* 0x874 */ u8 _874[4];
    /* 0x878 */ be<u32> mPartnerProcId; /* looked at in lookBack mode 4 */
    /* 0x87C */ u8 _87C[8];
    /* 0x884 */ cXyz mHomePos;
    /* 0x890 */ csXyz mHomeAngle;
    /* 0x896 */ csXyz mAngle;
    /* 0x89C */ u8 _89C[8];
    /* 0x8A4 */ cXyz mEyePos; /* head joint * (30, 30, 0) */
    /* 0x8B0 */ cXyz mLookPos;
    /* 0x8BC */ u8 _8BC[0xC];
    /* 0x8C8 */ cXyz mHeadPos;
    /* 0x8D4 */ u8 _8D4[0xC];
    /* 0x8E0 */ be<f32> mPrevMorfFrame;
    /* 0x8E4 */ u8 _8E4[4];
    /* 0x8E8 */ be<s16> mActorAngleY;
    /* 0x8EA */ be<s16> mJointHeadY;
    /* 0x8EC */ be<s16> mJointBackboneY;
    /* 0x8EE */ u8 _8EE[2];
    /* 0x8F0 */ be<s32> mbSetEyePos;
    /* 0x8F4 */ be<s16> mEventIDTbl[1];
    /* 0x8F6 */ be<s16> mEventIndex;
    /* 0x8F8 */ u8 _8F8[8];
    /* 0x900 */ be<s16> mTalkEndTimer;
    /* 0x902 */ u8 _902[8];
    /* 0x90A */ be<s16> mLookAngleY;
    /* 0x90C */ be<s8> mbMorfAnimStopped;
    /* 0x90D */ be<u8> mAnimationWrapped;
    /* 0x90E */ be<u8> mItemNo;
    /* 0x90F */ be<u8> mbResLoaded; /* _create: dComIfG_resLoad == complete */
    /* 0x910 */ be<u8> mDemoOnly;   /* no execute/draw outside demos */
    /* 0x911 */ be<u8> mbNoShapeAngle;
    /* 0x912 */ be<u8> mbNoDraw;
    /* 0x913 */ be<u8> mbHomeSet;
    /* 0x914 */ be<u8> mbAttention;
    /* 0x915 */ be<u8> mbTalk;
    /* 0x916 */ be<u8> mbHeadOnly;
    /* 0x917 */ be<u8> mbFar; /* wait_1: player farther than 300 */
    /* 0x918 */ be<u8> mbDemo;
    /* 0x919 */ be<s8> mActionIndex;
    /* 0x91A */ be<u8> mAnmAtr;
    /* 0x91B */ be<u8> mMesgAnimeTag;
    /* 0x91C */ be<s8> mBtpNum;
    /* 0x91D */ be<s8> mBckNum;
    /* 0x91E */ be<s8> mEvtState;
    /* 0x91F */ be<s8> mStt;
    /* 0x920 */ be<s8> mPrevStt;
    /* 0x921 */ be<s8> mLookMode;
    /* 0x922 */ be<s8> mTypeSet; /* decideType: > 0 when decided */
    /* 0x923 */ be<s8> mType;
    /* 0x924 */ be<s8> mActState;
    /* 0x925 */ be<s8> mAtrSet;
    /* 0x926 */ u8 _926[2];
};
WWHD_OFFSET(daNpc_Gk1_c, mArcName, 0x7F0);
WWHD_OFFSET(daNpc_Gk1_c, mBtpAnm, 0x7F4);
WWHD_OFFSET(daNpc_Gk1_c, mCurrProcFunc, 0x86C);
WWHD_OFFSET(daNpc_Gk1_c, mEyePos, 0x8A4);
WWHD_OFFSET(daNpc_Gk1_c, mPrevMorfFrame, 0x8E0);
WWHD_OFFSET(daNpc_Gk1_c, mbSetEyePos, 0x8F0);
WWHD_OFFSET(daNpc_Gk1_c, mLookAngleY, 0x90A);
WWHD_OFFSET(daNpc_Gk1_c, mAtrSet, 0x925);

WWHD_SIZE(daNpc_Gk1_c, 0x928);
struct daNpc_Gk1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;
        /* 0x02 */ be<s16> mMaxHeadY;
        /* 0x04 */ be<s16> mMinHeadX;
        /* 0x06 */ be<s16> mMinHeadY;
        /* 0x08 */ be<s16> mMaxBackBoneX;
        /* 0x0A */ be<s16> mMaxBackBoneY;
        /* 0x0C */ be<s16> mMinBackBoneX;
        /* 0x0E */ be<s16> mMinBackBoneY;
        /* 0x10 */ be<s16> mMaxTurnStep;
        /* 0x12 */ be<s16> mLookVel;
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<u8> mDebugDraw;
        /* 0x19 */ u8 _19[3];
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> mDebugIndex;
    /* 0x0C */ hio_prm_c mPrm;
};
WWHD_SIZE(daNpc_Gk1_HIO_c, 0x28);

#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
#include "gc_api/SSystem/c_lib.h"
s32 gkRandom(s32, s32);
s32 gkBtpInit(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32);
s8 gkNameIndex(JUTNameTab*, const char*);
bool gkLockonTruth(dAttention_c*);
fopAc_ac_c* gkLockonTarget(dAttention_c*, s32);
fopAc_ac_c* gkActionTarget(dAttention_c*, s32);
s8 gkRoomId(dBgS*, void*);
u8 gkPolyColor(dBgS*, void*);
u32 gkMaterialSound(dBgS*, void*);
void gkPlayerEyePos(cXyz*, f32);
void gkLookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
BOOL gkSearchByID(u32, be<u32>*);
u32 gkMessageStatus(msg_class*);
void gkNpcCtor(fopNpc_npc_l*);
void gkBtpCtor(void*);
void* gkDemoActor(void*, u8);
void* gkDemoBtp(void*, const char*);
void gkDemoData(daNpc_Gk1_c*, s32, mDoExt_McaMorf*, const char*, s32, s32, s32, s32);
s8 gkActionIndex(dEvent_manager_c*, s32, gptr<char>*, s32, BOOL, s32);
BOOL gkEventAdvance(void*, s32);
void gkCutEnd(void*, s32);
void gkCircleDtor(void*, s32);
void gkAcchDtor(dBgS_ObjAcch*, s32);
void gkActorDtor(fopAc_ac_c*, s32);
namespace gk1_cpp {
enum { STT_WAIT = 1, STT_TALK = 2 };
enum { LOOK_HOME = 0, LOOK_PLAYER = 1, LOOK_POSITION = 2, LOOK_ANGLE = 3, LOOK_PARTNER = 4 };
enum { fopMsgStts_MSG_DESTROYED_e = 0x13 };
enum : u32 { GK1_VTABLE = 0x1001A130, HIO_VTABLE = 0x10019FA8 };
struct CylinderSource {
    u8 _bytes[0x44];
};
WWHD_SIZE(CylinderSource, 0x44);
// HD: a NULL model-data pointer still addresses the animation slot at +0x38.
struct ModelDataView {
    u8 _00[0x38];
    gptr<J3DAnmTexPattern> mpAnmTexPattern;
};
WWHD_OFFSET(ModelDataView, mpAnmTexPattern, 0x38);
WWHD_SIZE(ModelDataView, 0x3C);
inline void removeTexPattern(J3DModelData* p) {
    gabi::store<u32>(gabi::ea(p) + offsetof(ModelDataView, mpAnmTexPattern), 0);
}
struct AcchVtables {
    u8 _00[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mActorVtable;
};
WWHD_OFFSET(AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(AcchVtables, mActorVtable, 0x20);
WWHD_SIZE(AcchVtables, 0x24);
inline AcchVtables* acch(dBgS_ObjAcch* p) {
    return reinterpret_cast<AcchVtables*>(p);
}
inline void* circle(dBgS_AcchCir* p) {
    return gabi::at<void>(gabi::ea(p) + 0x14);
}
struct AnimationVtable {
    u8 _00[0x14];
    be<u32> mGetFrameMax;
};
struct AnimationView {
    u8 _00[4];
    gptr<AnimationVtable> mpVtable;
};
WWHD_OFFSET(AnimationVtable, mGetFrameMax, 0x14);
WWHD_SIZE(AnimationVtable, 0x18);
WWHD_OFFSET(AnimationView, mpVtable, 4);
WWHD_SIZE(AnimationView, 8);
inline s32 getFrameMax(J3DAnmTexPattern* p) {
    auto* v = reinterpret_cast<AnimationView*>(p);
    return gabi::call_ptr<s32>(v->mpVtable->mGetFrameMax, p);
}
// HD: index arithmetic wraps in the guest's 32-bit address space.
template <class T> inline T& entry(T* table, s32 index) {
    return *gabi::at<T>(gabi::ea(table) + (u32)index * sizeof(T));
}
// HD: GHS member pointers carry a signed adjustment and an 8-byte virtual slot index.
inline void callAction(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = (s16)gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}

// HD: copying the member pointer stores its first word before loading the second.
inline void copyAction(ProcFunc_l* dst, const ProcFunc_l* src) {
    auto* d = reinterpret_cast<be<u32>*>(dst);
    auto* s = reinterpret_cast<const be<u32>*>(src);
    d[0] = (u32)s[0];
    d[1] = (u32)s[1];
}
struct MorfVtable {
    u8 _00[0xC];
    be<u32> mDeletingDestructor;
};
struct MorfView {
    gptr<MorfVtable> mpVtable;
};
WWHD_SIZE(MorfView, 4);
WWHD_OFFSET(MorfVtable, mDeletingDestructor, 0xC);
WWHD_SIZE(MorfVtable, 0x10);
inline void deleteMorf(mDoExt_McaMorf* p) {
    auto* v = reinterpret_cast<MorfView*>(p);
    gabi::call_ptr(v->mpVtable->mDeletingDestructor, p, 3);
}
inline be<u8>& archiveByte(char* buffer, s32 index) {
    return *gabi::at<be<u8>>(gabi::ea(buffer) + (u32)index);
}
inline void setJointCallBack(J3DModel* model, s8 joint, J3DJointCallBack callback) {
    model->getModelData()->getJointNodePointer((u16)(s16)joint)->setCallBack(callback);
}
// HD: publish each debug-color guard before the imported four-byte memcpy.
inline void initializeColor(be<u32>* guard, be<u32>* dst, const be<u32>* src) {
    if (*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}
} // namespace gk1_cpp
WWHD_OFFSET(daNpc_Gk1_c::anm_prm_c, morf, 4);
WWHD_OFFSET(daNpc_Gk1_c::anm_prm_c, loopMode, 0xC);
WWHD_SIZE(daNpc_Gk1_c::anm_prm_c, 0x10);
WWHD_OFFSET(daNpc_Gk1_HIO_c, mPrm, 0xC);
WWHD_SIZE(daNpc_Gk1_HIO_c::hio_prm_c, 0x1C);
