/* daNpc_Hi1_c (the King of Hyrule), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the verified functions of d_a_npc_hi1: fopNpc_npc_c is 0x7DC
 * (see d_a_npc_ba1.h). mShadowId (GameCube 0x6D4) is gone (HD shadows), so mBtkAnm starts at
 * +0x114; mDoExt_btkAnm and mDoExt_btpAnm grew from 0x14 to 0x74 each (constructors 025E7C6C /
 * 025E7820), so mBtpAnm is +0x174 and mBtpAnmFrame/mTimer1/mCurrActionFunc +0x1D4; the pointer
 * to member function is 8 bytes (GHS) instead of 12, so everything after it is +0x1D0.
 * Size 0x99C (profile g_profile_NPC_HI1 at 0x101BE7FC; GameCube 0x7CC). */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
struct Hi1ActionFunc {
    be<s16> d, i;
    be<u32> f;
};
WWHD_SIZE(Hi1ActionFunc, 8);

struct daNpc_Hi1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> field_0x0;
        /* 0x01 */ be<s8> field_0x1;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> field_0x4;
        /* 0x08 */ be<f32> field_0x8;
        /* 0x0C */ be<s32> field_0xC;
    };

    void _nodeCB_Head(J3DNode*, J3DModel*);
    void _nodeCB_BackBone(J3DNode*, J3DModel*);
    s32 btpResID(int);
    s32 btkResID(int);
    bool setBtp(s32, u32);           /* (s8, bool) */
    bool setBtk(s32, u32);           /* (s8, bool) */
    bool init_texPttrnAnm(s32, u32); /* (s8, bool) */
    BOOL bodyCreateHeap();
    BOOL CreateHeap();
    bool decideType(int);
    BOOL set_action(Hi1ActionFunc*, void*);
    bool init_HI1_0();
    void play_btp_anm();
    void play_btk_anm();
    void play_animation();
    void setAttention(u32); /* bool, tested as a word */
    void setMtx(u32);       /* bool, passed on */
    bool createInit();
    cPhs_State _create();
    BOOL _delete();
    void checkOrder();
    u8 demo(); /* bool */
    s32 isEventEntry();
    void endEvent();
    void privateCut(int);
    void event_proc(int);
    fopAc_ac_c* searchByID(fpc_ProcID, be<s32>*);
    void lookBack();
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    s32 bckResID(int);
    void setAnm_anm(anm_prm_c*);
    void setAnm_NUM(int, int);
    void setAnm();
    void setAnm_ATR();
    void chngAnmAtr(u32); /* u8, compared as a word */
    void anmAtr(u32);     /* u16, compared as a word */
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    bool chk_talk();
    u8 chk_parts_notMov(); /* bool */
    u8 chkAttention();     /* bool */
    void setStt(u32);      /* s8, compared as a word */
    BOOL wait_1();
    u32 talk_1(); /* returns chk_parts_notMov()'s register */
    BOOL wait_action1(void*);

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ char mArcName[3]; /* "Hi" (decideType) */
    /* 0x7E9 */ u8 _7E9[3];
    /* 0x7EC */ u8 mBtkAnm[0x74]; /* mDoExt_btkAnm (HD 0x74); +0x68 the J3DAnmTextureSRTKey* */
    /* 0x860 */ be<u8> mBtkAnmFrame;
    /* 0x861 */ u8 _861[3];
    /* 0x864 */ u8 mBtpAnm[0x74]; /* mDoExt_btpAnm (HD 0x74); +0x10 the J3DAnmTexPattern* */
    /* 0x8D8 */ be<u8> mBtpAnmFrame;
    /* 0x8D9 */ u8 _8D9;
    /* 0x8DA */ be<s16> mTimer1; /* blink timer */
    /* 0x8DC */ Hi1ActionFunc mCurrActionFunc;
    /* 0x8E4 */ u8 _8E4[4];
    /* 0x8E8 */ be<u32> mPId; /* lookBack state 4 */
    /* 0x8EC */ u8 _8EC[8];
    /* 0x8F4 */ cXyz field_0x724;  /* GameCube field_0x724 */
    /* 0x900 */ csXyz field_0x730; /* field_0x730 */
    /* 0x906 */ csXyz field_0x736; /* field_0x736: model rotation (setMtx) */
    /* 0x90C */ u8 _90C[8];
    /* 0x914 */ cXyz field_0x744; /* field_0x744: head joint * a_eye_pos_off */
    /* 0x920 */ cXyz field_0x750; /* field_0x750 */
    /* 0x92C */ u8 _92C[0xC];
    /* 0x938 */ cXyz field_0x768; /* field_0x768: head joint translation */
    /* 0x944 */ u8 _944[0xC];
    /* 0x950 */ be<f32> mFrame;
    /* 0x954 */ u8 _954[4];
    /* 0x958 */ csXyz field_0x788;
    /* 0x95E */ u8 _95E[2];
    /* 0x960 */ be<s32> field_0x790; /* wait_1 / talk_1 result: set eyePos */
    /* 0x964 */ be<s16> mEventIdx[1];
    /* 0x966 */ be<s16> field_0x796; /* current event index */
    /* 0x968 */ u8 _968[8];
    /* 0x970 */ be<s16> mTimer2;
    /* 0x972 */ u8 _972[8];
    /* 0x97A */ be<s16> field_0x7AA; /* lookBack state 3 target */
    /* 0x97C */ be<s8> field_0x7AC;  /* animation ended */
    /* 0x97D */ be<u8> field_0x7AD;
    /* 0x97E */ be<u8> mItemNo;
    /* 0x97F */ u8 _97F;
    /* 0x980 */ be<u8> field_0x7B0; /* run event_proc */
    /* 0x981 */ be<u8> mStateIsComplaete;
    /* 0x982 */ be<u8> field_0x7B2; /* no execute/draw (unless in a demo) */
    /* 0x983 */ be<u8> field_0x7B3; /* keep shape_angle */
    /* 0x984 */ be<u8> field_0x7B4; /* no draw */
    /* 0x985 */ be<u8> field_0x7B5; /* initial position saved */
    /* 0x986 */ be<u8> mHasAttention;
    /* 0x987 */ be<u8> field_0x7B7; /* talk ordered */
    /* 0x988 */ be<u8> field_0x7B8; /* lookAtTarget_2 head only */
    /* 0x989 */ be<u8> field_0x7B9; /* turn back to field_0x730 */
    /* 0x98A */ be<u8> field_0x7BA; /* in a demo */
    /* 0x98B */ be<s8> mActIdx;
    /* 0x98C */ be<u8> field_0x7BC; /* animation attribute */
    /* 0x98D */ be<u8> field_0x7BD; /* message animation tag */
    /* 0x98E */ be<s8> field_0x7BE; /* btp number */
    /* 0x98F */ be<s8> field_0x7BF; /* btk number */
    /* 0x990 */ be<s8> field_0x7C0; /* animation number */
    /* 0x991 */ be<s8> field_0x7C1; /* 1/2 talk, >= 3 event mEventIdx[n - 3] */
    /* 0x992 */ be<s8> field_0x7C2; /* status */
    /* 0x993 */ be<s8> field_0x7C3; /* previous status */
    /* 0x994 */ be<s8> field_0x7C4; /* lookBack state */
    /* 0x995 */ be<s8> field_0x7C5; /* type */
    /* 0x996 */ be<s8> field_0x7C6; /* specific type */
    /* 0x997 */ be<s8> field_0x7C7; /* set_action: 0 init, 9 leaving */
    /* 0x998 */ be<s8> field_0x7C8;
    /* 0x999 */ u8 _999[3];
};
WWHD_OFFSET(daNpc_Hi1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Hi1_c, mBtkAnm, 0x7EC);
WWHD_OFFSET(daNpc_Hi1_c, mBtpAnm, 0x864);
WWHD_OFFSET(daNpc_Hi1_c, mCurrActionFunc, 0x8DC);
WWHD_OFFSET(daNpc_Hi1_c, field_0x724, 0x8F4);
WWHD_OFFSET(daNpc_Hi1_c, field_0x744, 0x914);
WWHD_OFFSET(daNpc_Hi1_c, mFrame, 0x950);
WWHD_OFFSET(daNpc_Hi1_c, field_0x790, 0x960);
WWHD_OFFSET(daNpc_Hi1_c, mTimer2, 0x970);
WWHD_OFFSET(daNpc_Hi1_c, field_0x7B0, 0x980);
WWHD_OFFSET(daNpc_Hi1_c, field_0x7C8, 0x998);
WWHD_SIZE(daNpc_Hi1_c, 0x99C);

