/* daNpc_Km1_c (Mila, rich), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_km1: fopNpc_npc_c is 0x7DC (see
 * d_a_npc_ba1.h), so members are +0x118 up to m_backbone_jnt_num. mShadowId and the shadow
 * model (GameCube 0x6D0/0x6D4) are gone (HD shadows), so m_head_tex_pattern/mBtpAnm are +0x110;
 * mDoExt_btpAnm grew from 0x14 to 0x74 (mBtpFrame.. +0x170) and the pointer to member
 * function is 8 bytes (GHS) instead of 12, so mEventCut and everything after it is +0x16C.
 * Size 0x944 (profile g_profile_NPC_KM1 at 0x101BF764; GameCube 0x7D8). */
#pragma once
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/d/d_npc.h"
// HD: GHS action descriptors are eight bytes, with a signed this adjustment/index.
struct Km1ActionFunc { be<s16> d, i; be<u32> f; };
WWHD_SIZE(Km1ActionFunc, 8);

struct daNpc_Km1_c : fopNpc_npc_c {
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
    bool set_action(Km1ActionFunc*, void*);
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
    /* 0x7E6 */ u8 _7E6[2];
    /* 0x7E8 */ gptr<J3DAnmTexPattern> m_head_tex_pattern; /* HD: no mShadowId / shadow model before it */
    /* 0x7EC */ u8 mBtpAnm[0x74];                           /* mDoExt_btpAnm (HD 0x74) */
    /* 0x860 */ be<u8> mBtpFrame;
    /* 0x861 */ be<u8> field_0x6F1;
    /* 0x862 */ be<s16> mBlinkTimer;  /* was field_0x6F2: playTexPatternAnm 02271BD0 counts it down while the blink pattern runs and restarts it with 30 + rnd(60); init_btp 02271678 0 (tww km1 the same); after daNpcPeople_c::mBlinkTimer (npcs.md) */
    /* 0x864 */ Km1ActionFunc mAction;                         /* field_0x6F4 */
    /* 0x86C */ dNpc_EventCut_c mEventCut;
    /* 0x8D8 */ csXyz mInitialAngle;  /* was field_0x76C: _execute 022726A0 copies current.angle on the first run (with mInitialPos, then mbRanExecute = 1); wait01 02272DAC faces its y (mDesiredYRot = it.y) (tww km1 the same); same code as daNpc_Bm1_c / daNpc_Hi1_c / daNpc_Kp1_c::mInitialAngle */
    /* 0x8DE */ u8 _8DE[2];
    /* 0x8E0 */ cXyz mInitialPos;  /* was field_0x774: _execute 022726A0 copies current.pos on the first run; written only (tww km1 the same); after tww daNpc_Bm1_c mInitialPos (same code) */
    /* 0x8EC */ cXyz mAttPos;
    /* 0x8F8 */ cXyz mEyePos;
    /* 0x904 */ cXyz mLookPos;  /* was field_0x798: createInit 02271E78 current.pos; lookBack 02272424 looks at it (tww km1 the same); same code as daNpc_Bm1_c::mLookPos (createInit current.pos, lookBack dstPos) */
    /* 0x910 */ be<f32> mPrevMorfFrame;  /* was field_0x7A4: setMtx 02271D1C: a morf frame below it means the animation wrapped, then it takes the frame; setAnm_anm 02272A34 0 (tww km1 the same); after tww daNpc_Ls1_c mPrevMorfFrame */
    /* 0x914 */ u8 _914[4];
    /* 0x918 */ be<f32> field_0x7AC;
    /* 0x91C */ be<s16> mLookAtMaxVel;  /* was field_0x7B0: lookBack 02272424 eases it to HIO mMaxHeadTurnVel while turning, else 0, and passes it as the look-at's maxVel (tww km1 the same); after tww daNpc_Bm1_c mHeadLookAtMaxVel / daNpc_Ah_c mLookAtMaxVel */
    /* 0x91E */ be<s16> mDesiredYRot;  /* was field_0x7B2: lookBack 02272424: the look-at's default Y angle (targetY = it); wait01 02272DAC sets mInitialAngle.y (tww km1 the same); same code as daNpc_Bm1_c::mDesiredYRot */
    /* 0x920 */ be<s8> mbMorfAnimStopped;  /* was field_0x7B4: setMtx 02271D1C: the morf's play() result, or 1; setAnm_anm 02272A34 0 (tww km1 the same); after tww daNpc_Ls1_c mbMorfAnimStopped (same code) */
    /* 0x921 */ be<s8> field_0x7B5;
    /* 0x922 */ be<u8> field_0x7B6;
    /* 0x923 */ be<u8> mItemNo;  /* was field_0x7B7: wait01 02272DAC: 0xFF, or dComIfGp_event_getPreItemNo() on a talk with an item; talk01 02272E6C 0xFF (tww the same); same code as daNpc_Pm1_c::mItemNo */
    /* 0x924 */ be<u8> mbRanExecute;  /* was field_0x7B8: _execute 022726A0: 0 on the first run, when mInitialPos and mInitialAngle are taken, then 1 (tww km1 the same); after tww daNpc_Bm1_c mbRanExecute (same code) */
    /* 0x925 */ u8 _925[3];
    /* 0x928 */ be<s32> mbInitEyePos;  /* was field_0x7BC: createInit 02271E78 1; setAttention 02271CBC copies mEyePos to eyePos while it or mbSetEyePos is set, then clears it (one eye position right after create); same code as daNpc_Kp1_c / daNpc_Pm1_c::mbInitEyePos */
    /* 0x92C */ be<s32> mbSetEyePos;  /* was field_0x7C0: wait_action1 02272EEC stores talk01() / wait01(); setAttention 02271CBC tests it (tww km1 the same); after tww daNpc_Ls1_c mbSetEyePos (same code) */
    /* 0x930 */ be<u8> mHasAttention;  /* was field_0x7C4: wait_action1 02272EEC: chkAttention(); wait01 02272DAC looks at the player while it is set (tww km1 the same); after tww daNpc_Hi1_c mHasAttention (mHasAttention = chkAttention()) */
    /* 0x931 */ be<u8> mTalking;  /* was field_0x7C5: checkOrder 0227218C sets 1 on the talk command; wait01 02272DAC starts the talk while it is set; talk01 02272E6C clears it (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mTalking */
    /* 0x932 */ be<u8> mHeadOnlyFollow;
    /* 0x933 */ be<u8> mInDemo;  /* was field_0x7C7: demo 022721CC: 1 while demoActorID is set, 0 otherwise, and returns it; setMtx 02271D1C / _execute 022726A0 test it (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mInDemo */
    /* 0x934 */ be<u8> field_0x7C8;
    /* 0x935 */ be<s8> mActIdx;  /* was field_0x7C9: privateCut 02272334: dComIfGp_evmng_getMyActIdx(..), -1 ends (tww km1 the same); after tww daNpc_Hi1_c mActIdx (same code) */
    /* 0x936 */ be<s8> mEventActionNo;  /* was field_0x7CA: event_actionInit 022722A4 stores the cut's action number (tww field_0x7CA = *puVar1); event_action (inlined in WWHD) switches on it (tww switch (field_0x7CA)); same code as daNpc_Kp1_c::mEventActionNo */
    /* 0x937 */ be<u8> mAnmAtrNo;  /* was field_0x7CB: chngAnmAtr 02272BC0 stores the attribute; setAnm_ATR 02272B5C plays a_anm_prm_tbl[it]; setStt 02272CA4 / endEvent 02272D6C use it (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mAnmAtrNo */
    /* 0x938 */ be<u8> mMesgAnimeTag;  /* was field_0x7CC: anmAtr 02272BE4: the message's animation tag when it changes (if (uVar1 != 0xFF && uVar1 != it)) (tww km1 the same); after tww daNpc_Ls1_c mMesgAnimeTag (same code) */
    /* 0x939 */ be<s8> mBtpNum;  /* was field_0x7CD: setAnm_tex 02272A10 stores the btp number when it changes; initTexPatternAnm 0227178C / playTexPatternAnm 02271BD0 / CreateHeap 022717D0 use it (tww km1 the same); after tww daNpc_Ls1_c mBtpNum */
    /* 0x93A */ be<s8> mBckNum;  /* was field_0x7CE: setAnm_anm 02272A34: skips an entry whose bckNum is < 0 or equal to it, else stores it (tww km1 the same); after tww daNpc_Ls1_c mBckNum (same code) */
    /* 0x93B */ be<s8> mOrderEvent;  /* was field_0x7CF: eventOrder 02272668 orders by it (1 / 2 the talk) (tww); checkOrder 0227218C clears it once the talk starts; wait01 02272DAC sets it (tww); same code as daNpc_Hi1_c / daNpc_Kp1_c::mOrderEvent */
    /* 0x93C */ be<s8> mStt;  /* was field_0x7D0: setStt 02272CA4 stores the new state; setAnm 02272AF0 plays a_anm_prm_tbl[it]; wait_action1 02272EEC switches on it (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mStt */
    /* 0x93D */ be<s8> mPrevStt;  /* was field_0x7D1: setStt 02272CA4 keeps the previous mStt; talk01 02272E6C returns to it (setStt(it)) (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mPrevStt */
    /* 0x93E */ be<s8> mLookMode;  /* was field_0x7D2: lookBack 02272424 switches on it; setStt 02272CA4 / wait01 02272DAC set 1 / 3 (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mLookMode */
    /* 0x93F */ be<s8> field_0x7D3;
    /* 0x940 */ be<s8> field_0x7D4;
    /* 0x941 */ be<s8> mActionStatus;  /* was field_0x7D5: set_action 02271AA4 sets -1 for the leaving action and 0 for the new one; wait_action1 02272EEC switches on it (tww km1 the same); after tww daNpc_Bs1_c mActionStatus */
    /* 0x942 */ be<s8> mAnmAtrSetCnt;  /* was field_0x7D6: anmAtr 02272BE4: applies the message's animation attribute only when it is 0, then adds 1 (tww km1 the same); same code as daNpc_Hi1_c / daNpc_Kp1_c::mAnmAtrSetCnt */
    /* 0x943 */ be<s8> field_0x7D7;
};
WWHD_OFFSET(daNpc_Km1_c, m_head_tex_pattern, 0x7E8);
WWHD_OFFSET(daNpc_Km1_c, mAction, 0x864);
WWHD_OFFSET(daNpc_Km1_c, mEventCut, 0x86C);
WWHD_OFFSET(daNpc_Km1_c, mEyePos, 0x8F8);
WWHD_OFFSET(daNpc_Km1_c, mbInitEyePos, 0x928);
WWHD_OFFSET(daNpc_Km1_c, mBtpNum, 0x939);
WWHD_SIZE(daNpc_Km1_c, 0x944);

