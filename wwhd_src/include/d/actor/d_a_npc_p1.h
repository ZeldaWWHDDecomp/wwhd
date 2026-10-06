/* daNpc_P1_c (Gonzo, Senza, Nudge: Tetra's pirates), WWHD layout. 
 *
 * GameCube -> WWHD, measured from the functions of d_a_npc_p1: daNpc_P1_c derives from fopAc_ac_c
 * directly (+0x11C). The two pointers to member function are 8 bytes each (GHS) instead of 12, so
 * mPhs..mpTexture are +0x114; mDoExt_btpAnm grew from 0x14 to 0x74, so everything from mBlinkFrame
 * on is +0x174 (mShadowId keeps its slot, HD draws no blob shadow); dBgS_ObjAcch is 0x1C4 and
 * dBgS_AcchCir 0x40, so mAcchCir is +0x178 and the members after it +0x174 again; dNpc_HeadAnm_c
 * is 0x20 (GameCube 0x24), so the members after it are +0x170. Size 0x8B4 (GameCube 0x744). */
#pragma once
#include "d/actor/d_a_npc_ba1.h" /* ProcFunc_l (GHS pointer to member function, measured) */

/* ---- shared NPC class, WWHD layout (SHARED-CANDIDATE; same as d_a_npc_bms1.h) ---- */
/* dNpc_HeadAnm_c (HD 0x20, GameCube 0x24): csXyz, the swing pointer to member (8 bytes, GHS),
 * two floats, three s16. Constructor inline (zeroes all but the pointer to member). */
struct dNpc_HeadAnm_c_l {
    /* 0x00 */ csXyz field_0x00;
    /* 0x06 */ u8 _06[2];
    /* 0x08 */ u8 mProc[8];
    /* 0x10 */ be<f32> field_0x14;
    /* 0x14 */ be<f32> field_0x18;
    /* 0x18 */ be<s16> field_0x1C;
    /* 0x1A */ be<s16> field_0x1E;
    /* 0x1C */ be<s16> field_0x20;
    /* 0x1E */ u8 _1E[2];
    /* 0259F36C swing_vertical_init(s16, s16, s16, int) */
    void swing_vertical_init(s16 a, s16 b, s16 c, s32 d) { gabi::call(0x0259F36C, this, a, b, c, d); }
    /* 0259F67C move() */
    void move() { gabi::call(0x0259F67C, this); }
};
WWHD_SIZE(dNpc_HeadAnm_c_l, 0x20);

/* daNpc_P1_childHIO_c (HD: vtable at the end, 0x30) */
struct daNpc_P1_childHIO_c {
    /* 0x00 */ be<s8> unk4;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> mAttnYPosOffset;
    /* 0x08 */ be<s16> mMaxHeadX;
    /* 0x0A */ be<s16> mMinHeadX;
    /* 0x0C */ be<s16> mMaxBackboneX;
    /* 0x0E */ be<s16> mMinBackboneX;
    /* 0x10 */ be<s16> mMaxHeadY;
    /* 0x12 */ be<s16> mMinHeadY;
    /* 0x14 */ be<s16> mMaxBackboneY;
    /* 0x16 */ be<s16> mMinBackboneY;
    /* 0x18 */ be<s16> mMaxTurnStep;
    /* 0x1A */ be<s16> mLookBackTargetY;
    /* 0x1C */ be<s8> mUnused20;
    /* 0x1D */ u8 _1D[3];
    /* 0x20 */ be<f32> mMaxTalkDist;
    /* 0x24 */ be<f32> mMorfBackup;
    /* 0x28 */ be<f32> mUnused2C;
    /* 0x2C */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_P1_childHIO_c, 0x30);

/* daNpc_P1_HIO_c (HD: vtable at the end, 0x9C) */
struct daNpc_P1_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<s32> m8;
    /* 0x08 */ daNpc_P1_childHIO_c children[3];
    /* 0x98 */ be<u32> __vtbl;
};
WWHD_SIZE(daNpc_P1_HIO_c, 0x9C);

struct daNpc_P1_c : fopAc_ac_c {
    enum { TYPE_P1A_e = 0, TYPE_P1B_e = 1, TYPE_P1C_e = 2 };
    enum { ACTION_STARTING_e = 0, ACTION_ONGOING_e = 1, ACTION_ENDING_e = -1 };

