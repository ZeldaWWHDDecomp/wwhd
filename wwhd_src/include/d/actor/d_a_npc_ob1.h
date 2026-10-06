/* daNpc_Ob1_c (Rose, Outset), WWHD layout. 
 *
 * The GameCube header is empty (the TU is "Nonmatching"): members are named from their use in the
 * WWHD code. Size 0x97C (profile 101C2828). Base: fopNpc_npc_c, HD size 0x7DC (constructor
 * 025A1458 allocates 0x7DC; the GameCube vtable pointer at 0x6C0 is gone, the virtuals
 * next_msgStatus/getMsg/anmAtr are slots 2..4 of the actor's vtable at +0xB4).
 * The shared d_npc.h declares fopNpc_npc_c as 0x7E0 with a vtable word at 0x7DC and msg_class*
 * at 0x7CC: wrong, see fopNpc_npc_c_l below (SHARED-CANDIDATE). */
#pragma once
#include "bindings.h"

/* ---- fopNpc_npc_c, HD (SHARED-CANDIDATE: corrects d/d_npc.h) ----
 * Constructor 025A1458 (the matcher names it cDyl_LinkASync): fopAc_ac_c, vtable 10051858,
 * dNpc_JntCtrl_c 0x3AC, dNpc_EventCut_c 0x3E0, mpMorf 0x44C = NULL, dBgS_ObjAcch 0x450,
 * dBgS_AcchCir 0x614, dCcD_Stts 0x654, dCcD_Cyl 0x690, then 0x7C0/0x7C4/0x7D0 = 0, 0x7C8 = -1,
 * 0x7CC (u8) = 0, 0x7D4/0x7D6 (u16) = 0, 0x7D8 (u8) = 0. */
struct fopNpc_npc_c_l : fopAc_ac_c {
    /* 0x3AC */ dNpc_JntCtrl_c m_jnt;
    /* 0x3E0 */ dNpc_EventCut_c mEventCut;
    /* 0x44C */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x450 */ dBgS_ObjAcch mObjAcch;
    /* 0x614 */ dBgS_AcchCir mAcchCir;
    /* 0x654 */ dCcD_Stts mStts;
    /* 0x690 */ dCcD_Cyl mCyl;
    /* 0x7C0 */ be<u32> mCurrMsgNo;
    /* 0x7C4 */ be<u32> mEndMsgNo;
    /* 0x7C8 */ be<u32> mCurrMsgBsPcId;
    /* 0x7CC */ be<u8> mbCurrMsg;          /* HD: GameCube msg_class* mpCurrMsg; the status is fetched by id */
    /* 0x7CD */ u8 _7CD[3];
    /* 0x7D0 */ be<u32> mPartnerId;        /* GameCube field_0x6b4 (u8[6]): a process id, then a u16 */
    /* 0x7D4 */ be<u16> mPartnerMsgStts;
    /* 0x7D6 */ be<u16> field_0x7d6;       /* GameCube field_0x6ba */
    /* 0x7D8 */ be<u8> mManzaiStt;         /* GameCube field_0x6bc */
    /* 0x7D9 */ u8 _7D9[3];

    /* 025A15AC [v] */
    void setCollision(f32 r, f32 h) { gabi::call(0x025A15AC, this, r, h); }
    /* 025A11EC [v] */
    u16 talk(s32 p) { return gabi::call<u16>(0x025A11EC, this, p); }
};
WWHD_OFFSET(fopNpc_npc_c_l, mObjAcch, 0x450);
WWHD_OFFSET(fopNpc_npc_c_l, mCyl, 0x690);
WWHD_OFFSET(fopNpc_npc_c_l, mCurrMsgNo, 0x7C0);
WWHD_OFFSET(fopNpc_npc_c_l, mManzaiStt, 0x7D8);
WWHD_SIZE(fopNpc_npc_c_l, 0x7DC);

/* dNpc_PathRun_c (8 bytes, unchanged) */
struct dNpc_PathRun_c_l {
    /* 0x0 */ gptr<dPath> mPath;
    /* 0x4 */ be<u8> field_0x04;
    /* 0x5 */ be<u8> mIdx;
    /* 0x6 */ be<u8> mbDir;
    /* 0x7 */ be<u8> field_0x07;
};
WWHD_SIZE(dNpc_PathRun_c_l, 8);

/* GHS pointer to member function: {s16 this delta, s16 vtable index (0: NULL, < 0: not virtual),
 * u32 function (or, low half, the vtable offset)} */
