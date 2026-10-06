/* daNpcBmcon_c (Willi & Obli, the Flight Control Platform Ritos), WWHD layout, plus the
 * translation-unit statics and local bindings of d_a_npc_bmcon1.
 *
 * The GameCube decompilation has no layout for this class (every function is a "Nonmatching"
 * stub); the layout below is measured from the WWHD code. The GameCube header derives it from
 * fopAc_ac_c, but the constructor (02204CC0) calls fopNpc_npc_c::fopNpc_npc_c (025A1458) and the
 * code uses the fopNpc_npc_c members (m_jnt, mEventCut, mpMorf, mObjAcch, mAcchCir, mStts, mCyl,
 * mCurrMsgNo..): it is an fopNpc_npc_c, like daNpcPeople_c, whose code it closely follows.
 * Size 0x93C (constructor). The HD vtable at 0xB4 is 0x10017CC8. */
#pragma once
#include "bindings.h"

#define BMCON_SAFESTRING_VTBL 0x10017A70 /* this TU's sead::SafeString vtable */
#define BMCON_VTBL 0x10017CC8            /* daNpcBmcon_c vtable (constructor) */

/* animation table entry (3 bytes, lists ended by 0xFF): bck index, morf frames, loop count
 * (<= 0: loop) */
struct sBmconAnmDat {
    /* 0x00 */ be<u8> field_0x00;
    /* 0x01 */ be<u8> field_0x01;
    /* 0x02 */ be<s8> field_0x02;
};
WWHD_SIZE(sBmconAnmDat, 3);

/* per-NPC parameters (l_npc_dat, .rodata 0x101BD1B4, 0x4C bytes per NPC) */
struct daNpcBmcon_c__l_npc_dat {
    /* 0x00 */ be<s16> field_0x00;
    /* 0x02 */ be<s16> field_0x02;
    /* 0x04 */ be<s16> field_0x04;
    /* 0x06 */ be<s16> field_0x06;
    /* 0x08 */ be<s16> field_0x08;
    /* 0x0A */ be<s16> field_0x0A;
    /* 0x0C */ be<s16> field_0x0C;
    /* 0x0E */ be<s16> field_0x0E;
    /* 0x10 */ be<s16> field_0x10;
    /* 0x12 */ u8 _12[2];
    /* 0x14 */ be<f32> field_0x14;       /* eye offset (dNpc_playerEyePos) */
    /* 0x18 */ be<f32> field_0x18;
    /* 0x1C */ be<f32> field_0x1C;
    /* 0x20 */ be<f32> field_0x20;       /* attention distance */
    /* 0x24 */ be<f32> field_0x24;       /* look distance */
    /* 0x28 */ be<s16> field_0x28;       /* attention angle */
    /* 0x2A */ be<s16> field_0x2A;       /* turn speed */
    /* 0x2C */ be<s16> field_0x2C;       /* walk turn speed */
    /* 0x2E */ u8 _2E[2];
    /* 0x30 */ be<f32> field_0x30;       /* collision radius */
    /* 0x34 */ be<f32> field_0x34;       /* animation speed per unit of speedF */
    /* 0x38 */ be<f32> field_0x38;       /* walk speed */
    /* 0x3C */ u8 _3C[0x44 - 0x3C];
    /* 0x44 */ be<s16> field_0x44;       /* turn wait time: minimum */
    /* 0x46 */ be<s16> field_0x46;       /* turn wait time: maximum */
    /* 0x48 */ be<s16> field_0x48;       /* look timer */
    /* 0x4A */ be<u8> field_0x4A;
    /* 0x4B */ be<u8> field_0x4B;
};
WWHD_SIZE(daNpcBmcon_c__l_npc_dat, 0x4C);

/* dNpc_PathRun_c (8 bytes, unchanged) */
struct dNpc_PathRun_bmcon {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(dNpc_PathRun_bmcon, 8);

struct daNpcBmcon_c : fopNpc_npc_c {
    enum Prm_e {
        PRM_RAIL_ID_W = 0x8,
        PRM_RAIL_ID_S = 0x10,
    };
    enum { NPC_BMCON1 = 0, NPC_BMCON2 = 1 }; /* Willi, Obli */

    u8 getNpcNo() { return mNpcNo; }
    request_of_phase_process_class* getPhaseP() { return &mPhase; }

