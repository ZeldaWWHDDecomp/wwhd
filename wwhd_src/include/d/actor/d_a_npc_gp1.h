/* daNpc_Gp1_c (Maggie's father, rich), WWHD layout. 
 *
 * The GameCube decompilation has no layout (all functions are "Nonmatching" stubs); the members
 * below are measured from the WWHD code of d_a_npc_gp1 and named after their use. The class
 * derives from fopNpc_npc_c (0x7DC, see d_a_npc_ba1.h). Size 0x988 (profile g_profile_NPC_GP1,
 * size word at 0x101BE6D0). */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
struct Gp1ActionFunc {
    be<s16> d, i;
    be<u32> f;
};
WWHD_SIZE(Gp1ActionFunc, 8);

struct daNpc_Gp1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
    };

    void nodeGp1Control(J3DNode*, J3DModel*);
    J3DModelData* create_Anm();
    int btpNum_toResID(int);
    u32 setBtp(u32, int);
    u32 iniTexPttrnAnm(u32);
    BOOL CreateHeap();
    BOOL charDecide(int);
    BOOL set_action(Gp1ActionFunc*, void*);
    bool init_GP1_0();
    void plyTexPttrnAnm();
    u32 setAnm_tex(s8);
    int anmNum_toResID(int);
    BOOL setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, int);
    void ctrl_WAITanm();
    void setAttention(u32);
    void setMtx(u32);
    bool createInit();
    cPhs_State _create();
    BOOL _delete();
    fopAc_ac_c* searchByID(u32);
    u8 partner_srch_sub(u32);
    void partner_srch();
    void checkOrder();
    u8 demo();
    s32 isEventEntry();
    BOOL setAnm();
    void setStt(s8);
    void endEvent();
    void eInit_INI_KAERE_KAERE_();
    void eInit_END_KAERE_KAERE_();
    void event_actionInit(int);
    BOOL event_action();
    void privateCut(int);
    void lookBack();
    void event_proc(int);
    void gp_clcMovSpd();
    int gp_movPass();
    void gp_nMove();
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    void setAnm_ATR(int);
    void chg_anmAtr(u8);
    BOOL create_rupee();
    void control_anmAtr();
    void anmAtr(u16);
    u8 chk_talk();
    u8 chk_partsNotMove();
    u8 chk_forceTlkArea();
    u16 next_msgStatus(be<u32>*);
    u32 getMsg_GP1_0();
    u32 getMsg();
    u8 chkAttention();
    BOOL wait_1();
    u8 talk_1();
    BOOL walk_1();
    BOOL hair_1();
    BOOL wait_2();
    BOOL wait_action1(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ be<s8> m_hnd_L_jnt_num; /* rupees are thrown from the left hand */
    /* 0x7E7 */ u8 _7E7;
    /* 0x7E8 */ gptr<J3DAnmTexPattern> m_hed_tex_pttrn;
    /* 0x7EC */ u8 mBtpAnm[0x74]; /* mDoExt_btpAnm (HD 0x74) */
    /* 0x860 */ be<u8> mBtpFrame;
    /* 0x861 */ u8 _861;
    /* 0x862 */ be<s16> mBlinkTimer;
    /* 0x864 */ Gp1ActionFunc mCurrProcFunc;
    /* 0x86C */ dNpc_PathRun_c mPathRun;
    /* 0x874 */ dNpc_EventCut_c mEventCut; /* hides fopNpc_npc_c::mEventCut */
    /* 0x8E0 */ be<u32> mPartnerID;        /* the Bm (fpcNm 0x149) found by partner_srch */
    /* 0x8E4 */ cXyz mHomePos;
    /* 0x8F0 */ csXyz mHomeAngle;
    /* 0x8F6 */ csXyz mAngle;
    /* 0x8FC */ cXyz mEyePos; /* head joint * (24, -24, 0) */
    /* 0x908 */ cXyz mLookPos;
    /* 0x914 */ cXyz mPathPos; /* current path point */
    /* 0x920 */ u8 _920[0x92C - 0x920];
    /* 0x92C */ be<f32> mPrevMorfFrame;
    /* 0x930 */ be<f32> mEyeOffsetY;
    /* 0x934 */ u8 _934[4];
    /* 0x938 */ be<f32> mWalkSpd;
    /* 0x93C */ be<f32> mWalkAccel;
    /* 0x940 */ be<f32> mPassDist;
    /* 0x944 */ be<s16> mJointHeadY;
    /* 0x946 */ be<s16> mJointBackboneY;
    /* 0x948 */ be<s16> mActorAngleY;
    /* 0x94A */ be<s16> mEventIDTbl[3];
    /* 0x950 */ be<s16> mEventIndex;
    /* 0x952 */ u8 _952[2];
    /* 0x954 */ be<s16> mTalkTimer;
    /* 0x956 */ be<s16> mWaitTimer2;
    /* 0x958 */ be<s16> mHairTimer;
    /* 0x95A */ be<s16> mLookVel;
    /* 0x95C */ be<s16> mLookAngleY;
    /* 0x95E */ be<s16> mBuyNum;   /* skull necklaces handed over */
    /* 0x960 */ be<s16> mRupeeNum; /* rupees to throw */
    /* 0x962 */ be<s16> mWaitTimer;
    /* 0x964 */ be<s8> mbMorfAnimStopped;
    /* 0x965 */ be<s8> mLoopCnt;
    /* 0x966 */ be<s8> mbAfterEvent;
    /* 0x967 */ be<u8> mItemNo;
    /* 0x968 */ be<u8> mbHidden;
    /* 0x969 */ be<u8> mbNoShapeAngle;
    /* 0x96A */ be<u8> mbNoMove;
    /* 0x96B */ be<u8> mbNoDraw;
    /* 0x96C */ be<u8> mbPathEnd;
    /* 0x96D */ be<u8> mbPresent;
    /* 0x96E */ be<u8> mbHomeSet;
    /* 0x96F */ u8 _96F;
    /* 0x970 */ be<s32> mbSetEyePos;
    /* 0x974 */ be<u8> mbAttention;
    /* 0x975 */ be<u8> mbTalk;
    /* 0x976 */ be<u8> mbHeadOnly;
    /* 0x977 */ be<u8> mbDemo;
    /* 0x978 */ be<s8> mMoveState;
    /* 0x979 */ be<s8> mActionIndex;
    /* 0x97A */ be<s8> mActNo;
    /* 0x97B */ be<u8> mAnmAtr;
    /* 0x97C */ be<u8> mMesgAnimeTag;
    /* 0x97D */ be<s8> mBtpNum;
    /* 0x97E */ be<s8> mBckNum;
    /* 0x97F */ be<s8> mEvtState;
    /* 0x980 */ be<s8> mStt;
    /* 0x981 */ be<s8> mPrevStt;
    /* 0x982 */ be<s8> mLookMode;
    /* 0x983 */ be<s8> mSubtype;
    /* 0x984 */ be<s8> mType;
    /* 0x985 */ be<s8> mActState;
    /* 0x986 */ be<s8> mAtrSet;
    /* 0x987 */ u8 _987;
};
WWHD_OFFSET(daNpc_Gp1_c, mBtpAnm, 0x7EC);
WWHD_OFFSET(daNpc_Gp1_c, mCurrProcFunc, 0x864);
WWHD_OFFSET(daNpc_Gp1_c, mEventCut, 0x874);
WWHD_OFFSET(daNpc_Gp1_c, mPartnerID, 0x8E0);
WWHD_OFFSET(daNpc_Gp1_c, mEyePos, 0x8FC);
WWHD_OFFSET(daNpc_Gp1_c, mPrevMorfFrame, 0x92C);
WWHD_OFFSET(daNpc_Gp1_c, mEventIDTbl, 0x94A);
WWHD_OFFSET(daNpc_Gp1_c, mbSetEyePos, 0x970);
WWHD_OFFSET(daNpc_Gp1_c, mAtrSet, 0x986);
WWHD_SIZE(daNpc_Gp1_c, 0x988);

