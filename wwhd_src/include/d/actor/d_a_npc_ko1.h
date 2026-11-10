/* daNpc_Ko1_c (Joel & Zill, Outset), WWHD layout. 
 *
 * The GameCube header has no members (0x6C4..0x8AC unknown); the layout is measured from the
 * WWHD code. Size 0xA1C (profiles NPC_KO1/NPC_KO2 at 0x101BFD4C/0x101BFD7C; GameCube 0x8AC).
 * Base fopNpc_npc_c (0x7DC, d/d_npc.h). Fields whose role is not known are named by offset.
 *
 * Also holds the local bindings used by d_a_npc_ko1*.cpp and d_a_npc_ym1*.cpp (each .cpp defines
 * SAFESTRING_VTBL, its TU's sead::SafeString vtable, before including this header). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l (GHS pointer to member, 8 bytes) */
#include "d/d_npc.h"

#ifndef SAFESTRING_VTBL
#error "define SAFESTRING_VTBL (this TU's sead::SafeString vtable) before including d_a_npc_ko1.h"
#endif

/* ---- local bindings (SHARED-CANDIDATE; most are the same as in d_a_npc_ba1.cpp / ls1.cpp) ---- */
/* save info event flags: *(0x101F84DC) + 0x644 */
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGs_checkCollect(int i) { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0xD4 + i); }
/* temporary event flags (dSv_event_c at save + 0x1178) */
static inline dSv_event_c* dComIfGs_tmpEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x1178); }
static inline BOOL dComIfGs_isTmpBit(u16 f) { return dSv_event_isEventBit(dComIfGs_tmpEvent(), f); }
static inline void dComIfGs_onTmpBit(u16 f) { dSv_event_onEventBit(dComIfGs_tmpEvent(), f); }
static inline void dComIfGs_offTmpBit(u16 f) { gabi::call(0x025B8B7C, dComIfGs_tmpEvent(), f); }
/* play object fields (dComIfGp_get() at each use) */
static inline u8 dComIfGp_getSelectItem(int btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline u8 dComIfGp_event_getTalkXYBtn() { return gabi::load<u8>(dComIfGp_ea() + 0x52B0); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(dComIfGp_event_getTalkXYBtn() - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
static inline u8 dComIfGp_getMesgAnimeTagInfo() { return gabi::load<u8>(dComIfGp_ea() + 0x5BC6); }
static inline void dComIfGp_clearMesgAnimeTagInfo() { gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF); }
/* resources by id (HD: sead::SafeString key, per-TU vtable) */
static inline void* dComIfG_getObjectIDRes(const char* arc, s32 id) {
    gabi::Local<SafeString> key;
    key->mTextPtr = gabi::ea(arc);
    key->__vtbl = SAFESTRING_VTBL;
    return gabi::call<void*>(0x026067F4, dComIfG_resControl(), key.get(), id);
}
/* J3DAnmTexPattern/J3DAnmBase::getFrameMax: virtual (vtable pointer at +4, slot +0x14) */
static inline s32 J3DAnm_getFrameMax(void* p) {
    u32 vt = gabi::load<u32>(gabi::ea(p) + 4);
    return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), p);
}
/* 02055B64 cLib_calcTimer<s16> (out-of-line copy of another TU) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* 021E1E78 cLib_getRndValue<int>(base, range) */
static inline s32 cLib_getRndValue(s32 base, s32 range) { return gabi::call<s32>(0x021E1E78, base, range); }
/* 025E789C mDoExt_btpAnm::init / 025E7CE0 mDoExt_btkAnm::init */
static inline s32 mDoExt_btpAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E789C, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline s32 mDoExt_btkAnm_init(void* anm, J3DModelData* d, void* p, s32 play, s32 attr, f32 rate, s32 start, s32 end, u32 modify, s32 entry) {
    return gabi::call<s32>(0x025E7CE0, anm, d, p, play, attr, rate, start, end, modify, entry);
}
static inline J3DModelData* J3DModel_getModelData_l(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* dAttention_c (play + 0x5804): 024EC8D0 LockonTarget, 024EE464 ActionTarget */
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
/* dBgS (play + 0x12A0) */
static inline s8 dBgS_GetRoomId(dBgS* bgs, void* poly) { return gabi::call<s8>(0x024EF130, bgs, poly); }
static inline u8 dBgS_GetPolyColor(dBgS* bgs, void* poly) { return gabi::call<u8>(0x024EEEB8, bgs, poly); }
static inline u32 dBgS_GetMtrlSndId(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
/* event manager (play + 0x52C4) */
static inline void* dComIfGp_evmng_getMyIntegerP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 3); }
static inline void* dComIfGp_evmng_getMyFloatP(s32 staffId, const char* name) { return dComIfGp_evmng_getMySubstanceP(staffId, name, 0); }
static inline void dComIfGp_evmng_setGoal(cXyz* pos) { gabi::call(0x02543714, dComIfGp_getPEvtManager(), pos); }
static inline s8 dComIfGp_evmng_getMyActIdx(s32 staffId, u32 tbl, s32 n, BOOL force, s32 nameType) {
    return gabi::call<s8>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
/* 0259D54C dNpc_playerEyePos(f32): cXyz through a hidden result pointer (r3) */
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
/* J3DModelData (HD): joint name table / joint count (see d_a_npc_ba1.cpp) */
static inline u32 J3DModelData_getJointName(J3DModelData* d) {
    u32 h = gabi::call<u32>(0x027F68FC, d);
    u32 off = gabi::load<u32>(h + 0x10);
    return off != 0 ? h + 0x10 + off : 0;
}
static inline s8 JUTNameTab_getIndex(u32 tab, const char* name) { return gabi::call<s8>(0x027DF9B0, tab, name); }
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::call<u32>(0x027F3F94, d) + 8); }
/* HD message manager (*(0x101F4B5C)): 025F795C returns the current message's status */
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }
/* cLib_addCalc(value, target, scale, maxStep, minStep) */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
static inline void mDoMtx_XYZrotM(Mtx34* m, s16 x, s16 y, s16 z) { gabi::call(0x025F19F8, m, x, y, z); }

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

