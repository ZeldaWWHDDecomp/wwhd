#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
#include "gc_api/f_op/f_op_msg.h"
struct Kf1ActionFunc {
    be<s16> d, i;
    be<u32> f;
};
WWHD_SIZE(Kf1ActionFunc, 8);
struct daNpc_Kf1_HIO_c {
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
        /* 0x12 */ be<s16> mLookVelMax;
        /* 0x14 */ be<f32> mAttPosOffsetY;
        /* 0x18 */ be<u8> mDebugDraw;
        /* 0x19 */ u8 _19[1];
        /* 0x1A */ be<s16> mWalkTurnScale;
        /* 0x1C */ be<s16> mWalkTurnStep;
        /* 0x1E */ u8 _1E[2];
        /* 0x20 */ be<f32> mWalkAnmRate;
        /* 0x24 */ be<f32> mWalkSpeed;
        /* 0x28 */ be<f32> mWalkAccel;
        /* 0x2C */ u8 _2C[4];
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> mEntryID;
    /* 0x0C */ hio_prm_c mPrm;
};
WWHD_SIZE(daNpc_Kf1_HIO_c, 0x3C);

struct daNpc_Kf1_c : fopNpc_npc_c {
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
    int btpResID(int);
    int bckResID(int);
    u32 setBtp(s8, u32);
    u32 init_texPttrnAnm(s8, u32);
    BOOL bodyCreateHeap();
    BOOL itemCreateHeap();
    BOOL CreateHeap();
    u8 decideType(int);
    void set_pthPoint(u32);
    BOOL set_action(Kf1ActionFunc*, void*);
    bool init_KF1_0();
    void play_btp_anm();
    void play_animation();
    void setAttention(u32);
    void setMtx(u32);
    bool createInit();
    cPhs_State _create();
    BOOL _delete();
    u8 srch_Tsubo();
    void checkOrder();
    u8 demo();
    s32 isEventEntry();
    fopAc_ac_c* searchByID(u32, be<s32>*);
    s32 chk_tsubo();
    void setAnm_anm(anm_prm_c*);
    void setAnm();
    void setStt(s8);
    void setAnm_NUM(int, int);
    void endEvent();
    void cut_init_ANGRY_START(int);
    void cut_init_BENSYOU_START(int);
    void cut_init_TSUBO_CNT(int);
    void cut_init_BENSYOU(int);
    void cut_init_GET_OUT(int);
    void cut_init_DSP_RUPEE_CNT(int);
    void cut_init_PLYER_TRN(int);
    void cut_init_START_AGE(int);
    void cut_init_PLYER_MOV(int);
    void cut_init_RUPEE_SET(int);
    void cut_init_TSUBO_ATN(int);
    void cut_init_TLK_MSG(int);
    void cut_init_CONTNUE_TLK(int);
    u32 cut_move_GET_OUT();
    u32 cut_move_RUPEE_CNT_END();
    u32 cut_move_START_AGE();
    void create_rupee(cXyz*, int);
    void ready_kutaniCamera(int, int);
    u32 cut_move_RUPEE_SET();
    u32 cut_move_TSUBO_ATN();
    u32 cut_move_TLK_MSG();
    void privateCut(int);
    void event_proc(int);
    void lookBack();
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    void setAnm_ATR();
    void chngAnmAtr(u8);
    void ctrlAnmAtr();
    void anmAtr(u16);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_KF1_0();
    u32 getMsg();
    u8 chk_talk();
    u8 chkAttention();
    u8 orderTsuboEvent();
    BOOL wait_1();
    BOOL walk_1();
    BOOL talk_1();
    BOOL wait_action1(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_nck_jnt_num;
    /* 0x7E7 */ u8 _7E7;
    /* 0x7E8 */ gptr<J3DModel> mpItemModel; /* "Kf" res 0, carried on the head */
    /* 0x7EC */ char mArcName[4];           /* "Kf" (decideType) */
    /* 0x7F0 */ u8 mBtpAnm[0x74];           /* mDoExt_btpAnm (HD 0x74; anm pointer at +0x10) */
    /* 0x864 */ be<u8> mBtpFrame;
    /* 0x865 */ u8 _865;
    /* 0x866 */ be<s16> mBlinkTimer;
    /* 0x868 */ Kf1ActionFunc mCurrProcFunc;
    /* 0x870 */ be<u32> mSearchActorId; /* -1 in srch_Tsubo */
    /* 0x874 */ be<u32> mLookActorId;   /* look mode 4 */
    /* 0x878 */ dNpc_PathRun_c mPathRun;
    /* 0x880 */ cXyz mHomePos;
    /* 0x88C */ csXyz mHomeAngle;
    /* 0x892 */ csXyz mAngle;
    /* 0x898 */ u8 _898[8];
    /* 0x8A0 */ cXyz mEyePos; /* head joint * (30, 30, 0) */
    /* 0x8AC */ cXyz mLookPos;
    /* 0x8B8 */ u8 _8B8[0xC];
    /* 0x8C4 */ cXyz mHeadPos;
    /* 0x8D0 */ u8 _8D0[0xC];
    /* 0x8DC */ be<f32> mPrevMorfFrame;
    /* 0x8E0 */ u8 _8E0[4];
    /* 0x8E4 */ be<s16> mActorAngleY;
    /* 0x8E6 */ be<s16> mJointHeadY;
    /* 0x8E8 */ be<s16> mJointBackboneY;
    /* 0x8EA */ u8 _8EA[2];
    /* 0x8EC */ be<s32> mbSetEyePos;
    /* 0x8F0 */ be<s16> mEventIDTbl[3]; /* "angry", "rupee_age", "bensyou" */
    /* 0x8F6 */ be<s16> mEventIndex;
    /* 0x8F8 */ u8 _8F8[4];
    /* 0x8FC */ be<s16> mCutTimer;
    /* 0x8FE */ be<s16> mCutCount;
    /* 0x900 */ be<s16> mTalkTimer;
    /* 0x902 */ be<s16> mWaitTimer;
    /* 0x904 */ be<s16> mWalkTimer;
    /* 0x906 */ u8 _906[4];
    /* 0x90A */ be<s16> mLookAngleY;
    /* 0x90C */ be<s8> mbMorfAnimStopped;
    /* 0x90D */ be<u8> mAtrLoopCnt;
    /* 0x90E */ be<u8> mItemNo;
    /* 0x90F */ be<u8> mSwitchNo;
    /* 0x910 */ be<u8> mbCompensationAgreed;
    /* 0x911 */ be<u8> mbRupeesGiven;
    /* 0x912 */ u8 _912;
    /* 0x913 */ be<u8> mbAcchFlag20;   /* mObjAcch flag 0x20 (ground hit) */
    /* 0x914 */ be<u8> mbAcchFlag1000; /* mObjAcch flag 0x1000 */
    /* 0x915 */ be<u8> mbForceEvent;
    /* 0x916 */ be<u8> mbResLoaded;
    /* 0x917 */ be<u8> mbWaitForDemo;
    /* 0x918 */ be<u8> mbNoShapeAngle;
    /* 0x919 */ be<u8> mbNoDraw;
    /* 0x91A */ be<u8> mbHomeSet;
    /* 0x91B */ be<u8> mbAttention;
    /* 0x91C */ be<u8> mbTalk;
    /* 0x91D */ be<u8> mbHeadOnly;
    /* 0x91E */ be<u8> mbDemo;
    /* 0x91F */ u8 _91F;
    /* 0x920 */ be<u32> mRupeeIds[3];
    /* 0x92C */ be<u32> mTsuboIds[8];
    /* 0x94C */ be<u32> mKutaniIds[3]; /* three pots picked at random */
    /* 0x958 */ be<u32> mCameraItemId;
    /* 0x95C */ be<s16> mTsuboNum;
    /* 0x95E */ be<s16> mBrokenNum;
    /* 0x960 */ be<u16> mRupee;
    /* 0x962 */ be<s8> mActionIndex;
    /* 0x963 */ be<u8> mAnmAtr;
    /* 0x964 */ be<u8> mMesgAnimeTag;
    /* 0x965 */ be<s8> mBtpNum;
    /* 0x966 */ be<s8> mBckNum;
    /* 0x967 */ be<s8> mEvtState;
    /* 0x968 */ be<s8> mStt;
    /* 0x969 */ be<s8> mPrevStt;
    /* 0x96A */ be<s8> mLookMode;
    /* 0x96B */ be<s8> mHeapType;
    /* 0x96C */ be<s8> mType;
    /* 0x96D */ be<s8> mActState;
    /* 0x96E */ be<s8> mAtrSet;
    /* 0x96F */ u8 _96F;
};
WWHD_OFFSET(daNpc_Kf1_c, mBtpAnm, 0x7F0);
WWHD_OFFSET(daNpc_Kf1_c, mCurrProcFunc, 0x868);
WWHD_OFFSET(daNpc_Kf1_c, mPathRun, 0x878);
WWHD_OFFSET(daNpc_Kf1_c, mEyePos, 0x8A0);
WWHD_OFFSET(daNpc_Kf1_c, mPrevMorfFrame, 0x8DC);
WWHD_OFFSET(daNpc_Kf1_c, mEventIDTbl, 0x8F0);
WWHD_OFFSET(daNpc_Kf1_c, mbMorfAnimStopped, 0x90C);
WWHD_OFFSET(daNpc_Kf1_c, mRupeeIds, 0x920);
WWHD_OFFSET(daNpc_Kf1_c, mTsuboNum, 0x95C);
WWHD_OFFSET(daNpc_Kf1_c, mAtrSet, 0x96E);

// HD: virtual frame count, animation-removal and destructor layouts.
struct Kf1AnmVtable {
    u8 _0[0x14];
    be<u32> mGetFrameMax;
};
struct Kf1Anm {
    u8 _0[4];
    gptr<Kf1AnmVtable> mpVtable;
};
struct Kf1ModelAnimation {
    u8 _0[0x38];
    be<u32> mpBtpAnimation;
};
struct Kf1AcchVtables {
    u8 _0[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mPolyVtable;
};
WWHD_OFFSET(Kf1AnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Kf1Anm, mpVtable, 4);
WWHD_OFFSET(Kf1ModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(Kf1AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(Kf1AcchVtables, mPolyVtable, 0x20);
template <class T, class U> inline T* Kf1_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 Kf1_frameMax(J3DAnmTexPattern* p) {
    auto* a = Kf1_view<Kf1Anm>(p);
    auto* t = a->mpVtable.get();
    return gabi::call_ptr<s32>(t->mGetFrameMax, p);
}
inline void Kf1_loadAction(Kf1ActionFunc* dst, Kf1ActionFunc* src) {
    auto* d = Kf1_view<be<u32>>(dst);
    auto* s = Kf1_view<be<u32>>(src);
    d[0] = s[0];
    d[1] = s[1];
}
struct Kf1VirtualAction {
    u8 _0[6];
    be<s16> mVtableOffset;
};
inline void Kf1_callAction(void* self, Kf1ActionFunc* action, void* arg) {
    s16 index = action->i;
    u32 object = gabi::ea(self) + (s32)(s16)action->d;
    if (index < 0) {
        gabi::call_ptr<BOOL>(action->f, object, arg);
    } else {
        s16 offset = Kf1_view<Kf1VirtualAction>(action)->mVtableOffset;
        auto* vtable = gabi::at<be<u32>>(object + offset);
        auto* entry = gabi::at<be<u32>>((u32)*vtable + index * 8 + 4);
        gabi::call_ptr<BOOL>(*entry, object, arg);
    }
}
inline void Kf1_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if ((u32)*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

struct Kf1Btp {
    u8 _0[0x10];
    gptr<J3DAnmTexPattern> mpAnm;
};
struct Kf1MorfVtable {
    u8 _0[0xC];
    be<u32> mDestructor;
};
struct Kf1Morf {
    gptr<Kf1MorfVtable> mpVtable;
};

WWHD_OFFSET(Kf1Btp, mpAnm, 0x10);
WWHD_SIZE(Kf1Btp, 0x14);
WWHD_SIZE(Kf1Morf, 4);
WWHD_SIZE(Kf1MorfVtable, 0x10);

struct Kf1Message {
    u8 _0[0x921];
    be<u8> mbWait;
    u8 _922[0x26];
    be<u32> mSelectedAnswer;
};

WWHD_SIZE(daNpc_Kf1_c, 0x970);

struct Kf1Path {
    u8 _0[5];
    be<u8> mClosed;
};
WWHD_OFFSET(Kf1Path, mClosed, 5);
WWHD_SIZE(Kf1Path, 6);
using Kf1GuestWord = be<u32>;
using Kf1GuestInt = be<s32>;
inline const char* Kf1_staffName(dNpc_EventCut_c* p) {
    return gabi::at<const char>(p->mpEvtStaffName);
}
inline fopAc_ac_c* Kf1_actorAt(u32 id) { return gabi::at<fopAc_ac_c>(id); }
inline void Kf1_deleteMorf(mDoExt_McaMorf* p) {
    auto* table = Kf1_view<Kf1Morf>(p)->mpVtable.get();
    gabi::call_ptr(table->mDestructor, p, 3);
}
s16 Kf1_calcTimer(be<s16>*);
s32 Kf1_random(s32, s32);
s32 Kf1_btpInit(void*, J3DModelData*, void*, s32, s32, f32, s32, s32, u32, s32);
bool Kf1_lockonTruth(dAttention_c*);
fopAc_ac_c* Kf1_lockonTarget(dAttention_c*, s32);
fopAc_ac_c* Kf1_actionTarget(dAttention_c*, s32);
s8 Kf1_roomId(dBgS*, cBgS_PolyInfo*);
u8 Kf1_polyColour(dBgS*, cBgS_PolyInfo*);
u32 Kf1_materialSound(dBgS*, cBgS_PolyInfo*);
void Kf1_playerEyePos(cXyz*, f32);
void Kf1_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
BOOL Kf1_searchByID(fpc_ProcID, be<u32>*);
void Kf1_pathPoint(dNpc_PathRun_c*, cXyz*, u32);
void Kf1_npcCtor(daNpc_Kf1_c*);
void Kf1_btpCtor(void*);
void* Kf1_demoActor(u32, u8);
void* Kf1_demoBtp(void*, const char*);
void Kf1_demoData(daNpc_Kf1_c*, u32, mDoExt_McaMorf*, const char*, s32, u32, u32, s32);
void Kf1_circleDestroy(void*, s32);
void Kf1_acchDestroy(dBgS_Acch*, s32);
void Kf1_actorDestroy(daNpc_Kf1_c*, s32);
u32 Kf1_messageStatus(void*);
u32 Kf1_prmAbstract(fopAc_ac_c*, s32, s32);
void* Kf1_searchActors(u32, void*);

// HD: player event movement fields are beyond the common actor layout.
struct Kf1PlayerDemo {
    u8 _0[0x420];
    be<s16> mMode, mAngle;
    u8 _424[4];
    be<u32> mMoveMode;
    u8 _42c[4];
    be<u32> mOriginalDemo;
};
WWHD_OFFSET(Kf1PlayerDemo, mMode, 0x420);
WWHD_OFFSET(Kf1PlayerDemo, mAngle, 0x422);
WWHD_OFFSET(Kf1PlayerDemo, mMoveMode, 0x428);
WWHD_OFFSET(Kf1PlayerDemo, mOriginalDemo, 0x430);
WWHD_SIZE(Kf1PlayerDemo, 0x434);
struct Kf1AcchPositions {
    u8 _0[0x2C];
    gptr<be<u32>> mpPosition, mpOldPosition;
};
WWHD_OFFSET(Kf1AcchPositions, mpPosition, 0x2C);
WWHD_OFFSET(Kf1AcchPositions, mpOldPosition, 0x30);
WWHD_SIZE(Kf1AcchPositions, 0x34);
struct Kf1RupeeFloatSave {
    be<u32> mDoubleHigh, mDoubleLow;
    be<f32> mPairedSingle;
    u8 _c[4];
};
struct Kf1RupeeSelectionFrame {
    be<u32> mBackChain;
    u8 _4[4];
    be<u32> mSearchKey;
    u8 _c[4];
    be<s32> mIndex;
    u8 _14[4];
    be<u32> mUsed[2];
    be<u32> mRegisters[12];
    Kf1RupeeFloatSave mFloatRegisters[2];
};
WWHD_SIZE(Kf1RupeeFloatSave, 0x10);
WWHD_OFFSET(Kf1RupeeSelectionFrame, mSearchKey, 8);
WWHD_OFFSET(Kf1RupeeSelectionFrame, mIndex, 0x10);
WWHD_OFFSET(Kf1RupeeSelectionFrame, mUsed, 0x18);
WWHD_OFFSET(Kf1RupeeSelectionFrame, mRegisters, 0x20);
WWHD_OFFSET(Kf1RupeeSelectionFrame, mFloatRegisters, 0x50);
WWHD_SIZE(Kf1RupeeSelectionFrame, 0x70);
struct Kf1SavedLR {
    u8 _0[0x74];
    be<u32> mLinkRegister;
};
WWHD_OFFSET(Kf1SavedLR, mLinkRegister, 0x74);
WWHD_SIZE(Kf1SavedLR, 0x78);
inline void Kf1_saveRupeeFrame(Kf1RupeeSelectionFrame* frame) {
    frame->mBackChain = gabi::ea(frame) + 0x70;
    Kf1_view<Kf1SavedLR>(frame)->mLinkRegister = gabi::cpu->lr;
    for (int r = 20; r <= 31; r++)
        frame->mRegisters[r - 20] = gabi::cpu->r[r];
    for (int i = 0; i < 2; i++) {
        f64 ps0 = gabi::cpu->f[30 + i].ps0, ps1 = gabi::cpu->f[30 + i].ps1;
        u64 bits;
        memcpy(&bits, &ps0, 8);
        frame->mFloatRegisters[i].mDoubleHigh = (u32)(bits >> 32);
        frame->mFloatRegisters[i].mDoubleLow = (u32)bits;
        frame->mFloatRegisters[i].mPairedSingle = (f32)ps1;
    }
}
// HD: the random pot index deliberately has no bounds check. Guest addressing keeps the saved frame observable.
inline be<u8>& Kf1_usedPot(Kf1RupeeSelectionFrame* frame, s32 index) {
    return *gabi::at<be<u8>>(gabi::ea(frame) + 0x18 + index);
}
inline f32 Kf1_rupeeOffset(be<f32>* offsets, s32 index) {
    return *gabi::at<be<f32>>(gabi::ea(offsets) + index * 4);
}

// HD: the counter flags are a halfword in dComIfG_play_c's HD tail.
struct Kf1CounterFlags {
    u8 _0[0x52A6];
    be<u16> mFlags;
};
WWHD_OFFSET(Kf1CounterFlags, mFlags, 0x52A6);
WWHD_SIZE(Kf1CounterFlags, 0x52A8);
inline void Kf1_onRupeeCounter() {
    auto* p = Kf1_view<Kf1CounterFlags>(dComIfGp_get());
    p->mFlags = (u16)(p->mFlags | 0x80);
}
inline void Kf1_offRupeeCounter() {
    auto* p = Kf1_view<Kf1CounterFlags>(dComIfGp_get());
    p->mFlags = (u16)(p->mFlags & 0xFF7F);
}
void Kf1_XYZrotM(Mtx34*, s16, s16, s16);
void Kf1_nextStage(const char*, s16, s8, s8, f32, u32, s32, s8);
fopAc_ac_c* Kf1_createRupee(cXyz*, s32, s32, csXyz*, cXyz*, f32, f32, f32, s32);

struct Kf1RupeeCreationFrame {
    csXyz angle;
    u8 _6[2];
    be<f32> offs[3];
};
WWHD_OFFSET(Kf1RupeeCreationFrame, offs, 8);
WWHD_SIZE(Kf1RupeeCreationFrame, 0x14);

// HD: this cut compares the display count snapshot, not mMessageRupee.
struct Kf1SavedRupee {
    u8 _0[0x5BAE];
    be<u16> mSavedRupee;
};
WWHD_OFFSET(Kf1SavedRupee, mSavedRupee, 0x5BAE);
WWHD_SIZE(Kf1SavedRupee, 0x5BB0);
inline Kf1SavedRupee* Kf1_savedRupeeAt(u32 address) { return gabi::at<Kf1SavedRupee>(address); }

WWHD_OFFSET(daNpc_Kf1_c::anm_prm_c, bckNum, 0x00);
WWHD_OFFSET(daNpc_Kf1_c::anm_prm_c, btpNum, 0x01);
WWHD_OFFSET(daNpc_Kf1_c::anm_prm_c, morf, 0x04);
WWHD_OFFSET(daNpc_Kf1_c::anm_prm_c, speed, 0x08);
WWHD_OFFSET(daNpc_Kf1_c::anm_prm_c, loopMode, 0x0C);
WWHD_OFFSET(daNpc_Kf1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Kf1_c, m_hed_jnt_num, 0x7E4);
WWHD_OFFSET(daNpc_Kf1_c, m_bbone_jnt_num, 0x7E5);
WWHD_OFFSET(daNpc_Kf1_c, m_nck_jnt_num, 0x7E6);
WWHD_OFFSET(daNpc_Kf1_c, mpItemModel, 0x7E8);
WWHD_OFFSET(daNpc_Kf1_c, mArcName, 0x7EC);
WWHD_OFFSET(daNpc_Kf1_c, mBtpFrame, 0x864);
WWHD_OFFSET(daNpc_Kf1_c, mBlinkTimer, 0x866);
WWHD_OFFSET(daNpc_Kf1_c, mSearchActorId, 0x870);
WWHD_OFFSET(daNpc_Kf1_c, mLookActorId, 0x874);
WWHD_OFFSET(daNpc_Kf1_c, mHomePos, 0x880);
WWHD_OFFSET(daNpc_Kf1_c, mHomeAngle, 0x88C);
WWHD_OFFSET(daNpc_Kf1_c, mAngle, 0x892);
WWHD_OFFSET(daNpc_Kf1_c, mLookPos, 0x8AC);
WWHD_OFFSET(daNpc_Kf1_c, mHeadPos, 0x8C4);
WWHD_OFFSET(daNpc_Kf1_c, mActorAngleY, 0x8E4);
WWHD_OFFSET(daNpc_Kf1_c, mJointHeadY, 0x8E6);
WWHD_OFFSET(daNpc_Kf1_c, mJointBackboneY, 0x8E8);
WWHD_OFFSET(daNpc_Kf1_c, mbSetEyePos, 0x8EC);
WWHD_OFFSET(daNpc_Kf1_c, mEventIndex, 0x8F6);
WWHD_OFFSET(daNpc_Kf1_c, mCutTimer, 0x8FC);
WWHD_OFFSET(daNpc_Kf1_c, mCutCount, 0x8FE);
WWHD_OFFSET(daNpc_Kf1_c, mTalkTimer, 0x900);
WWHD_OFFSET(daNpc_Kf1_c, mWaitTimer, 0x902);
WWHD_OFFSET(daNpc_Kf1_c, mWalkTimer, 0x904);
WWHD_OFFSET(daNpc_Kf1_c, mLookAngleY, 0x90A);
WWHD_OFFSET(daNpc_Kf1_c, mAtrLoopCnt, 0x90D);
WWHD_OFFSET(daNpc_Kf1_c, mItemNo, 0x90E);
WWHD_OFFSET(daNpc_Kf1_c, mSwitchNo, 0x90F);
WWHD_OFFSET(daNpc_Kf1_c, mbCompensationAgreed, 0x910);
WWHD_OFFSET(daNpc_Kf1_c, mbRupeesGiven, 0x911);
WWHD_OFFSET(daNpc_Kf1_c, mbAcchFlag20, 0x913);
WWHD_OFFSET(daNpc_Kf1_c, mbAcchFlag1000, 0x914);
WWHD_OFFSET(daNpc_Kf1_c, mbForceEvent, 0x915);
WWHD_OFFSET(daNpc_Kf1_c, mbResLoaded, 0x916);
WWHD_OFFSET(daNpc_Kf1_c, mbWaitForDemo, 0x917);
WWHD_OFFSET(daNpc_Kf1_c, mbNoShapeAngle, 0x918);
WWHD_OFFSET(daNpc_Kf1_c, mbNoDraw, 0x919);
WWHD_OFFSET(daNpc_Kf1_c, mbHomeSet, 0x91A);
WWHD_OFFSET(daNpc_Kf1_c, mbAttention, 0x91B);
WWHD_OFFSET(daNpc_Kf1_c, mbTalk, 0x91C);
WWHD_OFFSET(daNpc_Kf1_c, mbHeadOnly, 0x91D);
WWHD_OFFSET(daNpc_Kf1_c, mbDemo, 0x91E);
WWHD_OFFSET(daNpc_Kf1_c, mTsuboIds, 0x92C);
WWHD_OFFSET(daNpc_Kf1_c, mKutaniIds, 0x94C);
WWHD_OFFSET(daNpc_Kf1_c, mCameraItemId, 0x958);
WWHD_OFFSET(daNpc_Kf1_c, mBrokenNum, 0x95E);
WWHD_OFFSET(daNpc_Kf1_c, mRupee, 0x960);
WWHD_OFFSET(daNpc_Kf1_c, mActionIndex, 0x962);
WWHD_OFFSET(daNpc_Kf1_c, mAnmAtr, 0x963);
WWHD_OFFSET(daNpc_Kf1_c, mMesgAnimeTag, 0x964);
WWHD_OFFSET(daNpc_Kf1_c, mBtpNum, 0x965);
WWHD_OFFSET(daNpc_Kf1_c, mBckNum, 0x966);
WWHD_OFFSET(daNpc_Kf1_c, mEvtState, 0x967);
WWHD_OFFSET(daNpc_Kf1_c, mStt, 0x968);
WWHD_OFFSET(daNpc_Kf1_c, mPrevStt, 0x969);
WWHD_OFFSET(daNpc_Kf1_c, mLookMode, 0x96A);
WWHD_OFFSET(daNpc_Kf1_c, mHeapType, 0x96B);
WWHD_OFFSET(daNpc_Kf1_c, mType, 0x96C);
WWHD_OFFSET(daNpc_Kf1_c, mActState, 0x96D);
WWHD_SIZE(daNpc_Kf1_c::anm_prm_c, 0x10);
WWHD_SIZE(Kf1AnmVtable, 0x18);
WWHD_SIZE(Kf1Anm, 8);
WWHD_SIZE(Kf1ModelAnimation, 0x3C);
WWHD_SIZE(Kf1AcchVtables, 0x24);
WWHD_SIZE(Kf1VirtualAction, 8);
WWHD_OFFSET(Kf1Message, mSelectedAnswer, 0x948);
WWHD_SIZE(Kf1Message, 0x94C);
