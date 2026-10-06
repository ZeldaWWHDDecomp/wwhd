/* daShip_c (King of Red Lions), WWHD layout, and the translation unit's statics and local bindings.
 *
 * GameCube -> WWHD (size 0x19F8 -> 0xD19C, constructor inlined in create 0247C358):
 * - +0x11C up to mpLinkModel (mFrameCtrl is the HD J3DFrameCtrl, 0x10 bytes);
 * - mRopeLine (mDoExt_3DlineMat1_c) is 0x188 bytes (GameCube 0x3C), followed by the HD beacon model;
 * - HD-only: a cannon-sight McaMorf (0x5B4), a water shadow model (0x5B8) with its btk (0x5BC)
 *   and alpha (0x630);
 * - m034A..m0353 +0x2E9; mEvtStaffId..mNextMessageNo +0x2EC; mShadowId is gone (HD shadows):
 *   mSailAngle..m0392 +0x2E8; then one s16 fewer (+0x2E6) up to m03BC, m03C4.. +0x2E4;
 * - one HD float after m040C: mGridID.. +0x2E8, up to m19C0;
 * - HD-only after mSph: a J3DPacket subclass (0x1B54), 128 rope collider stts/spheres
 *   (0x1BF8 / 0x39F8), then the GameCube particle callbacks from 0xCFF8 (+0xB7BC). */
#pragma once
#include "bindings.h"

/* ---- statics of this translation unit ---- */
#define SAFESTRING_VTBL 0x1003A1ECu /* this TU's sead::SafeString vtable */
#define l_arcName 0x101D034Cu       /* "Ship" */
#define DASHIP_VTBL 0x1003A2ECu     /* daShip_c vtable (HD virtual destructor) */

/* ---- HD J3D (as in d_a_kb): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868; a model's
 * joint matrices are in a block at +0x2C (+4 dirty flags, +0x10 matrices); user area +0xB8 ---- */
#ifndef WWHD_SHIP_J3D_L
#define WWHD_SHIP_J3D_L
struct J3DMtxBlock_s {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static inline u32 j3dSys_getModel() { return gabi::load<u32>(0x104B462C); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline J3DMtxBlock_s* model_mtxBlock(J3DModel* m) { return gabi::at<J3DMtxBlock_s>(gabi::load<u32>(gabi::ea(m) + 0x2C)); }
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty) */
static inline Mtx34* model_getAnmMtx(J3DModel* m, s32 jnt) {
    J3DMtxBlock_s* blk = model_mtxBlock(m);
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}
/* J3DModel::setAnmMtx (HD: marks dirty, copies the twelve values) */
static inline void model_setAnmMtx(J3DModel* m, s32 jnt, const Mtx34* src) {
    J3DMtxBlock_s* blk = model_mtxBlock(m);
    blk->mFlags |= 0x10;
    mtx_copy(gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30), src);
}
static inline u32 model_getUserArea(u32 m) { return gabi::load<u32>(m + 0xB8); }
/* function-local static cXyz (guard word, value) */
static inline cXyz* ship_staticVec(u32 guard, u32 addr, f32 x, f32 y, f32 z) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        gabi::at<cXyz>(addr)->set(x, y, z);
    }
    return gabi::at<cXyz>(addr);
}

/* HD helpers defined in d_a_ship.cpp */
BOOL daShip_checkSirenInside();
void daShip_sightPacket_setMtx(u8* pkt);
void daShip_sightPacket_setSegMtx(u8* pkt, s32 idx, Mtx34* mtx);

#endif

/* ---- c_lib ---- */
static inline f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}

static inline BOOL cLib_chaseUC(be<u8>* v, u8 target, u8 step) { return gabi::call<BOOL>(0x0200F4FC, v, target, step); }
static inline void dKy_get_seacolor(GXColor* amb, GXColor* dif) { gabi::call(0x025602F0, amb, dif); }

