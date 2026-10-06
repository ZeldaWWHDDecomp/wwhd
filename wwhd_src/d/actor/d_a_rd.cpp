/**
 * d_a_rd.cpp (WWHD)
 * Enemy - ReDead
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_rd.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_rd.h"

#define SAFESTRING_VTBL 0x10038E8C /* this TU's sead::SafeString vtable */
#define RD_VTBL 0x10038EE4         /* daRd_c vtable (HD virtual destructor) */
#define m_arc_name STR(0x1003937C) /* "Rd" (one object: daRd_c::m_arc_name) */
#define m_heapsize 0x2520

enum {
    dRes_INDEX_RD_BCK_SUWARIP_e = 0xF,
    dRes_INDEX_RD_BDL_RD_e = 0x17,
    dRes_INDEX_RD_BRK_NML_e = 0x1D,
    dRes_INDEX_RD_BTK_RD_CLOSE_e = 0x20,
};
enum { RD_JNT_REE_MUNE_1_e = 8, RD_JNT_REE_KUBI_1_e = 10, RD_JNT_REE_ATAMA_1_e = 12 };
enum {
    JA_SE_LK_SW_HIT_S = 0x2803, JA_SE_LK_W_WEP_HIT = 0x2833, JA_SE_LK_HAMMER_HIT = 0x2855,
    JA_SE_LK_HS_SPIKE = 0x286F, JA_SE_LK_ARROW_HIT = 0x2879, JA_SE_CV_RD_SCREAM = 0x48D3,
    JA_SE_CV_RD_DAMAGE = 0x48D4, JA_SE_CV_RD_DIE = 0x48D5, JA_SE_CM_PW_BECOME_SOLID = 0x5135,
};
enum { fpcNm_RD_e = 0xE0, DSNAP_TYPE_RD = 0xB9, dItemNo_MIRROR_SHIELD_e = 0x3C };
enum {
    AT_TYPE_SWORD = 0x2, AT_TYPE_BOMB = 0x20, AT_TYPE_BOOMERANG = 0x40, AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_FIRE = 0x200, AT_TYPE_MACHETE = 0x400, AT_TYPE_UNK800 = 0x800, AT_TYPE_NORMAL_ARROW = 0x4000,
    AT_TYPE_HOOKSHOT = 0x8000, AT_TYPE_SKULL_HAMMER = 0x10000, AT_TYPE_UNK20000 = 0x20000,
    AT_TYPE_FIRE_ARROW = 0x40000, AT_TYPE_ICE_ARROW = 0x80000, AT_TYPE_LIGHT_ARROW = 0x100000,
    AT_TYPE_WIND = 0x200000, AT_TYPE_LIGHT = 0x800000, AT_TYPE_STALFOS_MACE = 0x1000000,
    AT_TYPE_DARKNUT_SWORD = 0x4000000, AT_TYPE_GRAPPLING_HOOK = 0x8000000, AT_TYPE_MOBLIN_SPEAR = 0x10000000,
};

/* ---- l_HIO (daRd_HIO_c at 0x1046D6A0; HD layout = GameCube; mNpc (dNpc_HIO_c) from +0x04) ---- */
#define L_HIO 0x1046D6A0u
static inline f32 hio_f(u32 off) { return gabi::load<f32>(L_HIO + off); }
static inline s16 hio_s(u32 off) { return gabi::load<s16>(L_HIO + off); }
enum : u32 {
    HIO_m2C = 0x2C, HIO_m30 = 0x30, HIO_m34 = 0x34, HIO_mCryRadius = 0x38, HIO_mAttackRadius = 0x3C,
    HIO_m40 = 0x40, HIO_mCrySpreadAngle = 0x42, HIO_mAttackSpreadAngle = 0x44, HIO_m46 = 0x46, HIO_m48 = 0x48,
    HIO_m4A = 0x4A, HIO_m4C = 0x4C, HIO_m4E = 0x4E, HIO_m50 = 0x50, HIO_m52 = 0x52, HIO_m54 = 0x54,
    HIO_m58 = 0x58, HIO_m5C = 0x5C, HIO_m60 = 0x60, HIO_m64 = 0x64, HIO_m68 = 0x68, HIO_mReturnWalkSpeed = 0x6C,
    HIO_m70 = 0x70, HIO_m74 = 0x74, HIO_m78 = 0x78, HIO_mParalysisDuration = 0x7A,
    /* mNpc (dNpc_HIO_c): HD order head/backbone interleaved */
    HIO_mMaxHeadX = 0x08, HIO_mMaxBackboneX = 0x0A, HIO_mMaxHeadY = 0x0C, HIO_mMaxBackboneY = 0x0E,
    HIO_mMinHeadX = 0x10, HIO_mMinBackboneX = 0x12, HIO_mMinHeadY = 0x14, HIO_mMinBackboneY = 0x16,
    HIO_mMaxTurnStep = 0x18, HIO_mMaxHeadTurnVel = 0x1A,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD J3D: j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, mDoMtx_stack_c::now 0x1048D0CC;
 * joint matrices in the model's block at +0x2C (+0x4 flags, +0x10 matrices), user area +0xB8 */
static inline u32 j3dSys_model() { return gabi::load<u32>(0x104B462C); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }
static inline Mtx34* mtx_now() { return gabi::at<Mtx34>(0x1048D0CC); }
static inline Mtx34* rd_getAnmMtx(J3DModel* model, u32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(model) + 0x2C);
    u32 m = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10));
    return gabi::at<Mtx34>(m + jntNo * 0x30);
}
static inline J3DModelData* rd_modelData(J3DModel* m) { return gabi::at<J3DModelData>(gabi::load<u32>(gabi::ea(m) + 0xAC)); }
/* J3DModelData::getJointNodePointer(i) (HD inline): count +4, nodes (0x1C) at +8; out of range: node 0 */
static inline u32 rd_jointNode(J3DModelData* md, u32 i) {
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
/* 025E8154 mDoExt_brkAnm::init(data, key, anmPlay, attr, rate, start, end, modify, entry(stack)) */
static inline BOOL brkAnm_init(mDoExt_brkAnm_rd* a, J3DModelData* d, void* key, bool play, s32 mode, f32 rate, s16 start, s16 end,
                               bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, key, play, mode, rate, start, end, modify, entry);
}
/* 025E83FC mDoExt_brkAnm::entry(J3DModelData*, f32 frame) */
static inline void brkAnm_entry(mDoExt_brkAnm_rd* a, J3DModelData* d, f32 f) { gabi::call(0x025E83FC, a, d, f); }
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
static inline u32 cc_at_check(fopAc_ac_c* a, CcAtInfo_l* i) { return gabi::call<u32>(0x025192A8, a, i); }
/* audio (HD inlines with `this` known non-null: no checks) */
static inline void rd_seStart(daRd_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(a->current.roomNo);
    mDoAud_seStart(id, &a->eyePos, param, reverb);
}
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb) */
static inline void rd_monsSeStart(daRd_c* a, u32 id, u32 param) {
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

/* ---- functions ---- */

/* 0245A194 */
fopAc_ac_c* daRd_c::_searchNearDeadRd(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x0245A194, fopAc_ac_c*, this, i_actor);
    if (i_actor != nullptr && fpcM_GetName(i_actor) == fpcNm_RD_e) { /* HD: fopAcM_GetName checks for NULL */
        daRd_c* other = (daRd_c*)i_actor;
        if (other->mMode == MODE_DEATH) {
            if (fopAcM_searchActorDistanceXZ(this, i_actor) < hio_f(HIO_m34)) {
                return i_actor;
            }
        }
    }
    return nullptr;
}
VERIFY(0x0245A194, &daRd_c::_searchNearDeadRd);

/* 0245A20C */
static void* searchNeadDeadRd_CB(void* i_actor, void* i_this) {
    WWHD_FUNC(0x0245A20C, void*, i_actor, i_this);
    return ((daRd_c*)i_this)->_searchNearDeadRd((fopAc_ac_c*)i_actor);
}
VERIFY(0x0245A20C, searchNeadDeadRd_CB);

/* model->setAnmMtx(jntNo, mDoMtx_stack_c::get()) */
static inline void rd_setAnmMtx(J3DModel* model, u32 jntNo) {
    mtx_copy(rd_getAnmMtx(model, jntNo), mtx_now());
}

/* 0245A21C */
void daRd_c::_nodeControl(J3DNode* node, J3DModel* model) {
    WWHD_FUNC(0x0245A21C, void, this, node, model);
    J3DJoint* joint = J3DNode_toJoint(node);
    u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
    PSMTXCopy(rd_getAnmMtx(model, jntNo), mtx_now());

    if (mJntCtrl.mHeadJntNum == (s32)jntNo) {
        /* static cXyz l_offsetAttPos(0, 0, 0), l_offsetEyePos(24, -16, 0) (guarded function statics) */
        if (gabi::load<u32>(0x1046D748) == 0) {
            gabi::store<f32>(0x1046D688, 0.0f);
            gabi::store<f32>(0x1046D690, 0.0f);
            gabi::store<u32>(0x1046D748, 1);
            gabi::store<f32>(0x1046D68C, 0.0f);
        }
        if (gabi::load<u32>(0x1046D74C) == 0) {
            gabi::store<u32>(0x1046D74C, 1);
            gabi::store<f32>(0x1046D694, 24.0f);
            gabi::store<f32>(0x1046D69C, 0.0f);
            gabi::store<f32>(0x1046D698, -16.0f);
        }
        PSMTXMultVec(mtx_now(), gabi::at<cXyz>(0x1046D688), &mTargetPos);
        mDoMtx_XrotM(mtx_now(), mJntCtrl.mAngles[0][1]);
        mDoMtx_ZrotM(mtx_now(), mJntCtrl.mAngles[0][0]);
        PSMTXMultVec(mtx_now(), gabi::at<cXyz>(0x1046D694), &mRdEyePos);
        mDoMtx_XrotM(mtx_now(), mD1A);
        mDoMtx_ZrotM(mtx_now(), mD1C);
        mDoMtx_YrotM(mtx_now(), mD1E);
    } else if (mJntCtrl.mBackboneJntNum == (s32)jntNo) {
        mDoMtx_XrotM(mtx_now(), mJntCtrl.mAngles[1][1]);
        mDoMtx_ZrotM(mtx_now(), mJntCtrl.mAngles[1][0]);
    }

    PSMTXCopy(mtx_now(), J3DSys_mCurrentMtx());
    rd_setAnmMtx(model, jntNo);
}
VERIFY(0x0245A21C, &daRd_c::_nodeControl);

