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
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/m_Do/m_Do_mtx.h"

/* ---- statics of this translation unit ---- */
#define SAFESTRING_VTBL 0x1003A1ECu /* this TU's sead::SafeString vtable */
#define l_arcName 0x101D034Cu       /* "Ship" */
#define DASHIP_VTBL 0x1003A2ECu     /* daShip_c vtable (HD virtual destructor) */

/* ---- HD J3D (as in d_a_kb): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868; a model's
 * joint matrices are in a block at +0x2C (+4 dirty flags, +0x10 matrices); user area +0xB8 ---- */
#ifndef WWHD_SHIP_J3D_L
#define WWHD_SHIP_J3D_L
struct ShipMtxBlock {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
static inline u32 j3dSys_getModel() { return gabi::load<u32>(0x104B462C); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline ShipMtxBlock* model_mtxBlock(J3DModel* m) { return reinterpret_cast<ShipMtxBlock*>(m->mpMtxBuffer.get()); }
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty) */
static inline Mtx34* model_getAnmMtx(J3DModel* m, s32 jnt) {
    ShipMtxBlock* blk = model_mtxBlock(m);
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}
/* J3DModel::setAnmMtx (HD: marks dirty, copies the twelve values) */
static inline void model_setAnmMtx(J3DModel* m, s32 jnt, const Mtx34* src) {
    ShipMtxBlock* blk = model_mtxBlock(m);
    blk->mFlags |= 0x10;
    mtx_copy(gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30), src);
}
static inline u32 model_getUserArea(u32 m) { return gabi::at<J3DModel>(m)->mUserArea; }
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

// HD: private native APIs retain the verified argument widths.
void shipNative_startShock(u32 vibration, s32 a, s32 b, cXyz* v);
void shipNative_startQuake(u32 vibration, s32 a, s32 b, cXyz* v);
void shipNative_stopQuake(u32 vibration, s32 a);
u32 shipNative_getCameraOptions(u32 options);
void shipNative_cameraSet(u32 body, cXyz* center, cXyz* eye);
void shipNative_cameraStop(u32 body);
void shipNative_cameraStart(u32 body);
void shipNative_cameraReset(u32 body);
void shipNative_destroyLineCheck(void* check, s32 deleteFlag);
u32 shipNative_getDemoActor(u32 demo, u8 id);
void shipHD_divideVector(cXyz* result, cXyz* source, f32 divisor);
s32 shipHD_getPolyColor(dBgS* bg, cBgS_PolyInfo* poly);
s32 shipHD_getGroundCode(dBgS* bg, cBgS_PolyInfo* poly);
s32 shipHD_getRoomId(dBgS* bg, cBgS_PolyInfo* poly);
BOOL shipHD_startCheckOld(u32 events, u32 name);
u32 shipHD_nextRoomPath(u32 path, s32 room);
u32 shipHD_startSound(u32 sound, cXyz* position, u32 flags, s32 reverb);
void shipHD_convertSeaColor(void* result, void* source, f32 rate);
void shipHD_entryImmediate(u32 buffer, u8* packet, s32 sort);
void shipHD_drawSightModel(u32 model);
u32 shipHD_getShaderColor(u32 material, s32 index);
void shipHD_updateSightSegment(u32 segment);
void shipHD_subtractVector(cXyz* result, u32 source, cXyz* subtract);

/* ---- c_lib ---- */
f32 cLib_addCalc(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep);

BOOL cLib_chaseUC(be<u8>* v, u8 target, u8 step);
void dKy_get_seacolor(GXColor* amb, GXColor* dif);

/* ---- matrices ---- */
static inline Mtx34* mDoMtx_now() { return mDoMtx_stack_c::get(); }
void mDoMtx_ZrotS(Mtx34* m, s16 z);
void mDoMtx_XrotS_l(Mtx34* m, s16 x);
void PSMTXConcat_s(const Mtx34* a, const Mtx34* b, Mtx34* ab);
void PSMTXScale(Mtx34* m, f32 x, f32 y, f32 z);
static inline void PSMTXCopy_s(const Mtx34* src, Mtx34* dst) { MTXCopy(src, dst); }
void PSMTXInverse(const Mtx34* src, Mtx34* dst);
/* mDoMtx_stack_c::revConcat(m): now = m * now */
static inline void stack_revConcat(const Mtx34* m) { PSMTXConcat_s(m, mDoMtx_now(), mDoMtx_now()); }
static inline void stack_concat(const Mtx34* m) { PSMTXConcat_s(mDoMtx_now(), m, mDoMtx_now()); }
/* cMtx_copy(now, J3DSys::mCurrentMtx) */
static inline void stack_toCurrentMtx() { PSMTXCopy_s(mDoMtx_now(), J3DSys_mCurrentMtx()); }

/* J3DModel::setBaseScale with one value (scale at +0xBC) */
static inline void model_setBaseScale1(J3DModel* m, f32 s) {
    m->mBaseScale.x = s;
    m->mBaseScale.y = s;
    m->mBaseScale.z = s;
}

/* ---- sead::SafeString comparison with the start stage name (as in d_a_bg) ---- */
struct ShipSafeStringVtable {
    /* 0x00 */ u8 mPrefix[0x14];
    /* 0x14 */ be<u32> mAssureTermination;
};
WWHD_SIZE(ShipSafeStringVtable, 0x18);
WWHD_OFFSET(ShipSafeStringVtable, mAssureTermination, 0x14);
struct daShip_SafeString {
    be<u32> mStr;
    be<u32> __vtbl;
};
static inline void ship_ss_assure(daShip_SafeString* s) { gabi::call_ptr(gabi::at<ShipSafeStringVtable>(s->__vtbl)->mAssureTermination, s); }
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
        u8 ca = gabi::at<be<u8>>(pa)[n];
        u8 cb = gabi::at<be<u8>>(pb)[n];
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
/* j3dSys draw buffers (opa 0x104B4634, xlu 0x104B4638); play lists: P1 0x5D58/0x5D60, normal 0x5D78/0x5D7C */

void mDoMtx_lookAt(Mtx34* m, const cXyz* eye, const cXyz* center, s16 bank);
s32 JUTNameTab_getIndex_s(u32 tab, u32 name);
/* 027FA7F8 / 027FA678 (HD, on a 0x3C-byte per-model material entry): the texture SRT info and the
 * texture matrix pointer slot of tex matrix i */
