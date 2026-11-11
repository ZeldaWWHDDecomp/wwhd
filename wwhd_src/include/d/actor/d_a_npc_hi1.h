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
        /* 0x00 */ be<s8> bckNum;  /* was field_0x0: setAnm_anm 0223DB2C: the entry's bck animation number, compared with and stored to mBckNum (tww); after tww daNpc_Ls1_c::anm_prm_c bckNum */
        /* 0x01 */ be<s8> btpNum;  /* was field_0x1: setAnm_NUM 0223DBC4 / setAnm 0223DC34 / setAnm_ATR 0223DCA4 pass it to init_texPttrnAnm (tww); after tww daNpc_Ls1_c::anm_prm_c btpNum */
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> morf;  /* was field_0x4: setAnm_anm 0223DB2C: the morf frames passed to dNpc_setAnm (tww); after tww daNpc_Ls1_c::anm_prm_c morf */
        /* 0x08 */ be<f32> speed;  /* was field_0x8: setAnm_anm 0223DB2C: the play speed passed to dNpc_setAnm (tww); after tww daNpc_Ls1_c::anm_prm_c speed */
        /* 0x0C */ be<s32> loopMode;  /* was field_0xC: setAnm_anm 0223DB2C: the loop mode passed to dNpc_setAnm (tww); after tww daNpc_Ls1_c::anm_prm_c loopMode */
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
    /* 0x8F4 */ cXyz mInitialPos;  /* was field_0x724: _execute 0223D6EC copies current.pos on the first run (tww if (!field_0x7B5) { it = current.pos; ..}); written only, but the same code as tww daNpc_Bm1_c's _execute, which names this field mInitialPos (after tww daNpc_Bm1_c mInitialPos) */
    /* 0x900 */ csXyz mInitialAngle;  /* was field_0x730: _execute 0223D6EC copies current.angle on the first run, with mInitialPos = current.pos, then mbRanExecute = 1 (tww); wait_1 0223DFB4 turns current.angle.y back to its y (tww cLib_addCalcAngleS(&current.angle.y, it.y, ..)); same code as daNpc_Bm1_c::mInitialAngle (after tww daNpc_Bm1_c mInitialPos) */
    /* 0x906 */ csXyz mAngle;  /* was field_0x736: createInit 0223CB30 sets current.angle and copies it to shape_angle; setMtx 0223CA28 rotates the model by it (tww mDoMtx_stack_c::ZXYrotM(it)); _execute 0223D6EC current.angle each frame (tww); after tww daNpc_Ls1_c mAngle (same layout slot) */
    /* 0x90C */ u8 _90C[8];
    /* 0x914 */ cXyz mEyePos;  /* was field_0x744: _nodeCB_Head 0223BD84: the head matrix times a_eye_pos_off (tww multVec(&a_eye_pos_off, &it)); setAttention 0223C9D4 copies it to eyePos (tww); same name as daNpc_Zl1_c::mEyePos (npcs.md) */
    /* 0x920 */ cXyz mLookPos;  /* was field_0x750: lookBack 0223D3E8: the look-at target, the player's eye position in look mode 1, the partner's position at eye height in mode 4 (tww); same name as daNpc_Zl1_c::mLookPos (npcs.md) */
    /* 0x92C */ u8 _92C[0xC];
    /* 0x938 */ cXyz mHeadPos;  /* was field_0x768: _nodeCB_Head 0223BD84: the head joint matrix's translation (tww multVecZero(&it)); same name as daNpc_Zl1_c::mHeadPos (npcs.md) */
    /* 0x944 */ u8 _944[0xC];
    /* 0x950 */ be<f32> mFrame;
    /* 0x954 */ u8 _954[4];
    /* 0x958 */ csXyz mPartsAngle;  /* was field_0x788: lookBack 0223D3E8 stores x = current.angle.y, y = the head Y angle, z = the backbone Y angle (tww); chk_parts_notMov 0223DEA0 compares them with the current ones (tww); after tww daNpc_Ls1_c mActorAngleY / mJointHeadY / mJointBackboneY (three fields there) */
    /* 0x95E */ u8 _95E[2];
    /* 0x960 */ be<s32> mbSetEyePos;  /* was field_0x790: wait_action1 0223E204 stores wait_1() / talk_1() (tww); setAttention 0223C9D4 tests it with its param (tww if (it == 0 && !param_1)); after tww daNpc_Ls1_c mbSetEyePos (same code) */
    /* 0x964 */ be<s16> mEventIdx[1];
    /* 0x966 */ be<s16> mEventIndex;  /* was field_0x796: eventOrder 0223D67C sets mOrderEvent - 3 and orders mEventIdx[it] (tww); checkOrder 0223CE7C / event_proc 0223D2F0 check mEventIdx[it] (tww); after tww daNpc_Ls1_c mEventIndex (same code) */
    /* 0x968 */ u8 _968[8];
    /* 0x970 */ be<s16> mTimer2;
    /* 0x972 */ u8 _972[8];
    /* 0x97A */ be<s16> mLookAngleY;  /* was field_0x7AA: lookBack 0223D3E8: the target angle of look mode 3; never written in WWHD or tww (always 0); same role as daNpc_Zl1_c::mLookAngleY (the shared template); lookBack state 3 target */
    /* 0x97C */ be<s8> mbMorfAnimStopped;  /* was field_0x7AC: play_animation 0223C928: the morf's play() result, or 1 (tww); setAnm_anm 0223DB2C 0; after tww daNpc_Ls1_c mbMorfAnimStopped (same code) */
    /* 0x97D */ be<u8> field_0x7AD;
    /* 0x97E */ be<u8> mItemNo;
    /* 0x97F */ u8 _97F;
    /* 0x980 */ be<u8> mForceEventProc;  /* was field_0x7B0: _execute 0223D6EC runs event_proc while it is set even without a staff index; never written in WWHD or tww (always 0); same role as daNpc_Zl1_c::mForceEventProc (the shared template); run event_proc */
    /* 0x981 */ be<u8> mStateIsComplaete;
    /* 0x982 */ be<u8> mHideUntilDemo;  /* was field_0x7B2: _execute 0223D6EC returns early while it is set and no demo runs and clears it in a demo; _draw 0223D9C0 draws nothing while it is set; never set in WWHD or tww (only cleared); same role as daNpc_Zl1_c::mHideUntilDemo (the shared template); no execute/draw (unless in a demo) */
    /* 0x983 */ be<u8> mKeepShapeAngle;  /* was field_0x7B3: hi1_setAngle (in _execute 0223D6EC) copies current.angle into shape_angle only while it is 0; demo 0223CF54 clears it; never set in WWHD or tww; same role as daNpc_Zl1_c::mKeepShapeAngle (the shared template); keep shape_angle */
    /* 0x984 */ be<u8> mNoDraw;  /* was field_0x7B4: _draw 0223D9C0 draws nothing while it (or mHideUntilDemo) is set; never written in WWHD or tww (always 0); no draw */
    /* 0x985 */ be<u8> mbRanExecute;  /* was field_0x7B5: _execute 0223D6EC: 0 on the first run, when mInitialPos and mInitialAngle are taken, then 1 (tww if (!it) { ..; it = 1; }); after tww daNpc_Bm1_c mbRanExecute (same code) */
    /* 0x986 */ be<u8> mHasAttention;
    /* 0x987 */ be<u8> mTalking;  /* was field_0x7B7: checkOrder 0223CE7C sets 1 on the talk command (tww); wait_1 0223DFB4 starts the talk (setStt(2)) while it is set (tww); talk_1 0223E12C clears it (tww); after daNpcMt_c::mTalking (npcs.md) */
    /* 0x988 */ be<u8> mLookHeadOnly;  /* was field_0x7B8: lookBack 0223D3E8 passes it to lookAtTarget_2 as the head-only flag (d_npc.h lookAtTarget headOnly; tww bool temp4 = it); wait_1 0223DFB4 sets 1 while waiting and 0 when the talk starts (tww the same); same name as daNpc_So_c::mLookHeadOnly and daNpc_Zl1_c::mLookHeadOnly (same evidence) */
    /* 0x989 */ be<u8> mTurnToInitialAngle;  /* was field_0x7B9: wait_1 0223DFB4 turns back to mInitialAngle.y while it is set; cleared when the talk starts, set when the attention check fails (tww); wait_action1 0223E204 1 (tww) */
    /* 0x98A */ be<u8> mInDemo;  /* was field_0x7BA: demo 0223CF54: 1 while demoActorID is set (resets the joint angles when it turns on), 0 otherwise, and returns it (tww); _execute 0223D6EC skips the collision while set (tww) */
    /* 0x98B */ be<s8> mActIdx;
    /* 0x98C */ be<u8> mAnmAtrNo;  /* was field_0x7BC: chngAnmAtr 0223DD0C stores the attribute (tww it = param_1); setAnm_ATR 0223DCA4 plays a_anm_prm_tbl[it] (tww); checkOrder 0223CE7C / endEvent 0223D214 / setStt 0223DF68 0xFF; same name as daNpc_Zl1_c::mAnmAtrNo (npcs.md) */
    /* 0x98D */ be<u8> mMesgAnimeTag;  /* was field_0x7BD: anmAtr 0223DD28: the message's animation tag (dComIfGp_getMesgAnimeTagInfo) when it changes (tww it = tagInfo; chngAnmTag()); checkOrder / endEvent / setStt 0xFF (tww); after tww daNpc_Ls1_c mMesgAnimeTag (same code) */
    /* 0x98E */ be<s8> mBtpNum;  /* was field_0x7BE: setBtp 0223C0BC stores the btp number (tww it = param_1); play_btp_anm 0223C82C blinks only while it is 0 (tww); demo 0223CF54 1; after tww daNpc_Ls1_c mBtpNum (same code) */
    /* 0x98F */ be<s8> mBtkNum;  /* was field_0x7BF: setBtk 0223C1B4 stores the btk number (tww it = param_1); demo 0223CF54 1; after tww daNpc_Ls1_c mBtkNum (same code) */
    /* 0x990 */ be<s8> mBckNum;  /* was field_0x7C0: setAnm_anm 0223DB2C: skips an entry whose bckNum is < 0 or equal to it, else stores it (tww); createInit 0223CB30 2; after tww daNpc_Ls1_c mBckNum (same code) */
    /* 0x991 */ be<s8> mOrderEvent;  /* was field_0x7C1: eventOrder 0223D67C: 1 / 2 the talk (1 orders the speak event), 3+ orders mEventIdx[it - 3] (tww); checkOrder 0223CE7C clears it once that event starts (tww); setStt 0223DF68 / wait_1 0223DFB4 set it; after daNpc_Cb1_c::mOrderEvent */
    /* 0x992 */ be<s8> mStt;  /* was field_0x7C2: setStt 0223DF68 stores the new state and switches on it (tww); setAnm 0223DC34 plays a_anm_prm_tbl[it] (tww); wait_action1 0223E204 switches on it; same name as daNpc_Zl1_c::mStt (npcs.md) */
    /* 0x993 */ be<s8> mPrevStt;  /* was field_0x7C3: setStt 0223DF68 keeps the previous mStt here (tww); talk_1 0223E12C returns to it (tww setStt(it)); same name as daNpc_Zl1_c::mPrevStt (npcs.md) */
    /* 0x994 */ be<s8> mLookMode;  /* was field_0x7C4: lookBack 0223D3E8 switches on it: 1 the player's eyes, 2 mLookPos, 3 mLookAngleY, 4 the partner (tww); wait_1 0223DFB4 sets 0 / 1 (tww); after daNpcMt_c::mLookMode (npcs.md) */
    /* 0x995 */ be<s8> mHeapSizeIdx;  /* was field_0x7C5: decideType 0223C65C sets it (tww); _create 0223CCC4 creates the solid heap with a_heap_size_tbl[it] (tww a_siz_tbl[it]); the slot of tww daNpc_Ls1_c m854 (unnamed there) */
    /* 0x996 */ be<s8> mType;  /* was field_0x7C6: decideType 0223C65C sets it (tww); createInit 0223CB30 / getMsg_HI1_0 switch on it (tww); after tww daNpc_Ls1_c mType (same code) */
    /* 0x997 */ be<s8> mActionStatus;  /* was field_0x7C7: set_action 0223C6C0 sets 9 for the leaving action and 0 for the new one (tww); wait_action1 0223E204 switches on it and steps it (tww it++); after tww daNpc_Bs1_c mActionStatus */
    /* 0x998 */ be<s8> mAnmAtrSetCnt;  /* was field_0x7C8: anmAtr 0223DD28: while the message types, applies the message's animation attribute only when it is 0, then adds 1; MSG_DISPLAYED and setStt 0223DF68 reset it (tww) */
    /* 0x999 */ u8 _999[3];
};
WWHD_OFFSET(daNpc_Hi1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Hi1_c, mBtkAnm, 0x7EC);
WWHD_OFFSET(daNpc_Hi1_c, mBtpAnm, 0x864);
WWHD_OFFSET(daNpc_Hi1_c, mCurrActionFunc, 0x8DC);
WWHD_OFFSET(daNpc_Hi1_c, mInitialPos, 0x8F4);
WWHD_OFFSET(daNpc_Hi1_c, mEyePos, 0x914);
WWHD_OFFSET(daNpc_Hi1_c, mFrame, 0x950);
WWHD_OFFSET(daNpc_Hi1_c, mbSetEyePos, 0x960);
WWHD_OFFSET(daNpc_Hi1_c, mTimer2, 0x970);
WWHD_OFFSET(daNpc_Hi1_c, mForceEventProc, 0x980);
WWHD_OFFSET(daNpc_Hi1_c, mAnmAtrSetCnt, 0x998);
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
        /* 0x12 */ be<s16> mMaxHeadTurnVel;  /* was field_12: lookBack 0223D3E8 passes it to the look-at as the max turn speed (tww the same); same role as dNpc_HIO_c::mMaxHeadTurnVel */
        /* 0x14 */ be<f32> mAttentionYOffset;
        /* 0x18 */ be<u8> mDebug; /* debug colours in _draw */
        /* 0x19 */ u8 _19[3];
    };
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> mRefCount;  /* was field_0x8: daNpc_Hi1_HIO_c_ct 0223E2BC -1; never read in WWHD or tww; after our decomp's daNpc_Pm1_HIO_c / daNpc_Ba1_HIO_c mRefCount: the same HIO layout (mNo, then the s32 instance counter) */
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
WWHD_OFFSET(daNpc_Hi1_c::anm_prm_c, morf, 4);
WWHD_OFFSET(daNpc_Hi1_c::anm_prm_c, speed, 8);
WWHD_OFFSET(daNpc_Hi1_c::anm_prm_c, loopMode, 0xC);

struct Hi1BtkAnm {
    u8 _0[0x68];
    gptr<J3DAnmTextureSRTKey> mpAnimation;
};
WWHD_OFFSET(Hi1BtkAnm, mpAnimation, 0x68);
WWHD_SIZE(Hi1BtkAnm, 0x6C);
WWHD_OFFSET(daNpc_Hi1_c, mPartsAngle, 0x958);
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
WWHD_OFFSET(daNpc_Hi1_HIO_c::hio_prm_c, mMaxHeadTurnVel, 0x12);