/* 0245A494 */
static BOOL nodeControl_CB(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0245A494, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        u32 model = j3dSys_model();
        daRd_c* i_this = gabi::at<daRd_c>(gabi::load<u32>(model + 0xB8));
        if (i_this != nullptr) {
            i_this->_nodeControl(node, gabi::at<J3DModel>(model));
        }
    }
    return TRUE;
}
VERIFY(0x0245A494, nodeControl_CB);

/* 0245A4DC */
void daRd_c::_nodeHeadControl(J3DNode* node, J3DModel* model) {
    WWHD_FUNC(0x0245A4DC, void, this, node, model);
    J3DJoint* joint = J3DNode_toJoint(node);
    u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);

    f32 d38 = mD38; /* cXyz temp4(mD38, 0, 0) */
    PSMTXCopy(rd_getAnmMtx(model, jntNo), mtx_now());
    mDoMtx_stack_transM(d38, 0.0f, 0.0f);

    PSMTXCopy(mtx_now(), J3DSys_mCurrentMtx());
    gabi::Local<cXyz> temp3;
    temp3->x = 0.0f;
    temp3->y = 0.0f;
    temp3->z = 50.0f;
    PSMTXMultVec(mtx_now(), temp3, &mRdEyePos);
    rd_setAnmMtx(model, jntNo);
    PSMTXCopy(mtx_now(), &mCE8);

    gabi::Local<Mtx34> mtx;
    gabi::Local<csXyz> angle;
    mDoMtx_inverseTranspose(mtx_now(), mtx);
    mDoMtx_MtxToRot(mtx, angle);
    mHeadAngle = angle->y;
}
VERIFY(0x0245A4DC, &daRd_c::_nodeHeadControl);

/* 0245A678 */
static BOOL nodeHeadControl_CB(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x0245A678, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        u32 model = j3dSys_model();
        daRd_c* i_this = gabi::at<daRd_c>(gabi::load<u32>(model + 0xB8));
        if (i_this != nullptr) {
            i_this->_nodeHeadControl(node, gabi::at<J3DModel>(model));
        }
    }
    return TRUE;
}
VERIFY(0x0245A678, nodeHeadControl_CB);

/* 0245A6C0 */
bool daRd_c::createArrowHeap() {
    WWHD_FUNC(0x0245A6C0, bool, this);
    /* search_data (22 joint-hit shapes) in .data */
    mpJntHit = JntHit_create(mpMorf->getModel(), 0x101CF390, 0x16);
    if (mpJntHit) {
        jntHit = gabi::ea(mpJntHit.get()); /* fopAcM_SetJntHit */
    } else {
        return false;
    }
    return true;
}
VERIFY(0x0245A6C0, &daRd_c::createArrowHeap);

