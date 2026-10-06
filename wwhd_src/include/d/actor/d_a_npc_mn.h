/* daNpcMn_c (Manny, Windfall figure fan), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the constructor 0229BF48 (allocates 0x93C) and the verified
 * functions: fopNpc_npc_c is 0x7DC (d/d_npc.h), so members are +0x118 up to mBtpAnm (0x854);
 * mDoExt_btpAnm grew from 0x14 to 0x74 and mShadowId (GameCube 0x750) is gone (HD shadows), so
 * everything from mPathRun (GameCube 0x754) on is +0x174. Size 0x93C (GameCube 0x7C8). */
#pragma once
#include "bindings.h"

#define MN_SAFESTRING_VTBL 0x1001E640 /* this TU's sead::SafeString vtable */
#define MN_VTBL 0x1001E914            /* daNpcMn_c vtable (constructor) */

struct sMnAnmDat {
    /* 0x00 */ be<u8> mBckIdx;
    /* 0x01 */ be<u8> mMorf;
    /* 0x02 */ be<s8> mLoopCount;
};
WWHD_SIZE(sMnAnmDat, 3);

struct MnNpcDat {
    /* 0x00 */ be<s16> mMax_head_x;
    /* 0x02 */ be<s16> mMax_head_y;
    /* 0x04 */ be<s16> mMax_backbone_x;
    /* 0x06 */ be<s16> mMax_backbone_y;
    /* 0x08 */ be<s16> mMin_head_x;
    /* 0x0A */ be<s16> mMin_head_y;
    /* 0x0C */ be<s16> mMin_backbone_x;
    /* 0x0E */ be<s16> mMin_backbone_y;
    /* 0x10 */ be<s16> mMax_turn_step;
    /* 0x12 */ be<s16> field_0x12;
    /* 0x14 */ be<f32> mPlayerEyeOfsY;
    /* 0x18 */ be<f32> mAttnPosOfsY;
    /* 0x1C */ be<f32> mEyePosOfsY;
    /* 0x20 */ be<f32> mAttnDist;
    /* 0x24 */ be<f32> mNearDist;
    /* 0x28 */ be<s16> mAttnAngle;
    /* 0x2A */ be<s16> mLookAtMaxVel;
    /* 0x2C */ be<s16> mWalkLookAtMaxVel;
    /* 0x2E */ be<s16> field_0x2E;
    /* 0x30 */ be<f32> mCylRadius;
    /* 0x34 */ be<f32> mWalkAnmRate;
    /* 0x38 */ be<f32> mWalkSpeed;
    /* 0x3C */ be<s16> field_0x3C;
    /* 0x3E */ be<s16> field_0x3E;
    /* 0x40 */ be<s16> mWaitTimerMin;
    /* 0x42 */ be<s16> mWaitTimerMax;
    /* 0x44 */ be<s16> mTurnWaitMin;
    /* 0x46 */ be<s16> mTurnWaitMax;
    /* 0x48 */ be<s16> mLookResetTimer;
    /* 0x4A */ be<s8> mbAllowBodyTurn;
    /* 0x4B */ be<s8> mbLookOnly;
};
WWHD_SIZE(MnNpcDat, 0x4C);

/* dNpc_PathRun_c (8 bytes, unchanged) */
struct MnPathRun_l {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(MnPathRun_l, 8);

struct daNpcMn_c : fopNpc_npc_c {
    enum MoveProcIdx { MOVE_PROC_WAIT = 0, MOVE_PROC_TALK = 1, MOVE_PROC_WALK = 2, MOVE_PROC_TURN = 3, MOVE_PROC_TALK3 = 4 };
    enum LookMode { LOOK_MODE_NONE = 0, LOOK_MODE_ATTN = 1, LOOK_MODE_TURN = 2 };
    enum Talk3State { TALK3_INIT = 0, TALK3_ORDER = 1, TALK3_TALK = 2 };
    enum BckIdx { BCK_WAIT01 = 0, BCK_WAIT02 = 1, BCK_TALK01 = 2, BCK_TALK02 = 3, BCK_WALK = 4, BCK_BIKKURI = 5, BCK_JUMP01 = 6, BCK_JUMP02 = 7, BCK_NULL = 0xFF };