struct ptmf_l {
    /* 0x0 */ be<s16> delta;
    /* 0x2 */ be<s16> idx;
    /* 0x4 */ be<u32> fn;
};
WWHD_SIZE(ptmf_l, 8);

/* ---- local bindings used by d_a_npc_ob1 and d_a_npc_yw1 ---- */
/* (SHARED-CANDIDATE) */
/* 026067F4 HD: dRes_control_c::getIDRes(const sead::SafeString& arc, s32 index) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 index, u32 safestring_vtbl) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = safestring_vtbl;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), index);
}
/* 027F3F94 (matcher: __nw) / 027F68FC: self-relative pointer at *data + 0xC (two copies) */
static inline u32 J3DModelData_jointTable(J3DModelData* d) { return gabi::call<u32>(0x027F3F94, d); }
static inline u32 J3DModelData_jointTable2(J3DModelData* d) { return gabi::call<u32>(0x027F68FC, d); }
/* J3DModelData::getJointName()->getIndex(name): name table self-relative at +0x10 of the joint block */
static inline s32 J3DModelData_getJointIndex(J3DModelData* d, const char* name) {
    u32 blk = J3DModelData_jointTable2(d);
    u32 off = gabi::load<u32>(blk + 0x10);
    u32 tab = off != 0 ? blk + 0x10 + off : 0;
    return gabi::call<s32>(0x027DF9B0, tab, name); /* JUTNameTab::getIndex */
}
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(J3DModelData_jointTable(d) + 8); }
/* getJointNodePointer(i)->setCallBack(cb): HD nodes of 0x1C bytes from +8, index checked against +4 */
static inline void J3DModelData_setJointCallBack(u32 data, u32 i, u32 cb) {
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
static inline J3DModel* J3DModel_get(gptr<mDoExt_McaMorf>& m) { return m->getModel(); }
static inline u32 J3DModel_modelData(J3DModel* m) { return gabi::load<u32>(gabi::ea(m) + 0xAC); }
/* J3DModel::getAnmMtx (HD: joint matrices in a block at +0x2C, marked dirty) */
static inline Mtx34* J3DModel_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jnt * 0x30);
}
#define J3DSys_mCurrentMtx gabi::at<Mtx34>(0x104B4868)
static inline u32 j3dSys_getModel() { return gabi::load<u32>(0x104B462C); }
/* J3DAnmTexPattern::getFrameMax: HD virtual (vtable at +4, slot +0x14) */
static inline s32 J3DAnmTexPattern_getFrameMax(J3DAnmTexPattern* p) {
    return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(p) + 4) + 0x14), p);
}
static inline BOOL mDoExt_btpAnm_init(void* a, u32 d, J3DAnmTexPattern* btp, s32 anmPlay, s32 mode, f32 rate, s16 start,
                                      s16 end, s32 modify, s32 entry) {
    return gabi::call<BOOL>(0x025E789C, a, d, btp, anmPlay, mode, rate, start, end, modify, entry);
}
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_getEventReg(u16 r) { return dSv_event_getEventReg(dComIfGs_event(), r); }
static inline void dComIfGs_setEventReg(u16 r, u8 v) { dSv_event_setEventReg(dComIfGs_event(), r, v); }
enum { dSv_evtReg_OB1_PIGS = 0xB6FF };
/* 02055B64 cLib_calcTimer<s16> (out of line, shared) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 021E1E78 cLib_getRndValue<int>(min, range) = min + cM_rndF(range) (out of line, shared) */
static inline s32 cLib_getRndValue(s32 min, s32 range) { return gabi::call<s32>(0x021E1E78, min, range); }
/* the ground polygon: dBgS_GndChk's cBgS_PolyInfo (+0x14) */
static inline void* gnd_poly(dBgS_ObjAcch& a) { return gabi::at<void>(gabi::ea(a.m_gnd) + 0x14); }
static inline s32 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s32>(0x024EF130, bgs, poly); }
static inline s32 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<s32>(0x024EEEB8, bgs, poly); }
static inline void dNpc_PathRun_setInf(dNpc_PathRun_c_l* p, u8 path, s8 room, u8 fwd) { gabi::call(0x0259E6D0, p, path, room, fwd); }
static inline dPath* dNpc_PathRun_nextPath(dNpc_PathRun_c_l* p, s8 room) { return gabi::call<dPath*>(0x0259E744, p, room); }
/* getPoint returns a cXyz: hidden result pointer after `this` */
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_c_l* p, cXyz* out, u8 idx) { gabi::call(0x0259E778, p, out, idx); }
static inline BOOL dNpc_PathRun_chkPointPass(dNpc_PathRun_c_l* p, cXyz* pos, bool dir) { return gabi::call<BOOL>(0x0259E838, p, pos, dir); }
static inline BOOL dNpc_PathRun_nextIdxAuto(dNpc_PathRun_c_l* p) { return gabi::call<BOOL>(0x0259ED58, p); }
static inline u8 dNpc_PathRun_maxPoint(dNpc_PathRun_c_l* p) { return gabi::call<u8>(0x0259EDB8, p); }
/* dNpc_playerEyePos returns a cXyz through a hidden result pointer (d_npc.h has it as f32: wrong) */
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16, s16, bool) */
static inline void lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* dst, cXyz* eye, s16 defY, s16 maxVel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, dst, eye, defY, maxVel, headOnly);
}
static inline void setActorInfo2(dNpc_EventCut_c* c, const char* name, fopAc_ac_c* a) { gabi::call(0x0259F814, c, name, a); }
static inline s32 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 table, s32 num, s32 force, s32 last) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, table, num, force, last);
}
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
/* dDemo: the demo object pointer (101D5FFC), getActor(u8 id), getP_BtpData(arc), setDemoData */
static inline void* dDemo_object_getActor(u32 obj, u8 id) { return gabi::call<void*>(0x02526E70, obj, id); }
static inline J3DAnmTexPattern* dDemo_actor_getP_BtpData(void* ac, const char* arc) {
    return gabi::call<J3DAnmTexPattern*>(0x02527828, ac, arc);
}
static inline BOOL dDemo_setDemoData(fopAc_ac_c* a, u8 flags, mDoExt_McaMorf* m, const char* arc, s32 n, u32 p, u32 q, s8 r) {
    return gabi::call<BOOL>(0x02527028, a, flags, m, arc, n, p, q, r);
}
/* dAttention_c (play+0x5804): LockonTruth, LockonTarget(i) (matcher: ActionTarget), ActionTarget(i)
 * (matcher: LockonTarget) */