/* 0245A72C */
BOOL daRd_c::_createHeap() {
    WWHD_FUNC(0x0245A72C, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_RD_BDL_RD_e, SAFESTRING_VTBL);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x10038F18), 0x1F8, STR(0x10038F24)); /* modelData != NULL */
    }

    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_RD_BCK_SUWARIP_e, SAFESTRING_VTBL);
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 1,
                                    nullptr, 0x00080000, 0x37441622);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this)); /* setUserArea */

    if (!invisibleModel_create(mInvisModel, mpMorf->getModel())) {
        return FALSE;
    }

    J3DAnmTextureSRTKey* btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_RD_BTK_RD_CLOSE_e, SAFESTRING_VTBL);
    if (btk == nullptr) {
        JUT_ASSERT_fail(STR(0x10038F18), 0x20D, STR(0x10038F38)); /* btk != NULL */
    }
    if (!mBtkAnm.init(modelData, btk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    gabi::store<u32>(rd_jointNode(modelData, RD_JNT_REE_ATAMA_1_e) + 8, 0x0245A678 /* nodeHeadControl_CB */);

    J3DAnmTevRegKey* brk = (J3DAnmTevRegKey*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_RD_BRK_NML_e, SAFESTRING_VTBL);
    if (brk == nullptr) {
        JUT_ASSERT_fail(STR(0x10038F18), 0x226, STR(0x10038F44)); /* brk != NULL */
    }
    if (!brkAnm_init(&mBrkAnm, modelData, brk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    return createArrowHeap() ? TRUE : FALSE;
}
VERIFY(0x0245A72C, &daRd_c::_createHeap);

/* 0245A980 */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0245A980, BOOL, i_this);
    return ((daRd_c*)i_this)->_createHeap();
}
VERIFY(0x0245A980, createHeap_CB);

/* 0245A984: enemyfire::enemyfire (compiler-generated; this TU's copy) */
static enemyfire_l* enemyfire_ct(enemyfire_l* p) {
    WWHD_FUNC(0x0245A984, enemyfire_l*, p);
    if (p == nullptr) {
        p = (enemyfire_l*)operator_new(0x22C);
        if (p == nullptr) return p;
    }
    if (gabi::ea(p) + 0x8C == 0)
        operator_new(0xC);
    dCcD_Stts_ct(&p->mStts);
    gabi::call(0x025166F0, &p->mSph); /* dCcD_Sph::dCcD_Sph */
    p->m228 = 1.0f;
    return p;
}
VERIFY(0x0245A984, enemyfire_ct);

static const dBgS_ObjAcch_vt RD_OBJACCH_VT = {0x10038EB4, 0x10038ED4, 0x10038EC4};
#define RD_AAB_VTBL 0x10038EA4 /* this TU's cM3dGAab vtable */

/* 0245AA10: daRd_c::daRd_c (compiler-generated) */
static daRd_c* daRd_c_ct(daRd_c* p) {
    WWHD_FUNC(0x0245AA10, daRd_c*, p);
    if (p == nullptr) {
        p = (daRd_c*)operator_new(0xF1C);
        if (p == nullptr) return p;
    }
    fopAc_ac_c_ct(p);
    p->__vtbl = RD_VTBL;
    mDoExt_btkAnm::ct(&p->mBtkAnm);
    gabi::call(0x025E80D0, &p->mBrkAnm); /* mDoExt_brkAnm::mDoExt_brkAnm */
    dBgS_ObjAcch_ct(&p->mAcch, RD_OBJACCH_VT);
    dBgS_AcchCir_ct(&p->mAcchCir);
    dCcD_Stts_ct(&p->mStts);
    dCcD_Cyl_ct(&p->mCyl, RD_AAB_VTBL);
    gabi::call(0x025E895C, p->mInvisModel); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    gabi::call(0x0259DAA0, &p->mJntCtrl);   /* dNpc_JntCtrl_c::dNpc_JntCtrl_c */
    dCcD_Stts_ct(&p->mEnemyIce.mStts);
    dCcD_Cyl_ct(&p->mEnemyIce.mCyl, RD_AAB_VTBL);
    dBgS_AcchCir_ct(&p->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&p->mEnemyIce.mBgAcch, RD_OBJACCH_VT);
    enemyfire_ct(&p->mEnemyFire);
    return p;
}
VERIFY(0x0245AA10, daRd_c_ct);

/* 0245AB98 */
void daRd_c::getArg() {
    WWHD_FUNC(0x0245AB98, void, this);
    u32 param = fopAcM_GetParam(this);
    mWhichIdleAnm = param & 1;
    u8 radiusParam = (param >> 1) & 0x7F;
    int areaRadius = radiusParam;
    mChecksSwitch = (param >> 8) & 0xFF;
    mSwNo = (param >> 0x18) & 0xFF;
    if (areaRadius == 0x7F) {
        areaRadius = 0;
    }
    mAreaRadius = gabi::ftoi(hio_f(HIO_m30) + (f32)areaRadius);
}
VERIFY(0x0245AB98, &daRd_c::getArg);

/* setBtkAnm tables (.data): a_anm_idx_tbl (int[4]) and a_anm_prm_tbl ({s8, s8, int loopMode}[5]) */
static inline s32 btk_idx_tbl(s32 i) { return gabi::load<s32>(0x10038F58 + 4 * i); }
static inline s8 btk_prm_m00(s32 i) { return gabi::load<s8>(0x10038F68 + 8 * i); }
static inline s8 btk_prm_m01(s32 i) { return gabi::load<s8>(0x10038F68 + 8 * i + 1); }
static inline s32 btk_prm_loop(s32 i) { return gabi::load<s32>(0x10038F68 + 8 * i + 4); }

/* 0245AC14 */
void daRd_c::setBtkAnm(s8 idx) {
    WWHD_FUNC(0x0245AC14, void, this, idx);
    if (idx != 5) {
        m6DB = idx;
    }

    int r5 = btk_prm_m00(m6DB);
    if (m6DC != m6DB && r5 != -1) {
        J3DAnmTextureSRTKey* btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(m_arc_name, btk_idx_tbl(r5), SAFESTRING_VTBL);
        if (btk == nullptr) {
            JUT_ASSERT_fail(STR(0x10038F90), 0x7A7, STR(0x10038F9C)); /* btk != NULL */
        }
        J3DModelData* modelData = rd_modelData(mpMorf->getModel());
        mBtkAnm.init(modelData, btk, true, btk_prm_loop(m6DB), 1.0f, 0, -1, true, 0);

        /* isStop */
        if (mBtkAnm.mFrameCtrl.checkState(J3DFrameCtrl::STATE_STOP_E) || mBtkAnm.mFrameCtrl.getRate() == 0.0f) {
            if (btk_prm_m01(m6DB) != -1 && btk_prm_loop(m6DB) == J3DFrameCtrl::EMode_NONE) {
                m6DB = btk_prm_m01(m6DB);
            }
        }
    }

    m6DC = m6DB;
}
VERIFY(0x0245AC14, &daRd_c::setBtkAnm);

/* mode_tbl (.data, 12 entries of {init PTMF, run PTMF, name}) */
#define RD_MODE_TBL 0x10038FA8u

/* 0245ADB0 */
void daRd_c::modeProc(daRd_c::Proc_e proc, int newMode) {
    WWHD_FUNC(0x0245ADB0, void, this, proc, newMode);
    if (proc == PROC_INIT_e) {
        if (newMode == MODE_CRY || newMode == MODE_ATTACK) {
            onIkari();
            setBtkAnm(3);
        } else if (newMode != MODE_DAMAGE) {
            offIkari();
            setBtkAnm(4);
        }

        if (newMode == MODE_DEATH || newMode == MODE_SW_WAIT) {
            actor_status &= ~0x20u;            /* fopAcM_OffStatus(SHOWMAP) */
            *attention_flags(this) &= ~4u;      /* LOCKON_BATTLE */
        } else {
            actor_status |= 0x20u;
            *attention_flags(this) |= 4u;
        }

        mMode = newMode;
        ptmf_call(RD_MODE_TBL + 0x14 * newMode, this); /* (this->*mode_tbl[mMode].init)() */
    } else if (proc == PROC_EXEC_e) {
        ptmf_call(RD_MODE_TBL + 0x14 * mMode + 8, this); /* (this->*mode_tbl[mMode].run)() */
    }
}
VERIFY(0x0245ADB0, &daRd_c::modeProc);

/* 0245AF80 */
void daRd_c::setBrkAnm(s8 idx) {
    WWHD_FUNC(0x0245AF80, void, this, idx);
    J3DModel* model = mpMorf->getModel();
    J3DAnmTevRegKey* brk = (J3DAnmTevRegKey*)dComIfG_getObjectRes(m_arc_name, gabi::load<s32>(0x10039100 + 4 * idx), SAFESTRING_VTBL);
    if (brk == nullptr) {
        JUT_ASSERT_fail(STR(0x10039120), 0x77F, STR(0x1003912C)); /* brk != NULL */
    }
    brkAnm_init(&mBrkAnm, rd_modelData(model), brk, true, gabi::load<s32>(0x10039110 + 4 * idx), 1.0f, 0, -1, true, 0);
}
VERIFY(0x0245AF80, &daRd_c::setBrkAnm);

/* 0245B050 */
void daRd_c::setAnm(s8 anmPrmIdx, bool force) {
    WWHD_FUNC(0x0245B050, void, this, anmPrmIdx, force);
    if (anmPrmIdx != AnmPrm_NULL) {
        mAnmPrmIdx = anmPrmIdx;
    }

    if (mOldAnmPrmIdx != mAnmPrmIdx) {
        if (isAnm(AnmPrm_BEAM_HIT)) {
            setBrkAnm(1);
        } else if (isAnm(AnmPrm_BEAM)) {
            setBrkAnm(2);
        } else if (isAnm(AnmPrm_BEAM_END)) {
            setBrkAnm(3);
        } else {
            setBrkAnm(0);
        }
    }

    if (mBckIdx == BckIdx_BEAM_HIT || mBckIdx == BckIdx_BEAM || mBckIdx == BckIdx_BEAM_END) {
        /* mBrkAnm.setFrame(mpMorf->getFrame()) */
        gabi::store<u32>(gabi::ea(&mBrkAnm.mFrameCtrl.mFrame), gabi::load<u32>(gabi::ea(&mpMorf->mFrameCtrl.mFrame)));
    }

    dLib_bcks_setAnm(m_arc_name, mpMorf, &mBckIdx, &mAnmPrmIdx, &mOldAnmPrmIdx, 0x10039138 /* a_anm_bcks_tbl */,
                     0x10039170 /* a_anm_prm_tbl */, force);
}
VERIFY(0x0245B050, &daRd_c::setAnm);

/* 0245B154 */
void daRd_c::setMtx() {
    WWHD_FUNC(0x0245B154, void, this);
    J3DModel* model = mpMorf->getModel();
    J3DModel_setBaseScale(model, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    J3DModel_setBaseTRMtx(model, mtx_now());
}
VERIFY(0x0245B154, &daRd_c::setMtx);

/* createInit: the tail GHS duplicated into each branch */
static inline void createInit_tail(daRd_c* i_this) {
    i_this->setBrkAnm(0);
    i_this->setMtx();
    i_this->mBtkAnm.play();
    mDoExt_baseAnm_play(&i_this->mBrkAnm);
    i_this->mpMorf->play(&i_this->current.pos, 0, 0);
    i_this->mBrkAnm.mFrameCtrl.setFrame(0.0f);
    i_this->mpMorf->calc();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
}

/* 0245B234 */
void daRd_c::createInit() {
    WWHD_FUNC(0x0245B234, void, this);
    mStts.Init(0xFF, 0, this);
    mCyl.Set(gabi::at<dCcD_SrcCyl>(0x10039380) /* m_cyl_src */);
    mCyl.SetStts(&mStts);
    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mAcch.SetRoofNone();
    J3DModelData* modelData = rd_modelData(mpMorf->getModel());
    mJntCtrl.mHeadJntNum = RD_JNT_REE_KUBI_1_e;
    mJntCtrl.mBackboneJntNum = RD_JNT_REE_MUNE_1_e;
    gabi::store<u32>(rd_jointNode(modelData, RD_JNT_REE_KUBI_1_e) + 8, 0x0245A494 /* nodeControl_CB */);
    gabi::store<u32>(rd_jointNode(modelData, RD_JNT_REE_MUNE_1_e) + 8, 0x0245A494);
    setBtkAnm(2);

    if (mChecksSwitch == 0) {
        modeProcInit(MODE_SW_WAIT);
        gabi::Local<cXyz> offset;
        offset->x = 0.0f;
        offset->y = 40.0f;
        offset->z = 10.0f;
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_stack_c::YrotM(shape_angle.y);
        PSMTXMultVec(mtx_now(), offset, &current.pos);
    } else {
        modeProcInit(MODE_WAIT);
        switch ((u32)mWhichIdleAnm) {
        case 0:
            setAnm(AnmPrm_TACHIP1, false);
            break;
        case 1:
            setAnm(AnmPrm_SUWARIP, false);
            break;
        }
    }

    createInit_tail(this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel())); /* fopAcM_SetMtx */
    fopAcM_setCullSizeBox(this, -100.0f, -10.0f, -100.0f, 100.0f, 250.0f, 150.0f);

    mD3C = 1;
    mD40 = 1;
    stealItemLeft = 5;

    mEnemyFire.mpMcaMorf = mpMorf;
    mEnemyFire.mpActor = this;
    for (int i = 0; i < 10; i++) {
        mEnemyFire.mFlameJntIdxs[i] = gabi::load<s8>(0x101CF628 + i);                                        /* fire_j */
        gabi::store<u32>(gabi::ea(&mEnemyFire.mParticleScale[i]), gabi::load<u32>(0x101CF600 + 4 * i)); /* fire_sc */
    }

    mEnemyIce.mpActor = this;
    mEnemyIce.m00C = 1;
    mEnemyIce.mWallRadius = 50.0f;
    mEnemyIce.mCylHeight = 250.0f;

    max_health = (s8)hio_s(HIO_m46);
    health = max_health;
    mSpawnPos.copy(current.pos);
    mSpawnAngle = shape_angle.y;
    gravity = -4.5f;

    if (mWhichIdleAnm == 0) {
        itemTableIdx = dComIfGp_CharTbl_GetNameIndex(STR(0x10039284) /* "Rdead1" */, 0);
    }
    if (mWhichIdleAnm == 1) {
        itemTableIdx = dComIfGp_CharTbl_GetNameIndex(STR(0x1003928C) /* "Rdead2" */, 0);
    }
}
VERIFY(0x0245B234, &daRd_c::createInit);

/* 0245B6FC */
cPhs_State daRd_c::_create() {
    WWHD_FUNC(0x0245B6FC, cPhs_State, this);
    /* fopAcM_ct(this, daRd_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            daRd_c_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&mPhs, m_arc_name);
    if (phase_state == cPhs_COMPLEATE_e) {
        getArg();
        if (!fopAcM_entrySolidHeap(this, 0x0245A980 /* createHeap_CB */, m_heapsize)) {
            return cPhs_ERROR_e;
        }
        createInit();
    }
    return phase_state;
}
VERIFY(0x0245B6FC, &daRd_c::_create);

/* 0245B7B8 */
static cPhs_State daRdCreate(void* i_this) {
    WWHD_FUNC(0x0245B7B8, cPhs_State, i_this);
    return ((daRd_c*)i_this)->_create();
}
VERIFY(0x0245B7B8, daRdCreate);

/* 0245B7BC */
bool daRd_c::_delete() {
    WWHD_FUNC(0x0245B7BC, bool, this);
    dComIfG_resDelete(&mPhs, m_arc_name);
    enemy_fire_remove(&mEnemyFire);
    if (heap) {
        mpMorf->stopZelAnime();
    }
    return true;
}
VERIFY(0x0245B7BC, &daRd_c::_delete);

/* 0245B814 */
static BOOL daRdDelete(void* i_this) {
    WWHD_FUNC(0x0245B814, BOOL, i_this);
    return ((daRd_c*)i_this)->_delete();
}
VERIFY(0x0245B814, daRdDelete);

/* 0245B818 */
void daRd_c::setIceCollision() {
    WWHD_FUNC(0x0245B818, void, this);
    if (isAnm(AnmPrm_SUWARIP)) {
        mEnemyIce.mWallRadius = 50.0f;
        mEnemyIce.mCylHeight = 170.0f;
    } else {
        mEnemyIce.mWallRadius = 50.0f;
        mEnemyIce.mCylHeight = 250.0f;
    }
}
VERIFY(0x0245B818, &daRd_c::setIceCollision);