/* ---- matrices ---- */
static inline Mtx34* mDoMtx_now() { return mDoMtx_stack_c::get(); }
static inline void mDoMtx_ZrotS(Mtx34* m, s16 z) { gabi::call(0x025F181C, m, z); }
static inline void mDoMtx_XrotS_l(Mtx34* m, s16 x) { gabi::call(0x025F18EC, m, x); }
static inline void PSMTXConcat_s(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline void PSMTXScale(Mtx34* m, f32 x, f32 y, f32 z) { gabi::call(0x028E945C, m, x, y, z); }
static inline void PSMTXCopy_s(const Mtx34* src, Mtx34* dst) { gabi::call(0x028E90D4, src, dst); }
static inline void PSMTXInverse(const Mtx34* src, Mtx34* dst) { gabi::call(0x028E91EC, src, dst); }
/* mDoMtx_stack_c::revConcat(m): now = m * now */
static inline void stack_revConcat(const Mtx34* m) { PSMTXConcat_s(m, mDoMtx_now(), mDoMtx_now()); }
static inline void stack_concat(const Mtx34* m) { PSMTXConcat_s(mDoMtx_now(), m, mDoMtx_now()); }
/* cMtx_copy(now, J3DSys::mCurrentMtx) */
static inline void stack_toCurrentMtx() { PSMTXCopy_s(mDoMtx_now(), J3DSys_mCurrentMtx()); }

/* J3DModel::setBaseScale with one value (scale at +0xBC) */
static inline void model_setBaseScale1(J3DModel* m, f32 s) {
    gabi::store<f32>(gabi::ea(m) + 0xBC, s);
    gabi::store<f32>(gabi::ea(m) + 0xC0, s);
    gabi::store<f32>(gabi::ea(m) + 0xC4, s);
}

/* ---- sead::SafeString comparison with the start stage name (as in d_a_bg) ---- */
struct daShip_SafeString {
    be<u32> mStr;
    be<u32> __vtbl;
};
static inline void ship_ss_assure(daShip_SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
static inline bool ship_ss_cmp(daShip_SafeString* a, daShip_SafeString* b) {
    ship_ss_assure(a);
    ship_ss_assure(a);
    u32 pa = a->mStr;
    ship_ss_assure(b);
    u32 pb = b->mStr;
    if (pa == pb)
        return true;
    pa = a->mStr;
    pb = b->mStr;
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = gabi::load<u8>(pa + n);
        u8 cb = gabi::load<u8>(pb + n);
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 */
static inline bool ship_isStartStage(u32 lit) {
    gabi::Local<daShip_SafeString> a;
    a->mStr = lit;
    a->__vtbl = SAFESTRING_VTBL;
    u32 play = dComIfGp_ea();
    gabi::Local<daShip_SafeString> b;
    b->__vtbl = SAFESTRING_VTBL;
    b->mStr = play + 0x5134;
    return ship_ss_cmp(a.get(), b.get());
}

/* ---- draw (HD) ---- */
struct mDoExt_3DlineMat1_l { u8 _[0x188]; }; /* HD size (constructor 025EB82C) */
/* j3dSys draw buffers (opa 0x104B4634, xlu 0x104B4638); play lists: P1 0x5D58/0x5D60, normal 0x5D78/0x5D7C */
static inline void dComIfGd_setListP1() {
    gabi::store<u32>(0x104B4634, gabi::load<u32>(dComIfGp_ea() + 0x5D58));
    gabi::store<u32>(0x104B4638, gabi::load<u32>(dComIfGp_ea() + 0x5D60));
}
static inline void mDoMtx_lookAt(Mtx34* m, const cXyz* eye, const cXyz* center, s16 bank) { gabi::call(0x025F1C90, m, eye, center, bank); }
static inline s32 JUTNameTab_getIndex_s(u32 tab, u32 name) { return gabi::call<s32>(0x027DF9B0, tab, name); }
/* 027FA7F8 / 027FA678 (HD, on a 0x3C-byte per-model material entry): the texture SRT info and the
 * texture matrix pointer slot of tex matrix i */
static inline u32 hd_matGetTexSrt(u32 mat, s32 i) { return gabi::call<u32>(0x027FA7F8, mat, i); }
static inline u32 hd_matGetTexMtxSlot(u32 mat, s32 i) { return gabi::call<u32>(0x027FA678, mat, i); }
static inline void mDoExt_3DlineMat1_update(mDoExt_3DlineMat1_l* l, u16 segs, f32 size, const GXColor* color, u16 space,
                                            dKy_tevstr_c* tev) {
    gabi::call(0x025EC62C, l, segs, size, color, space, tev);
}
/* dComIfGd_set3DlineMat: the play's line packets (play+0x5FB4, 0x9C each) by material id (virtual +0x14,
 * vtable at +0x130) */
static inline void dComIfGd_set3DlineMat(mDoExt_3DlineMat1_l* l) {
    u32 pkt = dComIfGp_ea() + 0x5FB4;
    s32 id = gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(l) + 0x130) + 0x14), l);
    gabi::call(0x025EDD04, pkt + id * 0x9C, l);
}

/* ---- save data (HD: through the pointer at 0x101F84DC) ---- */
static inline u32 dSv_base() { return gabi::load<u32>(0x101F84DC); }
static inline BOOL dComIfGs_isEventBit(u16 flag) { return dSv_event_isEventBit(gabi::at<dSv_event_c>(dSv_base() + 0x644), flag); }
static inline void dComIfGs_onEventBit(u16 flag) { dSv_event_onEventBit(gabi::at<dSv_event_c>(dSv_base() + 0x644), flag); }
static inline BOOL dComIfGs_isGetItem(s32 i, s32 j) { return gabi::call<BOOL>(0x025B5D40, dSv_base() + 0x71, i, j); }
static inline BOOL dComIfGs_isSymbol(u8 i) { return gabi::call<BOOL>(0x025B7D90, dSv_base() + 0xD4, i); }
static inline BOOL dComIfGs_isTact(u8 i) { return gabi::call<BOOL>(0x025B7B10, dSv_base() + 0xD4, i); }
static inline s32 dComIfGs_getTriforceNum() { return gabi::call<s32>(0x025B7E00, dSv_base() + 0xD4); }
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
enum { dItemNo_BOMB_BAG_e = 0x31 };
enum { dSymbol_NAYRU_e = 0, dSymbol_DIN_e = 1, dSymbol_FARORE_e = 2 };
/* daPy_getPlayerActorClass()->checkMasterSwordEquip() (HD inline: the equipped sword in the save
 * data, +0x2E; the player pointer is fetched and unused) */
static inline bool ship_checkMasterSwordEquip() {
    dComIfGp_get();
    u8 sword = gabi::load<u8>(dSv_base() + 0x2E);
    return sword == 0x39 || sword == 0x3A || sword == 0x3E;
}
/* dStage_stagInfo_GetSTType(dComIfGp_getStageStagInfo()): virtual getStagInfo (+0x15C) of the stage
 * data at play+0x5150 */