u32 hd_matGetTexSrt(u32 mat, s32 i);
u32 hd_matGetTexMtxSlot(u32 mat, s32 i);
void mDoExt_3DlineMat1_update(mDoExt_3DlineMat1_c* l, u16 segs, f32 size, const GXColor* color, u16 space,
                                            dKy_tevstr_c* tev);
/* dComIfGd_set3DlineMat: the play's line packets (play+0x5FB4, 0x9C each) by material id (virtual +0x14,
 * vtable at +0x130) */


/* ---- save data (HD: through the pointer at 0x101F84DC) ---- */
static inline u32 dSv_base() { return gabi::load<u32>(0x101F84DC); }







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
cXyz* dKyw_get_wind_vec();
be<f32>* dKyw_get_wind_power();
void dKyw_tact_wind_set(s16 x, s16 y);
/* 02030B38: plays a named sound effect (HD) */
void hd_seStartByName(u32 name);
/* pad n (HD): R lock held (02007CDC); X / Y / Z triggers (020078E8 / 02007914 / 02007814); a fourth
 * trigger (02007898, bit 0x1 of the trigger word) */
BOOL CPad_R_LOCK_BUTTON(s32 pad);
BOOL CPad_CHECK_TRIG_X(s32 pad);
BOOL CPad_CHECK_TRIG_Y(s32 pad);
BOOL CPad_CHECK_TRIG_Z(s32 pad);
BOOL CPad_CHECK_TRIG_HD1(s32 pad);
/* held: X / Y / Z (0200770C / 02007738 / 02007638) and the fourth button (020076BC) */
BOOL CPad_CHECK_HOLD_X(s32 pad);
BOOL CPad_CHECK_HOLD_Y(s32 pad);
BOOL CPad_CHECK_HOLD_Z(s32 pad);
BOOL CPad_CHECK_HOLD_HD1(s32 pad);
enum { dItemNo_GRAPPLING_HOOK_e = 0x25 };
/* dVibration_c (play+0x599C) */
static inline void ship_StartShock(s32 a, s32 b, cXyz* v) { shipNative_startShock(dComIfGp_ea() + 0x599C, a, b, v); }
static inline void ship_StartQuake(s32 a, s32 b, cXyz* v) { shipNative_startQuake(dComIfGp_ea() + 0x599C, a, b, v); }
static inline void ship_StopQuake(s32 a) { shipNative_stopQuake(dComIfGp_ea() + 0x599C, a); }
/* dAttention_c (play+0x5804) */
BOOL dAttention_LockonTruth(u32 att);
u32 dAttention_GetLockonList(u32 att, s32 i);
fopAc_ac_c* dAttList_getActor(u32 l);
/* camera inversion option (HD save options: 027200D0(save+0x12C0) + 2) */
static inline bool ship_isCameraInverted() { return gabi::load<u8>(shipNative_getCameraOptions(dSv_base() + 0x12C0) + 2) == 0; }
void mDoAud_seStart_simple(u32 id);
/* 02007D4C: R lock trigger on pad n (HD) */
BOOL CPad_R_LOCK_TRIGGER(s32 pad);

/* ---- messages (HD: one message manager, *0x101F4B5C) ---- */
static inline u32 ship_msgMng() { return gabi::load<u32>(0x101F4B5C); }
u32 fopMsgM_messageSet(u32 mng, u32 msgNo, cXyz* pos);
/* 025F795C (matcher: fopMsgM_SearchByID): the manager's message status */
s32 msgMng_getStatus(u32 mng);
void msgMng_setStatus(u32 mng, s32 st);
enum { fopMsgStts_MSG_DISPLAYED_e = 0xE, fopMsgStts_MSG_CONTINUES_e = 0xF, fopMsgStts_MSG_ENDS_e = 0x10,
       fopMsgStts_BOX_CLOSED_e = 0x12, fopMsgStts_MSG_DESTROYED_e = 0x13 };

/* ---- camera ---- */
static inline s32 dComIfGp_getPlayerCameraID0() { return gabi::load<s8>(dComIfGp_ea() + 0x5B30); }

static inline u32 ship_getCameraAddress(s32 id) { return gabi::ea(dComIfGp_getCamera(id)); }
/* camera_process_class: dCamera_c mCamera at +0x248; Center() = +0x258 + shake +0x7B4, Eye() = +0x264 + +0x7C0 */
static inline void camera_Center(u32 cam, cXyz* out) { cXyz_pl(gabi::at<cXyz>(cam + 0x258), out, gabi::at<cXyz>(cam + 0x7B4)); }
static inline void camera_Eye(u32 cam, cXyz* out) { cXyz_pl(gabi::at<cXyz>(cam + 0x264), out, gabi::at<cXyz>(cam + 0x7C0)); }
/* dCamera_c::Set(cXyz center, cXyz eye): by value (pointers to copies) */
static inline void camera_Set(u32 cam, cXyz* center, cXyz* eye) { shipNative_cameraSet(cam + 0x248, center, eye); }
static inline void camera_Stop(u32 cam) { shipNative_cameraStop(cam + 0x248); }
static inline void camera_Start(u32 cam) { shipNative_cameraStart(cam + 0x248); }
static inline void camera_Reset(u32 cam) { shipNative_cameraReset(cam + 0x248); }
/* cXyz::normalize: normalizes in place and returns a copy through the hidden result pointer */
void cXyz_normalize(cXyz* v, cXyz* res);
static inline f32 cXyz_abs(cXyz* v) { return std_sqrtf(PSVECSquareMag(v)); }

/* ---- resources ---- */
static inline J3DAnmTransform* ship_getRes(s32 idx) { return (J3DAnmTransform*)dComIfG_getObjectRes(STR(l_arcName), idx, SAFESTRING_VTBL); }
/* J3DAnmBase::getFrameMax (virtual +0x14, vtable at +4) */
static inline s32 anm_getFrameMax(J3DAnmTransform* a) { return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(gabi::ea(a) + 4) + 0x14), a); }
void J3DFrameCtrl_init(J3DFrameCtrl* f, s32 end);

/* ---- pad (HD) ---- */
/* 020076E0: A held on pad n; 02007C50: analog R of pad n (f1, possibly not rounded to single) */
BOOL CPad_CHECK_HOLD_A(s32 pad);
f64 CPad_GET_ANALOG_R(s32 pad);