static inline BOOL dAttention_LockonTruth(dAttention_c* a) { return gabi::call<BOOL>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
/* 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out) (out of line) */
static inline BOOL fopAcM_SearchByID2(fpc_ProcID id, be<u32>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }
/* HD messages: the manager (*101F4B5C) */
static inline u32 l_msgMng() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 fopMsgM_SearchByID(u32 mng) { return gabi::call<u32>(0x025F795C, mng); }
static inline u32 fopMsgM_messageSet(u32 mng, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mng, msgNo, pos); }
/* play+0x5C30: {?, s32 n, fopAc_ac_c* list[] (1-based)}: the current talk partner */
static inline u32 dComIfGp_talkActor(u32 play) {
    u32 p = play + 0x5C30;
    return gabi::load<u32>(p + 4 * gabi::load<s32>(p + 4) + 4);
}
static inline void memcpy_l(u32 dst, u32 src, u32 n) { gabi::call(0xC000A848, dst, src, n); } /* memcpy (import) */
/* GHS pointer-to-member call with one argument */
static inline s32 ptmf_call1(ptmf_l* f, void* self, void* arg) {
    s16 idx = f->idx;
    void* p = gabi::at<void>(gabi::ea(self) + (s16)f->delta);
    if (idx < 0) return gabi::call_ptr<s32>(f->fn, p, arg);
    u32 vt = gabi::load<u32>(gabi::ea(p) + gabi::load<s16>(gabi::ea(f) + 6));
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + idx * 8 + 4), p, arg);
}
/* virtuals (actor vtable at +0xB4: getMsg +0x1C, anmAtr +0x24) */
static inline u32 vcall_getMsg(fopNpc_npc_c_l* a) { return gabi::call_ptr<u32>(gabi::load<u32>(a->__vtbl + 0x1C), a); }
static inline void vcall_anmAtr(fopNpc_npc_c_l* a, u16 st) { gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x24), a, st); }

/* mDoExt_btpAnm, HD 0x74 (constructor 025E7820) */
struct mDoExt_btpAnm_ob1 {
    u8 _[0x74];
};