// HD: the HIO vtable moved to the end and the debug colour option is a byte.
struct daNpc_Km1_HIO_c {
    struct hio_prm_c {
        /* 0x00 */ be<s16> mMaxHeadX;  /* was field_0: _execute 022726A0 passes it to m_jnt.setParam as argument 5 (tww dNpc_JntCtrl_c::setParam max_head_x), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c / daNpc_Hi1_HIO_c::hio_prm_c::mMaxHeadX */
        /* 0x02 */ be<s16> mMaxHeadY;  /* was field_2: _execute 022726A0 passes it to m_jnt.setParam as argument 6 (tww dNpc_JntCtrl_c::setParam max_head_y), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c / daNpc_Hi1_HIO_c::hio_prm_c::mMaxHeadY */
        /* 0x04 */ be<s16> mMinHeadX;  /* was field_4: _execute 022726A0 passes it to m_jnt.setParam as argument 7 (tww dNpc_JntCtrl_c::setParam min_head_x), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c / daNpc_Hi1_HIO_c::hio_prm_c::mMinHeadX */
        /* 0x06 */ be<s16> mMinHeadY;  /* was field_6: _execute 022726A0 passes it to m_jnt.setParam as argument 8 (tww dNpc_JntCtrl_c::setParam min_head_y), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c / daNpc_Hi1_HIO_c::hio_prm_c::mMinHeadY */
        /* 0x08 */ be<s16> mMaxBackboneX;  /* was mMaxBackBoneX (misnamed): our own spelling; the joint control's limit passed to m_jnt.setParam as argument 1 (tww dNpc_JntCtrl_c::setParam max_backbone_x), spelled as tww dNpc_HIO_c::mMaxBackboneX and the other NPC parameter blocks; was field_8: _execute 022726A0 passes it to m_jnt.setParam as argument 1 (tww dNpc_JntCtrl_c::setParam max_backbone_x), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c::mMaxBackBoneX / daNpc_Hi1_HIO_c::hio_prm_c::mMaxBackboneX */
        /* 0x0A */ be<s16> mMaxBackboneY;  /* was mMaxBackBoneY (misnamed): our own spelling; the joint control's limit passed to m_jnt.setParam as argument 2 (tww dNpc_JntCtrl_c::setParam max_backbone_y), spelled as tww dNpc_HIO_c::mMaxBackboneY and the other NPC parameter blocks; was field_A: _execute 022726A0 passes it to m_jnt.setParam as argument 2 (tww dNpc_JntCtrl_c::setParam max_backbone_y), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c::mMaxBackBoneY / daNpc_Hi1_HIO_c::hio_prm_c::mMaxBackboneY */
        /* 0x0C */ be<s16> mMinBackboneX;  /* was mMinBackBoneX (misnamed): our own spelling; the joint control's limit passed to m_jnt.setParam as argument 3 (tww dNpc_JntCtrl_c::setParam min_backbone_x), spelled as tww dNpc_HIO_c::mMinBackboneX and the other NPC parameter blocks; was field_C: _execute 022726A0 passes it to m_jnt.setParam as argument 3 (tww dNpc_JntCtrl_c::setParam min_backbone_x), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c::mMinBackBoneX / daNpc_Hi1_HIO_c::hio_prm_c::mMinBackboneX */
        /* 0x0E */ be<s16> mMinBackboneY;  /* was mMinBackBoneY (misnamed): our own spelling; the joint control's limit passed to m_jnt.setParam as argument 4 (tww dNpc_JntCtrl_c::setParam min_backbone_y), spelled as tww dNpc_HIO_c::mMinBackboneY and the other NPC parameter blocks; was field_E: _execute 022726A0 passes it to m_jnt.setParam as argument 4 (tww dNpc_JntCtrl_c::setParam min_backbone_y), the joint control's limit (tww the same); same slot and role as daNpc_Ls1_HIO_c::hio_prm_c::mMinBackBoneY / daNpc_Hi1_HIO_c::hio_prm_c::mMinBackboneY */
        /* 0x10 */ be<s16> mMaxTurnStep;  /* was field_10: _execute 022726A0 passes it to m_jnt.setParam as the turn step (9th argument) (tww the same); same role as dNpc_HIO_c::mMaxTurnStep / daNpc_Hi1_HIO_c::hio_prm_c::mMaxTurnStep */
        /* 0x12 */ be<s16> mMaxHeadTurnVel;  /* was field_12: lookBack 02272424: cLib_addCalcAngleS2 eases mLookAtMaxVel toward it while turning (tww the same); same role as dNpc_HIO_c::mMaxHeadTurnVel */
        /* 0x14 */ be<f32> mAttentionArrowYOffset;
        /* 0x18 */ be<u8> mDebug;  /* was field_18: HD: a byte flag; _draw 02272858 initialises the debug colour statics while it is set (tww: an unused f32 0.0); same role as daNpc_Hi1_HIO_c::hio_prm_c::mDebug */
        /* 0x19 */ u8 _19[3];
    };
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<s32> mRefCount;  /* was field_0x8: daNpc_Km1_HIO_c_ct 02272FB0 -1; _create 02271F84 creates the HIO child at < 0 and adds 1; _delete 02272110 subtracts 1 and deletes the child below 0 (tww the same); same code as daNpc_Pm1_HIO_c::mRefCount */
    /* 0x08 */ hio_prm_c mPrmTbl;
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_Km1_HIO_c, 0x28);
s16 Km1_calcTimer(be<s16>* t);
s32 Km1_btpInit(void* anm, J3DModelData* d, J3DAnmTexPattern* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry);
bool Km1_lockonTruth(dAttention_c* a);
fopAc_ac_c* Km1_lockonTarget(dAttention_c* a, s32 i);
fopAc_ac_c* Km1_actionTarget(dAttention_c* a, s32 i);
void Km1_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly);
BOOL Km1_demoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* morf, const char* arc, s32 n, void* ids, u32 p6, s8 p7);
void* Km1_demoGetActor(u32 obj, u8 id);
void Km1_npcCtor(daNpc_Km1_c* p);
void Km1_btpCtor(void* p);
void Km1_eventCutCtor(dNpc_EventCut_c* p);
void Km1_circleDestroy(void* p, s32 flags);
void Km1_acchDestroy(dBgS_Acch* p, s32 flags);
void Km1_actorDestroy(daNpc_Km1_c* p, s32 flags);