/* ---- line checks: this TU's dBgS_LinChk vtables (constructor / destructor) ---- */
static const dBgS_LinChk_vt SHIP_LINCHK_VT = {0x1003A2AC, 0x1003A2BC, 0x1003A2DC, 0x1003A2CC};
static inline void ship_LinChk_dt(void* c) {
    u32 b = gabi::ea(c);
    gabi::store<u32>(b + 0x58, 0x1003A2DC);
    gabi::store<u32>(b + 0x64, 0x1003A26C);
    gabi::store<u32>(b + 0x20, 0x1003A25C);
    shipNative_destroyLineCheck(c, 0); /* cBgS_LinChk::~cBgS_LinChk */
}
void cLib_offsetPos(cXyz* out, const cXyz* base, s16 angle, const cXyz* ofs);

/* ---- events / demo ---- */
static inline u32 ship_getSubst(s32 staffId, u32 name, s32 type) {
    return gabi::ea(dComIfGp_evmng_getMySubstanceP(staffId, STR(name), type));
}
/* dComIfGp_event_getPt1(): dEvt_control_c::convPId(play+0x51D0, play+0x5294) */


/* dComIfGp_demo_getActor(id) (HD inline: the demo object *0x101D5FFC, asserted) */
static inline u32 ship_demo_getActor(u8 id) {
    if (id == 0 || id > 0x20)
        return 0;
    if (gabi::load<u32>(0x101D5FFC) == 0)
        JUT_ASSERT_fail(STR(0x1003A324), 0x23A, STR(0x1003A314));
    return shipNative_getDemoActor(gabi::load<u32>(0x101D5FFC), id);
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
BOOL cLib_chaseS(be<s16>* v, s16 target, s16 step);
BOOL fpcM_IsCreating(u32 id);
void dPa_followEcallBack_end_l(dPa_followEcallBack* cb);
void dStage_changeScene(s32 exit, f32 speed, u32 mode, s8 roomNo);
f32 cM_atan2f(f32 y, f32 x);
u32 dCam_getBody();
void dCamera_SetTypeForce(u32 cam, u32 name, fopAc_ac_c* a);
BOOL daPy_shipSpecialDemoStart(fopAc_ac_c* link);
f32 cLib_addCalcPosXZ(cXyz* p, const cXyz* t, f32 scale, f32 maxStep, f32 minStep);

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
struct ShipShadowBtk {
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
    /* 0x0424 */ mDoExt_3DlineMat1_c mRopeLine;
    /* 0x05AC */ gptr<J3DModel> mpBeaconModel;          /* HD: beacon model ("beacon.bdl"), also the sight packet's model */
    /* 0x05B0 */ gptr<J3DModel> mpFallPointModel;          /* HD: cannon-sight impact marker */
    /* 0x05B4 */ gptr<mDoExt_McaMorf> mpFallPointAnm;    /* HD: cannon sight */
    /* 0x05B8 */ gptr<J3DModel> mpShadowModel;    /* HD */
    /* 0x05BC */ ShipShadowBtk mShadowBtk;      /* HD */
    /* 0x0630 */ be<u8> mShadowAlpha;             /* HD */
    /* 0x0631 */ u8 mShadowPadding[2];
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
    /* 0x063D */ be<u8> mSwiftSailActive; // HD: Swift Sail state.
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
    /* 0x06F4 */ be<f32> mHDMovementRate; // HD: extra movement scalar; opaque here.
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
    /* 0x1B24 */ u8 mSightState[0x30];                /* HD */
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
    /* 0xD190 */ gabi::ptmf<BOOL (daShip_c::*)()> mProc;
    /* 0xD198 */ u8 mProcPadding[4];                   // HD: descriptor is 8 bytes; this trailing word stays untouched.

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
    void initStartPos(cXyz* pos, s16 angle);
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
        mProc.d = 0;
        mProc.i = -1;
        mProc.f = fn;
    }
    bool isProc(u32 fn) {
        return mProc.i == -1 && mProc.d == 0 &&
               mProc.f == fn;
    }
};
WWHD_OFFSET(daShip_c, mpCannonModel, 0x418);
WWHD_OFFSET(daShip_c, mpFallPointAnm, 0x5B4);
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
    MODE_DEMO_e = 7, MODE_WHIRL_DOWN_e = 15, MODE_WAIT_e = 0, MODE_STEER_MOVE_e = 1, MODE_PADDLE_MOVE_e = 2, MODE_READY_FIRST_e = 3,
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