// HD: HIO vtable remains first.
struct daNpc_Hi1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;
        be<s16> mMaxHeadY;
        be<s16> mMinHeadX;
        be<s16> mMinHeadY;
        be<s16> mMaxBackboneX;
        be<s16> mMaxBackboneY;
        be<s16> mMinBackboneX;
        be<s16> mMinBackboneY;
        be<s16> mMaxTurnStep;        /* dNpc_JntCtrl_c::setParam arguments */
        /* 0x12 */ be<s16> field_12; /* lookBack */
        /* 0x14 */ be<f32> mAttentionYOffset;
        /* 0x18 */ be<u8> mDebug; /* debug colours in _draw */
        /* 0x19 */ u8 _19[3];
    };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Hi1_HIO_c, 0x28);

// HD: virtual frame count, animation-removal and destructor layouts.
struct Hi1AnmVtable {
    u8 _0[0x14];
    be<u32> mGetFrameMax;
};
struct Hi1Anm {
    u8 _0[4];
    gptr<Hi1AnmVtable> mpVtable;
};
struct Hi1ModelAnimation {
    u8 _0[0x38];
    be<u32> mpBtpAnimation;
    u8 _3C[8];
    be<u32> mpBtkAnimation;
};
struct Hi1AcchVtables {
    u8 _0[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mPolyVtable;
};
WWHD_OFFSET(Hi1AnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Hi1Anm, mpVtable, 4);
WWHD_OFFSET(Hi1ModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(Hi1AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(Hi1AcchVtables, mPolyVtable, 0x20);
template <class T, class U> inline T* Hi1_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
template <class T> inline s32 Hi1_frameMax(T* p) {
    auto* a = Hi1_view<Hi1Anm>(p);
    auto* t = a->mpVtable.get();
    return gabi::call_ptr<s32>(t->mGetFrameMax, p);
}
inline void Hi1_loadAction(Hi1ActionFunc* dst, Hi1ActionFunc* src) {
    auto* d = Hi1_view<be<u32>>(dst);
    auto* s = Hi1_view<be<u32>>(src);
    d[0] = s[0];
    d[1] = s[1];
}
struct Hi1VirtualAction {
    u8 _0[6];
    be<s16> mVtableOffset;
};
inline void Hi1_callAction(void* self, Hi1ActionFunc* action, void* arg) {
    s16 index = action->i;
    u32 object = gabi::ea(self) + (s32)(s16)action->d;
    if (index < 0) {
        gabi::call_ptr<BOOL>(action->f, object, arg);
    } else {
        s16 offset = Hi1_view<Hi1VirtualAction>(action)->mVtableOffset;
        auto* vtable = gabi::at<be<u32>>(object + offset);
        auto* entry = gabi::at<be<u32>>((u32)*vtable + index * 8 + 4);
        gabi::call_ptr<BOOL>(*entry, object, arg);
    }
}
inline void Hi1_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if ((u32)*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

struct Hi1BtpAnm {
    u8 _0[0x10];
    gptr<J3DAnmTexPattern> mpAnimation;
};
WWHD_OFFSET(Hi1BtpAnm, mpAnimation, 0x10);
WWHD_SIZE(Hi1BtpAnm, 0x14);
struct Hi1MorfVtable {
    u8 _0[0xC];
    be<u32> mDestroy;
};
struct Hi1MorfObject {
    gptr<Hi1MorfVtable> mpVtable;
};
WWHD_OFFSET(Hi1MorfVtable, mDestroy, 0xC);
WWHD_SIZE(Hi1MorfVtable, 0x10);
WWHD_OFFSET(Hi1MorfObject, mpVtable, 0);
WWHD_SIZE(Hi1MorfObject, 4);
inline void Hi1_destroyMorf(mDoExt_McaMorf* p) {
    gabi::call_ptr(Hi1_view<Hi1MorfObject>(p)->mpVtable->mDestroy, p, 3);
}
inline const char* Hi1_staffName(dNpc_EventCut_c* cut) {
    return gabi::at<const char>(cut->mpEvtStaffName);
}
inline void Hi1_copyWords(cXyz* dst, const cXyz* src) {
    auto* s = Hi1_view<be<u32>>(src);
    auto* d = Hi1_view<be<u32>>(dst);
    u32 x = s[0], y = s[1], z = s[2];
    d[0] = x;
    d[1] = y;
    d[2] = z;
}
inline be<u8>* Hi1_bytes(void* p) { return gabi::at<be<u8>>(gabi::ea(p)); }
WWHD_SIZE(Hi1AnmVtable, 0x18);
WWHD_SIZE(Hi1Anm, 8);
WWHD_SIZE(Hi1ModelAnimation, 0x48);
WWHD_OFFSET(Hi1ModelAnimation, mpBtkAnimation, 0x44);
WWHD_SIZE(Hi1AcchVtables, 0x24);
WWHD_OFFSET(Hi1VirtualAction, mVtableOffset, 6);
WWHD_SIZE(Hi1VirtualAction, 8);
WWHD_OFFSET(Hi1ActionFunc, d, 0);
WWHD_OFFSET(Hi1ActionFunc, i, 2);
WWHD_OFFSET(Hi1ActionFunc, f, 4);
WWHD_OFFSET(daNpc_Hi1_HIO_c, __vtbl, 0);
WWHD_OFFSET(daNpc_Hi1_HIO_c, mPrmTbl, 0xC);
WWHD_SIZE(daNpc_Hi1_HIO_c::hio_prm_c, 0x1C);
WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, mAttentionYOffset, 0x14);
WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, mDebug, 0x18);
WWHD_SIZE(daNpc_Hi1_c::anm_prm_c, 0x10);
WWHD_OFFSET(daNpc_Hi1_c::anm_prm_c, field_0x4, 4);
WWHD_OFFSET(daNpc_Hi1_c::anm_prm_c, field_0x8, 8);
WWHD_OFFSET(daNpc_Hi1_c::anm_prm_c, field_0xC, 0xC);

struct Hi1BtkAnm {
    u8 _0[0x68];
    gptr<J3DAnmTextureSRTKey> mpAnimation;
};
WWHD_OFFSET(Hi1BtkAnm, mpAnimation, 0x68);
WWHD_SIZE(Hi1BtkAnm, 0x6C);
WWHD_OFFSET(daNpc_Hi1_c, field_0x788, 0x958);
s8 Hi1_roomId(dBgS*, cBgS_PolyInfo*);
u8 Hi1_polyColour(dBgS*, cBgS_PolyInfo*);
s16 Hi1_calcTimer(be<s16>* t);
s32 Hi1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                s32 start, s32 end, u32 modify, s32 entry);
bool Hi1_lockonTruth(dAttention_c* a);
fopAc_ac_c* Hi1_lockonTarget(dAttention_c* a, s32 i);
fopAc_ac_c* Hi1_actionTarget(dAttention_c* a, s32 i);
void Hi1_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel,
                      u8 headOnly);