/* 0245B854 */
void daRd_c::setAttention() {
    WWHD_FUNC(0x0245B854, void, this);
    gabi::Local<cXyz> attnPos;
    attnPos->x = 60.0f;
    attnPos->y = 0.0f;
    attnPos->z = 0.0f;
    PSMTXCopy(rd_getAnmMtx(mpMorf->getModel(), RD_JNT_REE_ATAMA_1_e), mtx_now());
    PSMTXMultVec(mtx_now(), attnPos, attention_pos(this));
    /* mDoMtx_stack_c::multVecZero(&eyeballPos); eyePos = eyeballPos */
    Mtx34* m = mtx_now();
    eyePos.x = m->m[0][3];
    eyePos.z = m->m[2][3];
    f32 ey = m->m[1][3];
    eyePos.y = ey;
    attention_pos(this)->y = attention_pos(this)->y + hio_f(HIO_m58);
    eyePos.y = ey + hio_f(HIO_m5C);

    if (dComIfGp_event_runCheck()) {
        attention_pos(this)->copy(current.pos);
        eyePos.copy(current.pos);
        attention_pos(this)->y = attention_pos(this)->y + 150.0f;
        eyePos.y = eyePos.y + 150.0f;
    }

    if (mEnemyIce.mFreezeTimer > 20) {
        actor_status &= ~0x200000u; /* fopAcStts_UNK200000_e */
    } else {
        actor_status |= 0x200000u;
    }
}
VERIFY(0x0245B854, &daRd_c::setAttention);

/* 0245B9A8 */
void daRd_c::lookBack() {
    WWHD_FUNC(0x0245B9A8, void, this);
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    bool r29 = false;
    if (cLib_calcTimer(&mCE4) != 0 && mMode != MODE_ATTACK && mMode != MODE_CRY && mMode != MODE_DEATH) {
        mJntCtrl.mbTrn = 0;          /* clrTrn */
        mJntCtrl.mbHeadLock = 0;     /* offHeadLock */
        mJntCtrl.mbBackBoneLock = 0; /* offBackBoneLock */
        mTargetPos.copy(*daPy_getHeadTopPos(player));
    } else {
        switch ((u32)mMode) {
        case MODE_WAIT:
        case MODE_DEATH:
        case MODE_ATTACK:
            r29 = false;
            mJntCtrl.mbTrn = 0;
            mJntCtrl.mbHeadLock = 1;
            mJntCtrl.mbBackBoneLock = 1;
            break;
        case MODE_MOVE:
        case MODE_CRY:
        case MODE_RETURN:
        case MODE_SILENT_PRAY:
            mJntCtrl.mbTrn = 1; /* setTrn */
            /* fall through */
        default:
            mJntCtrl.mbHeadLock = 0;
            mJntCtrl.mbBackBoneLock = 0;
            break;
        }

        switch ((u32)mAnmPrmIdx) {
        case AnmPrm_SUWARIP:
        case AnmPrm_TATSU:
        case AnmPrm_SUWARU:
            r29 = false;
            mJntCtrl.mbTrn = 0;
            mJntCtrl.mbHeadLock = 1;
            mJntCtrl.mbBackBoneLock = 1;
            break;
        }

        switch ((u32)mMode) {
        case MODE_CRY:
            if (checkInFan(current.pos, player, shape_angle.y, hio_s(HIO_mCrySpreadAngle), 150.0f, 100.0f)) {
                mJntCtrl.mbTrn = 0;
                mJntCtrl.mbHeadLock = 1;
                mJntCtrl.mbBackBoneLock = 1;
            }
            mTargetPos.copy(*daPy_getHeadTopPos(player));
            break;
        case MODE_RETURN:
            if (checkInCircle(mSpawnPos, this, 100.0f, 1000.0f)) {
                mJntCtrl.mbTrn = 0;
                mJntCtrl.mbHeadLock = 1;
                mJntCtrl.mbBackBoneLock = 1;
            }
            mTargetPos.copy(mSpawnPos);
            break;
        case MODE_SILENT_PRAY:
            break;
        default:
            mTargetPos.copy(*daPy_getHeadTopPos(player));
            break;
        }
    }

    if (mJntCtrl.mbTrn != 0) { /* trnChk */
        cLib_addCalcAngleS2(&mMaxHeadTurnVel, hio_s(HIO_mMaxHeadTurnVel), 4, 0x800);
    } else {
        mMaxHeadTurnVel = 0;
    }

    lookAtTarget(&mJntCtrl, &shape_angle.y, &mTargetPos, mRdEyePos, shape_angle.y, mMaxHeadTurnVel, r29);
}
VERIFY(0x0245B9A8, &daRd_c::lookBack);

/* 0245BD98 */
void daRd_c::setCollision() {
    WWHD_FUNC(0x0245BD98, void, this);
    if (mMode == MODE_DEATH) {
        mCyl.OffCoSPrmBit(0x12); /* VsEnemy | IsEnemy */
        mCyl.OffTgSPrmBit(0x9);  /* Set | IsOther */
    } else if (mMode == MODE_ATTACK || mMode == MODE_CRY || dComIfGp_evmng_startCheckOld(STR(0x100392A4) /* "DEFAULT_RD_CRY" */)) {
        mCyl.OffCoSPrmBit(0x12);
    } else {
        mCyl.OnCoSPrmBit(0x12);
    }

    f32 r1 = REG_F(8, 1);
    bool suwari = isAnm(AnmPrm_SUWARIP);
    mCyl.SetR(r1 + 80.0f);
    f32 r0 = REG_F(8, 0);
    if (suwari) {
        mCyl.SetH(r0 + 170.0f);
    } else {
        mCyl.SetH(r0 + 250.0f);
    }

    mCyl.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mCyl);
}
VERIFY(0x0245BD98, &daRd_c::setCollision);

/* 0245BF50 */
bool daRd_c::_execute() {
    WWHD_FUNC(0x0245BF50, bool, this);
    if (mMode == MODE_SW_WAIT) {
        fopAcM_posMoveF(this, &mStts.m_cc_move);
        mAcch.CrrPos(dComIfG_Bgsp());
        setMtx();
        mpMorf->play(nullptr, 0, 0);
        mpMorf->calc();
        modeProc(PROC_EXEC_e, MODE_NULL);
        return true;
    }

    fopAcM_setGbaName(this, dItemNo_MIRROR_SHIELD_e, 0x12, 0x30);
    setIceCollision();
    if (mMode != MODE_SILENT_PRAY && mMode != MODE_DEATH && mMode != MODE_DAMAGE && mMode != MODE_ATTACK && mMode != MODE_CRY &&
        mMode != MODE_CRY_WAIT) {
        fopAc_ac_c* corpse = fopAcIt_Judge(0x0245A20C /* searchNeadDeadRd_CB */, this);
        if (corpse != nullptr) {
            mCorpseID = fopAcM_GetID(corpse);
            modeProcInit(MODE_SILENT_PRAY);
        }
    }
    if (mMode != MODE_ATTACK && mMode != MODE_WAIT && mMode != MODE_DEATH && mMode != MODE_RETURN) {
        if (dComIfGp_evmng_startCheckOld(STR(0x100392BC) /* "DEFAULT_RD_ATTACK" */)) {
            modeProcInit(MODE_RETURN);
        }
    }
    current.angle.y = shape_angle.y;
    current.angle.z = shape_angle.z;
    current.angle.x = shape_angle.x;

    if (mEnemyFire.mMode == 0) {
        mCyl.SetAtType(0);
        m6D4 = hio_s(HIO_m4E);
    } else {
        mCyl.SetAtType(AT_TYPE_FIRE);
        if (mMode != MODE_DEATH && mMode != MODE_CRY && mMode != MODE_ATTACK) {
            if (cLib_calcTimer(&m6D4) == 0) {
                modeProcInit(MODE_DEATH);
            }
        }
    }

    if (enemy_ice(&mEnemyIce)) {
        J3DModel* model = mpMorf->getModel();
        J3DModel_setBaseTRMtx(model, mtx_now());
        mpMorf->calc();
        speedF = 0.0f;
        setAttention();
        return true;
    }

    if (mCE0 == 1) {
        mCE4 = 4 * 30;
    }

    if (cLib_calcTimer(&mCE0) == 0) {
        cLib_addCalcAngleS2(&mD1E, 0, 0xC, 0x100);
        cLib_addCalcAngleS2(&mD2E, 0, 0xC, 0x100);
    } else {
        cLib_addCalcAngleS2(&mD1E, 0x2000, 0x4, 0x800);
        cLib_addCalcAngleS2(&mD2E, 0x500, 0x4, 0x200);
    }

    dComIfGp_get(); /* HD: a dead dComIfGp_get() call before the HIO reads */
    mJntCtrl.setParam(hio_s(HIO_mMaxBackboneX), hio_s(HIO_mMaxBackboneY), hio_s(HIO_mMinBackboneX), hio_s(HIO_mMinBackboneY),
                      hio_s(HIO_mMaxHeadX), hio_s(HIO_mMaxHeadY), hio_s(HIO_mMinHeadX), hio_s(HIO_mMinHeadY),
                      hio_s(HIO_mMaxTurnStep));

    if (mMode != MODE_PARALYSIS) {
        lookBack();
    }

    if (isIkari()) {
        cLib_addCalc2(&mD38, hio_f(HIO_m60), 0.1f, hio_f(HIO_m64));
    } else {
        cLib_addCalc2(&mD38, 0.0f, 0.1f, hio_f(HIO_m64));
    }

    if (isAnm(AnmPrm_WALK)) {
        f32 temp = speedF * hio_f(HIO_m70);
        f32 m74 = hio_f(HIO_m74);
        /* temp < m74 ? m74 : temp, as fsel (temp - m74 >= 0 ? temp : m74) */
        mpMorf->setPlaySpeed(temp - m74 >= 0.0f ? temp : m74);
        if (mMode == MODE_ATTACK) {
            mpMorf->setPlaySpeed(3.0f);
        }
    }

    setAttention();
    setCollision();
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    mAcch.CrrPos(dComIfG_Bgsp());
    setMtx();
    mBtkAnm.play();
    mDoExt_baseAnm_play(&mBrkAnm);
    mpMorf->play(&current.pos, 0, 0);
    mpMorf->calc();
    enemy_fire(&mEnemyFire);
    modeProc(PROC_EXEC_e, MODE_NULL);
    setAnm(AnmPrm_NULL, false);
    setBtkAnm(5);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);

    return false;
}
VERIFY(0x0245BF50, &daRd_c::_execute);