/* HD J3D: j3dSys.mModel at 0x104B462C, J3DSys::mCurrentMtx at 0x104B4868; a model's joint
 * matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static inline Mtx34* getAnmMtx(J3DModel* model, s32 jntNo) {
    J3DMtxBlock_l* blk = gabi::at<J3DMtxBlock_l>(gabi::load<u32>(gabi::ea(model) + 0x2C));
    blk->mFlags |= 0x10; /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jntNo * 0x30);
}
static inline u32 jntNo_of(J3DNode* node) { return gabi::load<u16>(gabi::ea(J3DNode_toJoint(node)) + 4); }
static inline Mtx34* j3dSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline J3DModel* j3dSys_mModel() { return gabi::at<J3DModel>(gabi::load<u32>(0x104B462C)); }

/* dNpc_PathRun_c (HD 8 bytes, as on GameCube) */
struct dNpc_PathRun_l {
    /* 0x00 */ gptr<dPath> mPath;
    /* 0x04 */ be<u8> field_0x04;
    /* 0x05 */ be<u8> mIdx;
    /* 0x06 */ be<u8> mbDir;
    /* 0x07 */ be<u8> field_0x07;
};
WWHD_SIZE(dNpc_PathRun_l, 8);

/* dPa_rippleEcallBack (0x14, HD: vtable at +0; constructor 025A9084) */
struct dPa_rippleEcallBack_l {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<u32> m04;
    /* 0x08 */ be<u32> m08;
    /* 0x0C */ be<u32> m0C;
    /* 0x10 */ be<f32> m10;
};
WWHD_SIZE(dPa_rippleEcallBack_l, 0x14);

struct daNpc_Ko1_c : fopNpc_npc_c {
    struct anm_prm_c {
        /* 0x00 */ be<s8> mAnmNum;  /* was mA0F: setAnm_anm 02276B9C: the body animation number (returns at once when it is unchanged; also picks the head animation); setMtx 02274A7C plays a sound in animation 4; createInit 02274E28 0xE; same code as daNpc_Bm1_c::mAnmNum (tww Bm1 setAnm_anm mAnmNum = anmNum) */
        /* 0x01 */ be<s8> mBtpNum;
        /* 0x02 */ u8 _02[2];
        /* 0x04 */ be<f32> mMorf;
        /* 0x08 */ be<f32> mSpeed;
        /* 0x0C */ be<s32> mLoopMode;
        /* 0x10 */ u8 _10[4]; /* HD: the tables (0x101BF7D8, ...) have a 0x14 stride */
    };