// HD: partial native layouts used by this owner source. Unobserved bytes stay opaque.
struct ShipJoint {
    /* 0x00 */ u8 mPrefix[4];
    /* 0x04 */ be<u16> mJntNo;
};
WWHD_SIZE(ShipJoint, 6);
WWHD_OFFSET(ShipJoint, mJntNo, 4);
struct ShipPlayer {
    /* 0x000 */ u8 mPrefix[0x3D0];
    /* 0x3D0 */ be<s16> mLookDownAngle;
    /* 0x3D2 */ u8 mPadding[0x430 - 0x3D2];
    /* 0x430 */ be<u32> mProcID;
};
WWHD_SIZE(ShipPlayer, 0x434);
WWHD_OFFSET(ShipPlayer, mLookDownAngle, 0x3D0);
WWHD_OFFSET(ShipPlayer, mProcID, 0x430);
struct ShipPlay {
    /* 0x0000 */ u8 mPrefix[0x5292];
    /* 0x5292 */ be<u8> mEventRun;
    /* 0x5293 */ u8 mPadding0[0x5B00 - 0x5293];
    /* 0x5B00 */ be<u32> mCameraAttentionStatus;
    /* 0x5B04 */ u8 mPadding1[0x5CDC - 0x5B04];
    /* 0x5CDC */ be<u32> mPlayerStatus1;
    /* 0x5CE0 */ u8 mPadding2[0x5CEA - 0x5CE0];
    /* 0x5CEA */ be<u8> mMiniGameType;
    /* 0x5CEB */ u8 mPadding3[0x5DA8 - 0x5CEB];
    /* 0x5DA8 */ be<u32> mSightDrawBuffer;
};
WWHD_SIZE(ShipPlay, 0x5DAC);
WWHD_OFFSET(ShipPlay, mEventRun, 0x5292);
WWHD_OFFSET(ShipPlay, mCameraAttentionStatus, 0x5B00);
WWHD_OFFSET(ShipPlay, mPlayerStatus1, 0x5CDC);
WWHD_OFFSET(ShipPlay, mMiniGameType, 0x5CEA);
WWHD_OFFSET(ShipPlay, mSightDrawBuffer, 0x5DA8);
inline ShipPlay* ship_play() { return gabi::at<ShipPlay>(dComIfGp_ea()); }
struct ShipSightPacket {
    /* 0x00 */ u8 mPrefix[0x98];
    /* 0x98 */ be<u32> mModel;
    /* 0x9C */ be<u32> mModelPacket;
    /* 0xA0 */ be<u32> mSegments;
};
WWHD_SIZE(ShipSightPacket, 0xA4);
WWHD_OFFSET(ShipSightPacket, mSegments, 0xA0);
struct ShipSight {
    /* 0x00 */ csXyz mAngle;
    /* 0x06 */ u8 mPadding[2];
    /* 0x08 */ cXyz mPosition;
    /* 0x14 */ cXyz mVelocity;
    /* 0x20 */ be<f32> mSpeed;
    /* 0x24 */ be<f32> mGravity;
    /* 0x28 */ be<s16> mDelay;
    /* 0x2A */ u8 mPadding2[2];
    /* 0x2C */ be<f32> mMinVelocity;
};
WWHD_SIZE(ShipSight, 0x30);
WWHD_OFFSET(ShipSight, mPosition, 8);
WWHD_OFFSET(ShipSight, mVelocity, 0x14);
WWHD_OFFSET(ShipSight, mSpeed, 0x20);
WWHD_OFFSET(ShipSight, mGravity, 0x24);
WWHD_OFFSET(ShipSight, mDelay, 0x28);
WWHD_OFFSET(ShipSight, mMinVelocity, 0x2C);
WWHD_SIZE(ShipMtxBlock, 0x14);
WWHD_OFFSET(ShipMtxBlock, mFlags, 4);
WWHD_OFFSET(ShipMtxBlock, mpMtx, 0x10);
WWHD_SIZE(daShip_SafeString, 8);
WWHD_SIZE(ShipShadowBtk, 0x74);
WWHD_SIZE(daShip_c, 0xD19C);

struct ShipWave {
    /* 0x000 */ u8 mPadding0[0x4];
    /* 0x004 */ be<u16> mState;
    /* 0x006 */ be<u16> mTimer;
    /* 0x008 */ be<f32> mSpeed;
    /* 0x00C */ u8 mPaddingC[0x4];
    /* 0x010 */ be<f32> mMaxDisSpeed;
    /* 0x014 */ be<f32> mPitch;
    /* 0x018 */ be<f32> mMaxSpeed;
    /* 0x01C */ cXyz mAnchorFront;
    /* 0x028 */ cXyz mAnchorBack;
    /* 0x034 */ u8 mPadding34[0x2C];
    /* 0x060 */ be<u32> mEmitter;
};
WWHD_SIZE(ShipWave, 0x64);
WWHD_OFFSET(ShipWave, mState, 0x4);
WWHD_OFFSET(ShipWave, mTimer, 0x6);
WWHD_OFFSET(ShipWave, mSpeed, 0x8);
WWHD_OFFSET(ShipWave, mMaxDisSpeed, 0x10);
WWHD_OFFSET(ShipWave, mPitch, 0x14);
WWHD_OFFSET(ShipWave, mMaxSpeed, 0x18);
WWHD_OFFSET(ShipWave, mAnchorFront, 0x1C);
WWHD_OFFSET(ShipWave, mAnchorBack, 0x28);
WWHD_OFFSET(ShipWave, mEmitter, 0x60);

struct ShipSplash {
    /* 0x000 */ u8 mPadding0[0x4];
    /* 0x004 */ be<u16> mState;
    /* 0x006 */ u8 mPadding6[0x2];
    /* 0x008 */ be<f32> mSpeed;
    /* 0x00C */ be<f32> mMaxSpeed;
    /* 0x010 */ u8 mPadding10[0x8];
    /* 0x018 */ be<u32> mEmitter;
};
WWHD_SIZE(ShipSplash, 0x1C);
WWHD_OFFSET(ShipSplash, mState, 0x4);
WWHD_OFFSET(ShipSplash, mSpeed, 0x8);
WWHD_OFFSET(ShipSplash, mMaxSpeed, 0xC);
WWHD_OFFSET(ShipSplash, mEmitter, 0x18);

struct ShipTrack {
    /* 0x000 */ u8 mPadding0[0x4];
    /* 0x004 */ be<u16> mState;
    /* 0x006 */ u8 mPadding6[0x36];
    /* 0x03C */ be<f32> mIndirectScroll;
    /* 0x040 */ be<f32> mIndirectScale;
    /* 0x044 */ be<f32> mSpeed;
    /* 0x048 */ u8 mPadding48[0x4];
    /* 0x04C */ be<u32> mEmitter;
};
WWHD_SIZE(ShipTrack, 0x50);
WWHD_OFFSET(ShipTrack, mState, 0x4);
WWHD_OFFSET(ShipTrack, mIndirectScroll, 0x3C);
WWHD_OFFSET(ShipTrack, mIndirectScale, 0x40);
WWHD_OFFSET(ShipTrack, mSpeed, 0x44);
WWHD_OFFSET(ShipTrack, mEmitter, 0x4C);

struct ShipRipple {
    /* 0x000 */ u8 mPadding0[0x4];
    /* 0x004 */ be<u32> mEmitter;
    /* 0x008 */ u8 mPadding8[0x8];
    /* 0x010 */ be<f32> mRate;
};
WWHD_SIZE(ShipRipple, 0x14);
WWHD_OFFSET(ShipRipple, mEmitter, 0x4);
WWHD_OFFSET(ShipRipple, mRate, 0x10);