static inline u32 ship_getStageType() {
    u32 dt = dComIfGp_ea() + 0x5150;
    u32 info = gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(dt) + 0x15C), dt);
    return (gabi::load<u32>(info + 0xC) >> 16) & 7;
}
enum { dStageType_DUNGEON_e = 1, dStageType_SEA_e = 7 };
static inline cXyz* dKyw_get_wind_vec() { return gabi::call<cXyz*>(0x0257DAA8); }
static inline be<f32>* dKyw_get_wind_power() { return gabi::call<be<f32>*>(0x0257DB04); }
static inline void dKyw_tact_wind_set(s16 x, s16 y) { gabi::call(0x0257E490, x, y); }
/* 02030B38: plays a named sound effect (HD) */
static inline void hd_seStartByName(u32 name) { gabi::call(0x02030B38, name); }
/* pad n (HD): R lock held (02007CDC); X / Y / Z triggers (020078E8 / 02007914 / 02007814); a fourth
 * trigger (02007898, bit 0x1 of the trigger word) */
static inline BOOL CPad_R_LOCK_BUTTON(s32 pad) { return gabi::call<BOOL>(0x02007CDC, pad); }
static inline BOOL CPad_CHECK_TRIG_X(s32 pad) { return gabi::call<BOOL>(0x020078E8, pad); }
static inline BOOL CPad_CHECK_TRIG_Y(s32 pad) { return gabi::call<BOOL>(0x02007914, pad); }
static inline BOOL CPad_CHECK_TRIG_Z(s32 pad) { return gabi::call<BOOL>(0x02007814, pad); }
static inline BOOL CPad_CHECK_TRIG_HD1(s32 pad) { return gabi::call<BOOL>(0x02007898, pad); }
/* held: X / Y / Z (0200770C / 02007738 / 02007638) and the fourth button (020076BC) */
static inline BOOL CPad_CHECK_HOLD_X(s32 pad) { return gabi::call<BOOL>(0x0200770C, pad); }
static inline BOOL CPad_CHECK_HOLD_Y(s32 pad) { return gabi::call<BOOL>(0x02007738, pad); }
static inline BOOL CPad_CHECK_HOLD_Z(s32 pad) { return gabi::call<BOOL>(0x02007638, pad); }
static inline BOOL CPad_CHECK_HOLD_HD1(s32 pad) { return gabi::call<BOOL>(0x020076BC, pad); }
enum { dItemNo_GRAPPLING_HOOK_e = 0x25 };
/* dVibration_c (play+0x599C) */
static inline void ship_StartShock(s32 a, s32 b, cXyz* v) { gabi::call(0x025CB374, dComIfGp_ea() + 0x599C, a, b, v); }
static inline void ship_StartQuake(s32 a, s32 b, cXyz* v) { gabi::call(0x025CB408, dComIfGp_ea() + 0x599C, a, b, v); }
static inline void ship_StopQuake(s32 a) { gabi::call(0x025CB610, dComIfGp_ea() + 0x599C, a); }
/* dAttention_c (play+0x5804) */
static inline BOOL dAttention_LockonTruth(u32 att) { return gabi::call<BOOL>(0x024EDFCC, att); }
static inline u32 dAttention_GetLockonList(u32 att, s32 i) { return gabi::call<u32>(0x024EE058, att, i); }
static inline fopAc_ac_c* dAttList_getActor(u32 l) { return gabi::call<fopAc_ac_c*>(0x024EBA14, l); }
/* camera inversion option (HD save options: 027200D0(save+0x12C0) + 2) */
static inline bool ship_isCameraInverted() { return gabi::load<u8>(gabi::call<u32>(0x027200D0, dSv_base() + 0x12C0) + 2) == 0; }
static inline void mDoAud_seStart_simple(u32 id) { gabi::call(0x025E1988, id); }
/* 02007D4C: R lock trigger on pad n (HD) */
static inline BOOL CPad_R_LOCK_TRIGGER(s32 pad) { return gabi::call<BOOL>(0x02007D4C, pad); }

/* ---- messages (HD: one message manager, *0x101F4B5C) ---- */
static inline u32 ship_msgMng() { return gabi::load<u32>(0x101F4B5C); }
static inline u32 fopMsgM_messageSet(u32 mng, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mng, msgNo, pos); }
/* 025F795C (matcher: fopMsgM_SearchByID): the manager's message status */
static inline s32 msgMng_getStatus(u32 mng) { return gabi::call<s32>(0x025F795C, mng); }
static inline void msgMng_setStatus(u32 mng, s32 st) { gabi::call(0x025F74D0, mng, st); }
enum { fopMsgStts_MSG_DISPLAYED_e = 0xE, fopMsgStts_MSG_CONTINUES_e = 0xF, fopMsgStts_MSG_ENDS_e = 0x10,
       fopMsgStts_BOX_CLOSED_e = 0x12, fopMsgStts_MSG_DESTROYED_e = 0x13 };

