/* daNpc_Kf1_c (Mila's father, rich; the Windfall pot shop), WWHD layout.
 *
 * The GameCube decompilation has no layout (all functions are "Nonmatching" stubs; the header has
 * only a dNpc_PathRun_c at GameCube 0x708): the members below are measured from the WWHD code of
 * d_a_npc_kf1 and named after their use (the same family as d_a_npc_aj1). Base fopNpc_npc_c
 * (0x7DC, d/d_npc.h). Members are read up to 0x96E.
 *
 * Also holds the local bindings used by d_a_npc_kf1*.cpp (SHARED-CANDIDATE; most are the same as in
 * d_a_npc_ko1.h / d_a_npc_aj1.cpp). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l (GHS pointer to member, 8 bytes) */
#include "d/d_npc.h"

#define SAFESTRING_VTBL 0x1001B8A0 /* this TU's sead::SafeString vtable */

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_getEventReg(u16 r) { return dSv_event_getEventReg(dComIfGs_event(), r); }
static inline void dComIfGs_setEventReg(u16 r, u8 v) { dSv_event_setEventReg(dComIfGs_event(), r, v); }
/* 025B7D90 dSv_player_collect_c::isSymbol (collect at save + 0xD4) */
static inline BOOL dComIfGs_isSymbol(u8 i) { return gabi::call<BOOL>(0x025B7D90, gabi::load<u32>(0x101F84DC) + 0xD4, i); }
/* rupees: save + 0x24 (u16) */
static inline u16 dComIfGs_getRupee() { return gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x24); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline void dComIfGp_evmng_setGoal(cXyz* pos) { gabi::call(0x02543714, dComIfGp_getPEvtManager(), pos); }
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline void* dComIfGp_evmng_getMyXyzP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 1); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* dComIfGp_getPlayer(0): play + 0x5B2C (the player as the event's partner) */
static inline fopAc_ac_c* kf1_getPlayer() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER)); }
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
static inline s32 J3DAnm_getFrameMax(void* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
static inline s32 cLib_getRndValue(s32 base, s32 range) { return gabi::call<s32>(0x021E1E78, base, range); }
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline u32 dBgS_GetMtrlSndId(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
static inline void* daNpc_gndPoly(fopNpc_npc_c* a) { return gabi::at<u8>(gabi::ea(&a->mObjAcch) + 0xD4 + 0x14); }
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259E2D4 dNpc_JntCtrl_c::lookAtTarget_2(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16 yrot, s16 vel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget_2(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259E2D4, j, outY, target, eye, yrot, vel, headOnly);
}
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }
/* 025D54C4 fopAcM_SearchByID(fpc_ProcID, fopAc_ac_c**) (out of line) */
static inline BOOL fopAcM_SearchByID_out(u32 id, be<u32>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }
/* 025DE508 fpcEx_Search(judge, data) */
static inline void* fpcEx_Search(u32 fn, void* data) { return gabi::call<void*>(0x025DE508, fn, data); }
/* 025D8E6C fopAcM_createItemForKP2(cXyz* pos, int itemNo, int roomNo, csXyz* angle, cXyz* scale, f32 speedF, f32 speedY, f32 gravity, int action) */
static inline fopAc_ac_c* fopAcM_createItemForKP2(cXyz* pos, s32 itemNo, s32 roomNo, csXyz* angle, cXyz* scale, f32 speedF, f32 speedY, f32 gravity, s32 action) {
    return gabi::call<fopAc_ac_c*>(0x025D8E6C, pos, itemNo, roomNo, angle, scale, speedF, speedY, gravity, action);
}
/* dEvt_control_c (play + 0x51D0): 0253F124 getPId; mPtItem at +0xD0 */
static inline void dComIfGp_event_setItemPartner(fopAc_ac_c* a) {
    u32 evt = dComIfGp_ea() + 0x51D0;
    gabi::store<u32>(evt + 0xD0, gabi::call<u32>(0x0253F124, evt, a));
}
/* 0252012C dComIfGp_setNextStage(stage, point, roomNo, layer, speed, mode, setPoint, wipe) */
static inline void dComIfGp_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 speed, u32 mode, s32 setPoint, s8 wipe) {
    gabi::call(0x0252012C, stage, point, roomNo, layer, speed, mode, setPoint, wipe);
}
/* dNpc_PathRun_c */
struct dNpc_PathRun_l {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(dNpc_PathRun_l, 8);
static inline void dNpc_PathRun_setInf(dNpc_PathRun_l* p, u8 path, s8 room, u8 fwd) { gabi::call(0x0259E6D0, p, path, room, fwd); }
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_l* p, cXyz* out, u8 idx) { gabi::call(0x0259E778, p, out, idx); }
static inline BOOL dNpc_PathRun_chkPointPass(dNpc_PathRun_l* p, cXyz* pos, u32 dir) { return gabi::call<BOOL>(0x0259E838, p, pos, dir); }
static inline BOOL dNpc_PathRun_nextIdx(dNpc_PathRun_l* p) { return gabi::call<BOOL>(0x0259ECFC, p); }
static inline BOOL dNpc_PathRun_nextIdxAuto(dNpc_PathRun_l* p) { return gabi::call<BOOL>(0x0259ED58, p); }

