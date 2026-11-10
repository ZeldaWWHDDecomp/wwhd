/* daNpc_Kp1_c (Maggie, rich, Windfall), WWHD layout. 
 *
 * The GameCube TU is "Nonmatching" (no members in its header). The layout is d_a_npc_km1's
 * (Km1 code is largely shared) with three extra members after the joint numbers (a third joint
 * "handL" and two hand-held models), so everything from m_head_tex_pattern on is Km1 + 0xC,
 * except 0x92F..0x933, where Kp1 has four message/event flags before the first-frame flag.
 * Size 0x950 (profile at 0x101BFF0C, process name 0x163). Field names follow the GameCube Km1
 * header where the use is the same. */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
struct Kp1ActionFunc {
    be<s16> d, i;
    be<u32> f;
};
WWHD_SIZE(Kp1ActionFunc, 8);

struct daNpc_Kp1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> bckNum;
        /* 0x01 */ be<s8> btpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;
        /* 0x08 */ be<f32> speed;
        /* 0x0C */ be<s32> loopMode;
    };

    bool createInit();
    void setMtx();
    bool anmResID(int, be<s32>*, be<s32>*);
    void BtpNum2ResID(int, be<s32>*);
    u32 setAnm_tex(s8);
    bool init_btp(u32, int);
    bool initTexPatternAnm(u32);
    void playTexPatternAnm();
    s32 setAnm_anm(anm_prm_c*);
    void setAnm();
    void setAnm_NUM(int, int);
    void ctrlAnmAtr();
    BOOL chk_talk();
    void chngAnmAtr(u32);
    void setAnm_ATR(int);
    void anmAtr(u16);
    void setStt(s8);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void eventOrder();
    void checkOrder();
    void lookBack();
    u8 chkAttention();
    void setAttention();
    bool decideType(int);
    void event_actionInit(int);
    bool event_action();
    void privateCut();
    void endEvent();
    void event_proc();
    bool set_action(Kp1ActionFunc*, void*);
    BOOL wait01();
    BOOL talk01();
    BOOL wait_action1(void*);
    u8 demo();
    BOOL _draw();
    BOOL _execute();
    BOOL _delete();
    cPhs_State _create();
    BOOL CreateHeap();

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_head_jnt_num;
    /* 0x7E5 */ be<s8> m_backbone_jnt_num;
    /* 0x7E6 */ be<s8> m_hand_jnt_num; /* HD (Kp1): a third joint (CreateHeap) */
    /* 0x7E7 */ u8 _7E7;
    /* 0x7E8 */ gptr<J3DModel> mpItemModel[2]; /* Kp1: two extra models (CreateHeap ids 6, 7) */
    /* 0x7F0 */ be<u8> mItemModelIndex;
    /* 0x7F1 */ u8 _7F1[3];
    /* 0x7F4 */ gptr<J3DAnmTexPattern>
        m_head_tex_pattern;       /* HD: no mShadowId / shadow model before it */
    /* 0x7F8 */ u8 mBtpAnm[0x74]; /* mDoExt_btpAnm (HD 0x74) */
    /* 0x86C */ be<u8> mBtpFrame;
    /* 0x86D */ be<u8> field_0x6F1;
    /* 0x86E */ be<s16> mBlinkTimer;  /* was field_0x6F2: playTexPatternAnm 0227B854 counts it down while the blink pattern runs and restarts it with 30 + rnd(60); init_btp 0227B1D0 0; after daNpcPeople_c::mBlinkTimer (npcs.md) */
    /* 0x870 */ Kp1ActionFunc mAction; /* field_0x6F4 */
    /* 0x878 */ dNpc_EventCut_c mEventCut;
    /* 0x8E4 */ csXyz mInitialAngle;  /* was field_0x76C: _execute 0227C774 copies current.angle on the first run (with mInitialPos, then mbRanExecute = 1); wait01 0227D3C8 faces its y (mDesiredYRot = it.y); same code as daNpc_Bm1_c / daNpc_Hi1_c::mInitialAngle */
    /* 0x8EA */ u8 _8EA[2];
    /* 0x8EC */ cXyz mInitialPos;  /* was field_0x774: _execute 0227C774 copies current.pos on the first run; written only; after tww daNpc_Bm1_c mInitialPos (same code) */
    /* 0x8F8 */ cXyz mAttPos;
    /* 0x904 */ cXyz mEyePos;
    /* 0x910 */ cXyz mLookPos;  /* was field_0x798: createInit 0227BC14 current.pos; lookBack 0227C41C looks at it; same code as daNpc_Bm1_c::mLookPos (createInit current.pos, lookBack dstPos) */
    /* 0x91C */ be<f32> mPrevMorfFrame;  /* was field_0x7A4: setMtx 0227B9AC: a morf frame below it means the animation wrapped, then it takes the frame; setAnm_anm 0227CB88 0; after tww daNpc_Ls1_c mPrevMorfFrame */
    /* 0x920 */ u8 _920[4];
    /* 0x924 */ be<f32> field_0x7AC;
    /* 0x928 */ be<s16> mLookAtMaxVel;  /* was field_0x7B0: lookBack 0227C41C eases it to HIO field_12 while turning, else 0, and passes it as the look-at's maxVel; after tww daNpc_Bm1_c mHeadLookAtMaxVel / daNpc_Ah_c mLookAtMaxVel */
    /* 0x92A */ be<s16> mDesiredYRot;  /* was field_0x7B2: lookBack 0227C41C: the look-at's default Y angle (targetY = it); wait01 0227D3C8 sets mInitialAngle.y; same code as daNpc_Bm1_c::mDesiredYRot */
    /* 0x92C */ be<s8> mbMorfAnimStopped;  /* was field_0x7B4: setMtx 0227B9AC: the morf's play() result, or 1; setAnm_anm 0227CB88 0; ctrlAnmAtr 0227CDE4 tests it; after tww daNpc_Ls1_c mbMorfAnimStopped (same code) */
    /* 0x92D */ be<s8> mAnmLoopCount;  /* was field_0x7B5: setAnm_anm 0227CB88 0; ctrlAnmAtr 0227CDE4 counts the morf stops and changes the attribute at 2; same code as daNpc_Bm1_c::mAnmLoopCount */
    /* 0x92E */ be<u8> mPreItemNo;  /* was field_0x7B6: chk_talk 0227D348: dComIfGp_event_getPreItemNo(), else 0xFF; getMsg 0227D104 picks the message by it; createInit 0227BC14 / talk01 0227D480 0xFF; same code as daNpc_Bm1_c::mPreItemNo */
    /* 0x92F */ be<u8> mLetterEventEnded;     /* Kp1: getMsg flag (event 1 ended) */
    /* 0x930 */ be<u8> mHeartPieceEventEnded; /* Kp1: getMsg flag (event 2 ended) */
    /* 0x931 */ be<u8> mLetterOffered;        /* Kp1: next_msgStatus/getMsg flag */
    /* 0x932 */ be<u8> mCancelPresent;        /* Kp1: present to cancel (talk01) */
    /* 0x933 */ be<u8> mbRanExecute;  /* was field_0x7B8: _execute 0227C774: 0 on the first run, when mInitialPos and mInitialAngle are taken, then 1; after tww daNpc_Bm1_c mbRanExecute (same code) */
    /* 0x934 */ be<s32> field_0x7BC;
    /* 0x938 */ be<s32> mbSetEyePos;  /* was field_0x7C0: wait_action1 0227D558 stores talk01() / wait01(); setAttention 0227B94C tests it; after tww daNpc_Ls1_c mbSetEyePos (same code) */
    /* 0x93C */ be<u8> mHasAttention;  /* was field_0x7C4: wait_action1 0227D558: chkAttention(); wait01 0227D3C8 looks at the player while it is set; after tww daNpc_Hi1_c mHasAttention (mHasAttention = chkAttention()) */
    /* 0x93D */ be<u8> mTalking;  /* was field_0x7C5: checkOrder 0227BF00 sets 1 on the talk command; wait01 0227D3C8 starts the talk while it is set; talk01 0227D480 clears it; same code as daNpc_Hi1_c::mTalking */
    /* 0x93E */ be<u8> mHeadOnlyFollow;
    /* 0x93F */ be<u8> mInDemo;  /* was field_0x7C7: demo 0227BFE0: 1 while demoActorID is set, 0 otherwise, and returns it; setMtx 0227B9AC / _execute 0227C774 test it; same code as daNpc_Hi1_c::mInDemo */
    /* 0x940 */ be<u8> field_0x7C8;
    /* 0x941 */ be<s8> mActIdx;  /* was field_0x7C9: privateCut 0227C32C: dComIfGp_evmng_getMyActIdx(..), -1 ends; after tww daNpc_Hi1_c mActIdx (same code) */
    /* 0x942 */ be<s8> mEventActionNo;  /* was field_0x7CA: event_actionInit 0227C1E0 stores the cut's action number (*actionNumber); event_action 0227C268 switches on it */
    /* 0x943 */ be<u8> mAnmAtrNo;  /* was field_0x7CB: chngAnmAtr 0227CD8C stores the attribute; setAnm_ATR 0227CD1C plays a_anm_prm_tbl[it]; ctrlAnmAtr 0227CDE4 / setStt 0227CF10 / endEvent 0227C1A0 use it; same code as daNpc_Hi1_c::mAnmAtrNo */
    /* 0x944 */ be<u8> mMesgAnimeTag;  /* was field_0x7CC: anmAtr 0227CE54: the message's animation tag when it changes (if (uVar1 != 0xFF && uVar1 != it)); after tww daNpc_Ls1_c mMesgAnimeTag (same code) */
    /* 0x945 */ be<s8> mBtpNum;  /* was field_0x7CD: setAnm_tex 0227CB64 stores the btp number when it changes; init_btp 0227B1D0 / initTexPatternAnm 0227B2FC / playTexPatternAnm 0227B854 / CreateHeap 0227B340 use it; after tww daNpc_Ls1_c mBtpNum */
    /* 0x946 */ be<s8> mBckNum;  /* was field_0x7CE: setAnm_anm 0227CB88: skips an entry whose bckNum is < 0 or equal to it, else stores it; after tww daNpc_Ls1_c mBckNum (same code) */
    /* 0x947 */ be<s8> mOrderEvent;  /* was field_0x7CF: eventOrder 0227C714 orders by it (1 / 2 the talk, 3 / 4 the events); checkOrder 0227BF00 clears it once they start; next_msgStatus 0227CF50 3 / 4; event_proc 0227C600 / wait01 0227D3C8 set it; same code as daNpc_Hi1_c::mOrderEvent */
    /* 0x948 */ be<s8> mStt;  /* was field_0x7D0: setStt 0227CF10 stores the new state; setAnm 0227CCB0 plays a_anm_prm_tbl[it]; wait_action1 0227D558 switches on it; same code as daNpc_Hi1_c::mStt */
    /* 0x949 */ be<s8> mPrevStt;  /* was field_0x7D1: setStt 0227CF10 keeps the previous mStt; talk01 0227D480 returns to it (setStt(it)); same code as daNpc_Hi1_c::mPrevStt */
    /* 0x94A */ be<s8> mLookMode;  /* was field_0x7D2: lookBack 0227C41C switches on it; setStt 0227CF10 / wait01 0227D3C8 set 1 / 3; same code as daNpc_Hi1_c::mLookMode */
    /* 0x94B */ be<s8> field_0x7D3;
    /* 0x94C */ be<s8> field_0x7D4;
    /* 0x94D */ be<s8> mActionStatus;  /* was field_0x7D5: set_action 0227B728 sets -1 for the leaving action and 0 for the new one; wait_action1 0227D558 switches on it; after tww daNpc_Bs1_c mActionStatus */
    /* 0x94E */ be<s8> mAnmAtrSetCnt;  /* was field_0x7D6: anmAtr 0227CE54: applies the message's animation attribute only when it is 0, then adds 1; same code as daNpc_Hi1_c::mAnmAtrSetCnt */
    /* 0x94F */ be<s8> field_0x7D7;
};
WWHD_OFFSET(daNpc_Kp1_c, m_head_tex_pattern, 0x7F4);
WWHD_OFFSET(daNpc_Kp1_c, mAction, 0x870);
WWHD_OFFSET(daNpc_Kp1_c, mEventCut, 0x878);
WWHD_OFFSET(daNpc_Kp1_c, mEyePos, 0x904);
WWHD_OFFSET(daNpc_Kp1_c, field_0x7BC, 0x934);
WWHD_OFFSET(daNpc_Kp1_c, mBtpNum, 0x945);
WWHD_SIZE(daNpc_Kp1_c, 0x950);