    u8 getNpcNo() { return mNpcNo; }
    request_of_phase_process_class* getPhaseP() { return &mPhs; }
    void setResFlag(u8 flag) { mResFlag = flag; }

    /* the declarations follow the GameCube header; return types are what the WWHD code returns */
    cPhs_State _create();
    BOOL createHeap();
    cPhs_State createInit();
    bool _delete();
    bool _draw();
    bool _execute();
    u8 executeCommon();
    void executeSetMode(u32); /* u8, not re-extended */
    int executeWaitInit();
    void executeWait();
    int executeTalkInit();
    void executeTalk();
    int executeTalk3Init();
    void executeTalk3();
    int executeWalkInit();
    void executeWalk();
    int executeTurnInit();
    void executeTurn();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetInit(int);
    bool eventMesSet();
    void eventGetItemInit();
    void eventWaitInit(int);
    bool eventWait(int);
    void eventSwOnInit(int);
    bool eventSwOn();
    void eventHatchInit();
    bool eventHatch();
    void eventBikkuriInit(int);
    bool eventBikkuri();
    u32 eventTurnInit();
    bool eventTurn(int);
    u32 eventWalkInit();
    bool eventWalk();
    u32 eventLookInit();
    bool eventLook();
    void eventJumpInit(int);
    bool eventJump();
    u16 talk2(int);
    u16 talk3(int);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    u32 getMsg3();
    void setMessage(u32);
    void setAnmFromMsgTag();
    u8 getPrmNpcNo(); /* bool */
    u8 getPrmRailID();
    u8 getPrmSwitchBit();
    u8 getPrmSwitchBit2();
    void setMtx();
    void chkAttention();
    void lookBack();
    BOOL initTexPatternAnm(u32); /* bool, passed on unnormalised */
    void playTexPatternAnm();
    void playAnm();
    void setAnm(u32, int, f32); /* u8 index, full register */
    bool setAnmTbl(sMnAnmDat*);
    s16 XyCheckCB(int);
    int getRand(int);
    void setCollision(dCcD_Cyl*, cXyz*, f32, f32); /* cXyz by value: pointer to a copy */
    BOOL chkEndEvent();
    u8 chkPosNo();
    u8 getPosNo();
    BOOL isChangePos(u32); /* u8, not re-extended */