/* ---- camera ---- */
static inline s32 dComIfGp_getPlayerCameraID0() { return gabi::load<s8>(dComIfGp_ea() + 0x5B30); }
static inline bool dComIfGp_checkCameraAttentionStatus(s32 id, u32 flag) {
    return (gabi::load<u32>(dComIfGp_ea() + 0x5B00 + id * 0x34) & flag) != 0;
}
static inline u32 dComIfGp_getCamera(s32 id) { return gabi::load<u32>(dComIfGp_ea() + 0x5AF8 + id * 0x34); }
/* camera_process_class: dCamera_c mCamera at +0x248; Center() = +0x258 + shake +0x7B4, Eye() = +0x264 + +0x7C0 */
static inline void camera_Center(u32 cam, cXyz* out) { cXyz_pl(gabi::at<cXyz>(cam + 0x258), out, gabi::at<cXyz>(cam + 0x7B4)); }
static inline void camera_Eye(u32 cam, cXyz* out) { cXyz_pl(gabi::at<cXyz>(cam + 0x264), out, gabi::at<cXyz>(cam + 0x7C0)); }
/* dCamera_c::Set(cXyz center, cXyz eye): by value (pointers to copies) */
static inline void camera_Set(u32 cam, cXyz* center, cXyz* eye) { gabi::call(0x02514F50, cam + 0x248, center, eye); }
static inline void camera_Stop(u32 cam) { gabi::call(0x02514F2C, cam + 0x248); }
static inline void camera_Start(u32 cam) { gabi::call(0x02514F38, cam + 0x248); }
static inline void camera_Reset(u32 cam) { gabi::call(0x02515048, cam + 0x248); }
/* cXyz::normalize: normalizes in place and returns a copy through the hidden result pointer */
static inline void cXyz_normalize(cXyz* v, cXyz* res) { gabi::call(0x0201B31C, v, res); }
static inline f32 cXyz_abs(cXyz* v) { return std_sqrtf(PSVECSquareMag(v)); }

/* ---- resources ---- */
static inline J3DAnmTransform* ship_getRes(s32 idx) { return (J3DAnmTransform*)dComIfG_getObjectRes(STR(l_arcName), idx, SAFESTRING_VTBL); }
/* J3DAnmBase::getFrameMax (virtual +0x14, vtable at +4) */
static inline s32 anm_getFrameMax(J3DAnmTransform* a) { return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(a) + 4) + 0x14), a); }
static inline void J3DFrameCtrl_init(J3DFrameCtrl* f, s32 end) { gabi::call(0x027F2BC0, f, end); }

/* ---- pad (HD) ---- */
/* 020076E0: A held on pad n; 02007C50: analog R of pad n (f1, possibly not rounded to single) */
static inline BOOL CPad_CHECK_HOLD_A(s32 pad) { return gabi::call<BOOL>(0x020076E0, pad); }
static inline f64 CPad_GET_ANALOG_R(s32 pad) { return gabi::call<f64>(0x02007C50, pad); }

/* ---- line checks: this TU's dBgS_LinChk vtables (constructor / destructor) ---- */
static const dBgS_LinChk_vt SHIP_LINCHK_VT = {0x1003A2AC, 0x1003A2BC, 0x1003A2DC, 0x1003A2CC};
static inline void ship_LinChk_dt(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x58, 0x1003A2DC);
    gabi::store<u32>(b + 0x64, 0x1003A26C);
    gabi::store<u32>(b + 0x20, 0x1003A25C);
    gabi::call(0x02008B4C, c, 0); /* cBgS_LinChk::~cBgS_LinChk */
}
static inline void cLib_offsetPos(cXyz* out, const cXyz* base, s16 angle, const cXyz* ofs) { gabi::call(0x0200FA40, out, base, angle, ofs); }

/* ---- events / demo ---- */
static inline u32 ship_getSubst(s32 staffId, u32 name, s32 type) {
    return gabi::ea(dComIfGp_evmng_getMySubstanceP(staffId, STR(name), type));
}
/* dComIfGp_event_getPt1(): dEvt_control_c::convPId(play+0x51D0, play+0x5294) */
static inline fopAc_ac_c* dComIfGp_event_getPt1() {
    u32 play = dComIfGp_ea();
    return gabi::call<fopAc_ac_c*>(0x0253EE04, play + 0x51D0, gabi::load<u32>(play + 0x5294));
}
static inline cXyz* dComIfGp_evmng_getGoal() { return gabi::call<cXyz*>(0x02544900, dComIfGp_ea() + 0x52C4); }
/* dComIfGp_demo_getActor(id) (HD inline: the demo object *0x101D5FFC, asserted) */
static inline u32 ship_demo_getActor(u8 id) {
    if (id == 0 || id > 0x20)
        return 0;
    if (gabi::load<u32>(0x101D5FFC) == 0)
        JUT_ASSERT_fail(STR(0x1003A324), 0x23A, STR(0x1003A314));
    return gabi::call<u32>(0x02526E70, gabi::load<u32>(0x101D5FFC), id);
}