    /* methods (no GameCube layout or bodies: types from the WWHD code) */
    void nodeArmControl(J3DNode*, J3DModel*);
    cPhs_State _create();
    BOOL createHeap();
    cPhs_State createInit();
    bool _delete();
    bool _draw();
    bool _execute();
    u8 executeCommon(); /* returns mTalk */
    void executeSetMode(u32); /* u8, not re-extended */
    s32 executeWaitInit();
    void executeWait();
    s32 executeTalkInit();
    void executeTalk();
    s32 executeWalkInit();
    void executeWalk();
    s32 executeTurnInit();
    void executeTurn();
    void checkOrder();
    void eventOrder();
    void eventMove();
    void privateCut();
    void eventMesSetInit(int);
    bool eventMesSet();
    void eventGetItemInit();
    u16 talk2(int);
    u16 next_msgStatus(be<u32>*);
    u32 getMsg();
    void chkMsg();
    void setMessage(u32);
    void setAnmFromMsgTag();
    u8 getPrmNpcNo();
    u8 getPrmRailID();
    void setMtx();
    void chkAttention();
    void lookBack();
    void playAnm();
    void setAnm(u32, int, f32); /* u8 index, full register */
    bool setAnmTbl(sBmconAnmDat*);
    s16 XyCheckCB(int);
    void setCollision(dCcD_Cyl*, cXyz*, f32, f32); /* cXyz by value: pointer to a copy */
    void calcFlyDist(cXyz* result); /* cXyz returned through a hidden pointer (r4) */
    s16 getFlyDistMax();
    void setFlyDistMax(s32); /* s16, used unextended */
    s16 getFlyDistNow();
    void setFlyDistNow(s32); /* s16, used unextended */
    BOOL chkEndEvent();
    BOOL isClear();