    /* 0x7DC */ u8 field_0x6C4[0x60];
    /* 0x83C */ request_of_phase_process_class mPhs;
    /* 0x844 */ request_of_phase_process_class mPhsMethod;
    /* 0x84C */ gptr<J3DModel> mpModel;
    /* 0x850 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x854 */ u8 mBtpAnm[0x74];                  /* mDoExt_btpAnm (HD 0x74, constructor 025E7820) */
    /* 0x8C8 */ MnPathRun_l mPathRun;              /* HD: no mShadowId before it */
    /* 0x8D0 */ cXyz field_0x75C;
    /* 0x8DC */ cXyz mLookAtPos;
    /* 0x8E8 */ gptr<sMnAnmDat> mpAnmDat;
    /* 0x8EC */ gptr<be<u32>> mpMsgNo;
    /* 0x8F0 */ be<f32> mTargetSpeedF;
    /* 0x8F4 */ be<f32> mAnmMorfOverride;
    /* 0x8F8 */ be<f32> mAttnDist;
    /* 0x8FC */ be<f32> mJumpSpeedY;
    /* 0x900 */ be<u32> mItemNo;
    /* 0x904 */ be<u32> mFigureArg;
    /* 0x908 */ be<u8> mHeadOnlyFollow;
    /* 0x909 */ be<u8> field_0x795;
    /* 0x90A */ be<s16> mHatchEventIdx;
    /* 0x90C */ be<s16> mWaitTimer;
    /* 0x90E */ be<s16> mLookResetTimer;
    /* 0x910 */ be<s16> mEventTimer;
    /* 0x912 */ be<s16> mAttnAngle;
    /* 0x914 */ be<s16> mHomeYRot;
    /* 0x916 */ be<u16> mLastMsgStatus;
    /* 0x918 */ be<u16> mEtcFlag;
    /* 0x91A */ be<s16> field_0x7A6;
    /* 0x91C */ be<s16> mBtpTimer;
    /* 0x91E */ be<s16> mLookAtMaxVel;
    /* 0x920 */ be<s16> mTurnVel;
    /* 0x922 */ be<s16> mTargetYRot;
    /* 0x924 */ be<u8> mTalkOrder;
    /* 0x925 */ be<u8> mbPlayerAttention;
    /* 0x926 */ be<u8> mEvtOrderType;
    /* 0x927 */ be<u8> mBtpFrame;
    /* 0x928 */ be<u8> mMoveState;
    /* 0x929 */ be<u8> mResFlag;
    /* 0x92A */ be<u8> mNpcNo;
    /* 0x92B */ be<u8> mSwitchFlag;
    /* 0x92C */ be<u8> mBckIdx;
    /* 0x92D */ be<u8> mAnmFlag;
    /* 0x92E */ be<s8> mAnmLoopCnt;
    /* 0x92F */ be<s8> mActIdx;
    /* 0x930 */ be<u8> field_0x7BC;
    /* 0x931 */ be<s8> mLookMode;
    /* 0x932 */ be<u8> mbAllowBodyTurn;
    /* 0x933 */ be<u8> mbLookOnly;
    /* 0x934 */ be<u8> mbNearPlayer;
    /* 0x935 */ be<u8> mPosFlag;
    /* 0x936 */ be<s8> mShoulderRJoint;
    /* 0x937 */ be<u8> mTalk3State;
    /* 0x938 */ be<u8> mbLookFigure;
    /* 0x939 */ be<u8> mFigureMsgIdx;
    /* 0x93A */ u8 _93A[2];
};
WWHD_OFFSET(daNpcMn_c, mPhs, 0x83C);
WWHD_OFFSET(daNpcMn_c, mBtpAnm, 0x854);
WWHD_OFFSET(daNpcMn_c, mPathRun, 0x8C8);
WWHD_OFFSET(daNpcMn_c, mHomeYRot, 0x914);
WWHD_OFFSET(daNpcMn_c, mNpcNo, 0x92A);
WWHD_OFFSET(daNpcMn_c, mFigureMsgIdx, 0x939);
WWHD_SIZE(daNpcMn_c, 0x93C);

/* ---- translation-unit statics (.data / .rodata addresses) ---- */
enum : u32 {
    MN_l_arcname_tbl = 0x101C2040,  /* const char*[1] ("Mn") */
    MN_l_npc_staff_id = 0x101C203C, /* char*[1] */
    MN_l_npc_dat = 0x101C2178,      /* MnNpcDat[2] */
    MN_l_bck_ix_tbl = 0x1001E678,   /* int[8] */
    MN_l_room_name = 0x101C20BC,    /* char*[10] */
    MN_l_figure_comp = 0x101C2154,  /* u16[17] */
    MN_l_method = 0x101C2210,       /* _create's phase handler table */
    MN_l_npc_anm_wait = 0x101C2054,
};
static inline const char* mn_arcname() { return gabi::at<const char>(gabi::load<u32>(MN_l_arcname_tbl)); }
static inline MnNpcDat* mn_npc_dat(u32 npcNo) { return gabi::at<MnNpcDat>(MN_l_npc_dat + npcNo * 0x4C); }

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_people.h unless noted) ---- */
static inline u32 dComIfGs_save() { return gabi::load<u32>(0x101F84DC); }
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_getEventReg(u16 reg) { return dSv_event_getEventReg(dComIfGs_event(), reg); }
static inline void dComIfGs_setEventReg(u16 reg, u8 v) { dSv_event_setEventReg(dComIfGs_event(), reg, v); }
static inline dSv_event_c* dComIfGs_tmpEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178); }
static inline BOOL dComIfGs_isTmpBit(u16 f) { return dSv_event_isEventBit(dComIfGs_tmpEvent(), f); }
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = MN_SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* the same, with the values GHS leaves in r6/r7 (the harness compares r3..r7 at getIDRes) */
static inline void* dComIfG_getObjectIDRes_r67(const char* arc, s32 id, u32 r6, u32 r7) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = MN_SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id, r6, r7);
}
static inline void mn_set_r(int n, u32 v) { gabi::cpu->r[n] = v; }
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start,
                                     s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum_l(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
static inline void J3DModelData_setJointCallBack_l(J3DModelData* d, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(gabi::ea(d) + 4);
    u32 p = gabi::load<u32>(gabi::ea(d) + 8);
    if (i < n) {
        p += i * 0x1C;
    }
    gabi::store<u32>(p + 8, cb);
}
/* HD J3D joint matrices: J3DModel +0x2C is the matrix block (flags u16 at +4, matrices at +0x10) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline Mtx34* mn_getAnmMtx(J3DModel* model, s32 jnt) {
    J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}
/* sead::SafeString equality (HD form of strcmp(a, b) == 0): the left string is terminated
 * through its vtable twice, the right one once, then compared by pointer, then byte by byte (at
 * most 0x40001 bytes) */
static inline bool mn_SafeString_eq(SafeString* a, SafeString* b) {
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a);
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b);
    u32 pa = a->mStringTop;
    u32 pb = b->mStringTop;
    if (pa == pb) {
        return true;
    }
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = gabi::load<u8>(pa + n);
        if (ca != gabi::load<u8>(pb + n)) {
            return false;
        }
        if (ca == 0) {
            return true;
        }
    }
    return false;
}
/* strcmp(dComIfGp_getNextStageName(), name) == 0 (play + 0x5140) */
static inline bool mn_isNextStage(u32 name) {
    gabi::Local<SafeString> a;
    a->mStringTop = name;
    a->__vtbl = MN_SAFESTRING_VTBL;
    u32 play = dComIfGp_ea();
    gabi::Local<SafeString> b;
    b->mStringTop = play + 0x5140;
    b->__vtbl = MN_SAFESTRING_VTBL;
    return mn_SafeString_eq(a.get(), b.get());
}
static inline cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class* p, u32 tbl, void* self) {
    return gabi::call<cPhs_State>(0x02525FE4, p, tbl, self);
}
static inline void fopNpc_npc_c_ct(void* p) { gabi::call(0x025A1458, p); }
static inline void mDoExt_btpAnm_ct(void* p) { gabi::call(0x025E7820, p); }
static inline void dNpc_PathRun_getPoint(MnPathRun_l* r, cXyz* out, u8 idx) { gabi::call(0x0259E778, r, out, idx); }
static inline BOOL dNpc_PathRun_incIdxLoop(MnPathRun_l* r) { return gabi::call<BOOL>(0x0259EB60, r); }
static inline bool dNpc_PathRun_setInf(MnPathRun_l* r, u8 idx, s8 room, u8 fwd) { return gabi::call<bool>(0x0259E6D0, r, idx, room, fwd); }
static inline u8 dNpc_PathRun_maxPoint(MnPathRun_l* r) { return gabi::call<u8>(0x0259EDB8, r); }
/* 025BD64C dSnap_GetFigRoomId(int) */
static inline s32 dSnap_GetFigRoomId(s32 fig) { return gabi::call<s32>(0x025BD64C, fig); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
/* dComIfGp_event_onEventFlag(f): the event control's flag word (play + 0x52B8) |= f */
static inline void dComIfGp_event_onEventFlag(u16 f) {
    u32 play = dComIfGp_ea();
    gabi::store<u16>(play + 0x52B8, gabi::load<u16>(play + 0x52B8) | f);
}
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + 0x5BC5); }
static inline void dComIfGp_setMesgAnimeAttrInfo(u8 v) { gabi::store<u8>(dComIfGp_ea() + 0x5BC5, v); }
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline void* dComIfGp_evmng_getMyFloatP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 0); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz a, cXyz b (pointers to copies), f32* dist, s16* angY) */
static inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* ang) { gabi::call(0x0259D624, a, b, dist, ang); }
static inline BOOL dNpc_PathRun_chkPointPass(MnPathRun_l* p, cXyz* pos, u32 dir) { return gabi::call<BOOL>(0x0259E838, p, pos, dir); }
static inline BOOL dNpc_PathRun_nextIdxAuto(MnPathRun_l* p) { return gabi::call<BOOL>(0x0259ED58, p); }
static inline u8 dNpc_PathRun_pointArg(MnPathRun_l* p, u8 idx) { return gabi::call<u8>(0x0259EE4C, p, idx); }
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
static inline fopAc_ac_c* fopAcM_searchFromName(const char* name, u32 mask, u32 prm) { return gabi::call<fopAc_ac_c*>(0x025D9F38, name, mask, prm); }
static inline BOOL fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D7970, a, b, ev, flag, hind);
}
static inline BOOL fopAcM_orderPotentialEvent(fopAc_ac_c* a, u16 type, u16 flag, u16 p) { return gabi::call<BOOL>(0x025D7B24, a, type, flag, p); }
static inline u32 fopAcM_createItemForPresentDemo(cXyz* pos, s32 itemNo, u8 flag, s32 bitNo, s32 roomNo, csXyz* angle, cXyz* scale) {
    return gabi::call<u32>(0x025D7DEC, pos, itemNo, flag, bitNo, roomNo, angle, scale);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
static inline s32 J3DAnm_getFrameMax(void* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
/* GHS pointer-to-member call without arguments, returning r3 */
static inline s32 mn_pmf_call0(void* self, u32 pmf) {
    s16 i = gabi::load<s16>(pmf + 2);
    u32 thisp = gabi::ea(self) + (s32)gabi::load<s16>(pmf);
    if (i < 0) {
        return gabi::call_ptr<s32>(gabi::load<u32>(pmf + 4), thisp);
    }
    s16 voff = gabi::load<s16>(pmf + 6);
    u32 vt = gabi::load<u32>(thisp + voff);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + i * 8 + 4), thisp);
}
/* HD: the message functions are virtual (vtable at +0xB4): slot 0x14 next_msgStatus, 0x1C
 * getMsg, 0x24 anmAtr(u16) */