/* ---- tornado (daTornado_c: model at +0x3B4) ---- */
/* daTornado_c::getJointXPos/YPos/ZPos(i): joint i of its model, or its position without a model */
static inline f32 tornado_getJointPos(fopAc_ac_c* t, s32 jnt, int axis) {
    J3DModel* m = gabi::at<J3DModel>(gabi::load<u32>(gabi::ea(t) + 0x3B4));
    if (m != nullptr)
        return model_getAnmMtx(m, jnt)->m[axis][3];
    return axis == 0 ? (f32)t->current.pos.x : axis == 1 ? (f32)t->current.pos.y : (f32)t->current.pos.z;
}
/* the first of 11 tornado joints above y (10 if none) */
static inline s32 tornado_findJoint(fopAc_ac_c* t, be<f32>* y) {
    s32 i = 0;
    for (; i < 11; i++) {
        if (*y < tornado_getJointPos(t, i, 1))
            break;
    }
    if (i == 11)
        i = 10;
    return i;
}
static inline BOOL cLib_chaseS(be<s16>* v, s16 target, s16 step) { return gabi::call<BOOL>(0x0200F564, v, target, step); }
static inline BOOL fpcM_IsCreating(u32 id) { return gabi::call<BOOL>(0x025DD868, id); }
static inline void dPa_followEcallBack_end_l(dPa_followEcallBack* cb) { gabi::call(0x025A5AC8, cb); }
static inline void dStage_changeScene(s32 exit, f32 speed, u32 mode, s8 roomNo) { gabi::call(0x025C3748, exit, speed, mode, roomNo); }
static inline f32 cM_atan2f(f32 y, f32 x) { return gabi::call<f32>(0x0201971C, y, x); }
static inline u32 dCam_getBody() { return gabi::call<u32>(0x024F8044); }
static inline void dCamera_SetTypeForce(u32 cam, u32 name, fopAc_ac_c* a) { gabi::call(0x02514EE4, cam, name, a); }
static inline BOOL daPy_shipSpecialDemoStart(fopAc_ac_c* link) { return gabi::call<BOOL>(0x0244300C, link); }
static inline f32 cLib_addCalcPosXZ(cXyz* p, const cXyz* t, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200EF78, p, t, scale, maxStep, minStep);
}

/* ---- player (daPy_lk_c) ---- */
/* dComIfGp_checkPlayerStatus0(0, flag): play+0x5CD8 */
static inline u32 ship_playerStatus0() { return gabi::load<u32>(dComIfGp_ea() + 0x5CD8); }
enum { daPyStts0_SHIP_RIDE_e = 0x10000 };
/* HD inline: the link player (play+0x5B34, read first) is the current player 0 (play+0x5B2C) */
static inline bool ship_isLinkPlayer0() {
    u32 link = gabi::ea(dComIfGp_getLinkPlayer());
    u32 p0 = gabi::ea(dComIfGp_getPlayer(0));
    return p0 == link;
}

/* ---- daShip_c ---- */
struct mDoExt_btkAnm_l {
    /* 0x00 */ J3DFrameCtrl mFrameCtrl;
    /* 0x10 */ u8 _10[0x74 - 0x10];
};