    /* ---- layout ---- */
    /* 0x7DC */ Mtx34 mArmLMtx;                      /* armLloc joint matrix (nodeArmControl) */
    /* 0x80C */ Mtx34 mArmRMtx;                      /* armRloc */
    /* 0x83C */ request_of_phase_process_class mPhase;   /* resource phase */
    /* 0x844 */ request_of_phase_process_class mPhase2;  /* phase handler (_create) */
    /* 0x84C */ gptr<mDoExt_McaMorf> mpArmMorf;
    /* 0x850 */ gptr<J3DModel> mpEtcModel;
    /* 0x854 */ gptr<J3DAnmTexPattern> m_head_tex_pattern;
    /* 0x858 */ u8 mBtpAnm[0x74];                    /* mDoExt_btpAnm (HD 0x74, constructor 025E7820) */
    /* 0x8CC */ dNpc_PathRun_bmcon mPathRun;
    /* 0x8D4 */ u8 field_0x8D4[0x8E0 - 0x8D4];
    /* 0x8E0 */ cXyz mLookAtPos;
    /* 0x8EC */ gptr<sBmconAnmDat> mpAnmDat;
    /* 0x8F0 */ be<u32> field_0x8F0;
    /* 0x8F4 */ be<f32> field_0x8F4;
    /* 0x8F8 */ be<f32> mMorfOverride;               /* < 0: none (setAnm) */
    /* 0x8FC */ be<f32> mAttDist;
    /* 0x900 */ be<u32> field_0x900;
    /* 0x904 */ be<u8> mbHeadOnly;
    /* 0x905 */ u8 _905;
    /* 0x906 */ be<s16> mEventIdx[2];
    /* 0x90A */ be<s16> field_0x90A;
    /* 0x90C */ be<s16> mLookTimer;
    /* 0x90E */ be<s16> mAttAngle;
    /* 0x910 */ be<s16> mHomeAngleY;
    /* 0x912 */ be<u16> field_0x912;
    /* 0x914 */ be<s16> field_0x914;
    /* 0x916 */ be<s16> field_0x916;
    /* 0x918 */ u8 _918[2];
    /* 0x91A */ be<s16> mTurnSpeed;
    /* 0x91C */ be<s16> mTurnStep;
    /* 0x91E */ be<s16> mTargetAngleY;
    /* 0x920 */ be<u8> mTalk;
    /* 0x921 */ be<u8> mbAttention;
    /* 0x922 */ be<u8> mOrderEventNum;
    /* 0x923 */ u8 _923;
    /* 0x924 */ be<s8> m_nec_jnt_num;
    /* 0x925 */ be<s8> m_arm_L_jnt_num;
    /* 0x926 */ be<s8> m_arm_R_jnt_num;
    /* 0x927 */ be<s8> m_armLloc_jnt_num;
    /* 0x928 */ be<s8> m_armRloc_jnt_num;
    /* 0x929 */ be<u8> mExeMode;                     /* index into l_execute (returned by the l_execute_init function) */
    /* 0x92A */ be<u8> mResFlag;
    /* 0x92B */ be<u8> mNpcNo;
    /* 0x92C */ be<u8> field_0x92C;
    /* 0x92D */ be<u8> mAnmIdx;
    /* 0x92E */ be<u8> field_0x92E;
    /* 0x92F */ be<s8> mAnmLoop;
    /* 0x930 */ be<u8> field_0x930;
    /* 0x931 */ u8 _931;
    /* 0x932 */ be<u8> mLookMode;                    /* 0 none, 1 look at mLookAtPos, 2 turn to mTargetAngleY */
    /* 0x933 */ be<u8> mbTurnOnLook;                 /* l_npc_dat 0x4A */
    /* 0x934 */ be<u8> mbLookPlayer;                 /* l_npc_dat 0x4B */
    /* 0x935 */ be<u8> mbFar;
    /* 0x936 */ be<u8> field_0x936;
    /* 0x937 */ be<u8> field_0x937;
    /* 0x938 */ be<u8> field_0x938;
    /* 0x939 */ be<u8> field_0x939;
    /* 0x93A */ u8 _93A[2];
};
WWHD_OFFSET(daNpcBmcon_c, mPhase, 0x83C);
WWHD_OFFSET(daNpcBmcon_c, mBtpAnm, 0x858);
WWHD_OFFSET(daNpcBmcon_c, mPathRun, 0x8CC);
WWHD_OFFSET(daNpcBmcon_c, mpAnmDat, 0x8EC);
WWHD_OFFSET(daNpcBmcon_c, mEventIdx, 0x906);
WWHD_OFFSET(daNpcBmcon_c, mNpcNo, 0x92B);
WWHD_OFFSET(daNpcBmcon_c, field_0x939, 0x939);
WWHD_SIZE(daNpcBmcon_c, 0x93C);

/* ---- translation-unit statics (.data / .rodata addresses) ---- */
enum : u32 {
    BMCON_l_arcname_tbl = 0x101BD050,   /* const char*[2] ("Bmcon1", "Bmcon1") */
    BMCON_l_npc_staff_id = 0x101BD048,  /* const char*[2] ("Bmcon1", "Bmcon2") */
    BMCON_l_npc_anm_tbl = 0x101BD0B8,   /* sBmconAnmDat*[] (createInit: by mAnmIdx) */
    BMCON_l_npc_dat = 0x101BD1B4,       /* daNpcBmcon_c__l_npc_dat[2] */
    BMCON_l_method = 0x101BD24C,        /* _create's phase handler table {phase_1, phase_2, NULL} */
    BMCON_l_bmd_ix_tbl = 0x10017A40,    /* int[2] body */
    BMCON_l_etc_bmd_ix_tbl = 0x10017A48, /* int[2] */
    BMCON_l_arm_bmd_ix_tbl = 0x10017A50, /* int[2] */
    BMCON_l_bck_ix_tbl = 0x10017AA8,    /* int[] body bck by animation index */
    BMCON_l_arm_bck_ix_tbl = 0x10017AC8, /* int[] arm bck by animation index */
};
static inline const char* bmcon_arcname(u32 npcNo) { return gabi::at<const char>(gabi::load<u32>(BMCON_l_arcname_tbl + npcNo * 4)); }
static inline daNpcBmcon_c__l_npc_dat* bmcon_npc_dat(u32 npcNo) { return gabi::at<daNpcBmcon_c__l_npc_dat>(BMCON_l_npc_dat + npcNo * 0x4C); }

/* ---- local bindings (SHARED-CANDIDATE; the same as in d_a_npc_people.h) ---- */
static inline u32 dComIfGs_save() { return gabi::load<u32>(0x101F84DC); }
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline dSv_event_c* dComIfGs_tmpEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178); }
static inline BOOL dComIfGs_isTmpBit(u16 f) { return dSv_event_isEventBit(dComIfGs_tmpEvent(), f); }
static inline void dComIfGs_onTmpBit(u16 f) { dSv_event_onEventBit(dComIfGs_tmpEvent(), f); }
static inline void dComIfGs_offTmpBit(u16 f) { gabi::call(0x025B8B7C, dComIfGs_tmpEvent(), f); } /* dSv_event_c::offEventBit */
static inline u8 dComIfGs_getTmpReg(u16 reg) { return dSv_event_getEventReg(dComIfGs_tmpEvent(), reg); }
static inline void dComIfGs_setTmpReg(u16 reg, u8 v) { dSv_event_setEventReg(dComIfGs_tmpEvent(), reg, v); }
static inline u8 dComIfGp_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
/* resources by id (HD: sead::SafeString key, per-TU vtable) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mStringTop = gabi::ea(arc);
    key->__vtbl = BMCON_SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* 02055B64 cLib_calcTimer<s16> (out of line) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* event manager (play + 0x52C4) */
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline void* dComIfGp_evmng_getMyFloatP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 0); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* 0259D54C dNpc_playerEyePos(f32): cXyz through a hidden result pointer (r3) */
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16 yrot, s16 vel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
/* 0259D624 dNpc_calc_DisXZ_AngY(cXyz, cXyz (pointers to copies), f32* dist, s16* angle) */
static inline void dNpc_calc_DisXZ_AngY(cXyz* a, cXyz* b, be<f32>* dist, be<s16>* ang) { gabi::call(0x0259D624, a, b, dist, ang); }
/* HD message manager (*(0x101F4B5C)) */
static inline u32 bmcon_msgManager() { return gabi::load<u32>(0x101F4B5C); }
/* J3DModelData (HD) joint names: 027F68FC returns the joint name table header */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
/* J3DModelData (HD): 027F3F94 (the matcher calls it __nw) returns the joint tree header (joint
 * count u16 at +8); joint nodes: array of 0x1C bytes at +8 (count at +4; an index past the end
 * gives element 0), callback at +8 */
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
struct J3DMtxBlock_bmcon {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
/* 02525FE4 dComLbG_PhaseHandler(phase, table, this) */
static inline cPhs_State dComLbG_PhaseHandler(request_of_phase_process_class* p, u32 tbl, void* self) {
    return gabi::call<cPhs_State>(0x02525FE4, p, tbl, self);
}
/* 025A1458 fopNpc_npc_c::fopNpc_npc_c, 025E7820 mDoExt_btpAnm::mDoExt_btpAnm */
static inline void fopNpc_npc_c_ct(void* p) { gabi::call(0x025A1458, p); }
static inline void mDoExt_btpAnm_ct(void* p) { gabi::call(0x025E7820, p); }
/* dNpc_PathRun_c */
static inline bool dNpc_PathRun_setInf(dNpc_PathRun_bmcon* r, u8 idx, s8 room, u8 fwd) { return gabi::call<bool>(0x0259E6D0, r, idx, room, fwd); }
static inline BOOL dNpc_PathRun_chkPointPass(dNpc_PathRun_bmcon* r, cXyz* pos, u8 dir) { return gabi::call<BOOL>(0x0259E838, r, pos, dir); } /* cXyz by value */
static inline BOOL dNpc_PathRun_nextIdxAuto(dNpc_PathRun_bmcon* r) { return gabi::call<BOOL>(0x0259ED58, r); }
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_bmcon* r, cXyz* out, u8 idx) { gabi::call(0x0259E778, r, out, idx); }
static inline BOOL dNpc_PathRun_incIdxLoop(dNpc_PathRun_bmcon* r) { return gabi::call<BOOL>(0x0259EB60, r); }
/* 025D7970 fopAcM_orderChangeEventId(actor, partner, s16 eventIdx, u16 flag, u16 hind) */
static inline BOOL fopAcM_orderChangeEventId(fopAc_ac_c* a, fopAc_ac_c* b, s16 ev, u16 flag, u16 hind) {
    return gabi::call<BOOL>(0x025D7970, a, b, ev, flag, hind);
}
/* 025DD868 fpcM_IsCreating(id) */
static inline BOOL fpcM_IsCreating(u32 id) { return gabi::call<BOOL>(0x025DD868, id); }