// HD: Kp1 keeps the HIO vtable first.
struct daNpc_Kp1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> field_0;
        /* 0x02 */ be<s16> field_2;
        /* 0x04 */ be<s16> field_4;
        /* 0x06 */ be<s16> field_6;
        /* 0x08 */ be<s16> field_8;
        /* 0x0A */ be<s16> field_A;
        /* 0x0C */ be<s16> field_C;
        /* 0x0E */ be<s16> field_E;
        /* 0x10 */ be<s16> field_10;
        /* 0x12 */ be<s16> field_12;
        /* 0x14 */ be<f32> mAttentionArrowYOffset;
        /* 0x18 */ be<u8> field_18; /* HD: a byte flag (debug colours in _draw) */
        /* 0x19 */ u8 _19[3];
    };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ hio_prm_c mPrmTbl;
};
WWHD_SIZE(daNpc_Kp1_HIO_c, 0x28);

s16 Kp1_calcTimer(be<s16>* t);
s32 Kp1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate,
                s32 start, s32 end, u32 modify, s32 entry);
bool Kp1_lockonTruth(dAttention_c* a);
fopAc_ac_c* Kp1_lockonTarget(dAttention_c* a, s32 i);
fopAc_ac_c* Kp1_actionTarget(dAttention_c* a, s32 i);
void Kp1_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel,
                      u8 headOnly);