struct daShip_c : fopAc_ac_c {
    /* 0x03AC */ request_of_phase_process_class mPhs;
    /* 0x03B4 */ gptr<mDoExt_McaMorf> mpBodyAnm;
    /* 0x03B8 */ gptr<mDoExt_McaMorf> mpHeadAnm;
    /* 0x03BC */ be<u32> m02A0; /* J3DTexMtx* */
    /* 0x03C0 */ be<u32> m02A4; /* J3DTexMtx* */
    /* 0x03C4 */ Mtx34 m02A8;
    /* 0x03F4 */ u8 m02D8[0x10];
    /* 0x0404 */ gptr<J3DAnmTransform> mAnmTransform;
    /* 0x0408 */ J3DFrameCtrl mFrameCtrl;
    /* 0x0418 */ gptr<J3DModel> mpCannonModel;
    /* 0x041C */ gptr<J3DModel> mpSalvageArmModel;
    /* 0x0420 */ gptr<J3DModel> mpLinkModel;
    /* 0x0424 */ mDoExt_3DlineMat1_l mRopeLine;
    /* 0x05AC */ gptr<J3DModel> mpHD5AC;          /* HD: beacon model ("beacon.bdl"), also the sight packet's model */
    /* 0x05B0 */ gptr<J3DModel> mpHD5B0;          /* HD: cannon-sight impact marker */
    /* 0x05B4 */ gptr<mDoExt_McaMorf> mpHD5B4;    /* HD: cannon sight */
    /* 0x05B8 */ gptr<J3DModel> mpShadowModel;    /* HD */
    /* 0x05BC */ mDoExt_btkAnm_l mShadowBtk;      /* HD */
    /* 0x0630 */ be<u8> mShadowAlpha;             /* HD */
    /* 0x0631 */ be<u8> mHD631;
    /* 0x0632 */ be<u8> mHD632;
    /* 0x0633 */ be<s8> m034A;
    /* 0x0634 */ be<u8> m034B;
    /* 0x0635 */ be<u8> mCurMode;
    /* 0x0636 */ be<u8> mNextMode;
    /* 0x0637 */ be<u8> mPart;
    /* 0x0638 */ be<u8> m034F;
    /* 0x0639 */ be<u8> m0350;
    /* 0x063A */ be<u8> m0351;
    /* 0x063B */ be<u8> m0352;
    /* 0x063C */ be<u8> m0353;
    /* 0x063D */ be<u8> mHD63D;
    /* 0x063E */ u8 _63E[2];
    /* 0x0640 */ be<s32> mEvtStaffId;
    /* 0x0644 */ be<u32> mStateFlag;
    /* 0x0648 */ be<u32> mNextMessageNo;
    /* 0x064C */ be<s16> mSailAngle;
    /* 0x064E */ be<s16> m0366;
    /* 0x0650 */ u8 m0368[4];
    /* 0x0654 */ be<s16> m036C;
    /* 0x0656 */ be<s16> m036E;
    /* 0x0658 */ be<s16> m0370;
    /* 0x065A */ be<s16> m0372;
    /* 0x065C */ be<s16> m0374;
    /* 0x065E */ be<s16> m0376;
    /* 0x0660 */ be<s16> m0378;
    /* 0x0662 */ be<s16> m037A;
    /* 0x0664 */ be<s16> m037C;
    /* 0x0666 */ be<s16> m037E;
    /* 0x0668 */ be<s16> m0380;
    /* 0x066A */ be<s16> m0382;
    /* 0x066C */ be<s16> m0384;
    /* 0x066E */ be<s16> m0386;
    /* 0x0670 */ be<s16> m0388;
    /* 0x0672 */ be<u16> m038A;
    /* 0x0674 */ be<s16> m038C;
    /* 0x0676 */ be<s16> m038E;
    /* 0x0678 */ be<u16> m0390;
    /* 0x067A */ be<u16> m0392;
    /* 0x067C */ be<s16> m0394;
    /* 0x067E */ be<s16> m0396;
    /* 0x0680 */ be<s16> m0398;
    /* 0x0682 */ be<s16> mCraneBaseAngle;
    /* 0x0684 */ be<s16> m039C;
    /* 0x0686 */ be<s16> mRopeCnt;
    /* 0x0688 */ be<s16> m03A0;
    /* 0x068A */ be<s16> m03A2;
    /* 0x068C */ be<s16> mStickMAng;
    /* 0x068E */ be<s16> m03A6;
    /* 0x0690 */ be<s16> m03A8;
    /* 0x0692 */ be<s16> m03AA;
    /* 0x0694 */ be<s16> m03AC;
    /* 0x0696 */ be<s16> m03AE;
    /* 0x0698 */ be<s16> m03B0;
    /* 0x069A */ be<s16> m03B2;
    /* 0x069C */ be<u16> m03B4;
    /* 0x069E */ be<s16> m03B6;
    /* 0x06A0 */ be<s16> m03B8;
    /* 0x06A2 */ csXyz m03BC;
    /* 0x06A8 */ be<s32> m03C4;
    /* 0x06AC */ be<s32> mTactWarpPosNum;
    /* 0x06B0 */ be<s32> m03CC;
    /* 0x06B4 */ be<f32> m03D0;
    /* 0x06B8 */ be<f32> m03D4;
    /* 0x06BC */ be<f32> m03D8;
    /* 0x06C0 */ be<f32> mTillerAngleRate;
    /* 0x06C4 */ be<f32> m03E0;
    /* 0x06C8 */ be<f32> mJumpRate;
    /* 0x06CC */ be<f32> m03E8;
    /* 0x06D0 */ be<f32> mStickMVal;
    /* 0x06D4 */ u8 m03F0[4];
    /* 0x06D8 */ be<f32> m03F4;
    /* 0x06DC */ be<f32> m03F8;
    /* 0x06E0 */ be<f32> mFwdVel;
    /* 0x06E4 */ be<f32> m0400;
    /* 0x06E8 */ be<f32> m0404;
    /* 0x06EC */ be<f32> m0408;
    /* 0x06F0 */ be<f32> m040C;
    /* 0x06F4 */ be<f32> mHD6F4;
    /* 0x06F8 */ be<u32> mGridID;
    /* 0x06FC */ be<u32> mpGrid;
    /* 0x0700 */ be<u32> mTornadoID;
    /* 0x0704 */ gptr<fopAc_ac_c> mTornadoActor;
    /* 0x0708 */ be<u32> mWhirlID;
    /* 0x070C */ gptr<fopAc_ac_c> mWhirlActor;
    /* 0x0710 */ gptr<cXyz> m0428;
    /* 0x0714 */ be<u32> mTactWarpID;
    /* 0x0718 */ be<u32> m0430;
    /* 0x071C */ gptr<cXyz> mCraneTop;
    /* 0x0720 */ cXyz mTillerTopPos;
    /* 0x072C */ cXyz m0444;
    /* 0x0738 */ cXyz m0450;
    /* 0x0744 */ cXyz m045C;
    /* 0x0750 */ cXyz mRopeLineSegments[250];
    /* 0x1308 */ cXyz mCraneRipplePos;
    /* 0x1314 */ cXyz m102C;
    /* 0x1320 */ cXyz m1038;
    /* 0x132C */ cXyz m1044;
    /* 0x1338 */ cXyz m1050;
    /* 0x1344 */ cXyz mEffPos;
    /* 0x1350 */ cXyz m1068;
    /* 0x135C */ cXyz m1074;
    /* 0x1368 */ dBgS_AcchCir mAcchCir[4];
    /* 0x1468 */ dBgS_ObjAcch mAcch;
    /* 0x162C */ dCcD_Stts mStts;
    /* 0x1668 */ dCcD_Cyl mCyl[3];
    /* 0x19F8 */ dCcD_Sph mSph;
    /* 0x1B24 */ u8 mHD1B24[0x30];                /* HD */
    /* 0x1B54 */ u8 mHDPacket[0xA4];              /* HD: J3DPacket subclass (draw 02471FB0) */
    /* 0x1BF8 */ dCcD_Stts mRopeStts[128];        /* HD */
    /* 0x39F8 */ dCcD_Sph mRopeSph[128];          /* HD */
    /* 0xCFF8 */ u8 mWaveR[0x64];
    /* 0xD05C */ u8 mWaveL[0x64];
    /* 0xD0C0 */ u8 mSplash[0x1C];
    /* 0xD0DC */ u8 mTrack[0x50];
    /* 0xD12C */ u8 mRipple[0x14];
    /* 0xD140 */ dPa_followEcallBack m1984;
    /* 0xD154 */ dPa_followEcallBack m1998;
    /* 0xD168 */ dPa_followEcallBack m19AC;
    /* 0xD17C */ u8 m19C0[0x14];
    /* 0xD190 */ u8 mProc[0xC];                   /* pointer to member function */