    /* Methods, one section per part file. Each part owns its section: it may fix the
     * declarations there (return types, parameter types) and add new ones (unnamed functions).
     * Return types marked unchecked come from mkbind.py hints. */
    /* ---- part A (d_a_npc_ko1.cpp) ---- */
    void nodeHedControl(J3DNode*, J3DModel*); /* 022732F0 */
    void nodeBlnControl(J3DNode*, J3DModel*); /* 022733C4 */
    void nodeKo1Control(J3DNode*, J3DModel*); /* 0227351C */
    J3DModelData* create_Anm(); /* 022736FC */
    J3DModelData* create_hed_Anm(); /* 02273944 */
    s32 btpNum_toResID(s32); /* 02273B08 */
    bool setBtp(u32, s32); /* 02273B80 */
    u32 iniTexPttrnAnm(u32); /* 02273C88 */
    J3DModelData* create_bln_Anm(); /* 02273C94 */
    bool create_itm_Mdl(); /* 02273E7C */
    bool CreateHeap(); /* 02273F48 */
    bool charDecide(int); /* 02274250 */
    BOOL set_action(ProcFunc_l*, void*); /* 022742C0 */
    bool init_HNA_0(); /* 022743EC */
    bool init_HNA_1(); /* 02274498 */
    bool init_HNA_2(); /* 0227452C */
    bool init_HNA_3(); /* 022745B4 */
    bool init_HNA_4(); /* 02274658 */
    bool init_BOU_0(); /* 022746E8 */
    bool init_BOU_1(); /* 02274778 */
    bool init_BOU_2(); /* 02274808 */
    bool init_BOU_3(); /* 022748B0 */
    void plyTexPttrnAnm(); /* 02274948 */
    void setAttention(u32); /* 02274A1C */
    /* ---- end of part A ---- */
    /* ---- part B (d_a_npc_ko1_b.cpp) ---- */
    void setMtx(u32); /* 02274A7C */
    bool createInit(); /* 02274E28 */
    cPhs_State _create(); /* 02275180 */
    BOOL _delete(); /* 022752D4 */
    u32 partner_srch_sub(u32 /* fpcM_SearchFunc */); /* 02275358: process id or -1 */
    void partner_srch(); /* 022753EC */
    void checkOrder(); /* 02275504 */
    u8 demo(); /* 02275544 */
    s32 isEventEntry(); /* 022756F8 */
    void endEvent(); /* 02275738 */
    void event_actionInit(s32); /* 0227577C */
    BOOL event_action(); /* 022757DC */
    void privateCut(s32); /* 022757E4 */
    void lookBack(); /* 022758B8 */
    void event_proc(s32); /* 02275B28 */
    f32 chk_ForwardGroundY(s16); /* 02275B80 */
    f32 chk_wallJump(s16); /* 02275D28 */
    void chk_routeAngle(cXyz*, be<s16>*); /* 02275D90 */
    void routeWallCheck(cXyz*, cXyz*, be<s16>*); /* 02275E44 */
    BOOL routeCheck(f32, be<s16>*); /* 02275F60 */
    void ko_clcMovSpd(); /* 02276028 */
    void setPlaySpd(f32); /* 02276198 */
    s32 ko_movPass(); /* 022761AC */
    void ko_clcSwmSpd(); /* 022762AC */
    void ko_nMove(); /* 022763A8 */
    void eventOrder(); /* 02276578 */
    BOOL _execute(); /* 022765B0 */
    BOOL _draw(); /* 0227684C */
    /* ---- end of part B ---- */
    /* ---- part C (d_a_npc_ko1_c.cpp) ---- */
    s32 anmNum_toResID(s32); /* 02276B2C */
    s32 headAnmNum_toResID(s32); /* 02276B40 */
    s32 balloon_anmNum_toResID(s32); /* 02276B68 */
    u32 setAnm_tex(s8); /* 02276B7C */
    BOOL setAnm_anm(anm_prm_c*); /* 02276B9C */
    BOOL set_balloonAnm_anm(anm_prm_c*); /* 02276C70 */
    BOOL set_balloonAnm_NUM(s32); /* 02276D04 */
    void setAnm_NUM(s32, s32); /* 02276D18 */
    bool setAnm(); /* 02276D84 */
    void chg_anmTag(); /* 02276E08 */
    void setAnm_ATR(s32); /* 02276E20 */
    void control_anmTag(); /* 02276E90 */
    void chg_anmAtr(u8); /* 02276EB8 */
    void control_anmAtr(); /* 02276F30 */
    void anmAtr(u16); /* 02276F58 */
    bool chk_talk(); /* 02277070 */
    fopAc_ac_c* searchByID(fpc_ProcID); /* 022770F0 (matcher: cLib_calcTimer<s>) */
    bool chk_manzai_1(); /* 02277124 */
    u8 chk_partsNotMove(); /* 0227732C */
    s8 bitCount(u8); /* 0227736C */
    u16 next_msgStatus(be<u32>*); /* 02277394 */
    u32 getMsg_HNA_0(); /* 022776C4 */
    u32 getMsg_HNA_1(); /* 02277704 */
    u32 getMsg_HNA_2(); /* 02277744 */
    u32 getMsg_HNA_3(); /* 02277784 */
    u32 getMsg_BOU_0(); /* 022777F4 */
    u32 getMsg_BOU_1(); /* 0227786C */
    u32 getMsg_BOU_2(); /* 022778AC */
    u32 getMsg(); /* 02277978 */
    u8 chkAttention(); /* 02277A30 */
    bool check_landOn(); /* 02277AB8 */
    void ko_setPthPos(); /* 02277B9C */
    void set_tgtPos(cXyz* o_result, cXyz* i_pos); /* 02277C44: returns a cXyz (hidden r4) */
    void setPrtcl_Hamon(f32, f32); /* 02277D48 */
    bool chk_start_swim(); /* 02277DEC */
    fpc_ProcID get_crsActorID(); /* 02277EC0 */
    bool chk_areaIn(f32, cXyz* i_pos); /* 02277F58 (cXyz by value: pointer to a copy) */
    void setPrtcl_HanaPachi(); /* 02277FE8 */
    void clrSpd(); /* 0227810C */
    /* ---- end of part C ---- */
    /* ---- part D1 (d_a_npc_ko1_d1.cpp) ---- */
    void setStt(s8); /* 02278134 */
    BOOL wait_1(); /* 0227896C */
    BOOL wait_2(); /* 02278AE0 */
    BOOL wait_3(); /* 02278B50 */
    BOOL wait_4(); /* 02278B84 */
    BOOL wait_5(s8); /* 02278CB4 */
    BOOL wait_6(); /* 02278D7C */
    BOOL wait_7(); /* 02278EC0 */
    BOOL wait_9(); /* 022790E4 */
    BOOL wait_a(); /* 022791F4 */
    BOOL walk_1(); /* 02279300 */
    BOOL walk_2(s8, s8); /* 022793C8 */
    BOOL walk_3(); /* 0227949C */
    BOOL swim_1(); /* 02279578 */
    BOOL swim_2(); /* 022796B4 */
    BOOL attk_1(); /* 022797FC */
    BOOL attk_2(s8, s8); /* 0227992C */
    BOOL attk_3(); /* 02279A58 */
    BOOL down_1(s8); /* 02279B90 */
    /* ---- end of part D1 ---- */
    /* ---- part D2 (d_a_npc_ko1_d2.cpp) ---- */
    BOOL talk_1(); /* 02279BC0 */
    BOOL talk_2(); /* 02279EF8 */
    BOOL manzai(); /* 02279FC0 */
    BOOL neru_1(); /* 0227A110 */
    BOOL neru_2(); /* 0227A1C8 */
    BOOL hana_action1(void*); /* 0227A374 */
    BOOL hana_action2(void*); /* 0227A4DC */
    BOOL hana_action3(void*); /* 0227A60C */
    BOOL hana_action4(void*); /* 0227A750 */
    BOOL hana_action5(void*); /* 0227A830 */
    BOOL wait_action1(void*); /* 0227A8C8 */
    BOOL wait_action2(void*); /* 0227A9C0 */
    BOOL wait_action3(void*); /* 0227AB28 */
    BOOL wait_action4(void*); /* 0227AC08 */
    /* ---- end of part D2 ---- */