/* GHS pointer to member function: load from .data, call */
static inline void pmf_load(ProcFunc_l* dst, u32 src) {
    gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(src));
    gabi::store<u32>(gabi::ea(dst) + 4, gabi::load<u32>(src + 4));
}
static inline void pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}

/* HD J3D joint matrices (see d_a_npc_ba1.cpp) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static inline Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
static inline u32 jntNo_of(J3DNode* node) { return gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4); }
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }

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
    /* 0x04 */ be<s8> m04;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> m08;
    /* 0x0C */ hio_prm_c mPrm;
};
WWHD_SIZE(daNpc_Kf1_HIO_c, 0x3C);
static inline daNpc_Kf1_HIO_c& l_HIO() { return *gabi::at<daNpc_Kf1_HIO_c>(0x104674E0); }

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
    BOOL set_action(ProcFunc_l*, void*);
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
    /* 0x7E8 */ gptr<J3DModel> mpItemModel;          /* "Kf" res 0, carried on the head */
    /* 0x7EC */ char mArcName[4];                     /* "Kf" (decideType) */
    /* 0x7F0 */ u8 mBtpAnm[0x74];                     /* mDoExt_btpAnm (HD 0x74; anm pointer at +0x10) */
    /* 0x864 */ be<u8> mBtpFrame;
    /* 0x865 */ u8 _865;
    /* 0x866 */ be<s16> mBlinkTimer;
    /* 0x868 */ ProcFunc_l mCurrProcFunc;
    /* 0x870 */ be<u32> m870;                         /* -1 in srch_Tsubo */
    /* 0x874 */ be<u32> mLookActorId;                 /* look mode 4 */
    /* 0x878 */ dNpc_PathRun_l mPathRun;
    /* 0x880 */ cXyz mHomePos;
    /* 0x88C */ csXyz mHomeAngle;
    /* 0x892 */ csXyz mAngle;
    /* 0x898 */ u8 _898[8];
    /* 0x8A0 */ cXyz mEyePos;                         /* head joint * (30, 30, 0) */
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
    /* 0x8F0 */ be<s16> mEventIDTbl[3];               /* "angry", "rupee_age", "bensyou" */
    /* 0x8F6 */ be<s16> mEventIndex;
    /* 0x8F8 */ u8 _8F8[4];
    /* 0x8FC */ be<s16> mCutTimer;
    /* 0x8FE */ be<s16> mCutCount;
    /* 0x900 */ be<s16> m900;
    /* 0x902 */ be<s16> mWaitTimer;
    /* 0x904 */ be<s16> mWalkTimer;
    /* 0x906 */ u8 _906[4];
    /* 0x90A */ be<s16> mLookAngleY;
    /* 0x90C */ be<s8> mbMorfAnimStopped;
    /* 0x90D */ be<u8> m90D;
    /* 0x90E */ be<u8> mItemNo;
    /* 0x90F */ be<u8> mSwitchNo;
    /* 0x910 */ be<u8> m910;
    /* 0x911 */ be<u8> m911;
    /* 0x912 */ u8 _912;
    /* 0x913 */ be<u8> mbAcchFlag20;     /* mObjAcch flag 0x20 (ground hit) */
    /* 0x914 */ be<u8> mbAcchFlag1000;   /* mObjAcch flag 0x1000 */
    /* 0x915 */ be<u8> m915;
    /* 0x916 */ be<u8> mbResLoaded;
    /* 0x917 */ be<u8> m917;
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
    /* 0x94C */ be<u32> mKutaniIds[3];                /* three pots picked at random */
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
