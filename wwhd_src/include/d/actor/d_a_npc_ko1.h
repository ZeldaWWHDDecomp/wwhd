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
    key->mStringTop = gabi::ea(arc);
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
        /* 0x00 */ be<s8> mAnmNum;
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
    /* 0x7E4 */ be<s8> m7E4;                 /* body joint (nodeKo1Control) */
    /* 0x7E5 */ be<s8> m7E5;                 /* body joint (nodeKo1Control) */
    /* 0x7E6 */ be<s8> m7E6;
    /* 0x7E7 */ be<s8> m7E7;                 /* joint of the 0x824 model (nodeBlnControl) */
    /* 0x7E8 */ be<s8> m7E8;                 /* joint of the 0x81C model (nodeHedControl) */
    /* 0x7E9 */ be<s8> m7E9;
    /* 0x7EA */ u8 _7EA[2];
    /* 0x7EC */ Mtx34 m7EC;
    /* 0x81C */ gptr<mDoExt_McaMorf> mpMorf81C;
    /* 0x820 */ gptr<J3DModel> mpModel820;
    /* 0x824 */ gptr<mDoExt_McaMorf> mpMorf824;
    /* 0x828 */ gptr<J3DAnmTexPattern> m_hed_tex_pttrn;
    /* 0x82C */ u8 mBtpAnm[0x74];               /* mDoExt_btpAnm (HD 0x74) */
    /* 0x8A0 */ be<u8> mBlinkFrame;
    /* 0x8A1 */ u8 _8A1;
    /* 0x8A2 */ be<s16> mBlinkTimer;
    /* 0x8A4 */ ProcFunc_l mCurrProcFunc;
    /* 0x8AC */ dNpc_PathRun_l mPathRun;
    /* 0x8B4 */ be<u32> m8B4;
    /* 0x8B8 */ dNpc_EventCut_c mEventCut;       /* hides fopNpc_npc_c::mEventCut */
    /* 0x924 */ be<u32> m924;
    /* 0x928 */ be<u32> m928;
    /* 0x92C */ be<u8> m92C;
    /* 0x92D */ u8 _92D[3];
    /* 0x930 */ be<u32> m930;
    /* 0x934 */ cXyz mInitialPos;
    /* 0x940 */ csXyz mInitialAngle;
    /* 0x946 */ csXyz m946;
    /* 0x94C */ cXyz m94C;
    /* 0x958 */ cXyz m958;
    /* 0x964 */ cXyz m964;
    /* 0x970 */ cXyz m970;
    /* 0x97C */ cXyz m97C;
    /* 0x988 */ cXyz m988;                       /* ground plane normal (_execute) */
    /* 0x994 */ be<f32> m994;
    /* 0x998 */ be<f32> m998;
    /* 0x99C */ be<f32> m99C;
    /* 0x9A0 */ be<f32> m9A0;
    /* 0x9A4 */ be<f32> m9A4;
    /* 0x9A8 */ be<f32> m9A8;
    /* 0x9AC */ be<f32> m9AC;
    /* 0x9B0 */ be<s16> m9B0;
    /* 0x9B2 */ be<s16> m9B2;
    /* 0x9B4 */ be<s16> m9B4;
    /* 0x9B6 */ u8 _9B6[2];
    /* 0x9B8 */ be<u32> m9B8;
    /* 0x9BC */ be<s16> m9BC;
    /* 0x9BE */ be<s16> m9BE;
    /* 0x9C0 */ be<s16> m9C0;
    /* 0x9C2 */ be<s16> m9C2;
    /* 0x9C4 */ be<s16> m9C4;
    /* 0x9C6 */ be<s16> m9C6;
    /* 0x9C8 */ be<s16> m9C8;
    /* 0x9CA */ be<u16> m9CA;
    /* 0x9CC */ u8 _9CC[2];
    /* 0x9CE */ be<u8> m9CE;
    /* 0x9CF */ be<u8> m9CF;
    /* 0x9D0 */ be<u8> m9D0;
    /* 0x9D1 */ be<u8> m9D1;
    /* 0x9D2 */ be<u8> m9D2;
    /* 0x9D3 */ be<u8> m9D3;
    /* 0x9D4 */ be<u8> m9D4;
    /* 0x9D5 */ be<u8> m9D5;
    /* 0x9D6 */ be<u8> m9D6;
    /* 0x9D7 */ be<u8> m9D7;
    /* 0x9D8 */ be<u8> m9D8;
    /* 0x9D9 */ be<u8> m9D9;
    /* 0x9DA */ be<u8> m9DA;
    /* 0x9DB */ be<u8> m9DB;
    /* 0x9DC */ be<u8> m9DC;
    /* 0x9DD */ be<u8> m9DD;
    /* 0x9DE */ be<u8> mbRanExecute;             /* _execute: initial pose stored */
    /* 0x9DF */ u8 _9DF;
    /* 0x9E0 */ be<u32> m9E0;
    /* 0x9E4 */ be<u8> m9E4;
    /* 0x9E5 */ be<u8> m9E5;
    /* 0x9E6 */ be<u8> m9E6;
    /* 0x9E7 */ be<u8> m9E7;
    /* 0x9E8 */ dPa_rippleEcallBack_l mRipple;
    /* 0x9FC */ be<u32> m9FC;
    /* 0xA00 */ be<u32> mA00;
    /* 0xA04 */ be<u32> mA04;
    /* 0xA08 */ be<u8> mA08;
    /* 0xA09 */ be<u8> mA09;
    /* 0xA0A */ be<u8> mA0A;
    /* 0xA0B */ be<u8> mA0B;
    /* 0xA0C */ be<u8> mA0C;
    /* 0xA0D */ be<u8> mA0D;
    /* 0xA0E */ be<s8> mBtpNum;                  /* iniTexPttrnAnm */
    /* 0xA0F */ be<u8> mA0F;
    /* 0xA10 */ be<u8> mA10;
    /* 0xA11 */ be<u8> mA11;
    /* 0xA12 */ be<s8> mA12;
    /* 0xA13 */ be<s8> mA13;
    /* 0xA14 */ be<s8> mA14;
    /* 0xA15 */ be<s8> mA15;
    /* 0xA16 */ be<s8> mType;                    /* 0..: resource/heap/HIO index (Joel, Zill, ...) */
    /* 0xA17 */ be<s8> mSpecificType;            /* init_* switch (0..8) and event staff name */
    /* 0xA18 */ be<s8> mA18;
    /* 0xA19 */ be<s8> mA19;
    /* 0xA1A */ u8 _A1A[2];
};
WWHD_OFFSET(daNpc_Ko1_c, mPhs, 0x7DC);
WWHD_OFFSET(daNpc_Ko1_c, mBtpAnm, 0x82C);
WWHD_OFFSET(daNpc_Ko1_c, mCurrProcFunc, 0x8A4);
WWHD_OFFSET(daNpc_Ko1_c, mEventCut, 0x8B8);
WWHD_OFFSET(daNpc_Ko1_c, mInitialPos, 0x934);
WWHD_OFFSET(daNpc_Ko1_c, m988, 0x988);
WWHD_OFFSET(daNpc_Ko1_c, mRipple, 0x9E8);
WWHD_OFFSET(daNpc_Ko1_c, mType, 0xA16);
WWHD_SIZE(daNpc_Ko1_c, 0xA1C);
WWHD_SIZE(daNpc_Ko1_c::anm_prm_c, 0x14);