BOOL Kp1_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids,
                  u32 p6, s8 p7);
void* Kp1_demoGetActor(u32 obj, u8 id);
void Kp1_npcCtor(daNpc_Kp1_c* p);
void Kp1_btpCtor(void* p);
void Kp1_eventCutCtor(dNpc_EventCut_c* p);
void Kp1_circleDestroy(void* p, s32 flags);
void Kp1_acchDestroy(dBgS_Acch* p, s32 flags);
void Kp1_actorDestroy(daNpc_Kp1_c* p, s32 flags);

// HD: virtual frame count, animation-removal and destructor layouts.
struct Kp1AnmVtable {
    u8 _0[0x14];
    be<u32> mGetFrameMax;
};
struct Kp1Anm {
    u8 _0[4];
    gptr<Kp1AnmVtable> mpVtable;
};
struct Kp1ModelAnimation {
    u8 _0[0x38];
    be<u32> mpBtpAnimation;
};
struct Kp1AcchVtables {
    u8 _0[0x14];
    be<u32> mCheckVtable;
    u8 _18[8];
    be<u32> mPolyVtable;
};
WWHD_OFFSET(Kp1AnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Kp1Anm, mpVtable, 4);
WWHD_OFFSET(Kp1ModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(Kp1AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(Kp1AcchVtables, mPolyVtable, 0x20);
template <class T, class U> inline T* Kp1_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 Kp1_frameMax(J3DAnmTexPattern* p) {
    auto* a = Kp1_view<Kp1Anm>(p);
    auto* t = a->mpVtable.get();
    return gabi::call_ptr<s32>(t->mGetFrameMax, p);
}
inline void Kp1_loadAction(Kp1ActionFunc* dst, Kp1ActionFunc* src) {
    auto* d = Kp1_view<be<u32>>(dst);
    auto* s = Kp1_view<be<u32>>(src);
    d[0] = s[0];
    d[1] = s[1];
}
struct Kp1VirtualAction {
    u8 _0[6];
    be<s16> mVtableOffset;
};
inline void Kp1_callAction(void* self, Kp1ActionFunc* action, void* arg) {
    s16 index = action->i;
    u32 object = gabi::ea(self) + (s32)(s16)action->d;
    if (index < 0) {
        gabi::call_ptr<BOOL>(action->f, object, arg);
    } else {
        s16 offset = Kp1_view<Kp1VirtualAction>(action)->mVtableOffset;
        auto* vtable = gabi::at<be<u32>>(object + offset);
        auto* entry = gabi::at<be<u32>>((u32)*vtable + index * 8 + 4);
        gabi::call_ptr<BOOL>(*entry, object, arg);
    }
}
inline void Kp1_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if ((u32)*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

s8 Kp1_roomId(dBgS*, cBgS_PolyInfo*);
u8 Kp1_polyColour(dBgS*, cBgS_PolyInfo*);

struct Kp1MessageManager {
    u8 _0[0x948];
    be<u32> mSelectedAnswer;
};
WWHD_OFFSET(Kp1MessageManager, mSelectedAnswer, 0x948);
WWHD_SIZE(Kp1MessageManager, 0x94C);
struct Kp1MorfVtable {
    u8 _0[0xC];
    be<u32> mDestroy;
};
struct Kp1MorfObject {
    gptr<Kp1MorfVtable> mpVtable;
};
WWHD_OFFSET(Kp1MorfVtable, mDestroy, 0xC);
WWHD_SIZE(Kp1MorfVtable, 0x10);
WWHD_SIZE(Kp1MorfObject, 4);
inline void Kp1_destroyMorf(mDoExt_McaMorf* morf) {
    auto* object = Kp1_view<Kp1MorfObject>(morf);
    gabi::call_ptr(object->mpVtable->mDestroy, morf, 3);
}
u32 Kp1_messageStatus(Kp1MessageManager*);
void Kp1_orderOtherEvent(fopAc_ac_c*, const char*, u16, u16);
BOOL Kp1_startEvent(dEvent_manager_c*, const char*);
BOOL Kp1_endEvent(dEvent_manager_c*, const char*);
u32 Kp1_createPresent(cXyz*, s32, u8, s32, s32, csXyz*, cXyz*);
J3DAnmTexPattern* Kp1_demoBtp(void*, const char*);

WWHD_OFFSET(Kp1ActionFunc, d, 0);
WWHD_OFFSET(Kp1ActionFunc, i, 2);
WWHD_OFFSET(Kp1ActionFunc, f, 4);
WWHD_SIZE(daNpc_Kp1_c::anm_prm_c, 0x10);
WWHD_OFFSET(daNpc_Kp1_c::anm_prm_c, bckNum, 0);
WWHD_OFFSET(daNpc_Kp1_c::anm_prm_c, btpNum, 1);
WWHD_OFFSET(daNpc_Kp1_c::anm_prm_c, morf, 4);
WWHD_OFFSET(daNpc_Kp1_c::anm_prm_c, speed, 8);
WWHD_OFFSET(daNpc_Kp1_c::anm_prm_c, loopMode, 0xC);
WWHD_OFFSET(daNpc_Kp1_HIO_c, __vtbl, 0);
WWHD_OFFSET(daNpc_Kp1_HIO_c, mNo, 4);
WWHD_OFFSET(daNpc_Kp1_HIO_c, field_0x8, 8);
WWHD_OFFSET(daNpc_Kp1_HIO_c, mPrmTbl, 0xC);
WWHD_SIZE(daNpc_Kp1_HIO_c::hio_prm_c, 0x1C);
WWHD_OFFSET(daNpc_Kp1_HIO_c::hio_prm_c, mAttentionArrowYOffset, 0x14);
WWHD_OFFSET(daNpc_Kp1_HIO_c::hio_prm_c, field_18, 0x18);
WWHD_SIZE(Kp1AnmVtable, 0x18);
WWHD_SIZE(Kp1Anm, 8);
WWHD_SIZE(Kp1ModelAnimation, 0x3C);
WWHD_SIZE(Kp1AcchVtables, 0x24);
WWHD_OFFSET(Kp1VirtualAction, mVtableOffset, 6);
WWHD_SIZE(Kp1VirtualAction, 8);
WWHD_OFFSET(Kp1MorfObject, mpVtable, 0);
