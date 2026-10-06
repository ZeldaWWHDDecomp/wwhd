/* d_a_fm local declarations shared by the d_a_fm*.cpp parts. 
 * Bindings here are SHARED-CANDIDATEs (copied from d_a_rd where identical). */
#pragma once
#include "d/actor/d_a_fm.h"

#define FM_VTBL 0x1000F350          /* daFm_c vtable (HD virtual destructor) */
#define L_HIO 0x10463E0Cu           /* daFm_HIO_c l_HIO (HD 0x15C; offsets = GameCube) */
static inline f32 hio_f(u32 off) { return gabi::load<f32>(L_HIO + off); }
static inline s16 hio_s(u32 off) { return gabi::load<s16>(L_HIO + off); }
static inline u8 hio_u8(u32 off) { return gabi::load<u8>(L_HIO + off); }
static inline cXyz* hio_xyz(u32 off) { return gabi::at<cXyz>(L_HIO + off); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD J3D: j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, mDoMtx_stack_c::now 0x1048D0CC;
 * joint matrices in the model's block at +0x2C (+0x4 flags, +0x10 matrices), user area +0xB8 */
static inline u32 j3dSys_model() { return gabi::load<u32>(0x104B462C); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline Mtx34* mtx_now() { return gabi::at<Mtx34>(0x1048D0CC); }
static inline Mtx34* fm_getAnmMtx(J3DModel* model, u32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    u32 m = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(m + jntNo * 0x30);
}
static inline J3DModelData* fm_modelData(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* J3DModelData::getJointNodePointer(i) (HD inline): count +4, nodes (0x1C) at +8; out of range: node 0 */
static inline u32 fm_jointNode(J3DModelData* md, u32 i) {
    u32 num = gabi::load<u32>(gabi::ea(md) + 4);
    u32 base = gabi::load<u32>(gabi::ea(md) + 8);
    return i < num ? base + i * 0x1C : base;
}
static inline void mDoMtx_inverseTranspose(Mtx34* src, Mtx34* dst) { gabi::call(0x025F20B0, src, dst); }
static inline void mDoMtx_MtxToRot(Mtx34* m, csXyz* out) { gabi::call(0x025F232C, m, out); }
/* 02552B60 JntHit_create(J3DModel*, __jnt_hit_data_c*, s16) */
static inline JntHit_c* JntHit_create(J3DModel* m, u32 data, s16 num) { return gabi::call<JntHit_c*>(0x02552B60, m, data, num); }
/* 025E8A48 mDoExt_invisibleModel::create(J3DModel*) */
static inline bool invisibleModel_create(void* inv, J3DModel* m) { return gabi::call<bool>(0x025E8A48, inv, m); }
/* 0259138C dMat_ice_c::entryDL (matcher; mDoExt_invisibleModel::entryDL per phpt) (morf, s32, invisibleModel*) */
static inline void dMat_control_iceEntryDL(mDoExt_McaMorf* m, s32 p, void* inv) { gabi::call(0x0259138C, m, p, inv); }

/* 025E83FC mDoExt_brkAnm::entry(J3DModelData*, f32 frame) */
/* 02587A0C dLib_bcks_setAnm(arc, morf, s8* bck, s8* prm, s8* old, const int* bcks, const dLib_anm_prm_c*, bool, HD int) */
static inline void dLib_bcks_setAnm(const char* arc, mDoExt_McaMorf* morf, be<s8>* bck, be<s8>* prm, be<s8>* old, u32 bcks,
                                    u32 prms, bool force) {
    gabi::call(0x02587A0C, arc, morf, bck, prm, old, bcks, prms, force, 0);
}
/* 02588230 dLib_checkActorInFan(cXyz center (copy), fopAc_ac_c*, s16 angle, s16 spread, f32 radius, f32 height) */
static inline bool dLib_checkActorInFan(cXyz* c, fopAc_ac_c* a, s16 ang, s16 spread, f32 r, f32 h) {
    return gabi::call<bool>(0x02588230, c, a, ang, spread, r, h);
}
/* 025880B4 dLib_checkActorInCircle(cXyz center (copy), fopAc_ac_c*, f32 radius, f32 height) */
static inline bool dLib_checkActorInCircle(cXyz* c, fopAc_ac_c* a, f32 r, f32 h) { return gabi::call<bool>(0x025880B4, c, a, r, h); }
static inline bool checkInFan(const cXyz& center, fopAc_ac_c* a, s16 ang, s16 spread, f32 r, f32 h) {
    gabi::Local<cXyz> c;
    c->copy(center);
    return dLib_checkActorInFan(c, a, ang, spread, r, h);
}
static inline bool checkInCircle(const cXyz& center, fopAc_ac_c* a, f32 r, f32 h) {
    gabi::Local<cXyz> c;
    c->copy(center);
    return dLib_checkActorInCircle(c, a, r, h);
}
/* 02587684 dLib_debugDrawFan(cXyz& pos, s16, s16, f32, const GXColor&), 02587514 dLib_debugDrawAxis(Mtx, f32) */
static inline void dLib_debugDrawFan(cXyz* p, s16 a, s16 s, f32 r, u32 color) { gabi::call(0x02587684, p, a, s, r, color); }
static inline void dLib_debugDrawAxis(Mtx34* m, f32 s) { gabi::call(0x02587514, m, s); }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (copy), s16 yrot, s16 vel, bool headOnly) */
static inline void lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, const cXyz& eye, s16 yrot, s16 vel, bool headOnly) {
    gabi::Local<cXyz> e;
    e->copy(eye);
    gabi::call(0x0259DED0, j, outY, target, e.get(), yrot, vel, headOnly);
}
/* cLib_calcTimer<int> 0211D2F8, cLib_calcTimer<u8> 0207A9A0 (out-of-line copies) */
static inline s32 cLib_calcTimer(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); }
static inline u8 cLib_calcTimer(be<u8>* t) { return gabi::call<u8>(0x0207A9A0, t); }
/* 0200F268 cLib_addCalcPosXZ2(cXyz*, const cXyz&, f32 scale, f32 maxStep) */
static inline void cLib_addCalcPosXZ2(cXyz* p, const cXyz* t, f32 s, f32 m) { gabi::call(0x0200F268, p, t, s, m); }
/* 024EFF3C dBgS_AcchCir::SetWallR */
static inline void AcchCir_SetWallR(dBgS_AcchCir* c, f32 r) { gabi::call(0x024EFF3C, c, r); }
/* 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out) */
static inline bool fopAcM_SearchByID_out(u32 id, gptr<fopAc_ac_c>* out) { return gabi::call<bool>(0x025D54C4, id, out); }
/* 025D77DC fopAcM_orderOtherEvent2(actor, name, flag, hind) */
static inline void fopAcM_orderOtherEvent2(fopAc_ac_c* a, const char* name, u16 flag, u16 hind) { gabi::call(0x025D77DC, a, name, flag, hind); }
/* 025DA088 fopAcM_setGbaName(actor, item, gbaName0, gbaName1) */
static inline void fopAcM_setGbaName(fopAc_ac_c* a, u8 item, u8 n0, u8 n1) { gabi::call(0x025DA088, a, item, n0, n1); }
/* 025445B8 dEvent_manager_c::startCheckOld(name) (play+0x52C4) */
static inline BOOL dComIfGp_evmng_startCheckOld(const char* name) { return gabi::call<BOOL>(0x025445B8, dComIfGp_getPEvtManager(), name); }
/* dComIfGp_event_runCheck: the event control's run flag (play+0x5292) */
static inline bool dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292) != 0; }
/* event reset through an already fetched play pointer (HD hoists dComIfGp_get) */
static inline void event_reset_at(u32 play) { gabi::store<u16>(play + 0x52B8, (u16)(gabi::load<u16>(play + 0x52B8) | 8)); }
static inline fopAc_ac_c* player0_at(u32 play) { return gabi::at<fopAc_ac_c>(gabi::load<u32>(play + PLAY_PLAYER)); }
/* 0252A038 dDetect_c::chk_light(cXyz*) (play+0x5A20) */
static inline BOOL dComIfGp_getDetect_chk_light(cXyz* pos) { return gabi::call<BOOL>(0x0252A038, dComIfGp_ea() + PLAY_DETECT, pos); }
/* 0200E814 cDT_NamePTbl::GetIndex (play+0x50AC) */
static inline s32 dComIfGp_CharTbl_GetNameIndex(const char* name, s32 p) {
    return gabi::call<s32>(0x0200E814, dComIfGp_ea() + PLAY_NAMETBL, name, p);
}
/* CPad (port 0): main stick X/Y (f32), A/B/X/Y trigger (bool) */
static inline f32 CPad_GET_STICK_POS_X(u32 port) { return gabi::call<f32>(0x0200796C, port); }
static inline f32 CPad_GET_STICK_POS_Y(u32 port) { return gabi::call<f32>(0x02007990, port); }
static inline BOOL CPad_CHECK_TRIG_A(u32 port) { return gabi::call<BOOL>(0x02007898, port); }
static inline BOOL CPad_CHECK_TRIG_B(u32 port) { return gabi::call<BOOL>(0x020078BC, port); }
static inline BOOL CPad_CHECK_TRIG_X(u32 port) { return gabi::call<BOOL>(0x020078E8, port); }
static inline BOOL CPad_CHECK_TRIG_Y(u32 port) { return gabi::call<BOOL>(0x02007914, port); }
/* c_damagereaction */
static inline BOOL enemy_ice(enemyice_l* e) { return gabi::call<BOOL>(0x020402C8, e); }
static inline void enemy_fire(enemyfire_l* f) { gabi::call(0x02041570, f); }
static inline void enemy_fire_remove(enemyfire_l* f) { gabi::call(0x02041C30, f); }
/* d_cc_uty: CcAtInfo (0x1C), cc_at_check */
#ifndef WWHD_CCATINFO_L
#define WWHD_CCATINFO_L
struct CcAtInfo_l {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ gptr<cXyz> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_SIZE(CcAtInfo_l, 0x1C);
#endif
static inline u32 cc_at_check(fopAc_ac_c* a, CcAtInfo_l* i) { return gabi::call<u32>(0x025192A8, a, i); }
/* audio (HD inlines with `this` known non-null: no checks) */
static inline void fm_seStart(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    mDoAud_seStart(id, &a->eyePos, param, reverb);
}
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb) */
static inline void fm_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    gabi::call(0x025E1AA4, id, &a->eyePos, gabi::load<u32>(gabi::ea(a) + 4), param, reverb);
}
/* daPy_py_c (HD layout): head top position +0x3D8, cut type +0x3AC; virtuals at the HD vtable (+0xB4) */
static inline cXyz* daPy_getHeadTopPos(fopAc_ac_c* p) { return gabi::at<cXyz>(gabi::ea(p) + 0x3D8); }
static inline u8 daPy_getCutType(fopAc_ac_c* p) { return gabi::load<u8>(gabi::ea(p) + 0x3AC); }
static inline f32 daPy_getBaseAnimeFrame(fopAc_ac_c* p) { return gabi::call_ptr<f32>(gabi::load<u32>(p->__vtbl + 0xA4), p); }
static inline void daPy_setPlayerPosAndAngle(fopAc_ac_c* p, cXyz* pos, s16 a) { gabi::call_ptr(gabi::load<u32>(p->__vtbl + 0x114), p, pos, a); }
/* daPy_lk_c::checkNoDamageMode (inline): +0x3BC bit 0, or the HD s16 at +0x699E */
static inline bool daPy_checkNoDamageMode(fopAc_ac_c* p) {
    return (gabi::load<u32>(gabi::ea(p) + 0x3BC) & 1) != 0 || gabi::load<s16>(gabi::ea(p) + 0x699E) != 0;
}
static inline void daPy_lk_setDamagePoint(fopAc_ac_c* p, f32 d) { gabi::call(0x023F51D0, p, d); }
/* 023F4CB0 (unnamed, HD): the no-damage-mode counterpart of setDamagePoint (plays SE 0x2886) */
static inline void daPy_lk_noDamagePoint(fopAc_ac_c* p, f32 d) { gabi::call(0x023F4CB0, p, d); }
/* daPy_py_c::changeOriginalDemo (inline): demo mode +0x428 = 0, demo type +0x420 = 3 */
static inline void daPy_changeOriginalDemo(fopAc_ac_c* p) {
    gabi::store<u32>(gabi::ea(p) + 0x428, 0);
    gabi::store<u16>(gabi::ea(p) + 0x420, 3);
}
/* save: dComIfGs_getLife (u16 at info+2) */
static inline u16 dComIfGs_getLife() { return gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x22); }

/* attention_info (fopAc_ac_c +0x388): position +0x08, flags +0x14 */
static inline cXyz* attention_pos(fopAc_ac_c* a) { return gabi::at<cXyz>(gabi::ea(a) + 0x390); }
static inline be<u32>* attention_flags(fopAc_ac_c* a) { return gabi::at<be<u32>>(gabi::ea(a) + 0x39C); }

/* daFm_c::isLinkControl (021446EC): player 0 is not the link player */