// HD: measured HIO layout; vtable first and byte debug switch.
struct daNpc_Gp1_HIO_c {
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
        /* 0x19 */ u8 _19[3];
        /* 0x1C */ be<f32> mForceTlkDist;
        /* 0x20 */ be<s16> mTurnScale;
        /* 0x22 */ be<s16> mTurnMaxStep;
        /* 0x24 */ be<f32> mAnmSpdRate;
        /* 0x28 */ be<f32> mWalkSpd;
        /* 0x2C */ be<f32> mWalkAccel;
        /* 0x30 */ be<f32> mPassDist;
    };
    /* 0x00 */ be<u32> __vtbl; /* HD: vtable first */
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> mEntryID;
    /* 0x0C */ hio_prm_c mPrm;
};
WWHD_SIZE(daNpc_Gp1_HIO_c, 0x40);
struct Gp1RupeeFrame {
    /* 0x00 */ cXyz pos;
    /* 0x0C */ be<f32>
        a_angle_tbl[3]; // HD: {-30, 0, 30}; signed lookup can reach the preceding position.
};
WWHD_OFFSET(Gp1RupeeFrame, pos, 0);
WWHD_OFFSET(Gp1RupeeFrame, a_angle_tbl, 0xC);
WWHD_SIZE(Gp1RupeeFrame, 0x18);
// HD: virtual frame count, animation-removal and destructor layouts.
struct Gp1AnmVtable {
    u8 _0[0x14];
    be<u32> mGetFrameMax;
};
struct Gp1Anm {
    u8 _0[4];
    gptr<Gp1AnmVtable> mpVtable;
};
struct Gp1ModelAnimation {
    u8 _0[0x38];
    be<u32> mpBtpAnimation;
};
struct Gp1AcchVtables {
    u8 _0[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mPolyVtable;
};
WWHD_OFFSET(Gp1AnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Gp1Anm, mpVtable, 4);
WWHD_OFFSET(Gp1ModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(Gp1AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(Gp1AcchVtables, mPolyVtable, 0x20);
template <class T, class U> inline T* Gp1_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 Gp1_frameMax(J3DAnmTexPattern* p) {
    auto* a = Gp1_view<Gp1Anm>(p);
    auto* t = a->mpVtable.get();
    return gabi::call_ptr<s32>(t->mGetFrameMax, p);
}
inline void Gp1_loadAction(Gp1ActionFunc* dst, Gp1ActionFunc* src) {
    auto* d = Gp1_view<be<u32>>(dst);
    auto* s = Gp1_view<be<u32>>(src);
    d[0] = s[0];
    d[1] = s[1];
}
struct Gp1VirtualAction {
    u8 _0[6];
    be<s16> mVtableOffset;
};
inline void Gp1_callAction(void* self, Gp1ActionFunc* action, void* arg) {
    s16 index = action->i;
    u32 object = gabi::ea(self) + (s32)(s16)action->d;
    if (index < 0) {
        gabi::call_ptr<BOOL>(action->f, object, arg);
    } else {
        s16 offset = Gp1_view<Gp1VirtualAction>(action)->mVtableOffset;
        auto* vtable = gabi::at<be<u32>>(object + offset);
        auto* entry = gabi::at<be<u32>>((u32)*vtable + index * 8 + 4);
        gabi::call_ptr<BOOL>(*entry, object, arg);
    }
}
inline void Gp1_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if ((u32)*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

s8 Gp1_roomId(dBgS*, cBgS_PolyInfo*);
u8 Gp1_polyColour(dBgS*, cBgS_PolyInfo*);

inline const char* Gp1_staffName(dNpc_EventCut_c* cut) {
    return gabi::at<const char>(cut->mpEvtStaffName);
}
inline void Gp1_copyWords(cXyz* dst, const cXyz* src) {
    auto* s = Gp1_view<be<u32>>(src);
    auto* d = Gp1_view<be<u32>>(dst);
    u32 x = s[0], y = s[1], z = s[2];
    d[0] = x;
    d[1] = y;
    d[2] = z;
}
inline be<u8>* Gp1_bytes(void* p) { return gabi::at<be<u8>>(gabi::ea(p)); }
WWHD_SIZE(Gp1AnmVtable, 0x18);
WWHD_SIZE(Gp1Anm, 8);
WWHD_SIZE(Gp1ModelAnimation, 0x3C);
WWHD_SIZE(Gp1AcchVtables, 0x24);
WWHD_OFFSET(Gp1VirtualAction, mVtableOffset, 6);
WWHD_SIZE(Gp1VirtualAction, 8);
WWHD_OFFSET(Gp1ActionFunc, d, 0);
WWHD_OFFSET(Gp1ActionFunc, i, 2);
WWHD_OFFSET(Gp1ActionFunc, f, 4);

struct Gp1PathFlags {
    u8 _0[5];
    be<u8> mClosed;
};
WWHD_OFFSET(Gp1PathFlags, mClosed, 5);
WWHD_SIZE(Gp1PathFlags, 6);
inline fopAc_ac_c* Gp1_actorFromResult(u32 a) { return gabi::at<fopAc_ac_c>(a); }
inline f32 Gp1_rupeeAngle(Gp1RupeeFrame* frame, s32 index) {
    return *gabi::at<be<f32>>(gabi::ea(&frame->a_angle_tbl[0]) + index * 4);
}
s8 Gp1_roomId(dBgS*, cBgS_PolyInfo*);
u8 Gp1_polyColour(dBgS*, cBgS_PolyInfo*);
s16 Gp1_calcTimer(be<s16>*);
s32 Gp1_btpInit(void*, J3DModelData*, J3DAnmTexPattern*, s32, s32, f32, s32, s32, u32, s32);
bool Gp1_lockonTruth(dAttention_c*);
fopAc_ac_c* Gp1_lockonTarget(dAttention_c*, s32);
fopAc_ac_c* Gp1_actionTarget(dAttention_c*, s32);
void Gp1_playerEyePos(cXyz*, f32);
u32 Gp1_messageStatus(void*);
void Gp1_lookAtTarget(dNpc_JntCtrl_c*, be<s16>*, cXyz*, cXyz*, s16, s16, u8);
void* Gp1_search(u32, void*);
BOOL Gp1_searchById(u32, be<u32>*);
u32 Gp1_eventPid(dEvt_control_c*, fopAc_ac_c*);
fopAc_ac_c* Gp1_createItem(cXyz*, s32, s32, csXyz*, cXyz*, f32, f32, f32, s32, u32);
void Gp1_npcCtor(daNpc_Gp1_c*);
void Gp1_btpCtor(void*);
void Gp1_eventCutCtor(dNpc_EventCut_c*);
void* Gp1_demoGetActor(u32, u8);
J3DAnmTexPattern* Gp1_demoBtp(void*, const char*);
void Gp1_demoData(daNpc_Gp1_c*, u32, mDoExt_McaMorf*, const char*, s32, u32, u32, s32);
void Gp1_circleDestroy(void*, s32);
void Gp1_acchDestroy(dBgS_Acch*, s32);
void Gp1_actorDestroy(daNpc_Gp1_c*, s32);

WWHD_SIZE(daNpc_Gp1_c::anm_prm_c, 0x10);
WWHD_OFFSET(daNpc_Gp1_c::anm_prm_c, bckNum, 0);
WWHD_OFFSET(daNpc_Gp1_c::anm_prm_c, btpNum, 1);
WWHD_OFFSET(daNpc_Gp1_c::anm_prm_c, morf, 4);
WWHD_OFFSET(daNpc_Gp1_c::anm_prm_c, speed, 8);
WWHD_OFFSET(daNpc_Gp1_c::anm_prm_c, loopMode, 0xC);
WWHD_OFFSET(daNpc_Gp1_HIO_c, mPrm, 0xC);
WWHD_SIZE(daNpc_Gp1_HIO_c::hio_prm_c, 0x34);
WWHD_OFFSET(daNpc_Gp1_HIO_c::hio_prm_c, mForceTlkDist, 0x1C);
WWHD_OFFSET(daNpc_Gp1_HIO_c::hio_prm_c, mWalkSpd, 0x28);
WWHD_OFFSET(daNpc_Gp1_HIO_c::hio_prm_c, mPassDist, 0x30);

WWHD_OFFSET(daNpc_Gp1_c, mbHidden, 0x968);
WWHD_OFFSET(daNpc_Gp1_c, mSubtype, 0x983);
WWHD_OFFSET(daNpc_Gp1_HIO_c, mNo, 4);
WWHD_OFFSET(daNpc_Gp1_HIO_c, mEntryID, 8);