    void setAnimFromMsg();
    BOOL setAnm(int, f32);
    BOOL normalAction(void*);
    BOOL confuseAction(void*);
    BOOL talkAction(void*);
    BOOL p1c_speakAction(void*);
    BOOL speakAction(void*);
    BOOL explainAction(void*);
    u32 getNextMsgNo(int);
    BOOL playTexPatternAnm();
    void demo_end_init();
    BOOL demo_move();
    BOOL event_move();
    BOOL evn_setAnm_init(int);
    BOOL evn_talk_init(int);
    BOOL evn_talk();
    BOOL minigameExplainCut();
    BOOL privateCut();
    BOOL setAttentionPos(cXyz*);
    cPhs_State _create();
    BOOL CreateHeap();
    BOOL _delete();
    u32 getKajiID();
    BOOL kaji_anm();
    BOOL _execute();
    BOOL _draw();
    BOOL lookBack();

    /* 0x3AC */ ProcFunc_l mActionFunc;
    /* 0x3B4 */ ProcFunc_l mPrevAction;
    /* 0x3BC */ request_of_phase_process_class mPhs;
    /* 0x3C4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3C8 */ u8 _3C8[0x3D4 - 0x3C8];
    /* 0x3D4 */ gptr<J3DModel> mpHeadModel;
    /* 0x3D8 */ gptr<J3DModel> mpDoraModel;
    /* 0x3DC */ gptr<J3DAnmTexPattern> mpTexture;
    /* 0x3E0 */ u8 mBtp[0x74];           /* mDoExt_btpAnm (HD 0x74); its J3DAnmTexPattern* at +0x10 */
    /* 0x454 */ be<u8> mBlinkFrame;
    /* 0x455 */ u8 _455;
    /* 0x456 */ be<s16> mBlinkTimer;
    /* 0x458 */ be<u32> mShadowId;       /* unused in HD */
    /* 0x45C */ dBgS_ObjAcch mObjAcch;
    /* 0x620 */ dBgS_AcchCir mAcchCir;
    /* 0x660 */ dCcD_Stts mStts;
    /* 0x69C */ dCcD_Cyl mCyl;
    /* 0x7CC */ u8 pad7CC;
    /* 0x7CD */ be<s8> mActionStatus;
    /* 0x7CE */ be<s8> m65A;
    /* 0x7CF */ be<u8> mType;
    /* 0x7D0 */ be<u8> mParam;
    /* 0x7D1 */ u8 _7D1[3];
    /* 0x7D4 */ be<u32> mCurrMesg;
    /* 0x7D8 */ be<u32> mPrevMesg;
    /* 0x7DC */ be<s32> mAnmNum;
    /* 0x7E0 */ be<s32> m66C;
    /* 0x7E4 */ be<u8> m670;
    /* 0x7E5 */ be<u8> m671;
    /* 0x7E6 */ u8 _7E6[2];
    /* 0x7E8 */ be<u32> mKajiId;
    /* 0x7EC */ be<s16> mKajiTimer;
    /* 0x7EE */ be<s16> filler;
    /* 0x7F0 */ dNpc_JntCtrl_c m_jnt;
    /* 0x824 */ dNpc_EventCut_c mEventCut6B0;
    /* 0x890 */ dNpc_HeadAnm_c_l mHeadAnm;
    /* 0x8B0 */ be<s8> m_handR_jnt_num;
    /* 0x8B1 */ be<u8> mbAttentionFlag;
    /* 0x8B2 */ be<s16> mMaxLookVel;
};
WWHD_OFFSET(daNpc_P1_c, mPhs, 0x3BC);
WWHD_OFFSET(daNpc_P1_c, mBlinkFrame, 0x454);
WWHD_OFFSET(daNpc_P1_c, mObjAcch, 0x45C);
WWHD_OFFSET(daNpc_P1_c, mStts, 0x660);
WWHD_OFFSET(daNpc_P1_c, mCyl, 0x69C);
WWHD_OFFSET(daNpc_P1_c, mActionStatus, 0x7CD);
WWHD_OFFSET(daNpc_P1_c, m_jnt, 0x7F0);
WWHD_OFFSET(daNpc_P1_c, mEventCut6B0, 0x824);
WWHD_OFFSET(daNpc_P1_c, mHeadAnm, 0x890);
WWHD_SIZE(daNpc_P1_c, 0x8B4);

/* ======================= helpers shared by the d_a_npc_p1 source files ======================= */
#define P1_SAFESTRING_VTBL 0x1001F87Cu /* this TU's sead::SafeString vtable */
#define P1_VTBL 0x1001F8D4u            /* daNpc_P1_c vtable (HD virtual destructor) */
#define P1_AAB_VTBL 0x1001F894u        /* this TU's cM3dGAab vtable copy */
enum { fpcNm_Obj_Pirateship_e = 0x39, fpcNm_Kaji_e = 0x3C };