struct ShipEmitter {
    /* 0x000 */ u8 mPadding0[0x8];
    /* 0x008 */ cXyz mEmitterScale;
    /* 0x014 */ u8 mPadding14[0x20];
    /* 0x034 */ be<f32> mRate;
    /* 0x038 */ u8 mPadding38[0x28];
    /* 0x060 */ be<u16> mLifeTime;
    /* 0x062 */ u8 mPadding62[0x2];
    /* 0x064 */ be<u16> mVolumeSize;
    /* 0x066 */ u8 mPadding66[0x1D2];
    /* 0x238 */ cXyz mGlobalParticleScale;
};
WWHD_SIZE(ShipEmitter, 0x244);
WWHD_OFFSET(ShipEmitter, mEmitterScale, 0x8);
WWHD_OFFSET(ShipEmitter, mRate, 0x34);
WWHD_OFFSET(ShipEmitter, mLifeTime, 0x60);
WWHD_OFFSET(ShipEmitter, mVolumeSize, 0x64);
WWHD_OFFSET(ShipEmitter, mGlobalParticleScale, 0x238);

struct ShipPath {
    /* 0x000 */ be<u16> mNum;
    /* 0x002 */ u8 mPadding2[0x6];
    /* 0x008 */ be<u32> mPoints;
};
WWHD_SIZE(ShipPath, 0xC);
WWHD_OFFSET(ShipPath, mNum, 0x0);
WWHD_OFFSET(ShipPath, mPoints, 0x8);

struct ShipPoint {
    /* 0x000 */ u8 mPadding0[0x4];
    /* 0x004 */ cXyz mPosition;
};
WWHD_SIZE(ShipPoint, 0x10);
WWHD_OFFSET(ShipPoint, mPosition, 0x4);

struct ShipTexMtx {
    /* 0x000 */ u8 mPadding0[0x1C];
    /* 0x01C */ be<f32> mTranslationX;
    /* 0x020 */ u8 mPadding20[0x4];
    /* 0x024 */ be<f32> mEffectMtx[16];
    /* 0x064 */ Mtx34 mMtx;
};
WWHD_SIZE(ShipTexMtx, 0x94);
WWHD_OFFSET(ShipTexMtx, mTranslationX, 0x1C);
WWHD_OFFSET(ShipTexMtx, mEffectMtx, 0x24);
WWHD_OFFSET(ShipTexMtx, mMtx, 0x64);

struct ShipModelDraw {
    /* 0x000 */ u8 mPadding0[0x14];
    /* 0x014 */ be<u32> mpResHeader;
    /* 0x018 */ u8 mPadding18[0x1C];
    /* 0x034 */ be<u32> mMaterials;
};
WWHD_SIZE(ShipModelDraw, 0x38);
WWHD_OFFSET(ShipModelDraw, mpResHeader, 0x14);
WWHD_OFFSET(ShipModelDraw, mMaterials, 0x34);

struct ShipResourceHeader {
    /* 0x000 */ u8 mPadding0[0x18];
    /* 0x018 */ be<s32> mMaterialNameOffset;
};
WWHD_SIZE(ShipResourceHeader, 0x1C);
WWHD_OFFSET(ShipResourceHeader, mMaterialNameOffset, 0x18);

struct ShipTextureSRT {
    /* 0x000 */ u8 mPadding0[0x10];
    /* 0x010 */ be<f32> mTranslationX;
};
WWHD_SIZE(ShipTextureSRT, 0x14);
WWHD_OFFSET(ShipTextureSRT, mTranslationX, 0x10);

struct ShipTevStr {
    /* 0x000 */ u8 mPadding0[0xBA];
    /* 0x0BA */ be<u8> mEnvrIdxOverride;
};
WWHD_SIZE(ShipTevStr, 0xBB);
WWHD_OFFSET(ShipTevStr, mEnvrIdxOverride, 0xBA);

struct ShipShadowMaterial {
    /* 0x000 */ u8 mPadding0[0x18];
    /* 0x018 */ be<u32> mTevBlock;
    /* 0x01C */ u8 mPadding1C[0x84];
    /* 0x0A0 */ be<u32> mShaderFlags;
};
WWHD_SIZE(ShipShadowMaterial, 0xA4);
WWHD_OFFSET(ShipShadowMaterial, mTevBlock, 0x18);
WWHD_OFFSET(ShipShadowMaterial, mShaderFlags, 0xA0);

struct ShipTevBlock {
    /* 0x000 */ u8 mPadding0[0x4];
    /* 0x004 */ be<u32> __vtbl;
};
WWHD_SIZE(ShipTevBlock, 0x8);
WWHD_OFFSET(ShipTevBlock, __vtbl, 0x4);

struct ShipTevVtable {
    /* 0x000 */ u8 mPadding0[0x24];
    /* 0x024 */ be<u32> mSetTevColor;
};
WWHD_SIZE(ShipTevVtable, 0x28);
WWHD_OFFSET(ShipTevVtable, mSetTevColor, 0x24);

struct ShipEnvironment {
    /* 0x000 */ u8 mPadding0[0x10B8];
    /* 0x10B8 */ be<f32> mSeaColorScale;
};
WWHD_SIZE(ShipEnvironment, 0x10BC);
WWHD_OFFSET(ShipEnvironment, mSeaColorScale, 0x10B8);

struct ShipFloatColor {
    /* 0x000 */ be<f32> r;
    /* 0x004 */ be<f32> g;
    /* 0x008 */ be<f32> b;
    /* 0x00C */ be<f32> a;
};
WWHD_SIZE(ShipFloatColor, 0x10);
WWHD_OFFSET(ShipFloatColor, r, 0x0);
WWHD_OFFSET(ShipFloatColor, g, 0x4);
WWHD_OFFSET(ShipFloatColor, b, 0x8);
WWHD_OFFSET(ShipFloatColor, a, 0xC);

struct ShipMorfTransform {
    /* 0x000 */ u8 mPadding0[0x30];
    /* 0x030 */ be<u32> mTransforms;
    /* 0x034 */ u8 mPadding34[0x20];
    /* 0x054 */ be<u32> mFlags;
};
WWHD_SIZE(ShipMorfTransform, 0x58);
WWHD_OFFSET(ShipMorfTransform, mTransforms, 0x30);
WWHD_OFFSET(ShipMorfTransform, mFlags, 0x54);

struct ShipTransformTable {
    /* 0x000 */ u8 mPadding0[0x124];
    /* 0x124 */ be<u32> mScale[3];
};
WWHD_SIZE(ShipTransformTable, 0x130);
WWHD_OFFSET(ShipTransformTable, mScale, 0x124);