    u32 checkStateFlg(u32 flag) { return mStateFlag & flag; }
    void onStateFlg(u32 flag) { mStateFlag |= flag; }
    void offStateFlg(u32 flag) { mStateFlag &= ~flag; }

    BOOL bodyJointCallBack(int jno);
    BOOL cannonJointCallBack(int jno);
    BOOL craneJointCallBack();
    BOOL headJointCallBack0();
    BOOL headJointCallBack1(int jno);
    BOOL draw();
    void drawShadow();
    BOOL checkForceMessage();
    void setInitMessage();
    BOOL setNextMessage(msg_class* msg);
    BOOL checkFrontObstacle();
    BOOL procReady_init();
    BOOL procToolDemo_init();
    BOOL procZevDemo_init();
    void sightStep();
    void setShadowBtkRate();
    void initStartPos(cXyz* pos, s16 angle) { gabi::call(0x024832E0, this, pos, angle); }
    void getMaxWaterY(cXyz* pos);
    BOOL procTornadoUp_init();
    void setTornadoActor();
    BOOL procWhirlDown_init();
    void setWhirlActor();
    u32 seStart(u32 se, cXyz* pos);
    BOOL procTalkReady_init();
    BOOL procTalk_init();
    BOOL checkOutRange();
    void firstDecrementShipSpeed(f32 speed);
    BOOL procCraneUp_init();
    void setControllAngle(s16 angle);
    s16 getAimControllAngle(s16 ref);
    void setRoomInfo();
    f32 getWaterY();
    void setYPos();
    void setWaveAngle(be<s16>* a, be<s16>* b);
    void setHeadAnm();
    f32 getAnglePartRate();
    void incRopeCnt(int len, int min);
    void setRopePos();
    void setEffectData(f32 y, s16 angle);
    BOOL execute();
    BOOL shipDelete();
    BOOL createHeap();
    void setPartOffAnime();
    void setPartOnAnime(u8 part);
    void setPartAnimeInit(u8 part);
    BOOL procSteerMove_init();
    BOOL procPaddleMove_init();
    BOOL procStartModeWarp_init();
    BOOL procStartModeThrow_init();
    BOOL procWait_init();
    cPhs_State create();
    void setSailAngle();
    void setMoveAngle(s16 angle);
    f32 decrementShipSpeed(f32 speed);
    BOOL procCannonReady_init();
    BOOL procCraneReady_init();
    void changeDemoEndProc();
    BOOL setCrashData(s16 angle);
    BOOL procGetOff_init();
    BOOL procTactWarp_init();
    BOOL checkNextMode(int mode);
    void setSelfMove(int p);
    BOOL procWait();
    BOOL procReady();
    BOOL procSteerMove();
    BOOL procPaddleMove();
    BOOL procCannon_init();
    BOOL procCannonReady();
    BOOL procCannon();
    BOOL procCrane_init();
    BOOL procCraneReady();
    BOOL procCrane();
    BOOL procCraneUp();
    BOOL procGetOff();
    BOOL procToolDemo();
    BOOL procZevDemo();
    BOOL procTalkReady();
    BOOL procTurn_init();
    BOOL procTalk();
    BOOL procTurn();
    BOOL procTornadoUp();
    BOOL procStartModeWarp();
    BOOL procTactWarp();
    BOOL procWhirlDown();
    BOOL procStartModeThrow();

    /* mProc = &daShip_c::procX (GHS pointer to member: this delta, vtable index -1, function) */
    void setProc(u32 fn) {
        gabi::store<s16>(gabi::ea(this) + 0xD190, 0);
        gabi::store<s16>(gabi::ea(this) + 0xD192, -1);
        gabi::store<u32>(gabi::ea(this) + 0xD194, fn);
    }
    bool isProc(u32 fn) {
        return gabi::load<s16>(gabi::ea(this) + 0xD192) == -1 && gabi::load<s16>(gabi::ea(this) + 0xD190) == 0 &&
               gabi::load<u32>(gabi::ea(this) + 0xD194) == fn;
    }
};
WWHD_OFFSET(daShip_c, mpCannonModel, 0x418);
WWHD_OFFSET(daShip_c, mpHD5B4, 0x5B4);
WWHD_OFFSET(daShip_c, mShadowAlpha, 0x630);
WWHD_OFFSET(daShip_c, mStateFlag, 0x644);
WWHD_OFFSET(daShip_c, m0392, 0x67A);
WWHD_OFFSET(daShip_c, m03C4, 0x6A8);
WWHD_OFFSET(daShip_c, mGridID, 0x6F8);
WWHD_OFFSET(daShip_c, mRopeLineSegments, 0x750);
WWHD_OFFSET(daShip_c, mAcchCir, 0x1368);
WWHD_OFFSET(daShip_c, mStts, 0x162C);
WWHD_OFFSET(daShip_c, mSph, 0x19F8);
WWHD_OFFSET(daShip_c, mRopeStts, 0x1BF8);
WWHD_OFFSET(daShip_c, mRopeSph, 0x39F8);
WWHD_OFFSET(daShip_c, mWaveR, 0xCFF8);
WWHD_OFFSET(daShip_c, mProc, 0xD190);