/* 0245C498 */
static BOOL daRdExecute(void* i_this) {
    WWHD_FUNC(0x0245C498, bool, i_this);
    return ((daRd_c*)i_this)->_execute();
}
VERIFY(0x0245C498, daRdExecute);

/* function-local static GXColor initialised on first use (guard, copy of 4 bytes from .rodata) */
static inline u32 rd_static_color(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        gabi::call(0xC000A848, dst, src, 4); /* memcpy (import, through the thunk 028FEAC0) */
    }
    return dst;
}

/* 0245C49C */
void daRd_c::debugDraw() {
    WWHD_FUNC(0x0245C49C, void, this);
    rd_static_color(0x101FDA48, 0x101FEBEC, 0x10038E78); /* the unused colour {0x00, 0xFF, 0x00, 0x80} */
    gabi::Local<cXyz> pos;
    f32 py = current.pos.y + 10.0f;
    pos->x = current.pos.x;
    pos->z = current.pos.z;
    pos->y = py;
    u32 yellow = rd_static_color(0x101FDA44, 0x101FEBE8, 0x10038E7C);
    dLib_debugDrawFan(pos, mHeadAngle, hio_s(HIO_mCrySpreadAngle), hio_f(HIO_mCryRadius), yellow);
    u32 red = rd_static_color(0x101FDA50, 0x101FEBF4, 0x10038E80);
    dLib_debugDrawFan(pos, shape_angle.y, hio_s(HIO_mAttackSpreadAngle), hio_f(HIO_mAttackRadius), red);
    u32 magenta = rd_static_color(0x101FDAC4, 0x101FEBFC, 0x10038E84);
    dLib_debugDrawFan(pos, shape_angle.y, hio_s(HIO_m40), hio_f(HIO_m34), magenta);
    dLib_debugDrawAxis(&mCE8, 50.0f);
}
VERIFY(0x0245C49C, &daRd_c::debugDraw);

/* 0245C620 */
bool daRd_c::_draw() {
    WWHD_FUNC(0x0245C620, bool, this);
    if (mMode == MODE_SW_WAIT) {
        return true;
    }

    if (gabi::load<u8>(L_HIO + HIO_m2C) != 0) {
        debugDraw();
    }

    J3DModel* model = mpMorf->getModel();
    J3DModelData* modelData = rd_modelData(model);
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);

    if (mEnemyIce.mFreezeTimer > 20) {
        dMat_control_iceEntryDL(mpMorf, -1, mInvisModel);
    } else {
        brkAnm_entry(&mBrkAnm, modelData, mBrkAnm.mFrameCtrl.getFrame());
        mBtkAnm.entry(modelData, mBtkAnm.getFrame());
        mpMorf->updateDL();
        /* HD: mBtkAnm.remove / mBrkAnm.remove inline: clear the model data's texture-SRT and tev-register animations */
        gabi::store<u32>(gabi::ea(modelData) + 0x44, 0);
        gabi::store<u32>(gabi::ea(modelData) + 0x48, 0);
    }

    /* HD: no blob shadow (dComIfGd_setShadow removed) */
    dSnap_RegistFig(DSNAP_TYPE_RD, this, 1.0f, 1.0f, 1.0f);
    return true;
}
VERIFY(0x0245C620, &daRd_c::_draw);

/* 0245C728 */
static BOOL daRdDraw(void* i_this) {
    WWHD_FUNC(0x0245C728, bool, i_this);
    return ((daRd_c*)i_this)->_draw();
}
VERIFY(0x0245C728, daRdDraw);

/* 0245C72C */
void daRd_c::modeWaitInit() {
    WWHD_FUNC(0x0245C72C, void, this);
    AcchCir_SetWallR(&mAcchCir, 60.0f);
    speedF = 0.0f;
}
VERIFY(0x0245C72C, &daRd_c::modeWaitInit);

/* 0245C770 */
bool daRd_c::checkTgHit() {
    WWHD_FUNC(0x0245C770, bool, this);
    if (mMode == MODE_DEATH || mMode == MODE_KANOKE || mMode == MODE_SW_WAIT) {
        return false;
    }

    void* hitObj;

    if (mCyl.ChkTgHit()) {
        hitObj = mCyl.GetTgHitObj();
        if (gabi::load<u32>(gabi::ea(hitObj) + 0x10) == AT_TYPE_LIGHT) { /* GetAtType */
            rd_seStart(this, JA_SE_CM_PW_BECOME_SOLID, 0);
            modeProcInit(MODE_PARALYSIS);
            return true;
        }
    }

    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    mStts.Move();
    if (cLib_calcTimer(&m2C4) == 0 && mCyl.ChkTgHit()) {
        bool r29 = true;
        hitObj = mCyl.GetTgHitObj();
        m2C4 = (u8)hio_s(HIO_m48);
        if (hitObj == nullptr) {
            return false;
        }

        switch (gabi::load<u32>(gabi::ea(hitObj) + 0x10)) {
        case AT_TYPE_SWORD:
        case AT_TYPE_MACHETE:
        case AT_TYPE_UNK800:
        case AT_TYPE_DARKNUT_SWORD:
        case AT_TYPE_MOBLIN_SPEAR:
            rd_seStart(this, JA_SE_LK_SW_HIT_S, 0x20);
            switch (daPy_getCutType(player)) {
            case 0x05: case 0x06: case 0x07: case 0x08: case 0x09: case 0x0A: /* jump cut, EA, EB, turn, roll, ... */
            case 0x0C: case 0x0E: case 0x0F: case 0x10: case 0x15: case 0x17: case 0x19: case 0x1A: case 0x1B:
                mHitType = 1;
                break;
            default:
                mHitType = 0;
                break;
            }
            break;
        case AT_TYPE_WIND:
            r29 = false;
            mHitType = 3;
            mCE0 = 40;
            break;
        case AT_TYPE_BOOMERANG:
        case AT_TYPE_BOKO_STICK:
            r29 = false;
            rd_seStart(this, JA_SE_LK_W_WEP_HIT, 0x44);
            mHitType = 4;
            mCE0 = 40;
            break;
        case AT_TYPE_HOOKSHOT:
            rd_seStart(this, JA_SE_LK_HS_SPIKE, 0x44);
            r29 = false;
            mHitType = 12;
            break;
        case AT_TYPE_SKULL_HAMMER:
        case AT_TYPE_STALFOS_MACE:
            rd_seStart(this, JA_SE_LK_HAMMER_HIT, 0x20);
            mHitType = 7;
            if (daPy_getCutType(player) == 0x11 /* CUT_TYPE_HAMMER_SIDESWING */) {
                mHitType = 8;
            }
            break;
        case AT_TYPE_BOMB:
            mHitType = 6;
            break;
        case AT_TYPE_ICE_ARROW:
            r29 = false;
            rd_seStart(this, JA_SE_LK_ARROW_HIT, 0x44);
            mEnemyIce.mFreezeDuration = hio_s(HIO_m4A);
            enemy_fire_remove(&mEnemyFire);
            health = health + 4;
            mHitType = 9;
            break;
        case AT_TYPE_FIRE_ARROW:
            r29 = false;
            rd_seStart(this, JA_SE_LK_ARROW_HIT, 0x44);
            mEnemyFire.mFireDuration = hio_s(HIO_m4C);
            mHitType = 0xA;
            break;
        case AT_TYPE_LIGHT_ARROW:
            r29 = false;
            rd_seStart(this, JA_SE_LK_ARROW_HIT, 0x20);
            mEnemyIce.mLightShrinkTimer = 1;
            mHitType = 0xB;
            break;
        case AT_TYPE_NORMAL_ARROW:
            mHitType = 5;
            if (!checkInCircle(current.pos, player, hio_f(HIO_mCryRadius), 1000.0f)) {
                rd_seStart(this, JA_SE_LK_ARROW_HIT, 0x44);
                mCE0 = 40;
                r29 = false;
            } else {
                rd_seStart(this, JA_SE_LK_ARROW_HIT, 0x20);
            }
            break;
        case AT_TYPE_FIRE:
        case AT_TYPE_UNK20000:
            r29 = false;
            if (mEnemyFire.mMode == 0) {
                mEnemyFire.mFireDuration = hio_s(HIO_m4C);
            } else {
                mHitType = 0xD;
            }
            break;
        case AT_TYPE_GRAPPLING_HOOK:
            dComIfGp_particle_set(0x27B /* dPa_name::ID_IT_JN_PIYOHIT00 */, attention_pos(this));
            rd_seStart(this, JA_SE_LK_W_WEP_HIT, 0x44);
            mHitType = 0xE;
            r29 = false;
            mCE0 = 40;
            break;
        }

        gabi::Local<CcAtInfo_l> atInfo;
        atInfo->pParticlePos = nullptr;
        atInfo->mpObj = gabi::ea(mCyl.GetTgHitObj());
        if (r29) {
            cXyz* hitPos = mCyl.GetTgHitPosP();
            cc_at_check(this, atInfo);
            /* HD: a stronger hit (resulting attack type 1) re-arms the hit timer at 3 while the player's
             * byte +0x69E8 is set */
            if (atInfo->mResultingAttackType == 1 && gabi::load<u8>(gabi::ea(player) + 0x69E8) != 0) {
                m2C4 = 3;
            }
            if (mHitType == 1 || mHitType == 7 || mHitType == 8 || health <= 0) {
                dComIfGp_particle_set(0x10 /* ID_AK_JN_CRITICALHITFLASH */, hitPos);
                gabi::Local<cXyz> scale;
                scale->x = 2.0f;
                scale->y = 2.0f;
                scale->z = 2.0f;
                dComIfGp_particle_set(0xF /* ID_AK_JN_CRITICALHIT */, hitPos, &player->shape_angle, scale);
                if (health <= 0) {
                    modeProcInit(MODE_DEATH);
                } else {
                    modeProcInit(MODE_DAMAGE);
                }
            } else {
                dComIfGp_particle_set(0xD /* ID_AK_JN_OK */, hitPos, &player->shape_angle);
                modeProcInit(MODE_DAMAGE);
            }
        } else if (mHitType == 0xE) {
            s8 origHealth = health;
            health = 0xA;
            cc_at_check(this, atInfo);
            health = origHealth;
        }

        return true;
    }

    if (dComIfGp_getDetect_chk_light(&current.pos)) {
        rd_seStart(this, JA_SE_CM_PW_BECOME_SOLID, 0);
        modeProcInit(MODE_PARALYSIS);
        return true;
    }

    return false;
}
VERIFY(0x0245C770, &daRd_c::checkTgHit);