/* file statics: l_HIO (0x10467F40); l_msgId (0x10467F20; HD: l_msg is gone, the message is
 * reached through the HD message manager) */
static inline daNpc_P1_HIO_c* p1_HIO() { return gabi::at<daNpc_P1_HIO_c>(0x10467F40); }
static inline daNpc_P1_childHIO_c* p1_child(u32 type) { return &p1_HIO()->children[type]; }
static inline be<u32>& p1_msgId() { return *gabi::at<be<u32>>(0x10467F20); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD message manager (*(0x101F4B5C)): 025F795C status, 025F74D0 setStatus, 025F7DB0
 * messageSet(msgNo, cXyz* pos) -> id; its select number (GameCube msg_class::mSelectNum) at +0x948 */
static inline u32 p1_msgMgr() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 p1_msgGetStatus(u32 m) { return gabi::call<u32>(0x025F795C, m); }
static inline void p1_msgSetStatus(u32 m, u32 st) { gabi::call(0x025F74D0, m, st); }
static inline u32 p1_msgSet(u32 m, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, m, msgNo, pos); }
/* save events: dSv_event_c at *(0x101F84DC) + 0x644 */
static inline BOOL p1_isEventBit(u16 f) { return dSv_event_isEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f); }
static inline void p1_onEventBit(u16 f) { dSv_event_onEventBit(gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644), f); }
/* dComIfGs_getClearCount(): save byte *(0x101F84DC) + 0x1C0 */
static inline u8 p1_getClearCount() { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1C0); }
/* dComIfGp_getStartStagePoint(): play + 0x513C (s16); getStartStageLayer(): play + 0x513F (s8) */
static inline s16 p1_getStartStagePoint() { return gabi::load<s16>(dComIfGp_ea() + 0x513C); }
static inline s8 p1_getStartStageLayer() { return gabi::load<s8>(dComIfGp_ea() + 0x513F); }
/* dComIfGp_event_reset(): play + 0x52B8 |= 8 */
static inline void p1_event_reset() {
    u32 a = dComIfGp_ea() + 0x52B8;
    gabi::store<u16>(a, (u16)(gabi::load<u16>(a) | 8));
}
/* dComIfGp_event_getMode(): play + 0x5292 */
static inline u8 p1_event_getMode() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
/* dComIfGp_getMesgAnimeAttrInfo / set: play + 0x5BC5 */
static inline u8 p1_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + 0x5BC5); }
static inline void p1_setMesgAnimeAttrInfo(u8 v) { gabi::store<u8>(dComIfGp_ea() + 0x5BC5, v); }
/* dComIfGp_checkMesgSendButton(): play + 0x5BD2 */
static inline u8 p1_checkMesgSendButton() { return gabi::load<u8>(dComIfGp_ea() + 0x5BD2); }
/* dComIfGp_onCameraAttentionStatus(0, 4) / off: play + 0x5B00 */
static inline void p1_onCameraAttentionStatus4() {
    u32 a = dComIfGp_ea() + 0x5B00;
    gabi::store<u32>(a, gabi::load<u32>(a) | 4);
}
static inline void p1_offCameraAttentionStatus4() {
    u32 a = dComIfGp_ea() + 0x5B00;
    gabi::store<u32>(a, gabi::load<u32>(a) & ~4u);
}
/* dComIfGp_getCamera(0)->mCamera.SkipSmoother() (inline: three flags of the camera at
 * *(play + 0x5AF8)); the store order differs per site */