inline cXyz* ship_initVec(be<u32>& guard, cXyz& value, f32 x, f32 y, f32 z) { if (guard == 0) { guard = 1; value.set(x, y, z); } return &value; }

// HD: guarded BSS hull samples are initialized by drawShadow in native store order.
struct ShipShadowPositions {
    /* 0x00 */ cXyz mPositions[5];
};
WWHD_SIZE(ShipShadowPositions, 0x3C);

// tww event-bit names used by the owner source. Values retain the HD save numbering.
namespace ShipEvent {
enum {
    UNK_0902 = 0x0902,
    UNK_0908 = 0x0908,
    UNK_0A01 = 0x0A01,
    ENDLESS_NIGHT = 0x0A02,
    UNK_0A04 = 0x0A04,
    UNK_0A08 = 0x0A08,
    UNK_0A10 = 0x0A10,
    UNK_0A20 = 0x0A20,
    UNK_0A80 = 0x0A80,
    UNK_1610 = 0x1610,
    UNK_1620 = 0x1620,
    UNK_1940 = 0x1940,
    UNK_1980 = 0x1980,
    UNK_1E40 = 0x1E40,
    UNK_1F02 = 0x1F02,
    UNK_1F04 = 0x1F04,
    UNK_2110 = 0x2110,
    UNK_2A01 = 0x2A01,
    UNK_2A02 = 0x2A02,
    RODE_KORL = 0x2A08,
    UNK_2B80 = 0x2B80,
    UNK_2C01 = 0x2C01,
    BARRIER_BREAK = 0x2C02,
    ZELDA_AWAKENED = 0x2D02,
    UNK_2D08 = 0x2D08,
    UNK_2D10 = 0x2D10,
    UNK_2E02 = 0x2E02,
    UNK_2E04 = 0x2E04,
    UNK_2F20 = 0x2F20,
    UNK_3040 = 0x3040,
    UNK_3201 = 0x3201,
    UNK_3380 = 0x3380,
    UNK_3810 = 0x3810,
    UNK_3910 = 0x3910,
    UNK_3A02 = 0x3A02,
    UNK_3D02 = 0x3D02,
    UNK_3E01 = 0x3E01,
    UNK_3E10 = 0x3E10,
    UNK_3E40 = 0x3E40,
    UNK_3F80 = 0x3F80,
    UNK_4004 = 0x4004,
};
}

namespace ShipSound {
enum {
    JA_SE_LK_SHIP_CRANE_DROP = 0x381C,
    JA_SE_LK_SHIP_CRANE_LIFTUP = 0x381D,
    JA_SE_LK_SHIP_CRANE_SALVAGE = 0x381E,
    JA_SE_LK_SHIP_CRANE_STOP = 0x284E,
    JA_SE_LK_SHIP_CRANE_WORK = 0x204D,
    JA_SE_SHIP_JUMP_ALIGHT = 0x6914,
    JA_SE_SHIP_LOOK_BACK = 0x6915,
};
}

// HD: guest entry targets stored in the compact GHS procedure descriptor.
namespace ShipProcedure {
enum : u32 {
    procCannon = 0x0247F0D0,
    procCraneUp = 0x0247FBC4,
    procTalk = 0x02481630,
    procTalkReady = 0x02480FD4,
    procToolDemo = 0x0247FF54,
    procTornadoUp = 0x02481C94,
    procWhirlDown = 0x024827D4,
    procZevDemo = 0x024800A8,
};
}
WWHD_OFFSET(daShip_c, mpBeaconModel, 0x5AC);
WWHD_OFFSET(daShip_c, mpFallPointModel, 0x5B0);
WWHD_OFFSET(daShip_c, mpShadowModel, 0x5B8);
WWHD_OFFSET(daShip_c, mShadowBtk, 0x5BC);
WWHD_OFFSET(daShip_c, mSwiftSailActive, 0x63D);
WWHD_OFFSET(daShip_c, mSightState, 0x1B24);
WWHD_OFFSET(daShip_c, mHDPacket, 0x1B54);
WWHD_OFFSET(daShip_c, mProcPadding, 0xD198);

// HD: each cannon-sight segment carries a matrix after its material packet.
struct ShipSightSegment {
    /* 0x00 */ u8 mMaterialPacket[0x74];
    /* 0x74 */ Mtx34 mMtx;
    /* 0xA4 */ u8 mTail[4];
};
WWHD_SIZE(ShipSightSegment, 0xA8);
WWHD_OFFSET(ShipSightSegment, mMtx, 0x74);