struct daNpc_Ob1_c : fopNpc_npc_c_l {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNo;
        /* 0x01 */ be<s8> mTexNo;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
        /* 0x10 */ be<s32> field_0x10;
    };

    void nodeOb1Control(J3DNode* node, J3DModel* model);
    J3DModelData* create_Anm();
    J3DModelData* create_hed_Mdl();
    s32 btpNum_toResID(int num);
    BOOL setBtp(s32 modify, int num);
    BOOL iniTexPttrnAnm(s32 modify);
    BOOL CreateHeap();
    BOOL charDecide(int type);
    BOOL set_action(ptmf_l* action, void* arg);
    BOOL init_OB1_0();
    BOOL init_OB1_1();
    BOOL init_OB1_2();
    void plyTexPttrnAnm();
    void setAttention(s32 force);
    void setMtx(s32 force);
    BOOL createInit();
    cPhs_State _create();
    BOOL _delete();
    void partner_srch();
    void checkOrder();
    u8 demo();
    s32 isEventEntry();
    void endEvent();
    void event_actionInit(int staffIdx);
    BOOL event_action();
    void privateCut(int staffIdx);
    void lookBack();
    void event_proc(int staffIdx);
    void ob_clcMovSpd();
    s32 ob_movPass();
    void ob_nMove();
    void eventOrder();
    BOOL _execute();
    BOOL _draw();
    s32 anmNum_toResID(int num);
    u32 setAnm_tex(s8 tex);
    BOOL setAnm_anm(anm_prm_c* prm);
    void setAnm_NUM(int num, int withTex);
    BOOL setAnm();
    void setAnm_ATR(int withTex);
    void chg_anmAtr(u8 atr);
    void control_anmAtr();
    void anmAtr(u16 msgStatus);
    BOOL chk_talk();
    u8 chk_partsNotMove();
    u16 next_msgStatus(u32* pMsgNo);
    u32 getMsg_OB1_0();
    u32 getMsg_OB1_1();
    u32 getMsg_OB1_2();
    u32 getMsg();
    BOOL chkAttention();
    fopAc_ac_c* searchByID(fpc_ProcID id);
    s8 bitCount(u8 bits);
    void set_pigCnt();
    void ob_setPthPos();
    void get_attPos(cXyz* out);
    void clrSpd();
    void setStt(s8 stt);
    BOOL wait_1();
    BOOL wait_2();
    BOOL wait_3();
    BOOL walk_1();
    BOOL talk_1();
    BOOL manzai();
    BOOL wait_action1(void*);
    BOOL wait_action2(void*);

    s16 eventIdx(s32 i) { return gabi::load<s16>(gabi::ea(this) + 0x940 + 2 * i); }

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;
    /* 0x7E6 */ u8 _7E6[2];
    /* 0x7E8 */ gptr<J3DModel> mpHedModel;
    /* 0x7EC */ gptr<J3DAnmTexPattern> m_hed_tex_pttrn;
    /* 0x7F0 */ mDoExt_btpAnm_ob1 mBtpAnm;
    /* 0x864 */ be<u8> mBtpFrame;
    /* 0x865 */ u8 _865;
    /* 0x866 */ be<s16> mBlinkTimer;
    /* 0x868 */ ptmf_l mAction;
    /* 0x870 */ dNpc_PathRun_c_l mPathRun;
    /* 0x878 */ dNpc_EventCut_c mEventCut2;   /* the TU's own event cut ("Ob1") */
    /* 0x8E4 */ be<u32> field_0x8e4;           /* a process id (searched in setStt, result unused) */
    /* 0x8E8 */ cXyz mHomePos;
    /* 0x8F4 */ csXyz mHomeAngle;
    /* 0x8FA */ csXyz mAngle;
    /* 0x900 */ cXyz mEyePos;
    /* 0x90C */ cXyz mLookPos;
    /* 0x918 */ cXyz mTargetPos;
    /* 0x924 */ be<f32> mPrevFrame;
    /* 0x928 */ be<f32> mSpdTarget;
    /* 0x92C */ be<f32> mSpdStep;
    /* 0x930 */ be<f32> mPassDist;
    /* 0x934 */ be<s16> mSaveHeadY;
    /* 0x936 */ be<s16> mSaveBboneY;
    /* 0x938 */ be<s16> mSaveAngleY;
    /* 0x93A */ u8 _93A[2];
    /* 0x93C */ be<u32> mMsgNo;
    /* 0x940 */ be<s16> mEventIdx[1];
    /* 0x942 */ be<s16> mEvtNo;
    /* 0x944 */ u8 _944[2];
    /* 0x946 */ be<s16> mTimer;
    /* 0x948 */ u8 _948[2];
    /* 0x94A */ be<s16> mWaitTimer;
    /* 0x94C */ u8 _94C[2];
    /* 0x94E */ be<s16> mHeadTurnSpd;
    /* 0x950 */ be<s16> mTargetAngY;
    /* 0x952 */ be<u16> mMsgStatus;
    /* 0x954 */ be<s8> mAnmEnd;
    /* 0x955 */ be<s8> mLoopCnt;
    /* 0x956 */ u8 _956;
    /* 0x957 */ be<s8> mPigCntGot;     /* pigs already caught (event register bits) */
    /* 0x958 */ be<s8> mPigCntHere;    /* pigs on the map */
    /* 0x959 */ be<s8> mPigCntNew;     /* pigs on the map not yet caught */
    /* 0x95A */ be<s8> mTalkStep;
    /* 0x95B */ be<u8> mPreItemNo;
    /* 0x95C */ be<u8> m95C;
    /* 0x95D */ be<u8> m95D;
    /* 0x95E */ be<u8> m95E;
    /* 0x95F */ be<u8> m95F;
    /* 0x960 */ be<u8> m960;
    /* 0x961 */ be<u8> m961;
    /* 0x962 */ be<u8> m962;
    /* 0x963 */ be<u8> mbInit;
    /* 0x964 */ be<s32> mActRet;
    /* 0x968 */ be<u8> mbAttention;
    /* 0x969 */ be<u8> mbTalk;
    /* 0x96A */ be<u8> mbHeadOnly;
    /* 0x96B */ be<u8> mbDemo;
    /* 0x96C */ be<s8> mMoveMode;
    /* 0x96D */ u8 _96D;
    /* 0x96E */ be<s8> mActIdx;
    /* 0x96F */ be<s8> mEvtActNo;
    /* 0x970 */ be<u8> mAnmAtr;
    /* 0x971 */ be<u8> mAnmTag;
    /* 0x972 */ be<s8> mTexNo;
    /* 0x973 */ be<s8> mAnmNo;
    /* 0x974 */ be<s8> mOrderType;
    /* 0x975 */ be<s8> mStt;
    /* 0x976 */ be<s8> mPrevStt;
    /* 0x977 */ be<s8> mLookMode;
    /* 0x978 */ be<u8> field_0x978;
    /* 0x979 */ be<s8> mType;
    /* 0x97A */ be<s8> mActStep;
    /* 0x97B */ be<s8> mAtrCnt;
};
WWHD_OFFSET(daNpc_Ob1_c, mBtpFrame, 0x864);
WWHD_OFFSET(daNpc_Ob1_c, mEventCut2, 0x878);
WWHD_OFFSET(daNpc_Ob1_c, mHomePos, 0x8E8);
WWHD_OFFSET(daNpc_Ob1_c, mMsgNo, 0x93C);
WWHD_OFFSET(daNpc_Ob1_c, mActRet, 0x964);
WWHD_SIZE(daNpc_Ob1_c, 0x97C);

/* l_HIO (0x10467D50, 0x3C): vtable, s8 mNo, s32 field_0x8, hio_prm_c (0x30, copied from
 * a_prm_tbl 101C27D0) */
struct daNpc_Ob1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> field_0x8;
    /* 0x0C */ be<s16> mPrm[9];          /* jnt setParam: max bbone x/y, min bbone x/y, max head x/y, min head x/y, turn step */
    /* 0x1E */ be<s16> mHeadTurnSpd;     /* lookBack */
    /* 0x20 */ be<f32> mAttnYOffset;
    /* 0x24 */ be<u8> mDebugDraw;
    /* 0x25 */ u8 _25[3];
    /* 0x28 */ be<s16> mWalkTurnStep;
    /* 0x2A */ u8 _2A[2];
    /* 0x2C */ be<f32> mWalkAnmRate;
    /* 0x30 */ be<f32> mWalkSpd;
    /* 0x34 */ be<f32> mWalkAccel;
    /* 0x38 */ be<f32> mPassDist;
};
WWHD_SIZE(daNpc_Ob1_HIO_c, 0x3C);
inline daNpc_Ob1_HIO_c& l_HIO_ob1() { return *gabi::at<daNpc_Ob1_HIO_c>(0x10467D50); }