static inline u32 p1_camera0() { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8); }
/* eventInfo: condition (0xF8), command flags (0xFA) */
static inline BOOL p1_checkCommandTalk(fopAc_ac_c* a) { return gabi::load<u16>(gabi::ea(a) + 0xF8) == 1; }
static inline void p1_onCondition(fopAc_ac_c* a, u16 c) {
    u32 p = gabi::ea(a) + 0xFA;
    gabi::store<u16>(p, (u16)(gabi::load<u16>(p) | c));
}
static inline void* p1_getRes(u32 arc, s32 idx) { return dComIfG_getObjectRes(STR(arc), idx, P1_SAFESTRING_VTBL); }
static inline s32 p1_getMyActIdx(s32 staffId, u32 tbl, s32 n, s32 force, s32 nameType) {
    return gabi::call<s32>(0x02542EDC, dComIfGp_getPEvtManager(), staffId, tbl, n, force, nameType);
}
/* mDoExt_McaMorf::checkFrame(getEndFrame() - d): the end frame is the frame control's s16 end */
static inline BOOL p1_checkEnd(mDoExt_McaMorf* m, f32 d) {
    f32 e = (f32)(s32)m->mFrameCtrl.mEnd;
    return m->mFrameCtrl.checkPass(e - d);
}
/* |(a - b).xz| through cXyz::operator- and PSVECSquareMag/sqrtf on {x, 0, z} (word copies) */
static inline f32 p1_absXZ_mi(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> diff;
    gabi::Local<cXyz> xz;
    cXyz_mi(a, diff.get(), b);
    gabi::store<f32>(gabi::ea(xz.get()) + 4, 0.0f);
    gabi::store<u32>(gabi::ea(xz.get()) + 0, gabi::load<u32>(gabi::ea(diff.get()) + 0));
    gabi::store<u32>(gabi::ea(xz.get()) + 8, gabi::load<u32>(gabi::ea(diff.get()) + 8));
    f32 sq = PSVECSquareMag(xz.get());
    return std_sqrtf(sq);
}
static inline void p1_copy12(u32 dst, u32 src) {
    u32 a = gabi::load<u32>(src), b = gabi::load<u32>(src + 4), c = gabi::load<u32>(src + 8);
    gabi::store<u32>(dst, a);
    gabi::store<u32>(dst + 4, b);
    gabi::store<u32>(dst + 8, c);
}
/* matrix assignment: GHS loads all twelve values into FPRs (lfs), then stores them (stfs); the
 * recompiled lfs quiets a signalling NaN here, so the copy goes through f32 */
static inline void p1_mtx_copy(u32 dst, u32 src) {
    f32 t[12];
    for (int i = 0; i < 12; i++) t[i] = gabi::load<f32>(src + 4 * i);
    for (int i = 0; i < 12; i++) gabi::store<f32>(dst + 4 * i, t[i]);
}
/* J3DModel::getAnmMtx (HD: joint matrix block at +0x2C, dirty flag 0x10 at +4, matrices at +0x10) */
static inline u32 p1_getAnmMtx(J3DModel* model, s32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::load<u32>(blk + 0x10) + jntNo * 0x30;
}
/* GHS pointer to member function call */
static inline void p1_pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
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
/* checkAction(&daNpc_P1_c::fn) */
static inline bool p1_checkAction(daNpc_P1_c* a, u32 fn) {
    return a->mActionFunc.i == -1 && a->mActionFunc.d == 0 && a->mActionFunc.f == fn;
}
/* setAction(&daNpc_P1_c::fn, NULL, 0) (inline) */
static inline void p1_setAction(daNpc_P1_c* a, u32 fn) {
    ProcFunc_l* cur = &a->mActionFunc;
    s16 i = cur->i;
    if (i == -1 && cur->d == 0 && cur->f == fn) return;
    if (i != 0) {
        a->mActionStatus = -1; /* ACTION_ENDING */
        p1_pmf_call(a, cur, nullptr);
    }
    s16 oi = cur->i;
    s16 od = cur->d;
    u32 of = cur->f;
    a->mPrevAction.d = od;
    a->mPrevAction.i = oi;
    a->mPrevAction.f = of;
    cur->d = 0;
    cur->i = -1;
    cur->f = fn;
    a->mActionStatus = 0; /* ACTION_STARTING */
    gabi::call_ptr<BOOL>(cur->f, a, (u32)0);
}
/* setAction(mPrevAction, NULL, 0) (inline, generic pointer to member) */
static inline void p1_setActionPrev(daNpc_P1_c* a) {
    ProcFunc_l* cur = &a->mActionFunc;
    s16 ti = a->mPrevAction.i;
    u32 tf = a->mPrevAction.f;
    s16 td = a->mPrevAction.d;
    s16 ci = cur->i;
    if (ci == ti && (ci == 0 || (cur->d == td && cur->f == tf))) return;
    if (ci != 0) {
        a->mActionStatus = -1;
        p1_pmf_call(a, cur, nullptr);
    }
    s16 oi = cur->i;
    s16 od = cur->d;
    u32 of = cur->f;
    a->mPrevAction.d = od;
    a->mPrevAction.i = oi;
    a->mPrevAction.f = of;
    cur->d = td;
    cur->i = ti;
    cur->f = tf;
    a->mActionStatus = 0;
    p1_pmf_call(a, cur, nullptr);
}
#define P1_normalAction 0x022B43ECu
#define P1_confuseAction 0x022B4764u
#define P1_talkAction 0x022B48D0u
#define P1_speakAction 0x022B4E54u
#define P1_p1c_speakAction 0x022B53A0u
#define P1_explainAction 0x022B5744u