/* 0245D088 */
bool daRd_c::isLinkControl() {
    WWHD_FUNC(0x0245D088, bool, this);
    fopAc_ac_c* p0 = dComIfGp_getPlayer(0);
    return p0 != dComIfGp_getLinkPlayer();
}
VERIFY(0x0245D088, &daRd_c::isLinkControl);

/* 0245D0C8 */
bool daRd_c::checkPlayerInAttack() {
    WWHD_FUNC(0x0245D0C8, bool, this);
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    return checkInFan(current.pos, player, shape_angle.y, hio_s(HIO_mAttackSpreadAngle), hio_f(HIO_mAttackRadius), 100.0f);
}
VERIFY(0x0245D0C8, &daRd_c::checkPlayerInAttack);

/* the wake-up test shared by modeWait and modeReturn */
static inline bool rd_noticePlayer(daRd_c* i_this, fopAc_ac_c* player, BOOL isOto) {
    if (checkInCircle(i_this->mSpawnPos, i_this, (f32)i_this->mAreaRadius, 1000.0f)) {
        /* Bug (GameCube too): the 200-unit-tall cylinder is around the spawn point */
        if (checkInCircle(i_this->mSpawnPos, player, (f32)i_this->mAreaRadius, 100.0f)) {
            if ((checkInCircle(i_this->mSpawnPos, player, (f32)i_this->mAreaRadius, 100.0f) &&
                 player->speedF > REG_F(12, 4) + 10.0f) ||
                isOto ||
                checkInFan(i_this->current.pos, player, i_this->shape_angle.y, hio_s(HIO_m40), hio_f(HIO_m34), 100.0f)) {
                return true;
            }
        }
    }
    return false;
}

/* 0245D134 */
void daRd_c::modeWait() {
    WWHD_FUNC(0x0245D134, void, this);
    if (checkTgHit()) {
        return;
    }

    speedF = 0.0f;
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    BOOL isOto = fopAcM_otoCheck(this, (f32)mAreaRadius);
    if (!isLinkControl()) {
        if (rd_noticePlayer(this, player, isOto)) {
            modeProcInit(MODE_MOVE);
            return;
        }
    }

    if (checkInCircle(mSpawnPos, this, 100.0f, 1000.0f)) {
        cLib_addCalcAngleS2(&shape_angle.y, mSpawnAngle, 0x4, 0x200);
        if (cLib_distanceAngleS(shape_angle.y, mSpawnAngle) <= 0x200) {
            shape_angle.y = mSpawnAngle;
            if (mWhichIdleAnm == 1 && !isAnm(AnmPrm_SUWARIP)) {
                setAnm(AnmPrm_SUWARU, false);
            }
            if (mWhichIdleAnm == 0) {
                setAnm(AnmPrm_TACHIP1, false);
            }
        }
    } else if (!checkInCircle(mSpawnPos, this, (f32)mAreaRadius, 1000.0f) ||
               !checkInCircle(mSpawnPos, player, (f32)mAreaRadius, 100.0f)) {
        modeProcInit(MODE_RETURN);
        return;
    }
    if (!isLinkControl() && checkPlayerInAttack() && isAnm(AnmPrm_TACHIP1)) {
        modeProcInit(MODE_ATTACK);
    }
}
VERIFY(0x0245D134, &daRd_c::modeWait);

/* 0245D558 */
void daRd_c::modeDamageInit() {
    WWHD_FUNC(0x0245D558, void, this);
    rd_monsSeStart(this, JA_SE_CV_RD_DAMAGE, 0);
    setAnm(AnmPrm_DAMAGE, true);
    speedF = 0.0f;
}
VERIFY(0x0245D558, &daRd_c::modeDamageInit);

static inline bool morf_isStop(mDoExt_McaMorf* m) { return m->isStop(); }

/* 0245D5C0 */
void daRd_c::modeDamage() {
    WWHD_FUNC(0x0245D5C0, void, this);
    if (checkTgHit()) {
        return;
    }
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    if (!morf_isStop(mpMorf)) {
        return;
    }

    if (checkInCircle(mSpawnPos, player, (f32)mAreaRadius, 100.0f)) {
        modeProcInit(MODE_MOVE);
    } else {
        modeProcInit(MODE_WAIT);
    }
}
VERIFY(0x0245D5C0, &daRd_c::modeDamage);

/* 0245D698 */
void daRd_c::modeParalysisInit() {
    WWHD_FUNC(0x0245D698, void, this);
    if (!isAnm(AnmPrm_BEAM_HIT) && !isAnm(AnmPrm_BEAM)) {
        setAnm(AnmPrm_BEAM_HIT, false);
    }
    mTimer1 = hio_s(HIO_mParalysisDuration);
    speedF = 0.0f;
}
VERIFY(0x0245D698, &daRd_c::modeParalysisInit);

/* 0245D700 */
void daRd_c::modeParalysis() {
    WWHD_FUNC(0x0245D700, void, this);
    if (isAnm(AnmPrm_BEAM_HIT) && morf_isStop(mpMorf)) {
        setAnm(AnmPrm_BEAM, false);
    } else if (isAnm(AnmPrm_BEAM)) {
        if (cLib_calcTimer(&mTimer1) == 0) {
            setAnm(AnmPrm_BEAM_END, false);
        }
    } else if (isAnm(AnmPrm_BEAM_END) && morf_isStop(mpMorf)) {
        modeProcInit(MODE_WAIT);
    }

    checkTgHit();
}
VERIFY(0x0245D700, &daRd_c::modeParalysis);

/* 0245D7E8 */
void daRd_c::modeDeathInit() {
    WWHD_FUNC(0x0245D7E8, void, this);
    rd_monsSeStart(this, JA_SE_CV_RD_DIE, 0);
    setAnm(AnmPrm_DEAD, false);
    speedF = 0.0f;
    /* HD: the body stays for a random 50..150 frames (GameCube: 10 seconds) */
    mTimer1 = gabi::ftoi(cM_rndF(100.0f) + 50.0f);
}
VERIFY(0x0245D7E8, &daRd_c::modeDeathInit);

/* 0245D87C */
void daRd_c::modeDeath() {
    WWHD_FUNC(0x0245D87C, void, this);
    if (!morf_isStop(mpMorf)) {
        return;
    }

    dComIfGs_onActor(setID, home.roomNo); /* fopAcM_onActor */

    /* Do not consider the ReDead to be a living enemy while its death animation is playing out. */
    group = 3; /* fopAcM_SetGroup(this, fopAc_ENV_e) */

    if (cLib_calcTimer(&mTimer1) == 0) {
        /* HD: the disappear effect 20 units higher and size 8 (GameCube: at the position, size 5); same drop type 0 (daDisItem_IBALL_e) */
        gabi::Local<cXyz> pos;
        f32 y = current.pos.y + 20.0f;
        pos->x = current.pos.x;
        pos->z = current.pos.z;
        pos->y = y;
        fopAcM_createDisappear(this, pos, 8, 0, 0xFF);
        fopAcM_delete(this);
    }
}
VERIFY(0x0245D87C, &daRd_c::modeDeath);

/* 0245D940 */
void daRd_c::modeMoveInit() {
    WWHD_FUNC(0x0245D940, void, this);
    if (mWhichIdleAnm == 1 && (isAnm(AnmPrm_SUWARIP) || isAnm(AnmPrm_SUWARU))) {
        setAnm(AnmPrm_TATSU, false);
    }
}
VERIFY(0x0245D940, &daRd_c::modeMoveInit);

/* 0245D970 */
bool daRd_c::checkPlayerInCry() {
    WWHD_FUNC(0x0245D970, bool, this);
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    return checkInFan(current.pos, player, mHeadAngle, hio_s(HIO_mCrySpreadAngle), hio_f(HIO_mCryRadius), 100.0f);
}
VERIFY(0x0245D970, &daRd_c::checkPlayerInCry);

/* the walk speed toward the player (modeMove, modeCry) */
static inline void rd_walkToPlayer(daRd_c* i_this, f32 dist) {
    f32 temp = (dist / 100.0f) + REG_F(12, 3);
    if (temp > 1.0f) {
        temp = 1.0f;
    }
    if (temp < 0.1f) {
        temp = 0.1f;
    }
    f32 temp2 = hio_f(HIO_m68);
    if (i_this->mEnemyFire.mMode != 0) {
        temp2 = temp2 + temp2;
    }
    cLib_addCalc2(&i_this->speedF, temp2 * temp, 0.1f, REG_F(12, 0) + 0.1f);
}

/* 0245D9DC */
void daRd_c::modeMove() {
    WWHD_FUNC(0x0245D9DC, void, this);
    if (!checkTgHit() && !isAnm(AnmPrm_TATSU)) {
        if (!isLinkControl() && checkPlayerInCry()) {
            modeProcInit(MODE_CRY);
            return;
        }
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        if (!checkInCircle(mSpawnPos, this, (f32)mAreaRadius, 1000.0f) ||
            !checkInCircle(mSpawnPos, player, (f32)mAreaRadius, 100.0f)) {
            modeProcInit(MODE_RETURN);
            return;
        }
        int angle = cLib_distanceAngleS(fopAcM_searchPlayerAngleY(this), mHeadAngle);
        if (angle < 0x50) {
            rd_walkToPlayer(this, fopAcM_searchPlayerDistanceXZ(this));
        } else {
            cLib_addCalc2(&speedF, 0.0f, 0.1f, REG_F(12, 0) + 0.1f);
        }
        if (speedF < 0.1f) {
            setAnm(AnmPrm_TACHIP1, false);
        } else {
            setAnm(AnmPrm_WALK, false);
        }
        if (!isLinkControl() && checkPlayerInAttack()) {
            modeProcInit(MODE_ATTACK);
        }
    }
}
VERIFY(0x0245D9DC, &daRd_c::modeMove);