enum daSHIP_SFLG {
    daSFLG_FLY_e = 0x00000001,
    daSFLG_UNK2_e = 0x00000002,
    daSFLG_UNK4_e = 0x00000004,
    daSFLG_UNK8_e = 0x00000008,
    daSFLG_UNK10_e = 0x00000010,
    daSFLG_UNK20_e = 0x00000020,
    daSFLG_JUMP_e = 0x00000040,
    daSFLG_LAND_e = 0x00000080,
    daSFLG_UNK100_e = 0x00000100,
    daSFLG_SAIL_ON_e = 0x00000200,
    daSFLG_UNK400_e = 0x00000400,
    daSFLG_UNK800_e = 0x00000800,
    daSFLG_UNK1000_e = 0x00001000,
    daSFLG_JUMP_RIDE_e = 0x00002000,
    daSFLG_JUMP_OK_e = 0x00004000,
    daSFLG_UNK8000_e = 0x00008000,
    daSFLG_UNK10000_e = 0x00010000,
    daSFLG_SHOOT_CANNON_e = 0x00020000,
    daSFLG_CRANE_UP_END_e = 0x00040000,
    daSFLG_UNK80000_e = 0x00080000,
    daSFLG_UNK100000_e = 0x00100000,
    daSFLG_HEAD_NO_DRAW_e = 0x00200000,
    daSFLG_UNK400000_e = 0x00400000,
    daSFLG_UNK800000_e = 0x00800000,
    daSFLG_UNK1000000_e = 0x01000000,
    daSFLG_UNK2000000_e = 0x02000000,
    daSFLG_UNK4000000_e = 0x04000000,
    daSFLG_UNK8000000_e = 0x08000000,
    daSFLG_UNK10000000_e = 0x10000000,
    daSFLG_UNK20000000_e = 0x20000000,
    daSFLG_UNK40000000_e = 0x40000000,
    daSFLG_UNK80000000_e = 0x80000000,
};

enum {
    MODE_WAIT_e = 0, MODE_STEER_MOVE_e = 1, MODE_PADDLE_MOVE_e = 2, MODE_READY_FIRST_e = 3,
    MODE_READY_SECOND_e = 4, MODE_GET_OFF_FIRST_e = 5, MODE_GET_OFF_SECOND_e = 6, MODE_TALK_e = 8,
    MODE_CANNON_e = 9, MODE_CRANE_e = 10, MODE_CRANE_UP_e = 11, MODE_TORNADO_UP_e = 12,
    MODE_START_MODE_WARP_e = 13, MODE_TACT_WARP_e = 14, MODE_START_MODE_THROW_e = 16,
};
enum { PART_WAIT_e = 0, PART_STEER_e = 1, PART_CANNON_e = 2, PART_CRANE_e = 3 };

/* resources and joints (res/Object/Ship.h) */
enum {
    dRes_INDEX_SHIP_BCK_AKIBI1_e = 0x5, dRes_INDEX_SHIP_BCK_DAMAGE1_e = 0x6, dRes_INDEX_SHIP_BCK_FN_LOOK_L_e = 0x7,
    dRes_INDEX_SHIP_BCK_FN_LOOK_R_e = 0x8, dRes_INDEX_SHIP_BCK_FN_LOSE1_e = 0x9, dRes_INDEX_SHIP_BCK_FN_MAST_OFF2_e = 0xA,
    dRes_INDEX_SHIP_BCK_FN_MAST_ON2_e = 0xB, dRes_INDEX_SHIP_BCK_FN_TALK_A_e = 0xC, dRes_INDEX_SHIP_BCK_FN_TALK_B_e = 0xD,
    dRes_INDEX_SHIP_BCK_KYAKKAN1_e = 0xE, dRes_INDEX_SHIP_BDL_FN_BODY_e = 0x11, dRes_INDEX_SHIP_BDL_FN_HEAD_H_e = 0x12,
    dRes_INDEX_SHIP_BDL_VFNCN_e = 0x13, dRes_INDEX_SHIP_BDL_VFNCR_e = 0x14,
};
enum {
    FN_BODY_JNT_J_FN_GATTAI_e = 0x4, FN_BODY_JNT_J_FN_KAJI_e = 0x5, FN_BODY_JNT_J_FN_MAST_e = 0x6,
    FN_BODY_JNT_J_FN_SAIL1_e = 0x7, FN_BODY_JNT_J_FN_STEER1_e = 0xA,
    FN_HEAD_H_JNT_J_FN_KUBI1_e = 0x2, FN_HEAD_H_JNT_J_FN_KUBI6_e = 0x7, FN_HEAD_H_JNT_J_FN_ATAMA_e = 0x8,
    FN_HEAD_H_JNT_J_FN_AGO2_e = 0xA,
    VFNCN_JNT_CANON1_e = 0x1, VFNCN_JNT_CANON2_e = 0x2, VFNCR_JNT_V_CRANE_ROTATION_e = 0x1,
};