    /* 0x7DC */ request_of_phase_process_class mPhs;
    /* 0x7E4 */ be<s8> m_hed_jnt_num;  /* was m7E4: create_Anm 022736FC: the "head" joint's index, asserted with the original text "m_hed_jnt_num >= 0" (line 4317); nodeKo1Control 0227351C turns it by the joint control's head angles and takes the eye position there; setMtx 02274A7C puts the head model on it; tww Bm1 / Ls1 m_hed_jnt_num (same code) */
    /* 0x7E5 */ be<s8> m_bbone_jnt_num;  /* was m7E5: create_Anm 022736FC: the "backbone" joint's index, asserted with the original text "m_bbone_jnt_num >= 0"; nodeKo1Control 0227351C turns it by the joint control's backbone angles; tww Bm1 / Ls1 m_bbone_jnt_num (same code) */
    /* 0x7E6 */ be<s8> m_armR2_jnt_num;  /* was m7E6: create_Anm 022736FC: the "armR2" joint's index, asserted with the original text "m_armR2_jnt_num >= 0"; setMtx 02274A7C puts the item model (mpItemModel) on it */
    /* 0x7E7 */ be<s8> m_hed_2_jnt_num;  /* was m7E7: create_hed_Anm 02273944: the head model's "head2" joint index, asserted with the original text "m_hed_2_jnt_num >= 0"; nodeHedControl 022732F0 keeps that joint's matrix in mHed2Mtx */
    /* 0x7E8 */ be<s8> m_bln_loc_jnt_num;  /* was m7E8: create_bln_Anm 02273C94: the balloon model's "balloon_loc" joint index, asserted with the original text "m_bln_loc_jnt_num >= 0"; nodeBlnControl 022733C4 gives that joint mHed2Mtx */
    /* 0x7E9 */ be<s8> m_bln_jnt_num;  /* was m7E9: create_bln_Anm 02273C94: the balloon model's "ko_balloon" joint index, asserted with the original text "m_bln_jnt_num >= 0" */
    /* 0x7EA */ u8 _7EA[2];
    /* 0x7EC */ Mtx34 mHed2Mtx;  /* was m7EC: nodeHedControl 022732F0 copies the head model's head2 joint matrix (m_hed_2_jnt_num) here; nodeBlnControl 022733C4 makes it the balloon_loc joint's matrix (the balloon follows the head) */
    /* 0x81C */ gptr<mDoExt_McaMorf> mpBalloonMorf;  /* was mpMorf81C: create_bln_Anm 02273C94 creates the balloon morf from the balloon model; set_balloonAnm_anm 02276C70 sets its animation, setMtx 02274A7C syncs its frame and calcs it; same name as the views in d_a_npc_ko1_c.h / _b.h */
    /* 0x820 */ gptr<J3DModel> mpItemModel;  /* was mpModel820: create_itm_Mdl 02273E7C creates the hand item's model; setMtx 02274A7C puts it on the armR2 joint and calcs it; same name as the view in d_a_npc_ko1_b.h */
    /* 0x824 */ gptr<mDoExt_McaMorf> mpHeadMorf;  /* was mpMorf824: create_hed_Anm 02273944 creates the head morf; setAnm_anm 02276B9C sets its animation (headAnmNum_toResID), setMtx 02274A7C puts it on the head joint; same name as the views in d_a_npc_ko1_c.h / _b.h */
    /* 0x828 */ gptr<J3DAnmTexPattern> m_hed_tex_pttrn;
    /* 0x82C */ u8 mBtpAnm[0x74];               /* mDoExt_btpAnm (HD 0x74) */
    /* 0x8A0 */ be<u8> mBlinkFrame;
    /* 0x8A1 */ u8 _8A1;
    /* 0x8A2 */ be<s16> mBlinkTimer;
    /* 0x8A4 */ ProcFunc_l mCurrProcFunc;
    /* 0x8AC */ dNpc_PathRun_l mPathRun;
    /* 0x8B4 */ be<u32> mSavedPath;  /* was m8B4: setStt 02278134 keeps the current path here before the attack / swim states and restores it (setInfDrct) when walk_3 starts; same name as the view in d_a_npc_ko1_d1.h */
    /* 0x8B8 */ dNpc_EventCut_c mEventCut;       /* hides fopNpc_npc_c::mEventCut */
    /* 0x924 */ be<u32> mPartnerId;  /* was m924: partner_srch 022753EC: the first partner's process id (partner_srch_sub); setStt 02278134 / wait_7 02278EC0 / attk_3 02279A58 search it; same name as the views in d_a_npc_ko1_d1.h / _b.h */
    /* 0x928 */ be<u32> mOtherPartnerId;  /* was m928: partner_srch 022753EC: the second partner's process id (searchActor_Ob) for the types with two partners; chk_manzai_1 02277124 / talk_2 02279EF8 read it as element 1 of the partner list; same name as the view in d_a_npc_ko1_b.h */
    /* 0x92C */ be<u8> mPartnerCount;  /* was m92C: partner_srch 022753EC sets 1 or 2 with the partner ids; talk_2 02279EF8 loops over that many partners; same name as the view in d_a_npc_ko1_b.h */
    /* 0x92D */ u8 _92D[3];
    /* 0x930 */ be<u32> mAttackActorId;  /* was m930: attk_1 022797FC / attk_2 0227992C / attk_3 02279A58 store the actor id they act on; same name as the view in d_a_npc_ko1_d1.h */
    /* 0x934 */ cXyz mInitialPos;
    /* 0x940 */ csXyz mInitialAngle;
    /* 0x946 */ csXyz mModelAngle;  /* was m946: createInit 02274E28 copies current.angle; setMtx 02274A7C turns the model by its y; same name as the view in d_a_npc_ko1_b.h */
    /* 0x94C */ cXyz mEyePos;  /* was m94C: nodeKo1Control 0227351C: the eye position from the head joint's matrix (a_eye_pos_off); setAttention 02274A1C copies it to eyePos while mbSetEyePos or the argument is set; same code as daNpc_Bm1_c::mEyePos (tww Bm1 eyePos.set(mEyePos.x, ..)) */
    /* 0x958 */ cXyz mLookPos;  /* was m958: setStt 02278134 / wait_7 02278EC0 set the partner's position at its eye height; createInit 02274E28 current.pos; lookBack 022758B8 looks at it in look state 2; same code as daNpc_Bm1_c::mLookPos (tww Bm1 dstPos = m82C); same name as the view in d_a_npc_ko1_d1.h */
    /* 0x964 */ cXyz mTargetPos;  /* was m964: setStt 02278134 sets the path point or the player's position; ko_movPass 022761AC / ko_clcMovSpd 02276028 / ko_clcSwmSpd 022762AC move towards it; same name as daNpc_Bm1_c::mTargetPos and the view in d_a_npc_ko1_d1.h */
    /* 0x970 */ cXyz m970;
    /* 0x97C */ cXyz mAreaCenter;  /* was m97C: createInit 02274E28 current.pos; wait_4 02278B84 / wait_5 02278CB4 / wait_6 02278D7C / wait_9 022790E4 / walk_1 02279300 check the player against an area around it (chk_areaIn); _execute 022765B0 deletes the actor once it is more than 3000 away (XZ); same name as the view in d_a_npc_ko1_d1.h */
    /* 0x988 */ cXyz mGroundNormal;  /* was m988: _execute 022765B0 stores the ground polygon's plane normal; chk_routeAngle 02275D90 turns the route along it; same name as the view in d_a_npc_ko1_b.h */
    /* 0x994 */ be<f32> mPrevMorfFrame;  /* was m994: setMtx 02274A7C: a morf frame below it means the animation wrapped (mAnimationEnded = 1), then it takes the frame; setAnm_anm 02276B9C 0; same code as daNpc_Km1_c / daNpc_Ls1_c::mPrevMorfFrame (after tww Ls1) */
    /* 0x998 */ be<f32> mBalloonAnimationFrame;  /* was m998: set_balloonAnm_anm 02276C70 0 when the balloon animation changes; written only; same name as the view in d_a_npc_ko1_c.h */
    /* 0x99C */ be<f32> mMoveSpeed;  /* was m99C: setStt 02278134 sets the HIO walk / run speed (0 for the waits), clrSpd 0227810C 0; ko_clcMovSpd 02276028 chases speedF to it; same name as the views in d_a_npc_ko1_c.h / _d1.h */
    /* 0x9A0 */ be<f32> mVerticalSpeed;  /* was m9A0: setStt 02278134 / swim_1 02279578 / swim_2 022796B4 set -4 or 4; ko_clcSwmSpd 022762AC chases speed.y to it; same name as the view in d_a_npc_ko1_d1.h */
    /* 0x9A4 */ be<f32> mMoveAcceleration;  /* was m9A4: setStt 02278134 sets the HIO walk / run acceleration, clrSpd 0227810C 0; ko_clcMovSpd 02276028 / ko_nMove 022763A8 chase speedF with it as the step; same name as the views in d_a_npc_ko1_c.h / _d1.h */
    /* 0x9A8 */ be<f32> m9A8;
    /* 0x9AC */ be<f32> mPassDist;  /* was m9AC: ko_movPass 022761AC: the point counts as reached once the XZ distance is <= it; setStt 02278134 sets the HIO walk / run / swim value; same code as daNpc_Bm1_c::mPassDist (tww Bm1 bm_movPass if (dist_xz <= m858)) */
    /* 0x9B0 */ be<s16> mJointHeadY;  /* was m9B0: lookBack 022758B8 stores the joint control's head Y angle; chk_partsNotMove 0227732C compares it; same code as daNpc_Bm1_c::mJointHeadY (after tww Ls1) */
    /* 0x9B2 */ be<s16> mJointBackboneY;  /* was m9B2: lookBack 022758B8 stores the joint control's backbone Y angle; chk_partsNotMove 0227732C compares it; same code as daNpc_Bm1_c::mJointBackboneY (after tww Ls1) */
    /* 0x9B4 */ be<s16> mActorAngleY;  /* was m9B4: lookBack 022758B8 stores current.angle.y; chk_partsNotMove 0227732C compares it; same code as daNpc_Bm1_c::mActorAngleY (after tww Ls1); same name as the view in d_a_npc_ko1_c.h */
    /* 0x9B6 */ u8 _9B6[2];
    /* 0x9B8 */ be<u32> mPartnerMsgNo;  /* was m9B8: manzai 02279FC0 copies the partner's current message number, talk_1 02279BC0 0xFFFFFFFF; chg_anmAtr 02276EB8 turns to the player on some of them; same name as the views in d_a_npc_ko1_c.h / _d2.h */
    /* 0x9BC */ be<s16> m9BC;
    /* 0x9BE */ be<s16> mTalkWaitTimer;  /* was m9BE: wait_1 0227896C / wait_a 022791F4 / talk_1 02279BC0 set 60 and count it down; setStt 02278134 0; same name as the views in d_a_npc_ko1_d1.h / _d2.h */
    /* 0x9C0 */ be<s16> mZigzagTimer;  /* was m9C0: ko_clcMovSpd 02276028: in the chase states the run direction flips when it runs out, then it restarts with 8 + rnd(20); same name as the view in d_a_npc_ko1_b.h */
    /* 0x9C2 */ be<s16> mSttTimer;  /* was m9C2: setStt 02278134 sets it per state (90 + rnd(90) for wait_1, 180 + rnd(180) for walk_1); wait_1 0227896C / walk_1 02279300 / neru_1 0227A110 / neru_2 0227A1C8 change the state or the sleep step when it runs out; same code as daNpc_Bm1_c::mSttTimer (tww Bm1 setStt m870 = ..) */
    /* 0x9C4 */ be<s16> mBalloonTimer;  /* was m9C4: setStt 02278134 / neru_2 0227A1C8 set it (counter & 3 + 1, 3 + rnd(10)); wait_1 0227896C / wait_2 02278AE0 / neru_2 count it down; same name as the views in d_a_npc_ko1_d1.h / _d2.h */
    /* 0x9C6 */ be<s16> mTurnVelocity;  /* was m9C6: lookBack 022758B8 eases it towards the HIO maximum and passes it as the look-at's turn velocity (0 while m_jnt.mbTrn is clear); same name as the view in d_a_npc_ko1_b.h */
    /* 0x9C8 */ be<s16> mDesiredYRot;  /* was m9C8: lookBack 022758B8: the look-at's Y angle in look state 3; setStt 02278134 / wait_1 0227896C / wait_a 022791F4 / manzai 02279FC0 set mInitialAngle.y; same code as daNpc_Bm1_c::mDesiredYRot (tww Bm1 desiredYrot = m876) */
    /* 0x9CA */ be<u16> mTargetOffsetAngle;  /* was m9CA: set_tgtPos 02277C44 offsets the target around the player by it and steps it by 0x400; same name as the view in d_a_npc_ko1_c.h */
    /* 0x9CC */ u8 _9CC[2];
    /* 0x9CE */ be<u8> mBalloonAnimationState;  /* was m9CE: set_balloonAnm_anm 02276C70 0 when the balloon animation changes; written only; same name as the view in d_a_npc_ko1_c.h */
    /* 0x9CF */ be<u8> mbBalloonAnimationStopped;  /* was m9CF: set_balloonAnm_anm 02276C70 0 when the balloon animation changes; written only; same name as the view in d_a_npc_ko1_c.h */
    /* 0x9D0 */ be<u8> mAnimationEnded;  /* was m9D0: setMtx 02274A7C: the morf's play() result, also set when the frame wraps; setAnm_anm 02276B9C 0; control_anmTag 02276E90 / control_anmAtr 02276F30 and the states test it; same name as the views in d_a_npc_ko1_c.h / _d1.h / _d2.h / _b.h */
    /* 0x9D1 */ be<u8> mAnimationState;  /* was m9D1: setAnm_anm 02276B9C 0 when the animation changes; written only; same name as the view in d_a_npc_ko1_c.h */
    /* 0x9D2 */ be<u8> m9D2;
    /* 0x9D3 */ be<u8> mMessageSelector;  /* was m9D3: chk_talk 02277070 stores the item shown (dComIfGp_event_getPreItemNo) or 0xFF; getMsg picks the message by it; talk_1 02279BC0 0xFF; same name as the views in d_a_npc_ko1_c.h / _d2.h */
    /* 0x9D4 */ be<u8> mZigzagDirection;  /* was m9D4: ko_clcMovSpd 02276028 flips it when mZigzagTimer runs out and turns the run angle by -/+0x2000 by it; same name as the view in d_a_npc_ko1_b.h */
    /* 0x9D5 */ be<u8> mSleepState;  /* was m9D5: neru_1 0227A110 toggles it, neru_2 0227A1C8 switches on it and steps it; setStt 02278134 0; setMtx 02274A7C syncs the balloon frame while it is below 2; same name as the views in d_a_npc_ko1_d1.h / _d2.h */
    /* 0x9D6 */ be<u8> m9D6;
    /* 0x9D7 */ be<u8> mHidden;  /* was m9D7: _draw 0227684C draws nothing while it is set; _execute 022765B0 clears it outside a demo; same name as the view in d_a_npc_ko1_b.h */
    /* 0x9D8 */ be<u8> mKeepShape;  /* was m9D8: _execute 022765B0 copies current.angle to shape_angle and mModelAngle only while it is clear; same name as the view in d_a_npc_ko1_b.h */
    /* 0x9D9 */ be<u8> mSkipMovement;  /* was m9D9: ko_nMove 022763A8 sets it when the move step is done; _execute 022765B0 skips ko_nMove and the position move while it is set; same name as the view in d_a_npc_ko1_b.h */
    /* 0x9DA */ be<u8> mDrawHidden;  /* was m9DA: _draw 0227684C draws nothing while it is set; same name as the view in d_a_npc_ko1_b.h */
    /* 0x9DB */ be<u8> mbMoveFinished;  /* was m9DB: ko_nMove 022763A8 sets it when a point or the path end is reached or a jump lands; setStt 02278134 clears it; walk_2 022793C8 / walk_3 0227949C / down_1 02279B90 change the state on it; same name as the view in d_a_npc_ko1_d1.h */
    /* 0x9DC */ be<u8> mPreviousGroundHit;  /* was m9DC: setMtx 02274A7C stores the ground-hit flag before the position correction; ko_nMove 022763A8 ends a jump when it was clear and the ground is hit now; setStt 02278134 sets 1 when the down / jump states start; same name as the view in d_a_npc_ko1_b.h */
    /* 0x9DD */ be<u8> mbTalkTriggered;  /* was m9DD: talk_1 02279BC0 sets 1; getMsg_HNA_3 02277784 picks the second message once it is set; same name as the views in d_a_npc_ko1_c.h / _d2.h */
    /* 0x9DE */ be<u8> mbRanExecute;             /* _execute: initial pose stored */
    /* 0x9DF */ u8 _9DF;
    /* 0x9E0 */ be<u32> mbSetEyePos;  /* was m9E0: hana_action1 0227A374 .. store the state function's result; setAttention 02274A1C copies mEyePos to eyePos only while it or the argument is set; same code as daNpc_Bm1_c::mbSetEyePos (tww Bm1 mbSetEyePos = wait_1(); if (!mbSetEyePos && !i_setEyePos) return;) */
    /* 0x9E4 */ be<u8> mAttention;  /* was m9E4: hana_action1 0227A374 .. store chkAttention(); wait_1 0227896C / wait_2 02278AE0 / wait_a 022791F4 react while it is set; same name as the views in d_a_npc_ko1_d1.h / _d2.h */
    /* 0x9E5 */ be<u8> mTalking;  /* was m9E5: checkOrder 02275504 sets 1 on the talk command; the waits (wait_1 0227896C ..) start the talk while it is set; talk_1 02279BC0 clears it; same code as daNpc_Bm1_c::mTalking (tww Bm1 checkOrder m895 = true) */
    /* 0x9E6 */ be<u8> mHeadOnlyFollow;  /* was m9E6: lookBack 022758B8 passes it as the head-only flag of the joint look-at; setStt 02278134 sets 1 for the moving states and 0 for the others, chg_anmAtr 02276EB8 0; same code as daNpc_Bm1_c::mHeadOnlyFollow (tww Bm1 bool headOnlyFollow = m896) */
    /* 0x9E7 */ be<u8> mInDemo;  /* was m9E7: demo 02275544 sets and returns it; setMtx 02274A7C skips the animation and ground work while it is set; same name as the view in d_a_npc_ko1_b.h */
    /* 0x9E8 */ dPa_rippleEcallBack_l mRipple;
    /* 0x9FC */ be<u32> mRippleEmitter;  /* was m9FC: setPrtcl_Hamon 02277D48 keeps the ripple emitter, chk_start_swim 02277DEC clears it; same name as the view in d_a_npc_ko1_c.h */
    /* 0xA00 */ be<u32> mBalloonEmitter;  /* was mA00: setPrtcl_HanaPachi 02277FE8 keeps the balloon-pop emitter; same name as the view in d_a_npc_ko1_c.h */
    /* 0xA04 */ be<u32> mBalloonLightEmitter;  /* was mA04: setPrtcl_HanaPachi 02277FE8 keeps the second balloon-pop emitter; same name as the view in d_a_npc_ko1_c.h */
    /* 0xA08 */ be<u8> mMoveMode;  /* was mA08: setStt 02278134 sets 0 wait, 1 walk, 2 run, 3 swim, 4 jump; ko_nMove 022763A8 switches on it; same name as the views in d_a_npc_ko1_d1.h / _b.h */
    /* 0xA09 */ be<u8> mPreviousMoveMode;  /* was mA09: setStt 02278134 keeps mMoveMode here when a jump starts; ko_nMove 022763A8 returns to it on landing; same name as the views in d_a_npc_ko1_d1.h / _b.h */
    /* 0xA0A */ be<u8> mCutIndex;  /* was mA0A: privateCut 022757E4: the cut index from dComIfGp_evmng_getMyActIdx; same name as the view in d_a_npc_ko1_b.h */
    /* 0xA0B */ be<u8> mCutAction;  /* was mA0B: event_actionInit 0227577C: the cut's "ActNo" integer; same name as the view in d_a_npc_ko1_b.h */
    /* 0xA0C */ be<u8> mAnmAtrNo;  /* was mA0C: chg_anmAtr 02276EB8 stores the message animation attribute (below 0xD, when it changes); setAnm_ATR 02276E20 picks the attribute animation by it, control_anmAtr 02276F30 returns from 0xB; setStt 02278134 / endEvent 02275738 / anmAtr 02276F58 0xFF; same code as daNpc_Bm1_c::mAnmAtrNo (tww Bm1 m8F7) */
    /* 0xA0D */ be<u8> mMesgAnimeTag;  /* was mA0D: anmAtr 02276F58: the message's animation tag when it changes (if (tag != 0xFF && it != tag) { it = tag; chg_anmTag(); }); chg_anmTag 02276E08 / control_anmTag 02276E90 act on tag 10; setStt 02278134 / endEvent 02275738 0xFF; same code as daNpc_Bm1_c::mMesgAnimeTag (tww Bm1 m8F8) */
    /* 0xA0E */ be<s8> mBtpNum;                  /* iniTexPttrnAnm */
    /* 0xA0F */ be<u8> mAnmNum;
    /* 0xA10 */ be<u8> mA10;
    /* 0xA11 */ be<u8> mBalloonAnimationNum;  /* was mA11: set_balloonAnm_anm 02276C70: the balloon animation number (returns at once when it is unchanged); same name as the view in d_a_npc_ko1_c.h */
    /* 0xA12 */ be<s8> mOrderEvent;  /* was mA12: eventOrder 02276578 orders the talk while it is 1 or 2 (speak event at 1); checkOrder 02275504 clears it on the talk command; the waits set 2, setStt 02278134 0; same code as daNpc_Bm1_c::mOrderEvent (tww Bm1 m8FD) */
    /* 0xA13 */ be<s8> mStatus;  /* was mA13: setStt 02278134 stores the state; setAnm 02276D84 picks the state animation by it; hana_action1 0227A374 .. switch on it; same name as daNpc_Bm1_c::mStatus (tww) and the views in d_a_npc_ko1_c.h / _d1.h / _d2.h */
    /* 0xA14 */ be<s8> mResumeStatus;  /* was mA14: setStt 02278134 keeps the previous mStatus when the talk starts; talk_1 02279BC0 / talk_2 02279EF8 / manzai 02279FC0 return to it; same name as the views in d_a_npc_ko1_d1.h / _d2.h */
    /* 0xA15 */ be<s8> mLookBackState;  /* was mA15: lookBack 022758B8 switches on it (1 the player, 2 mLookPos, 3 mDesiredYRot); chg_anmAtr 02276EB8 sets 1 with m_jnt.mbTrn; setStt 02278134 / manzai 02279FC0 set it per state; same code as daNpc_Bm1_c::mLookBackState (tww Bm1 switch (mLookBackState)) */
    /* 0xA16 */ be<s8> mType;                    /* 0..: resource/heap/HIO index (Joel, Zill, ...) */
    /* 0xA17 */ be<s8> mSpecificType;            /* init_* switch (0..8) and event staff name */
    /* 0xA18 */ be<s8> mActionStatus;  /* was mA18: set_action 022742C0 sets 9 for the leaving action and 0 for the new one; hana_action1 0227A374 .. step it from 0; partner_srch 022753EC runs at 1; same code as daNpc_Bm1_c::mActionStatus (after tww daNpc_Bs1_c mActionStatus) */
    /* 0xA19 */ be<s8> mbMessageTagsStarted;  /* was mA19: anmAtr 02276F58: while the message types, applies the message's animation attribute only when it is 0, then adds 1; MSG_DISPLAYED (0xE) resets it; same name as the view in d_a_npc_ko1_c.h */
    /* 0xA1A */ u8 _A1A[2];
};
WWHD_OFFSET(daNpc_Ko1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Ko1_c, mBtpAnm, 0x82C);
WWHD_OFFSET(daNpc_Ko1_c, mCurrProcFunc, 0x8A4);
WWHD_OFFSET(daNpc_Ko1_c, mEventCut, 0x8B8);
WWHD_OFFSET(daNpc_Ko1_c, mInitialPos, 0x934);
WWHD_OFFSET(daNpc_Ko1_c, mGroundNormal, 0x988);
WWHD_OFFSET(daNpc_Ko1_c, mRipple, 0x9E8);
WWHD_OFFSET(daNpc_Ko1_c, mType, 0xA16);
WWHD_SIZE(daNpc_Ko1_c, 0xA1C);
WWHD_SIZE(daNpc_Ko1_c::anm_prm_c, 0x14);