/* 0245DC9C */
void daRd_c::modeCryInit() {
    WWHD_FUNC(0x0245DC9C, void, this);
    if (isLinkControl()) {
        modeProcInit(MODE_RETURN);
        return;
    }
    if (dComIfGp_evmng_startCheckOld(STR(0x100392D8) /* "DEFAULT_RD_CRY" */)) {
        dComIfGp_event_reset();
    }
    fopAcM_orderOtherEvent2(this, STR(0x100392D8), 1, 0xFFFF);
    rd_monsSeStart(this, JA_SE_CV_RD_SCREAM, 0);
    mTimer1 = hio_s(HIO_m54);
    mBreakFreeCounter = hio_s(HIO_m78);
}
VERIFY(0x0245DC9C, &daRd_c::modeCryInit);

/* mashing the stick and buttons to break free (modeCry, modeAttack) */
static inline void rd_breakFree(daRd_c* i_this, f32 stickPosX, f32 stickPosY) {
    if (i_this->mD3C > 0 && stickPosX < 0.0f) {
        i_this->mD3C = -1;
        i_this->mBreakFreeCounter = i_this->mBreakFreeCounter - 1;
    } else if (i_this->mD3C < 0 && stickPosX > 0.0f) {
        i_this->mD3C = 1;
        i_this->mBreakFreeCounter = i_this->mBreakFreeCounter - 1;
    }

    if (i_this->mD40 > 0 && stickPosY < 0.0f) {
        i_this->mD40 = -1;
        i_this->mBreakFreeCounter = i_this->mBreakFreeCounter - 1;
    } else if (i_this->mD40 < 0 && stickPosY > 0.0f) {
        i_this->mD40 = 1;
        i_this->mBreakFreeCounter = i_this->mBreakFreeCounter - 1;
    }

    if (CPad_CHECK_TRIG_A(0) || CPad_CHECK_TRIG_B(0) || CPad_CHECK_TRIG_X(0) || CPad_CHECK_TRIG_Y(0)) {
        i_this->mBreakFreeCounter = i_this->mBreakFreeCounter - 1;
    }
}

/* 0245DD80 */
void daRd_c::modeCry() {
    WWHD_FUNC(0x0245DD80, void, this);
    setAnm(AnmPrm_WALK, false);

    f32 stickPosX = CPad_GET_STICK_POS_X(0);
    f32 stickPosY = CPad_GET_STICK_POS_Y(0);
    if (eventInfo_checkCommandDemoAccrpt(this) || dComIfGp_evmng_startCheckOld(STR(0x100392E8) /* "DEFAULT_RD_CRY" */)) {
        if (isLinkControl()) {
            dComIfGp_event_reset();
            modeProcInit(MODE_RETURN);
            return;
        }
        bool hit = checkTgHit();
        u32 play = dComIfGp_ea(); /* HD: fetched once for both branches */
        if (hit) {
            event_reset_at(play);
        } else {
            fopAcM_searchActorAngleY(this, player0_at(play)); /* s16 targetY (unused) */
            rd_walkToPlayer(this, fopAcM_searchPlayerDistanceXZ(this));

            rd_breakFree(this, stickPosX, stickPosY);

            if (cLib_calcTimer(&mTimer1) == 0 || mBreakFreeCounter < 0) {
                dComIfGp_event_reset();
                modeProcInit(MODE_CRY_WAIT);
            } else if (checkPlayerInAttack()) {
                dComIfGp_event_reset();
                modeProcInit(MODE_ATTACK);
            }
        }
    } else {
        modeProcInit(MODE_CRY_WAIT);
    }
}
VERIFY(0x0245DD80, &daRd_c::modeCry);

/* 0245E144 */
void daRd_c::modeCryWaitInit() {
    WWHD_FUNC(0x0245E144, void, this);
    mTimer1 = 60;
    mTimer2 = 45;
}
VERIFY(0x0245E144, &daRd_c::modeCryWaitInit);

/* 0245E158 */
void daRd_c::modeCryWait() {
    WWHD_FUNC(0x0245E158, void, this);
    if (checkTgHit()) {
        return;
    }
    if (isLinkControl()) {
        modeProcInit(MODE_RETURN);
        return;
    }
    if (dComIfGp_evmng_startCheckOld(STR(0x100392F8) /* "DEFAULT_RD_CRY" */)) {
        onIkari();
        setBtkAnm(3);
    }
    if (dComIfGp_event_runCheck()) {
        mTimer1 = 60;
        mTimer2 = 45;
    } else if (isAnm(AnmPrm_WALK)) {
        if (cLib_calcTimer(&mTimer1) == 0) {
            modeProcInit(MODE_MOVE);
        } else if (cLib_calcTimer(&mTimer2) == 0 && checkPlayerInAttack()) {
            modeProcInit(MODE_ATTACK);
        }
    }
}
VERIFY(0x0245E158, &daRd_c::modeCryWait);

/* 0245E298 */
void daRd_c::modeAttackInit() {
    WWHD_FUNC(0x0245E298, void, this);
    AcchCir_SetWallR(&mAcchCir, 70.0f);
    mTimer1 = hio_s(HIO_m52);
    mBreakFreeCounter = hio_s(HIO_m50);
    mTimer2 = 30;
    setAnm(AnmPrm_WALK, false);
    speedF = 0.0f;
}
VERIFY(0x0245E298, &daRd_c::modeAttackInit);

/* 0245E30C */
void daRd_c::modeAttack() {
    WWHD_FUNC(0x0245E30C, void, this);
    if (dComIfGp_evmng_startCheckOld(STR(0x10039328) /* "DEFAULT_RD_CRY" */)) {
        dComIfGp_event_reset();
    }
    if ((dComIfGp_evmng_startCheckOld(STR(0x10039328)) || dComIfGp_evmng_startCheckOld(STR(0x10039314) /* "DEFAULT_RD_ATTACK" */)) &&
        isLinkControl()) {
        dComIfGp_event_reset();
        modeProcInit(MODE_RETURN);
        return;
    }

    f32 stickPosX = CPad_GET_STICK_POS_X(0);
    f32 stickPosY = CPad_GET_STICK_POS_Y(0);
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        fopAc_ac_c* player = dComIfGp_getLinkPlayer();
        if (isAnm(AnmPrm_ATACK)) {
            mDoExt_McaMorf* morf = mpMorf;
            f32 frame = daPy_getBaseAnimeFrame(player);
            morf->setFrame(frame);
            cLib_addCalcAngleS2(&shape_angle.y, player->shape_angle.y, 0x4, 0x2000);
        } else {
            s16 targetY = fopAcM_searchPlayerAngleY(this);
            cLib_addCalcAngleS2(&shape_angle.y, targetY, 0x4, 0x2000);
        }

        bool hit = checkTgHit();
        u32 play = dComIfGp_ea(); /* HD: fetched once for both branches */
        if (hit) {
            event_reset_at(play);
            return;
        }
        f32 dist = fopAcM_searchActorDistanceXZ(this, player0_at(play));
        /* HD branches: written as the original tests them (not greater than) */
        if (!(dist > REG_F(12, 5) + 50.0f) && !isAnm(AnmPrm_ATACK)) {
            offIkari();
            setAnm(AnmPrm_WALK2ATACK, false);
        }
        if (!(dist > REG_F(12, 2) + 20.0f)) {
            cLib_addCalcPosXZ2(&current.pos, &player->current.pos, 0.3f, 1.0f);
            if (cLib_calcTimer(&mTimer2) == 0) {
                bool noDamage = daPy_checkNoDamageMode(dComIfGp_getLinkPlayer());
                fopAc_ac_c* link = dComIfGp_getLinkPlayer();
                if (!noDamage) {
                    daPy_lk_setDamagePoint(link, -1.0f);
                } else {
                    daPy_lk_noDamagePoint(link, -1.0f); /* HD */
                }
                mTimer2 = 30;
            }
        } else if (mAcch.ChkWallHit()) {
            daPy_changeOriginalDemo(player);
            gabi::Local<cXyz> pos;
            pos->copy(player->current.pos);
            cLib_addCalcPosXZ2(pos, &current.pos, 0.3f, 10.0f);
            daPy_setPlayerPosAndAngle(player, pos, player->shape_angle.y);
        } else {
            cLib_addCalcPosXZ2(&current.pos, &player->current.pos, 0.3f, 10.0f);
        }

        if (dComIfGs_getLife() == 0) {
            dComIfGp_event_reset();
            setAnm(AnmPrm_ATACK2WALK, false);
            modeProcInit(MODE_WAIT);
            return;
        }

        rd_breakFree(this, stickPosX, stickPosY);

        if (cLib_calcTimer(&mTimer1) == 0 || mBreakFreeCounter < 0) {
            dComIfGp_event_reset();
            setAnm(AnmPrm_ATACK2WALK, false);
            modeProcInit(MODE_CRY_WAIT);
        }
    } else if (!checkTgHit()) {
        fopAcM_orderOtherEvent2(this, STR(0x10039314) /* "DEFAULT_RD_ATTACK" */, 1, 0xFF6F); /* GameCube: 0x1CF */
    }
}
VERIFY(0x0245E30C, &daRd_c::modeAttack);

/* 0245F250 */
void daRd_c::modeReturnInit() {
    WWHD_FUNC(0x0245F250, void, this);
}
VERIFY(0x0245F250, &daRd_c::modeReturnInit);

/* 0245E964 */
void daRd_c::modeReturn() {
    WWHD_FUNC(0x0245E964, void, this);
    if (checkTgHit()) {
        return;
    }

    setAnm(AnmPrm_WALK, false);

    if (checkInCircle(mSpawnPos, this, 100.0f, 1000.0f)) {
        speedF = 0.0f;
        cLib_addCalcAngleS2(&shape_angle.y, mSpawnAngle, 0xA, 0x200);
        if (cLib_distanceAngleS(shape_angle.y, mSpawnAngle) <= 0x200) {
            shape_angle.y = mSpawnAngle;
            modeProcInit(MODE_WAIT);
            return;
        }
    } else {
        cLib_addCalc2(&speedF, hio_f(HIO_mReturnWalkSpeed), 0.1f, REG_F(12, 0) + 0.1f);
    }

    if (dComIfGp_evmng_startCheckOld(STR(0x10039338) /* "DEFAULT_RD_ATTACK" */)) {
        return;
    }

    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    BOOL isOto = fopAcM_otoCheck(this, (f32)mAreaRadius);
    if (!isLinkControl()) {
        if (rd_noticePlayer(this, player, isOto)) {
            modeProcInit(MODE_MOVE);
            return;
        }
    }
}
VERIFY(0x0245E964, &daRd_c::modeReturn);