// HD: additional bindings used by the dependent procedures, without new shared APIs.
void shipFirstWaveRemove(ShipWave* callback);
void shipFirstSplashRemove(ShipSplash* callback);
void shipFirstTrackRemove(ShipTrack* callback);
void shipFirstSetDamageRupee(u32 link, f32 damage);
void shipFirstSetDamagePointWait(u32 link, f32 damage);
u32 shipFirstGetTgHitGObj(dCcD_GObjInf* object);
void shipFirstStartEventCamera(u32 camera, u32 mode, u32 actorID, u32 flags);
namespace shipFirst {
enum : u32 {
    procWait = 0x0247E670, procReady = 0x0247E77C, procGetOff = 0x0247FE54,
    procPaddleMove = 0x0247EEE8, procSteerMove = 0x0247E854,
    procCannonReady = 0x0247F024, procCraneReady = 0x0247F618,
    procStartModeWarp = 0x02481FAC, procTactWarp = 0x02482438,
    procStartModeThrow = 0x02482970,
    CAMERA_EVENT_TACT_WARP = 0xE, ACT_STATUS_STOP = 0x13,
    AT_TYPE_WIND = 0x200000, PLAYER_STATUS_SAIL = 0x400, LINK_SHIP_DROP = 0x200,
    JA_SE_LK_SHIP_CRANE_IN = 0x284B, JA_SE_LK_SHIP_CANNON_IN = 0x2850,
    JA_SE_SHIP_SAIL_IN = 0x2832, JA_SE_SHIP_SAIL_OUT = 0x2831,
    JA_SE_SHIP_SAIL_MOVE = 0x2825,
    JA_SE_LK_SHIP_CANNON_OUT = 0x284F, JA_SE_LK_SHIP_CRANE_OUT = 0x284A,
};
struct AttackObject {
    /* 0x00 */ u8 mPrefix[0x10];
    /* 0x10 */ be<u32> mType;
};
WWHD_SIZE(AttackObject, 0x14);
WWHD_OFFSET(AttackObject, mType, 0x10);
struct LinkState {
    /* 0x0000 */ u8 mPrefix[0x3B8];
    /* 0x03B8 */ be<u32> mResetFlg0;
    /* 0x03BC */ be<u32> mNoResetFlg0;
    /* 0x03C0 */ u8 mPadding[0x699E - 0x3C0];
    /* 0x699E */ be<s16> mDamageWaitTimer;
    /* 0x69A0 */ be<s16> mShipDropAngle;
};
WWHD_SIZE(LinkState, 0x69A2);
WWHD_OFFSET(LinkState, mResetFlg0, 0x3B8);
WWHD_OFFSET(LinkState, mNoResetFlg0, 0x3BC);
WWHD_OFFSET(LinkState, mDamageWaitTimer, 0x699E);
WWHD_OFFSET(LinkState, mShipDropAngle, 0x69A0);
// HD: native Link accesses add the field offset to the guest EA even for guest zero.
// Keep that wrapping behavior instead of dereferencing a null host actor view.
inline u32 linkNoResetFlags(u32 link) { return gabi::load<u32>(link + offsetof(LinkState, mNoResetFlg0)); }
inline u32 linkResetFlags(u32 link) { return gabi::load<u32>(link + offsetof(LinkState, mResetFlg0)); }
inline void setLinkResetFlags(u32 link, u32 flags) { gabi::store<u32>(link + offsetof(LinkState, mResetFlg0), flags); }
inline s16 linkDamageWait(u32 link) { return gabi::load<s16>(link + offsetof(LinkState, mDamageWaitTimer)); }
inline void setLinkDropAngle(u32 link, s16 angle) { gabi::store<s16>(link + offsetof(LinkState, mShipDropAngle), angle); }
}

namespace shipFirst {
inline u32 attackType(u32 object) { return gabi::load<u32>(object + offsetof(AttackObject, mType)); }
inline void clearSailStatus(u32 play) {
    u32 status = play + offsetof(ShipPlay, mPlayerStatus1);
    gabi::store<u32>(status, gabi::load<u32>(status) & ~PLAYER_STATUS_SAIL);
}
}