BOOL Hi1_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids,
                  u32 p6, s8 p7);
void* Hi1_demoGetActor(u32 obj, u8 id);
void Hi1_npcCtor(daNpc_Hi1_c* p);
void Hi1_btpCtor(void* p);
void Hi1_circleDestroy(void* p, s32 flags);
void Hi1_acchDestroy(dBgS_Acch* p, s32 flags);
void Hi1_actorDestroy(daNpc_Hi1_c* p, s32 flags);
J3DAnmTexPattern* Hi1_demoBtp(void*, const char*);
s32 Hi1_getRndValue(s32, s32);
BOOL Hi1_checkAttention(fopAc_ac_c*, cXyz*, f32, f32, f32, u32);
u32 Hi1_materialSound(dBgS*, cBgS_PolyInfo*);
BOOL Hi1_searchById(u32, be<u32>*);
BOOL Hi1_playMorf(mDoExt_McaMorf*, cXyz*, u32, s32);
u32 Hi1_messageStatus(void*);
s32 Hi1_btkInit(void* anm, J3DModelData* d, J3DAnmTextureSRTKey* p, s32 play, s32 attr, f32 rate,
                s32 start, s32 end, u32 modify, s32 entry);
J3DAnmTextureSRTKey* Hi1_demoBtk(void*, const char*);
void Hi1_btkCtor(void*);
void Hi1_playerEyePos(cXyz*, f32);

inline fopAc_ac_c* Hi1_actorFromIdResult(u32 p) { return gabi::at<fopAc_ac_c>(p); }

WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, mMaxHeadX, 0x0);
WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, mMaxHeadY, 0x2);
WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, mMinHeadX, 0x4);
WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, mMinHeadY, 0x6);
WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, mMaxBackboneX, 0x8);
WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, mMaxBackboneY, 0xA);
WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, mMinBackboneX, 0xC);
WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, mMinBackboneY, 0xE);
WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, mMaxTurnStep, 0x10);
WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, field_12, 0x12);