/* 0245F254 */
void daRd_c::modeSilentPrayInit() {
    WWHD_FUNC(0x0245F254, void, this);
}
VERIFY(0x0245F254, &daRd_c::modeSilentPrayInit);

/* 0245EC74 */
void daRd_c::modeSilentPray() {
    WWHD_FUNC(0x0245EC74, void, this);
    if (checkTgHit()) {
        return;
    }

    setAnm(AnmPrm_WALK, false);

    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    gabi::Local<gptr<fopAc_ac_c>> corpse;
    if (fopAcM_SearchByID_out(mCorpseID, corpse)) {
        mTargetPos.copy((*corpse)->current.pos);
        if (checkInCircle(mTargetPos, this, 200.0f, 1000.0f)) {
            setAnm(AnmPrm_TACHIP1, false);
            speedF = 0.0f;
        } else {
            cLib_addCalc2(&speedF, hio_f(HIO_mReturnWalkSpeed), 0.1f, REG_F(12, 0) + 0.1f);
        }
    } else if (!isLinkControl()) {
        if (checkInCircle(mSpawnPos, player, (f32)mAreaRadius, 100.0f)) {
            modeProcInit(MODE_MOVE);
        } else {
            modeProcInit(MODE_WAIT);
        }
    } else {
        modeProcInit(MODE_RETURN);
    }

    if (!isLinkControl()) {
        if (checkPlayerInCry()) {
            modeProcInit(MODE_CRY);
        }
        if (checkPlayerInAttack()) {
            modeProcInit(MODE_ATTACK);
        }
    }
}
VERIFY(0x0245EC74, &daRd_c::modeSilentPray);

/* 0245EE9C */
void daRd_c::modeSwWaitInit() {
    WWHD_FUNC(0x0245EE9C, void, this);
    setAnm(AnmPrm_KANOKEP, false);
}
VERIFY(0x0245EE9C, &daRd_c::modeSwWaitInit);

/* 0245EEA8 */
void daRd_c::modeSwWait() {
    WWHD_FUNC(0x0245EEA8, void, this);
    if (dComIfGs_isSwitch(mSwNo, fopAcM_GetRoomNo(this))) {
        modeProcInit(MODE_KANOKE);
    }
}
VERIFY(0x0245EEA8, &daRd_c::modeSwWait);

/* 0245EF04 */
void daRd_c::modeKanokeInit() {
    WWHD_FUNC(0x0245EF04, void, this);
    setAnm(AnmPrm_KANOKEP, false);
    mTimer1 = 90;
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = 150.0f;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    PSMTXMultVec(mtx_now(), offset, &mSpawnPos);
}
VERIFY(0x0245EF04, &daRd_c::modeKanokeInit);

/* 0245EF98 */
void daRd_c::modeKanoke() {
    WWHD_FUNC(0x0245EF98, void, this);
    if (cLib_calcTimer(&mTimer1) == 0) {
        modeProcInit(MODE_RETURN);
    }
}
VERIFY(0x0245EF98, &daRd_c::modeKanoke);

/* 0245EFE0 */
static void* daRd_HIO_c_ct(void* p) {
    WWHD_FUNC(0x0245EFE0, void*, p);
    if (p == nullptr) {
        p = operator_new(0xA8);
        if (p == nullptr) return p;
    }
    u32 h = gabi::ea(p);
    gabi::store<u32>(h, 0x10038EF4); /* vtable */
    gabi::call(0x0259DA18, h + 4);    /* dNpc_HIO_c::dNpc_HIO_c */
    gabi::call(0x02552BE8, h + 0x7C); /* JntHit_HIO_c::JntHit_HIO_c */
    gabi::store<u8>(h + HIO_m2C, 0);
    gabi::store<f32>(h + HIO_m30, 650.0f);
    gabi::store<f32>(h + HIO_m34, 650.0f);
    gabi::store<f32>(h + HIO_mCryRadius, 500.0f);
    gabi::store<f32>(h + HIO_mAttackRadius, 125.0f);
    gabi::store<s16>(h + HIO_m40, 0x2000);
    gabi::store<s16>(h + HIO_mCrySpreadAngle, 0x1B58);
    gabi::store<s16>(h + HIO_mAttackSpreadAngle, 0x6000);
    gabi::store<s16>(h + HIO_m46, 0xA);
    gabi::store<s16>(h + HIO_m48, 0x5);
    gabi::store<s16>(h + HIO_m4A, 0x3C);
    gabi::store<s16>(h + HIO_m4C, 0x32);
    gabi::store<s16>(h + HIO_m4E, 0x384);
    gabi::store<f32>(h + HIO_m58, 10.0f);
    gabi::store<f32>(h + HIO_m5C, 10.0f);
    gabi::store<f32>(h + HIO_m60, 50.0f);
    gabi::store<f32>(h + HIO_m64, 30.0f);
    gabi::store<f32>(h + HIO_m68, 1.8f);
    gabi::store<f32>(h + HIO_mReturnWalkSpeed, 2.0f);
    gabi::store<f32>(h + HIO_m70, 1.25f);
    gabi::store<f32>(h + HIO_m74, 0.9f);
    gabi::store<s16>(h + HIO_m50, 0x28);
    gabi::store<s16>(h + HIO_m78, 0x2D);
    gabi::store<s16>(h + HIO_m54, 0x87);
    gabi::store<s16>(h + HIO_m52, 0x96);
    gabi::store<s16>(h + HIO_mParalysisDuration, 2 * 30);
    gabi::store<f32>(h + 0x04, -20.0f); /* mNpc.m04 */
    gabi::store<s16>(h + HIO_mMaxHeadX, 0x1FFE);
    gabi::store<s16>(h + HIO_mMaxHeadY, 0x4000);
    gabi::store<s16>(h + HIO_mMaxBackboneX, 0x0);
    gabi::store<s16>(h + HIO_mMaxBackboneY, 0x2000);
    gabi::store<s16>(h + HIO_mMinHeadX, -0x9C4);
    gabi::store<s16>(h + HIO_mMinHeadY, -0x4000);
    gabi::store<s16>(h + HIO_mMinBackboneX, 0x0);
    gabi::store<s16>(h + HIO_mMinBackboneY, -0x2000);
    gabi::store<s16>(h + HIO_mMaxTurnStep, 0x250);
    gabi::store<s16>(h + HIO_mMaxHeadTurnVel, 0x150);
    gabi::store<f32>(h + 0x1C, 50.0f);  /* mNpc.mAttnYOffset */
    gabi::store<s16>(h + 0x20, 0x7FFF); /* mNpc.mMaxAttnAngleY */
    gabi::store<u8>(h + 0x22, 0);       /* mNpc.m22 */
    gabi::store<f32>(h + 0x24, 400.0f); /* mNpc.mMaxAttnDistXZ */
    return p;
}
VERIFY(0x0245EFE0, daRd_HIO_c_ct);

/* 0245F194: static initialisation (header statics, then l_HIO) */
static void __sinit_d_a_rd_cpp() {
    WWHD_FUNC(0x0245F194, void, (u32)0);
    sinit_header_statics(0x1046D66C, 0x101CF634);
    daRd_HIO_c_ct(gabi::at<void>(L_HIO));
}
VERIFY(0x0245F194, __sinit_d_a_rd_cpp);

/* 0245F234 sead::SafeString deleting destructor (this TU's vtable 0x10038E8C, slot 0xC) */
static void rd_SafeString_dtor(void* p, s32 flags) {
    WWHD_FUNC(0x0245F234, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0245F234, rd_SafeString_dtor);

/* 0245F248 */
static BOOL daRdIsDelete(void* i_this) {
    WWHD_FUNC(0x0245F248, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0245F248, daRdIsDelete);

/* dBgS_ObjAcch destructor (inline): this TU's vtables back, then dBgS_Acch::~dBgS_Acch */
static inline void rd_objacch_dt(dBgS_ObjAcch* a) {
    gabi::store<u32>(gabi::ea(a) + 0x20, 0x10038EC4);
    gabi::store<u32>(gabi::ea(a) + 0x14, 0x10038ED4);
    gabi::call(0x024EFD9C, a, 0);
}

/* 0245F258: daRd_c deleting destructor (compiler-generated; HD virtual destructor) */
static void daRd_c_dt(daRd_c* self, s32 flags) {
    WWHD_FUNC(0x0245F258, void, self, flags);
    if (self == nullptr) return;
    gabi::call(0x02515AE8, &self->mEnemyFire.mSph, 2); /* dCcD_Sph::~dCcD_Sph */
    dCcD_Stts_dt(&self->mEnemyFire.mStts, 2);
    rd_objacch_dt(&self->mEnemyIce.mBgAcch);
    gabi::call(0x02018034, gabi::ea(&self->mEnemyIce.mBgAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    gabi::call(0x025E89F8, self->mInvisModel, 2); /* mDoExt_invisibleModel::~mDoExt_invisibleModel */
    dCcD_Cyl_dt(&self->mCyl, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    gabi::call(0x02018034, gabi::ea(&self->mAcchCir) + 0x14, 2);
    rd_objacch_dt(&self->mAcch);
    gabi::call(0x025D50BC, self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}
VERIFY(0x0245F258, daRd_c_dt);

/* 0245F360: empty virtual (sead::SafeString::assureTerminationImpl_, this TU's vtable + 0x14) */
static void rd_SafeString_assureTermination(void*) {
    WWHD_FUNC(0x0245F360, void, (u32)0);
}
VERIFY(0x0245F360, rd_SafeString_assureTermination);