static inline u32 mn_vfn(void* self, u32 slot) { return gabi::load<u32>(gabi::load<u32>(gabi::ea(self) + 0xB4) + slot); }
static inline u32 mn_v_next_msgStatus(daNpcMn_c* self, be<u32>* msgNo) { return gabi::call_ptr<u32>(mn_vfn(self, 0x14), self, msgNo); }
static inline u32 mn_v_getMsg(daNpcMn_c* self) { return gabi::call_ptr<u32>(mn_vfn(self, 0x1C), self); }
static inline void mn_v_anmAtr(daNpcMn_c* self, u16 status) { gabi::call_ptr(mn_vfn(self, 0x24), self, status); }
/* HD message manager (*(0x101F4B5C)): 025F795C status, 025F74D0 setStatus, 025F7DB0
 * messageSet(msgNo, cXyz* pos), 025F7E68 scopeMessageSet(msgNo) */
static inline u32 mn_msgManager() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 msgMng_getStatus(u32 mgr) { return gabi::call<u32>(0x025F795C, mgr); }
static inline void msgMng_setStatus(u32 mgr, u32 st) { gabi::call(0x025F74D0, mgr, st); }
static inline u32 msgMng_messageSet(u32 mgr, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mgr, msgNo, pos); }
static inline u32 msgMng_scopeMessageSet(u32 mgr, u32 msgNo) { return gabi::call<u32>(0x025F7E68, mgr, msgNo); }
/* play + 0x5BB3: scope message status */
static inline u8 dComIfGp_getScopeMesgStatus() { return gabi::load<u8>(dComIfGp_ea() + 0x5BB3); }
static inline void dComIfGp_setScopeMesgStatus(u8 v) { gabi::store<u8>(dComIfGp_ea() + 0x5BB3, v); }
/* dComIfGp_checkPlayerStatus0(0, daPyStts0_TELESCOPE_LOOK_e): play + 0x5CD8 & 0x200000 */
static inline BOOL mn_checkTelescopeLook() { return (gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & 0x200000) != 0; }