// HD: virtual frame count, animation-removal and destructor layouts.
struct Km1AnmVtable { u8 _0[0x14]; be<u32> mGetFrameMax; };
struct Km1Anm { u8 _0[4]; gptr<Km1AnmVtable> mpVtable; };
struct Km1ModelAnimation { u8 _0[0x38]; be<u32> mpBtpAnimation; };
struct Km1AcchVtables { u8 _0[0x14]; be<u32> mCheckVtable; u8 _18[8]; be<u32> mPolyVtable; };
WWHD_OFFSET(Km1AnmVtable, mGetFrameMax, 0x14);
WWHD_OFFSET(Km1Anm, mpVtable, 4);
WWHD_OFFSET(Km1ModelAnimation, mpBtpAnimation, 0x38);
WWHD_OFFSET(Km1AcchVtables, mCheckVtable, 0x14);
WWHD_OFFSET(Km1AcchVtables, mPolyVtable, 0x20);
template<class T,class U> inline T* Km1_view(U* p) { return gabi::at<T>(gabi::ea(p)); }
inline s32 Km1_frameMax(J3DAnmTexPattern* p) { auto* a=Km1_view<Km1Anm>(p); auto* t=a->mpVtable.get(); return gabi::call_ptr<s32>(t->mGetFrameMax,p); }
inline void Km1_loadAction(Km1ActionFunc* dst, Km1ActionFunc* src) {
    auto* d=Km1_view<be<u32>>(dst); auto* s=Km1_view<be<u32>>(src);
    d[0]=s[0];d[1]=s[1];
}
struct Km1VirtualAction { u8 _0[6]; be<s16> mVtableOffset; };
inline void Km1_callAction(void* self, Km1ActionFunc* action, void* arg) {
    s16 index=action->i; u32 object=gabi::ea(self)+(s32)(s16)action->d;
    if(index<0) { gabi::call_ptr<BOOL>(action->f,object,arg); }
    else {
        s16 offset=Km1_view<Km1VirtualAction>(action)->mVtableOffset;
        auto* vtable=gabi::at<be<u32>>(object+offset);
        auto* entry=gabi::at<be<u32>>((u32)*vtable+index*8+4);
        gabi::call_ptr<BOOL>(*entry,object,arg);
    }
}
inline be<u32>* Km1_word(u32 a) { return gabi::at<be<u32>>(a); }
inline void Km1_initDebugColour(be<u32>* guard, be<u32>* dst, be<u32>* src) {
    if((u32)*guard==0) { *guard=1; memcpy_g(dst,src,4); }
}

s8 Km1_roomId(dBgS*, cBgS_PolyInfo*);
u8 Km1_polyColour(dBgS*, cBgS_PolyInfo*);