// HD: dependent Ship procedures use value accessors so adding a native field offset
// wraps in the 32-bit guest address space before any memory operation.
namespace shipTail {
struct Play {
    /* 0x0000 */ u8 mPrefix[0x5134];
    /* 0x5134 */ char mStartStageName[8];
    /* 0x513C */ u8 mPaddingStage[0x5292 - 0x513C];
    /* 0x5292 */ be<u8> mEventRun;
    /* 0x5293 */ u8 mPadding0[0x5CDC - 0x5293];
    /* 0x5CDC */ be<u32> mPlayerStatus1;
    /* 0x5CE0 */ u8 mPadding1[0x5D2C - 0x5CE0];
    /* 0x5D2C */ be<u8> mSwiftSailEnabled;
};
WWHD_SIZE(Play, 0x5D2D);
WWHD_OFFSET(Play, mStartStageName, 0x5134);
WWHD_OFFSET(Play, mEventRun, 0x5292);
WWHD_OFFSET(Play, mPlayerStatus1, 0x5CDC);
WWHD_OFFSET(Play, mSwiftSailEnabled, 0x5D2C);
struct Grid {
    /* 0x0000 */ u8 mPrefix[0x2B8C];
    /* 0x2B8C */ be<s16> mSailTimer;
};
WWHD_SIZE(Grid, 0x2B8E); WWHD_OFFSET(Grid, mSailTimer, 0x2B8C);
struct DemoActor {
    /* 0x00 */ u8 mPrefix[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 mPadding0[2];
    /* 0x08 */ cXyz mTranslation;
    /* 0x14 */ u8 mPadding1[0x22 - 0x14];
    /* 0x22 */ be<s16> mRotationY;
};
WWHD_SIZE(DemoActor, 0x24);
WWHD_OFFSET(DemoActor, mFlags, 4); WWHD_OFFSET(DemoActor, mTranslation, 8);
WWHD_OFFSET(DemoActor, mRotationY, 0x22);
struct MessageManager {
    /* 0x000 */ u8 mPrefix[0x948];
    /* 0x948 */ be<u32> mSelectedAnswer;
};
WWHD_SIZE(MessageManager, 0x94C); WWHD_OFFSET(MessageManager, mSelectedAnswer, 0x948);
struct LinkState {
    /* 0x0000 */ u8 mPrefix[0x3BC];
    /* 0x03BC */ be<u32> mNoResetFlags;
    /* 0x03C0 */ u8 mPadding[0x65F0 - 0x3C0];
    /* 0x65F0 */ be<u32> mProcedure;
};
WWHD_SIZE(LinkState, 0x65F4);
WWHD_OFFSET(LinkState, mNoResetFlags, 0x3BC); WWHD_OFFSET(LinkState, mProcedure, 0x65F0);
struct Tornado {
    /* 0x000 */ u8 mPrefix[0x6B0];
    /* 0x6B0 */ be<s16> mScaleOn;
    /* 0x6B2 */ u8 mPadding[0x744 - 0x6B2];
    /* 0x744 */ be<f32> mScale;
};
WWHD_SIZE(Tornado, 0x748);
WWHD_OFFSET(Tornado, mScaleOn, 0x6B0); WWHD_OFFSET(Tornado, mScale, 0x744);
struct LinkVtable {
    /* 0x00 */ u8 mPrefix[0x9C];
    /* 0x9C */ be<u32> mBaseAnimationRate;
};
WWHD_SIZE(LinkVtable, 0xA0); WWHD_OFFSET(LinkVtable, mBaseAnimationRate, 0x9C);
struct SubstanceWord {
    /* 0x00 */ u8 mHigh[2];
    /* 0x02 */ be<s16> mSignedLow;
};
WWHD_SIZE(SubstanceWord, 4); WWHD_OFFSET(SubstanceWord, mSignedLow, 2);
struct SubstanceByte {
    /* 0x00 */ u8 mHigh[3];
    /* 0x03 */ be<u8> mLow;
};
WWHD_SIZE(SubstanceByte, 4); WWHD_OFFSET(SubstanceByte, mLow, 3);
struct Attention {
    /* 0x00 */ u8 mPrefix[0x20];
    /* 0x20 */ be<u32> mFlags;
};
WWHD_SIZE(Attention, 0x24); WWHD_OFFSET(Attention, mFlags, 0x20);
inline u32 fieldAddress(u32 address, u32 offset) { return address + (u32)offset; }
inline u32 attentionFlags(u32 attention) { return gabi::load<u32>(fieldAddress(attention, offsetof(Attention, mFlags))); }
inline cXyz* actorEyePosition(u32 actor) { return gabi::at<cXyz>(fieldAddress(actor, offsetof(fopAc_ac_c, eyePos))); }
inline u8 eventRun(u32 play) { return gabi::load<u8>(fieldAddress(play, offsetof(Play, mEventRun))); }
inline u8 swiftSailEnabled(u32 play) { return gabi::load<u8>(fieldAddress(play, offsetof(Play, mSwiftSailEnabled))); }
inline u32 playerStatus1(u32 play) { return gabi::load<u32>(fieldAddress(play, offsetof(Play, mPlayerStatus1))); }
inline void setSailGridTimer(u32 grid, s16 timer) { gabi::store<s16>(fieldAddress(grid, offsetof(Grid, mSailTimer)), timer); }
inline void setRippleRate(u32 ripple, f32 rate) { gabi::store<f32>(fieldAddress(ripple, offsetof(ShipRipple, mRate)), rate); }
inline u32 rippleEmitter(u32 ripple) { return gabi::load<u32>(fieldAddress(ripple, offsetof(ShipRipple, mEmitter))); }
inline u16 demoFlags(u32 actor) { return gabi::load<u16>(fieldAddress(actor, offsetof(DemoActor, mFlags))); }
inline f32 demoTranslationX(u32 actor) { return gabi::load<f32>(fieldAddress(actor, offsetof(DemoActor, mTranslation) + offsetof(cXyz, x))); }
inline f32 demoTranslationZ(u32 actor) { return gabi::load<f32>(fieldAddress(actor, offsetof(DemoActor, mTranslation) + offsetof(cXyz, z))); }
inline s16 demoRotationY(u32 actor) { return gabi::load<s16>(fieldAddress(actor, offsetof(DemoActor, mRotationY))); }
inline u32 selectedAnswer(u32 manager) { return gabi::load<u32>(fieldAddress(manager, offsetof(MessageManager, mSelectedAnswer))); }
inline u32 linkProcedure(u32 link) { return gabi::load<u32>(fieldAddress(link, offsetof(LinkState, mProcedure))); }
inline u32 linkNoResetFlags(u32 link) { return gabi::load<u32>(fieldAddress(link, offsetof(LinkState, mNoResetFlags))); }
inline void setLinkNoResetFlags(u32 link, u32 flags) { gabi::store<u32>(fieldAddress(link, offsetof(LinkState, mNoResetFlags)), flags); }
inline f32 tornadoScale(u32 actor) { return gabi::load<f32>(fieldAddress(actor, offsetof(Tornado, mScale))); }
inline void setTornadoScaleOn(u32 actor, s16 flag) { gabi::store<s16>(fieldAddress(actor, offsetof(Tornado, mScaleOn)), flag); }
inline f32 baseAnimationRate(fopAc_ac_c* link) {
    return gabi::call_ptr<f32>(gabi::load<u32>(fieldAddress(link->__vtbl, offsetof(LinkVtable, mBaseAnimationRate))), link);
}
inline void copyWord(be<f32>* destination, const be<f32>* source) {
    gabi::store<u32>(gabi::ea(destination), gabi::load<u32>(gabi::ea(source)));
}
inline u32 word(u32 address) { return gabi::load<u32>(address); }
inline s32 integer(u32 address) { return gabi::load<s32>(address); }
inline f32 scalar(u32 address) { return gabi::load<f32>(address); }
inline s16 angle(u32 address) { return gabi::load<s16>(fieldAddress(address, offsetof(SubstanceWord, mSignedLow))); }
inline u8 part(u32 address) { return gabi::load<u8>(fieldAddress(address, offsetof(SubstanceByte, mLow))); }
inline u32 startStageName(u32 play) { return fieldAddress(play, offsetof(Play, mStartStageName)); }
enum : u32 {
    procCrane = 0x0247F7CC, procTurn = 0x02481874,
    LINK_SHIP_TACT = 0x1000,
    JA_SE_SHIP_LOOK_FORWARD = 0x6916,
    daPyStts0_UNK1000000_e = 0x1000000,
    daPyStts1_PICTO_BOX_AIM_e = 8,
    SAIL_POWER_UP = 0x28A6,
    SAIL_POWER_DOWN = 0x28A7,
    JA_SE_LK_SHIP_CANNON_MOVE = 0x2051,
    JA_SE_ITEM_TARGET_OUT = 0x883,
    JA_SE_LK_SHIP_CRANE_ARM = 0x284C,
    ENABLE_TRANS_e = 2,
    ENABLE_ROTATE_e = 8,
    DEMO_INIT_e = 0,
    DEMO_OPEN_e = 8,
    DEMO_MOVE_e = 1,
    DEMO_UNK03_e = 3,
    DEMO_RACE_FAIL_e = 4,
    DEMO_KEEP_e = 5,
    DEMO_NECK_e = 6,
    DEMO_THROW_e = 7,
    DEMO_HWARP_UP_e = 10,
    DEMO_HWARP_DOWN_e = 11,
    ID_AK_JN_SHIPWARPSPLASH00 = 0x285,
    ID_IT_SN_FN_SHIBUKI00 = 0x82D7,
    ID_IT_SN_FN_SHIBUKI01 = 0x82D8,
};
} // namespace shipTail
// HD: setNextMessage is called with r3 only; r4 remains the status query's live register.
BOOL shipTailNextMessage(daShip_c* ship);
void shipTailChangeScene(s32 exit, f32 speed, s32 mode, s32 room);
void shipTailEndEventCamera(u32 camera, u32 actorID);
void shipTailTactMuteOff();
void shipTailSetNextStage(u32 stage, s16 point, s8 room, s32 layer, f32 speed, s32 wipe, s32 reset, s32 flags);
