/**
 * d_a_gy.cpp (WWHD)
 * Enemy - Gyorg
 *
 * The GameCube decompilation of this unit has only
 * "Nonmatching" stubs (zeldaret/tww src/d/actor/d_a_gy.cpp): every function here is written
 * from the WWHD code (cking.rpx) and verified against it. GameCube function names are kept.
 */
#include "d/actor/d_a_gy.h"
#include <cmath>

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* play+0x5B3C: mpPlayerPtr[2] (GameCube dComIfGp_getShipActor) */
static inline fopAc_ac_c* dComIfGp_getShipActor_gy() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B3C)); }
/* 0259138C mDoExt_invisibleModel::entryDL (the matcher names it dMat_ice_c::entryDL; as in d_a_oq) */
static inline void mDoExt_invisibleModel_entryDL_gy(mDoExt_McaMorf* morf, s32 p, void* inv) { gabi::call(0x0259138C, morf, p, inv); }
static inline BOOL enemy_ice_gy(enemyice_l* e) { return gabi::call<BOOL>(0x020402C8, e); }
/* 0211D2F8 cLib_calcTimer<int> */
static inline s32 cLib_calcTimer_gy(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); }
static inline s32 dBgS_GetPolyColor_gy(dBgS* bgs, void* p) { return gabi::call<s32>(0x024EEEB8, bgs, p); }
/* 025E1FD8 mDoAud_onEnemyDamage() (as in d_a_am) */
static inline void mDoAud_onEnemyDamage_gy() { gabi::call(0x025E1FD8); }
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb) */
static inline void mDoAud_monsSeStart_gy(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb) { gabi::call(0x025E1AA4, id, pos, pid, param, reverb); }
/* 02520C0C dComIfGs_checkGetItem(u8) (as in d_a_warpf) */
static inline BOOL dComIfGs_checkGetItem_gy(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
/* 025D54C4 fopAcM_SearchByID(id, fopAc_ac_c** out) (out of line) */
static inline BOOL fopAcM_SearchByID_gy(fpc_ProcID id, be<u32>* out) { return gabi::call<BOOL>(0x025D54C4, id, out); }
static inline s32 dComIfGp_CharTbl_GetNameIndex_gy(const char* name, s32 n) {
    return gabi::call<s32>(0x0200E814, gabi::at<u8>(dComIfGp_ea() + PLAY_NAMETBL), name, n);
}
struct Quaternion_gy {
    be<f32> x, y, z, w;
};
static inline void PSMTXConcat_gy(const Mtx34* a, const Mtx34* b, Mtx34* ab) { gabi::call(0x028E9108, a, b, ab); }
static inline void mDoMtx_stack_quatM_gy(const Quaternion_gy* q) { gabi::call(0x025F25CC, q); }
/* 028E9BC0 C_QUATSlerp(p, q, r, t) */
static inline void C_QUATSlerp_gy(const Quaternion_gy* p, const Quaternion_gy* q, Quaternion_gy* r, f32 t) { gabi::call(0x028E9BC0, p, q, r, t); }
/* 023127F8 daObj::quat_rotVec(Quaternion*, const cXyz&, const cXyz&) */
static inline void daObj_quat_rotVec_gy(Quaternion_gy* q, const cXyz* a, const cXyz* b) { gabi::call(0x023127F8, q, a, b); }
static inline dSv_event_c* dComIfGs_getEvent_gy() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
/* 025E8A48 mDoExt_invisibleModel::create(J3DModel*) */
static inline BOOL mDoExt_invisibleModel_create(void* p, J3DModel* m) { return gabi::call<BOOL>(0x025E8A48, p, m); }
/* 027F596C (HD): passes a J3DModel's new flags (+0x74) on to its material packets */
static inline void J3DModel_setFlags_gy(J3DModel* m, u32 mask) {
    u32 p = gabi::ea(m);
    u32 f = gabi::load<u32>(p + 0x74) & mask;
    gabi::store<u32>(p + 0x74, f);
    gabi::call(0x027F596C, m, f);
}
/* 02552B60 JntHit_create(J3DModel*, __jnt_hit_data_c*, s16) */
static inline u32 JntHit_create(J3DModel* m, u32 data, s16 n) { return gabi::call<u32>(0x02552B60, m, data, n); }
/* 027F3F94 (the matcher names it __nw): the model data's joint tree; joint count (u16) at +8 (as in d_a_kb) */
static inline u16 J3DModelData_getJointNum_gy(J3DModelData* d) { return gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, d)) + 8); }
/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index checked against +4) */
static inline void setJointCallBack_gy(J3DModelData* d, u16 i, u32 cb) {
    u32 data = gabi::ea(d);
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
/* J3D (HD, as in d_a_kb): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint block at
 * model+0x2C (flags +4, matrices +0x10), user area +0xB8 */
struct J3DMtxBlock_gy {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_gy> mpMtxBlock;
    /* 0x30 */ u8 _30[0xB8 - 0x30];
    /* 0xB8 */ be<u32> mUserArea;
};
static inline Mtx34* J3DSys_mCurrentMtx_gy() { return gabi::at<Mtx34>(0x104B4868); }
static inline J3DModel_l* j3dSys_getModel_gy() { return gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C)); }
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty) */
static inline Mtx34* getAnmMtx_gy(J3DModel* m, s32 jnt) {
    J3DMtxBlock_gy* blk = ((J3DModel_l*)m)->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}

/* a float moved by lfs/stfs without arithmetic is quieted on load: be<f32> models that */

/* 02166FC0: createHeap_CB */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x02166FC0, BOOL, i_this);
    return ((daGy_c*)i_this)->_createHeap();
}
VERIFY(0x02166FC0, createHeap_CB);

/* 02166CC0 */
static BOOL nodeControl_CB(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x02166CC0, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel_l* model = j3dSys_getModel_gy();
        daGy_c* i_this = gabi::at<daGy_c>(model->mUserArea);
        if (i_this != nullptr) {
            i_this->_nodeControl(node, (J3DModel*)model);
        }
    }
    return TRUE;
}
VERIFY(0x02166CC0, nodeControl_CB);

/* 02167258 */
void daGy_c::modeDiveInit() {
    WWHD_FUNC(0x02167258, void, this);
    mMode = 0;
    mApproachRate = 0.0f;
}
VERIFY(0x02167258, &daGy_c::modeDiveInit);

/* 02167270 */
f32 daGy_c::getWaterY() {
    WWHD_FUNC(0x02167270, f32, this);
    f32 waterY;
    if (mAcch.m_flags & dBgS_Acch::WATER_HIT) {
        waterY = gabi::load<f32>(gabi::ea(&mAcch) + 0x1BC); /* the water check's height */
    } else {
        gabi::Local<cXyz> pos;
        f32 x = current.pos.x;
        f32 y = current.pos.y + 1000.0f;
        f32 z = current.pos.z;
        pos->x = x;
        pos->z = z;
        pos->y = y;
        waterY = dBgS_GetWaterHeight(pos);
    }
    if (current.pos.y > waterY) {
        gravity = -2.5f;
        return current.pos.y;
    }
    gravity = 0.0f;
    return waterY;
}
VERIFY(0x02167270, &daGy_c::getWaterY);

/* 02167B60 */
static cPhs_State daGyCreate(void* i_this) {
    WWHD_FUNC(0x02167B60, cPhs_State, i_this);
    return ((daGy_c*)i_this)->_create();
}
VERIFY(0x02167B60, daGyCreate);

/* 02167B64 */
BOOL daGy_c::_delete() {
    WWHD_FUNC(0x02167B64, BOOL, this);
    dComIfG_resDelete(&mPhase, GY_ARC);
    gabi::call(0x025A92C0, &mWave[1]); /* dPa_waveEcallBack::remove */
    gabi::call(0x025A92C0, &mWave[0]);
    gabi::call(0x025A99B8, &mSplash);  /* dPa_splashEcallBack::remove */
    return TRUE;
}
VERIFY(0x02167B64, &daGy_c::_delete);

/* 02167BB8 */
static BOOL daGyDelete(void* i_this) {
    WWHD_FUNC(0x02167BB8, BOOL, i_this);
    return ((daGy_c*)i_this)->_delete();
}
VERIFY(0x02167BB8, daGyDelete);

/* 02167BBC: HD keeps LineCross's result in r3 (the GameCube signature is void) */
void daGy_c::lineCheck(cXyz* start, cXyz* end) {
    WWHD_FUNC(0x02167BBC, void, this, start, end);
    dBgS_LinChk_Set(&mLinChk, start, end, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk)) {
        end->copy(*gabi::at<cXyz>(gabi::ea(&mLinChk) + 0x30)); /* the cross point */
        mLineHit = 1;
    }
}
VERIFY(0x02167BBC, &daGy_c::lineCheck);

/* 0216810C */
void daGy_c::modeDamageInit() {
    WWHD_FUNC(0x0216810C, void, this);
    mAnmIdxNext = 6;
    mMode = 7;
    speed.y = 30.0f;
    mDoAud_seStart(0x58D1, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
}
VERIFY(0x0216810C, &daGy_c::modeDamageInit);

/* 021685EC */
void daGy_c::setAnm() {
    WWHD_FUNC(0x021685EC, void, this);
    gabi::call(0x025877F8, GY_ARC, mpMorf.get(), &mAnmIdx, &mAnmIdxNext, &mAnmIdxOld, 0x10010A30u, 0x10010A78u, 0u,
               mpMorf2.get()); /* dLib_setAnm (HD: the second morf as a stack argument) */
}
VERIFY(0x021685EC, &daGy_c::setAnm);

/* 02169264 */
static BOOL daGyExecute(void* i_this) {
    WWHD_FUNC(0x02169264, BOOL, i_this);
    return ((daGy_c*)i_this)->_execute();
}
VERIFY(0x02169264, daGyExecute);

/* 021695BC */
static BOOL daGyDraw(void* i_this) {
    WWHD_FUNC(0x021695BC, BOOL, i_this);
    return ((daGy_c*)i_this)->_draw();
}
VERIFY(0x021695BC, daGyDraw);

/* 02169710 */
void daGy_c::setAimSpeedF() {
    WWHD_FUNC(0x02169710, void, this);
    daGy_HIO_c* hio = gy_hio();
    if (mpCtrl->mTarget == 0) {
        mAimSpeedF = hio->mSpeedSlow;
        f32 frame = mpMorf->getFrame();
        if (frame > hio->mStrokeStart && mpMorf->getFrame() < hio->mStrokeEnd) {
            mAimSpeedF = hio->mSpeedSlowStroke;
        }
    } else {
        mAimSpeedF = hio->mSpeedFast;
        f32 frame = mpMorf->getFrame();
        if (frame > hio->mStrokeStart && mpMorf->getFrame() < hio->mStrokeEnd) {
            mAimSpeedF = hio->mSpeedFastStroke;
        }
    }
}
VERIFY(0x02169710, &daGy_c::setAimSpeedF);

/* 02169AA8 */
void daGy_c::modeWithCircleInit() {
    WWHD_FUNC(0x02169AA8, void, this);
    daGy_HIO_c* hio = gy_hio();
    mMode = 6;
    m614 = gabi::ftoi(cM_rndF((f32)(s16)hio->m132));
    if (m614 <= hio->m130) {
        m614 = hio->m130;
    }
    mTimer = hio->m110 + m614;
}
VERIFY(0x02169AA8, &daGy_c::modeWithCircleInit);

/* 02169B48 */
void daGy_c::modeAttackPlayerInit() {
    WWHD_FUNC(0x02169B48, void, this);
    mMode = 3;
    mAnmIdxNext = 2;
    mSubMode = 0;
    mBiteCount = 0;
}
VERIFY(0x02169B48, &daGy_c::modeAttackPlayerInit);

/* 02169B68: modeAttackInit */
void daGy_c::modeAttackInit() {
    WWHD_FUNC(0x02169B68, void, this);
    mMode = 2;
    mAnmIdxNext = 3;
    mTargetPos.copy(dComIfGp_getPlayer(0)->current.pos);
    mA20.copy(m3D8);
    mTimer = gy_hio()->m10C;
    mSubMode = 0;
}
VERIFY(0x02169B68, &daGy_c::modeAttackInit);

/* 0216A674: modeWithAttackInit */
void daGy_c::modeWithAttackInit() {
    WWHD_FUNC(0x0216A674, void, this);
    mMode = 5;
    mTargetPos.copy(dComIfGp_getPlayer(0)->current.pos);
    mA20.copy(m3D8);
    mTimer = gy_hio()->m10C;
    mSubMode = 0;
}
VERIFY(0x0216A674, &daGy_c::modeWithAttackInit);

/* 0216B5A4 */
static BOOL daGyIsDelete(void*) {
    WWHD_FUNC(0x0216B5A4, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0216B5A4, daGyIsDelete);

/* 0216B590: this TU's sead::SafeString deleting destructor (vtable 0x100108B0) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0216B590, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x0216B590, SafeString_dt);

/* 0216B6E4: this TU's sead::SafeString::assureTerminationImpl_ (empty) */
static void SafeString_assureTermination(void*) {
    WWHD_FUNC(0x0216B6E4, void, (u32)0);
}
VERIFY(0x0216B6E4, SafeString_assureTermination);

/* the death splash: particle 0x3C at the actor's position (HD: the scale has the debug offsets
 * 0x1047BBB0/B4 added, (2+a, 2+a+b, 2+a)) */
static inline void gy_deathSplash(daGy_c* i_this) {
    gabi::Local<cXyz> scale;
    f32 s = gabi::load<f32>(0x1047BBB0) + 2.0f;
    f32 sy = s + gabi::load<f32>(0x1047BBB4);
    scale->x = s;
    scale->z = s;
    scale->y = sy;
    dPa_control_set(dComIfGp_getParticle(), 0, 0x3C, &i_this->current.pos, nullptr, scale, 0xFF, nullptr, -1, nullptr,
                    nullptr, nullptr);
}
static inline be<u32>& gy_attnFlags(daGy_c* i_this) { return *gabi::at<be<u32>>(gabi::ea(i_this) + 0x39C); }

/* 02167F30 */
void daGy_c::modeDeleteBombInit() {
    WWHD_FUNC(0x02167F30, void, this);
    gy_attnFlags(this) = gy_attnFlags(this) & ~4u;
    mAimSpeedF = 0.0f;
    mBodyY = 0.0f;
    speedF = 0.0f;
    mMode = 9;
    mAnmIdxNext = 7;
    actor_status = actor_status & ~0x20u;
    speed.y = gy_hio()->m15C;
    mDoAud_seStart(0x58CF, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    mBodyYStep = gy_hio()->m0C8 * 10.0f;
}
VERIFY(0x02167F30, &daGy_c::modeDeleteBombInit);

/* 02167FE0 */
void daGy_c::modeDeleteInit() {
    WWHD_FUNC(0x02167FE0, void, this);
    actor_status = actor_status & ~0x20u;
    gy_attnFlags(this) = gy_attnFlags(this) & ~4u;
    s32 n = dSv_event_getEventReg(dComIfGs_getEvent_gy(), 0x7EFF) + 1;
    u8 v = n > 0xFF ? 0xFF : (u8)n;
    dSv_event_setEventReg(dComIfGs_getEvent_gy(), 0x7EFF, v);
    mMode = 8;
    mAimSpeedF = 0.0f;
    speedF = 0.0f;
    mAnmIdxNext = 8;
    speed.y = gy_hio()->m15C;
    mDoAud_seStart(0x58CF, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    gy_deathSplash(this);
}
VERIFY(0x02167FE0, &daGy_c::modeDeleteInit);

/* 02168938: modeProcCall (HD: a table of pointers to member functions at 0x10010B44, indexed by
 * the mode; the HIO can force a mode) */
void daGy_c::modeProcCall() {
    WWHD_FUNC(0x02168938, void, this);
    if (gy_hio()->mDebugMode != 0) {
        mMode = gy_hio()->mDebugModeNo;
    }
    u32 pmf = 0x10010B44 + (u32)(s32)mMode * 8;
    s16 index = gabi::load<s16>(pmf + 2);
    u32 receiver = gabi::ea(this) + (s32)gabi::load<s16>(pmf);
    u32 target;
    if (index < 0) {
        target = gabi::load<u32>(pmf + 4);
    } else {
        u32 vtbl = gabi::load<u32>(receiver + (s32)gabi::load<s16>(pmf + 6));
        target = gabi::load<u32>(vtbl + (u32)(s32)index * 8 + 4);
    }
    gabi::call_ptr(target, receiver);
}
VERIFY(0x02168938, &daGy_c::modeProcCall);

/* 02169EE8 */
void daGy_c::modeAttackBackInit() {
    WWHD_FUNC(0x02169EE8, void, this);
    mMode = 4;
    mAnmIdxNext = 4;
    speedF = gy_hio()->m104;
    speed.y = gy_hio()->m108;
    mDoAud_seStart(0x58D1, &eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
    gy_deathSplash(this);
}
VERIFY(0x02169EE8, &daGy_c::modeAttackBackInit);

/* 0216B100 */
daGy_HIO_c* daGy_HIO_ct(daGy_HIO_c* hio) {
    WWHD_FUNC(0x0216B100, daGy_HIO_c*, hio);
    if (hio == nullptr) {
        hio = (daGy_HIO_c*)operator_new(sizeof(daGy_HIO_c));
        if (hio == nullptr) return hio;
    }
    hio->__vtbl = 0x100109A8;
    hio->m004 = 1.0f;
    hio->m008 = 0.9f;
    hio->m00C = 200;
    hio->m010 = 200.0f;
    hio->m014 = 300.0f;
    hio->m018 = 2.0f;
    hio->m01C = 2.0f;
    hio->m020 = 0.0f;
    hio->m024 = 15.0f;
    hio->m028 = -80.0f;
    hio->m02C = -50.0f;
    hio->m030 = -150.0f;
    hio->m034 = -40.0f;
    hio->m038 = -100.0f;
    hio->m03C = -350.0f;
    hio->m040 = 60.0f;
    hio->m044 = 2500.0f;
    hio->mSpeedSlow = 10.0f;
    hio->mSpeedSlowStroke = 20.0f;
    hio->mSpeedFast = 30.0f;
    hio->mSpeedFastStroke = 40.0f;
    hio->m058 = 15.0f;
    hio->m05C = 5.0f;
    hio->m060 = 40.0f;
    hio->m064 = 20.0f;
    hio->m068 = 55.0f;
    hio->m06C = 10.0f;
    hio->m070 = 20.0f;
    hio->m074 = 70.0f;
    hio->m078 = 70.0f;
    hio->m07C = 80.0f;
    hio->m080 = 70.0f;
    hio->m084 = 70.0f;
    hio->m088 = 110.0f;
    hio->m08C = 100.0f;
    hio->m090 = 1.0f;
    hio->mDebugOffset = 0;
    hio->mDebugMode = 0;
    hio->m096 = 0;
    hio->m097 = 0;
    hio->m098 = 0;
    hio->m099 = 0;
    hio->m09C = 5000.0f;
    hio->m0A0 = -500.0f;
    hio->m0A4 = 180.0f;
    hio->m0A8 = -400.0f;
    hio->m0AC = -80.0f;
    hio->m0B0 = 20.0f;
    hio->m0B4 = -25.0f;
    hio->mDebugOffsetPos.x = 0.0f;
    hio->mDebugOffsetPos.y = 0.0f;
    hio->mDebugOffsetPos.z = 0.0f;
    hio->m0C4 = 8.0f;
    hio->m0C8 = 30.0f;
    hio->mStrokeStart = 2.0f;
    hio->mStrokeEnd = 20.0f;
    hio->mHeadOffset.x = 80.0f;
    hio->mHeadOffset.y = 0.0f;
    hio->mHeadOffset.z = 0.0f;
    hio->m0E0 = 0.0f;
    hio->m0E4 = 0.0f;
    hio->m0E8 = 0.0f;
    hio->mSplashOffsetLow.x = 0.0f;
    hio->mSplashOffsetLow.y = 0.0f;
    hio->mSplashOffsetLow.z = 120.0f;
    hio->mSplashOffsetHigh.x = 0.0f;
    hio->mSplashOffsetHigh.y = 0.0f;
    hio->mSplashOffsetHigh.z = 300.0f;
    hio->m104 = -40.0f;
    hio->m108 = 25.0f;
    hio->m10C = 120;
    hio->m10E = 60;
    hio->m110 = 120;
    hio->m112 = 180;
    hio->m114 = 360;
    hio->m116 = 90;
    hio->m118 = 240;
    for (int i = 0; i < 9; i++) hio->m11C[i] = 0;
    hio->m12C = 10.0f;
    hio->m130 = 100;
    hio->m132 = 300;
    hio->m134 = 45.0f;
    hio->m138 = 45.0f;
    hio->m13C = 30.0f;
    hio->m140 = 0x1500;
    hio->m142 = 0x800;
    hio->m144 = 400.0f;
    hio->m148 = 350.0f;
    hio->m150 = 60.0f;
    hio->m154 = 60.0f;
    hio->m158 = 40.0f;
    hio->m15C = 45.0f;
    hio->m160 = 10.0f;
    hio->m164 = 90;
    hio->m168 = -1000.0f;
    hio->m16C = -100.0f;
    hio->m170 = 50.0f;
    hio->m174 = 0.1f;
    hio->m178 = 0.5f;
    hio->m17C = 0.1f;
    hio->mHeadAngle = 0x2000;
    hio->mHeadDist = 4000.0f;
    hio->mHeadSlerp = 0.2f;
    hio->m18C = 3000.0f;
    hio->m190 = 200.0f;
    hio->m194 = 3000.0f;
    hio->m198 = 1;
    return hio;
}
VERIFY(0x0216B100, daGy_HIO_ct);

/* 0216B4F0: static initialisation of the translation unit */
static void __sinit_d_a_gy_cpp() {
    WWHD_FUNC(0x0216B4F0, void, (u32)0);
    sinit_header_statics(0x10464450, 0x101B6CFC);
    daGy_HIO_ct(gy_hio());
}
VERIFY(0x0216B4F0, __sinit_d_a_gy_cpp);

/* 0216B5AC: daGy_c deleting destructor (virtual, vtable 0x100109B8) */
static void daGy_dt(daGy_c* i_this, s32 flags) {
    WWHD_FUNC(0x0216B5AC, void, i_this, flags);
    if (i_this == nullptr) return;
    u32 p = gabi::ea(i_this);
    /* ~dBgS_ObjLinChk (inline: the per-TU vtables, then cBgS_LinChk::~cBgS_LinChk) */
    gabi::store<u32>(p + 0xF88, 0x10010958);
    gabi::store<u32>(p + 0xF94, 0x100108E8);
    gabi::store<u32>(p + 0xF50, 0x100108D8);
    gabi::call(0x02008B4C, &i_this->mLinChk, 0);
    /* ~dBgS_ObjAcch (mAcch2) */
    gabi::store<u32>(p + 0xC5C, GY_OBJACCH_VT.v20);
    gabi::store<u32>(p + 0xC50, GY_OBJACCH_VT.v14);
    gabi::call(0x024EFD9C, &i_this->mEnemyIce.mBgAcch, 0);
    gabi::call(0x02018034, p + 0xBFC + 0x14, 2);     /* ~dBgS_AcchCir (inline): ~cM3dGCir on its circle */
    dCcD_Cyl_dt(&i_this->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&i_this->mEnemyIce.mStts, 2);
    dCcD_Stts_dt(&i_this->mStts, 2);
    gabi::call(0x02515980, &i_this->mCps, 2);        /* ~dCcD_Cps (the matcher: ~dCcD_GObjInf) */
    gabi::call(0x02515AE8, &i_this->mSph, 2);        /* ~dCcD_Sph */
    gabi::call(0x02515AE8, &i_this->mHeadSph, 2);
    gabi::call(0x02018034, p + 0x5C0 + 0x14, 2);
    gabi::store<u32>(p + 0x41C, GY_OBJACCH_VT.v20);
    gabi::store<u32>(p + 0x410, GY_OBJACCH_VT.v14);
    gabi::call(0x024EFD9C, &i_this->mAcch, 0);
    gabi::call(0x025E89F8, i_this->mInvisibleModel, 2); /* ~mDoExt_invisibleModel */
    gabi::call(0x025D50BC, i_this, 0);               /* ~fopAc_ac_c */
    if (flags & 1) operator_delete(i_this);
}
VERIFY(0x0216B5AC, daGy_dt);

/* 02166FC4: daGy_c::daGy_c (GameCube: weak, out of line) */
daGy_c* daGy_ct(daGy_c* i_this) {
    WWHD_FUNC(0x02166FC4, daGy_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daGy_c*)operator_new(sizeof(daGy_c));
        if (i_this == nullptr) return i_this;
    }
    u32 p = gabi::ea(i_this);
    fopAc_ac_c_ct(i_this);
    i_this->__vtbl = GY_VTBL;
    gabi::call(0x025E895C, i_this->mInvisibleModel); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    dBgS_ObjAcch_ct(&i_this->mAcch, GY_OBJACCH_VT);
    dBgS_AcchCir_ct(&i_this->mAcchCir);
    gabi::call(0x025166F0, &i_this->mHeadSph);       /* dCcD_Sph::dCcD_Sph */
    gabi::call(0x025166F0, &i_this->mSph);
    /* dCcD_Cps (inline) */
    gabi::call(0x02515FB8, &i_this->mCps);           /* dCcD_GObjInf::dCcD_GObjInf */
    gabi::store<u32>(p + 0x880 + 0x114, 0x100015A8); /* cCcD_ShapeAttr */
    gabi::store<u32>(p + 0x880 + 0x110, GY_AAB_VTBL);
    gabi::call(0x02018150, p + 0x880 + 0x118);       /* cM3dGCps::cM3dGCps */
    i_this->mCps.__vtbl_hitinf = 0x1004AF18;
    gabi::store<u32>(p + 0x880 + 0x130, 0x1004AF60);
    gabi::store<u32>(p + 0x880 + 0x114, 0x1004AF70);
    dCcD_Stts_ct(&i_this->mStts);
    dCcD_Stts_ct(&i_this->mEnemyIce.mStts);
    dCcD_Cyl_ct(&i_this->mEnemyIce.mCyl, GY_AAB_VTBL);
    dBgS_AcchCir_ct(&i_this->mEnemyIce.mBgAcchCir);
    dBgS_ObjAcch_ct(&i_this->mEnemyIce.mBgAcch, GY_OBJACCH_VT);
    /* the wave callbacks (their cXyz members' constructors allocate only for a null this) */
    for (int w = 0; w < 2; w++) {
        u32 cb = gabi::ea(&i_this->mWave[w]);
        i_this->mWave[w].__vtbl = 0x100521A8;
        if (cb + 0x3C == 0) operator_new(0xC);
        if (cb + 0x48 == 0) operator_new(0xC);
        if (cb + 0x54 == 0) operator_new(0xC);
    }
    i_this->mSplash.__vtbl = 0x100521E8;
    dBgS_LinChk_ct(&i_this->mLinChk, GY_LINCHK_VT, true);
    return i_this;
}
VERIFY(0x02166FC4, daGy_ct);

/* 02166D08 */
BOOL daGy_c::_createHeap() {
    WWHD_FUNC(0x02166D08, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(GY_ARC, 0xF, GY_SAFESTRING_VTBL);
    if (modelData == nullptr) {
        JUT_ASSERT_fail(STR(0x100109D8), 0x378, STR(0x100109E4)); /* "d_a_gy.cpp", "modelData != (0)" */
    }
    mpMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr,
                                    (J3DAnmTransform*)dComIfG_getObjectRes(GY_ARC, 0xB, GY_SAFESTRING_VTBL), 2, 1.0f, 0,
                                    -1, 1, nullptr, 0, 0x11020203);
    if (mpMorf == nullptr || mpMorf->getModel() == nullptr) {
        return FALSE;
    }
    gabi::store<u32>(gabi::ea(mpMorf->getModel()) + 0xB8, gabi::ea(this));
    if (!mDoExt_invisibleModel_create(mInvisibleModel, mpMorf->getModel())) {
        return FALSE;
    }
    mpMorf2 = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr,
                                     (J3DAnmTransform*)dComIfG_getObjectRes(GY_ARC, 0xB, GY_SAFESTRING_VTBL), 2, 1.0f, 0,
                                     -1, 1, nullptr, 0, 0x11020203);
    if (mpMorf2 == nullptr || mpMorf2->getModel() == nullptr) {
        return FALSE;
    }
    gabi::store<u32>(gabi::ea(mpMorf2->getModel()) + 0xB8, gabi::ea(this));
    J3DModel_setFlags_gy(mpMorf2->getModel(), ~6u);
    J3DModel_setFlags_gy(mpMorf->getModel(), ~1u);
    for (u16 i = 0; i < J3DModelData_getJointNum_gy(modelData); i++) {
        if (i == 2 || (i >= 5 && i <= 7) || (i >= 9 && i <= 10)) {
            setJointCallBack_gy(J3DModel_getModelData(mpMorf->getModel()), i, 0x02166CC0 /* nodeControl_CB */);
        }
    }
    mpJntHit = JntHit_create(mpMorf->getModel(), 0x101B6C24, 8);
    if (mpJntHit == 0) {
        return FALSE;
    }
    gabi::store<u32>(gabi::ea(this) + 0x36C, mpJntHit); /* jntHit */
    return TRUE;
}
VERIFY(0x02166D08, &daGy_c::_createHeap);

/* 0216731C */
void daGy_c::setMtx() {
    WWHD_FUNC(0x0216731C, void, this);
    J3DModel* model = mpMorf->getModel();
    J3DModel_setBaseScale(model, &scale);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    gabi::Local<cXyz> offset;
    offset->y = 0.0f;
    offset->z = 500.0f;
    offset->x = 0.0f;
    PSMTXMultVec(mDoMtx_stack_c::get(), offset, &mFA0);
    daGy_HIO_c* hio = gy_hio();
    if (hio->mDebugOffset != 0) {
        mDoMtx_stack_c::transM(hio->mDebugOffsetPos.x, hio->mDebugOffsetPos.y, hio->mDebugOffsetPos.z);
    }
    mDoMtx_stack_c::transM(0.0f, mBodyY, 0.0f);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    /* the shadow model on the water surface */
    model = mpMorf2->getModel();
    cXyz* s2 = gabi::at<cXyz>(gabi::ea(model) + 0xBC);
    s2->x = scale.x;
    s2->z = scale.z;
    s2->y = 0.1f;
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    f32 waterY = getWaterY();
    mDoMtx_stack_c::transM(0.0f, waterY + 15.0f, 0.0f);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    if (mBodyY > hio->m0AC + 10.0f) {
        mDoMtx_stack_c::transM(hio->mSplashOffsetHigh.x, hio->mSplashOffsetHigh.y, hio->mSplashOffsetHigh.z);
    } else {
        mDoMtx_stack_c::transM(hio->mSplashOffsetLow.x, hio->mSplashOffsetLow.y, hio->mSplashOffsetLow.z);
    }
    Mtx34* now = mDoMtx_stack_c::get();
    mSplashPos.x = now->m[0][3];
    mSplashPos.y = now->m[1][3];
    mSplashPos.z = now->m[2][3];
}
VERIFY(0x0216731C, &daGy_c::setMtx);

/* 02166944 */
void daGy_c::_nodeControl(J3DNode* node, J3DModel* model) {
    WWHD_FUNC(0x02166944, void, this, node, model);
    J3DJoint* joint = J3DNode_toJoint(node);
    Mtx34* now = mDoMtx_stack_c::get();
    u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
    daGy_HIO_c* hio = gy_hio();
    gabi::Local<cXyz> headOffset;
    if (jntNo == 2) {
        PSMTXCopy(getAnmMtx_gy(model, 2), now);
        headOffset->copy(hio->mHeadOffset);
        PSMTXMultVec(now, headOffset, &mHeadPos);
    }
    gabi::Local<Mtx34> mtx;
    PSMTXCopy(getAnmMtx_gy(model, jntNo), mtx);
    f32 ty = mtx->m[1][3];
    f32 tz = mtx->m[2][3];
    mtx->m[2][3] = 0.0f;
    f32 tx = mtx->m[0][3];
    mtx->m[0][3] = 0.0f;
    mtx->m[1][3] = 0.0f;
    PSMTXTrans(now, tx, ty, tz);
    gabi::Local<cXyz> headOffset2;
    gabi::Local<cXyz> offset5;
    if (jntNo == 2) {
        /* turn the head towards the player */
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        gabi::Local<cXyz> diff;
        cXyz_mi(&current.pos, diff, &player->current.pos);
        gabi::Local<cXyz> diffXZ;
        diffXZ->x = diff->x;
        diffXZ->y = 0.0f;
        diffXZ->z = diff->z;
        f32 dist = std_sqrtf(PSVECSquareMag(diffXZ));
        s16 angle = fopAcM_searchActorAngleY(this, player);
        s32 dAngle = cLib_distanceAngleS(shape_angle.y, angle);
        s8 anm;
        if (dAngle < hio->mHeadAngle && dist < hio->mHeadDist && (anm = mAnmIdxNext) != 8 && anm != 9 && anm != 6 &&
            anm != 11 && anm != 3) {
            f32 px = player->current.pos.x;
            mHeadTarget.x = px;
            f32 py = player->current.pos.y;
            s32 mode = mMode;
            mHeadTarget.y = py;
            f32 pz = player->current.pos.z;
            mHeadTarget.z = pz;
            if (mode == 2) {
                py = daSea_calcWave(px, pz) + hio->m0A8;
                mode = mMode;
                mHeadTarget.y = py;
            }
            if (mode == 3 && mSubMode == 1) {
                mHeadTarget.y = py + hio->m0A4;
            }
            gabi::Local<cXyz> toTarget;
            cXyz_mi(&mHeadTarget, toTarget, &mHeadPos);
            gabi::Local<cXyz> zero;
            cXyz_mi(&mHeadPos, zero, &mHeadPos);
            gabi::Local<Quaternion_gy> q;
            daObj_quat_rotVec_gy(q, zero, toTarget);
            C_QUATSlerp_gy((Quaternion_gy*)mHeadQuat, q, (Quaternion_gy*)mHeadQuat, hio->mHeadSlerp);
            mDoMtx_stack_quatM_gy((Quaternion_gy*)mHeadQuat);
        } else {
            C_QUATSlerp_gy((Quaternion_gy*)mHeadQuat, gabi::at<Quaternion_gy>(0x101E9C38) /* ZeroQuat */,
                           (Quaternion_gy*)mHeadQuat, hio->mHeadSlerp);
            mDoMtx_stack_quatM_gy((Quaternion_gy*)mHeadQuat);
        }
    }
    PSMTXConcat_gy(now, mtx, now);
    if (jntNo == 2) {
        headOffset2->copy(hio->mHeadOffset);
        PSMTXMultVec(now, headOffset2, &mHeadPos);
    } else if (jntNo == 5) {
        offset5->x = 100.0f;
        offset5->y = -100.0f;
        offset5->z = 0.0f;
        PSMTXMultVec(now, offset5, &m9B8);
    }
    PSMTXCopy(now, J3DSys_mCurrentMtx_gy());
    mtx_copy(getAnmMtx_gy(model, jntNo), now);
}
VERIFY(0x02166944, &daGy_c::_nodeControl);

/* 021675C0 */
void daGy_c::createWave() {
    WWHD_FUNC(0x021675C0, void, this);
    /* function-local statics (guard word + value) */
    if (gabi::load<s32>(0x10464608) == 0) {
        gabi::store<f32>(0x10464614, 1.0f);
        gabi::store<s32>(0x10464608, 1);
        gabi::store<f32>(0x10464610, 0.5f);
        gabi::store<f32>(0x10464618, -0.3f);
    }
    if (gabi::load<s32>(0x1046460C) == 0) {
        gabi::store<f32>(0x10464620, 1.0f);
        gabi::store<s32>(0x1046460C, 1);
        gabi::store<f32>(0x1046461C, -0.5f);
        gabi::store<f32>(0x10464624, -0.3f);
    }
    cXyz* waveOffsetL = gabi::at<cXyz>(0x10464610);
    cXyz* waveOffsetR = gabi::at<cXyz>(0x1046461C);
    if (mWave[0].mpEmitter == nullptr) {
        dPa_control_set(dComIfGp_getParticle(), 0, 0x37, &mSplashPos, &mSplashAngle, nullptr, 0xFF,
                        (dPa_levelEcallBack*)&mWave[0], -1, nullptr, nullptr, nullptr);
        if (mWave[0].mpEmitter != nullptr) {
            u32 e = gabi::ea(mWave[0].mpEmitter.get());
            gabi::store<f32>(e + 0x28, waveOffsetL->x);
            gabi::store<f32>(e + 0x2C, waveOffsetL->y);
            gabi::store<f32>(e + 0x30, waveOffsetL->z);
        }
    }
    if (mWave[1].mpEmitter == nullptr) {
        dPa_control_set(dComIfGp_getParticle(), 0, 0x37, &mSplashPos, &mSplashAngle, nullptr, 0xFF,
                        (dPa_levelEcallBack*)&mWave[1], -1, nullptr, nullptr, nullptr);
        if (mWave[1].mpEmitter != nullptr) {
            u32 e = gabi::ea(mWave[1].mpEmitter.get());
            gabi::store<f32>(e + 0x28, waveOffsetR->x);
            gabi::store<f32>(e + 0x2C, waveOffsetR->y);
            gabi::store<f32>(e + 0x30, waveOffsetR->z);
        }
    }
    if (mSplash.mpEmitter == nullptr) {
        dPa_control_set(dComIfGp_getParticle(), 0, 0x35, &mSplashPos, &mSplashAngle, nullptr, 0xFF,
                        (dPa_levelEcallBack*)&mSplash, -1, nullptr, nullptr, nullptr);
    }
}
VERIFY(0x021675C0, &daGy_c::createWave);

/* 0216779C */
void daGy_c::createInit() {
    WWHD_FUNC(0x0216779C, void, this);
    daGy_HIO_c* hio = gy_hio();
    u32 q = 0x101E9C38; /* ZeroQuat */
    for (u32 i = 0; i < 16; i += 4) gabi::store<u32>(gabi::ea(this) + 0xE14 + i, gabi::load<u32>(q + i));
    gbaName = 0x1D;
    f32 s = hio->m090;
    mLightTimer = -1;
    scale.z = s;
    mIceTimer = -1;
    scale.x = s;
    mEnemyIce.mWallRadius = 0.0f;
    mEnemyIce.m00C = 1;
    mEnemyIce.mpActor = this;
    scale.y = s;
    mEnemyIce.mCylHeight = 0.0f;
    itemTableIdx = dComIfGp_CharTbl_GetNameIndex_gy(STR(0x10010A20) /* "GyCtrl" */, 0);
    mAnmIdxNext = 1;
    gy_attnFlags(this) = 4;
    gabi::store<u8>(gabi::ea(this) + 0x38A, 0x22); /* attention_info.distances[2] */
    mDisappearTimer = -1;
    mBodyYScale = hio->m174;
    mA0C = 0x78;
    mBodyY = hio->m0A0;
    modeDiveInit();
    m3D8.copy(current.pos);
    for (s32 i = 0; i < J3DModelData_getJointNum_gy(J3DModel_getModelData(mpMorf->getModel())); i++) {
    }
    setMtx();
    mpMorf->calc();
    mpMorf2->calc();
    settingTevStruct(dKy_getEnvlight(), 0, &current.pos, &tevStr);
    mAcchCir.SetWall(30.0f, 30.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed, nullptr, nullptr);
    mAcch.m_flags = mAcch.m_flags | 0xC; /* SetWallNone, SetRoofNone */
    gravity = -2.5f;
    fopAcM_posMoveF(this, nullptr);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMorf->getModel()));
    mStts.Init(100, 100, this);
    mSph.Set(gabi::at<dCcD_SrcSph>(0x101B6B78));     /* l_sph_src */
    mSph.SetStts(&mStts);
    gabi::call(0x025164C0, &mCps, 0x101B6BD8);         /* dCcD_Cps::Set(l_cps_src) */
    mCps.SetStts(&mStts);
    mHeadSph.Set(gabi::at<dCcD_SrcSph>(0x101B6B38)); /* l_sph_head_src */
    mHeadSph.SetStts(&mStts);
    max_health = 1;
    health = 1;
    createWave();
}
VERIFY(0x0216779C, &daGy_c::createInit);

/* 02167A00 */
cPhs_State daGy_c::_create() {
    WWHD_FUNC(0x02167A00, cPhs_State, this);
    if (!(actor_condition & fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            daGy_ct(this);
        }
        actor_condition = actor_condition | fopAcCnd_INIT_e;
    }
    cPhs_State phase = dComIfG_resLoad(&mPhase, GY_ARC);
    if (phase == cPhs_COMPLEATE_e) {
        if (!dComIfGs_checkGetItem_gy(0x2D)) {
            return cPhs_ERROR_e;
        }
        if (parentActorID == (u32)-1) {
            return cPhs_ERROR_e;
        }
        gabi::Local<be<u32>> parent;
        if (!fopAcM_SearchByID_gy(parentActorID, parent)) {
            return cPhs_ERROR_e;
        }
        if (*parent == 0 || !fopAc_IsActor(gabi::at<void>(*parent))) {
            return cPhs_ERROR_e;
        }
        u32 ctrl = *parent;
        if (ctrl == 0) {
            return cPhs_ERROR_e;
        }
        if (fpcM_GetName(gabi::at<void>(ctrl)) != 0xE5) {
            if (ctrl == 0 || fpcM_GetName(gabi::at<void>(ctrl)) != 0xE6) {
                return cPhs_ERROR_e;
            }
        }
        mpCtrl = gabi::at<daGyCtrl_c>(ctrl);
        mIndex = mpCtrl->mSpawnIndex;
        mpCtrl->mSpawnIndex = mpCtrl->mSpawnIndex + 1;
        ctrl = *parent;
        if (ctrl != 0 && fpcM_GetName(gabi::at<void>(ctrl)) == 0xE5) {
            fopAcM_setStageLayer(this);
        }
        if (!fopAcM_entrySolidHeap(this, 0x02166FC0 /* createHeap_CB */, 0x3FA0)) {
            return cPhs_ERROR_e;
        }
        createInit();
    }
    return phase;
}
VERIFY(0x02167A00, &daGy_c::_create);

/* the head sphere's attack bits (raw GObjInf offsets: At SPrm +0x0 bit 1 "set", +0x4, Atp +0x14,
 * At Se +0x6C, At Spl +0x6F, Co SPrm +0x2C bit 1 "set") */
static inline void gy_headAtOn(daGy_c* i_this, bool coOn) {
    u32 h = gabi::ea(&i_this->mHeadSph);
    gabi::store<u8>(h + 0x6F, 1);
    gabi::store<u32>(h + 0x0, gabi::load<u32>(h + 0x0) | 1);
    gabi::store<u8>(h + 0x14, 2);
    gabi::store<u32>(h + 0x4, 1);
    gabi::store<u8>(h + 0x6C, 6);
    if (coOn) {
        gabi::store<u32>(h + 0x2C, gabi::load<u32>(h + 0x2C) | 1);
    } else {
        gabi::store<u32>(h + 0x2C, gabi::load<u32>(h + 0x2C) & ~1u);
    }
}
static inline void gy_headAtOff(daGy_c* i_this) {
    u32 h = gabi::ea(&i_this->mHeadSph);
    gabi::store<u32>(h + 0x0, gabi::load<u32>(h + 0x0) & ~1u);
    gabi::store<u32>(h + 0x4, gabi::load<u32>(h + 0x4) & ~1u);
    gabi::store<u8>(h + 0x14, 0);
}
static inline void gy_headCoOn(daGy_c* i_this) {
    u32 h = gabi::ea(&i_this->mHeadSph);
    gabi::store<u32>(h + 0x2C, gabi::load<u32>(h + 0x2C) | 1);
}

/* 02167C34 */
void daGy_c::setAtCollision() {
    WWHD_FUNC(0x02167C34, void, this);
    daGy_HIO_c* hio = gy_hio();
    mHeadSph.SetR(gabi::fmuls_ppc(hio->m088, hio->m090));
    mHeadSph.SetC(&mHeadPos);
    s32 mode = mMode;
    if (mode == 3) {
        s32 frame = gabi::ftoi(mpMorf->getFrame());
        if (mAnmIdxNext == 11 && frame < 0x12) {
            gy_headAtOn(this, false);
        } else {
            gy_headAtOff(this);
            gy_headCoOn(this);
        }
    } else if (mode == 2 || mode == 5) {
        if (mpCtrl->mTarget == 1) {
            if (mHeadSph.ChkCoHit()) {
                fopAc_ac_c* ac = mHeadSph.GetCoHitAc();
                if (ac != nullptr && fpcM_GetName(ac) == 0xA5) {
                    gy_headAtOn(this, true);
                } else {
                    gy_headCoOn(this);
                }
            } else {
                gy_headAtOff(this);
                gy_headCoOn(this);
            }
        }
    } else {
        gy_headAtOff(this);
        gy_headCoOn(this);
        if (mHeadSph.ChkCoHit() && mMode != 9 && mMode != 8) {
            modeDiveInit();
        }
    }
    dComIfG_Ccsp_Set(&mHeadSph);
}
VERIFY(0x02167C34, &daGy_c::setAtCollision);

/* 02167E84 */
void daGy_c::setCollision() {
    WWHD_FUNC(0x02167E84, void, this);
    daGy_HIO_c* hio = gy_hio();
    mSph.SetR(140.0f * hio->m090);
    mSph.SetC(&current.pos);
    dComIfG_Ccsp_Set(&mSph);
    gabi::call(0x02018808, gabi::ea(&mCps) + 0x118, &m9B8, &m9B8); /* cM3dGLin::SetStartEnd */
    gabi::store<f32>(gabi::ea(&mCps) + 0x134, gabi::fmuls_ppc(hio->m08C, hio->m090)); /* cM3dGCps radius */
    dComIfG_Ccsp_Set(&mCps);
}
VERIFY(0x02167E84, &daGy_c::setCollision);

static inline void gy_seStart(daGy_c* i_this, u32 id, u32 param) {
    mDoAud_seStart(id, &i_this->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
}
static inline void gy_removeWaves(daGy_c* i_this) {
    gabi::call(0x025A99B8, &i_this->mSplash); /* dPa_splashEcallBack::remove */
    gabi::call(0x025A92C0, &i_this->mWave[0]); /* dPa_waveEcallBack::remove */
    gabi::call(0x025A92C0, &i_this->mWave[1]);
}

/* 02168170 */
void daGy_c::checkTgHit() {
    WWHD_FUNC(0x02168170, void, this);
    mStts.Move();
    gabi::Local<cXyz> hitPos;
    void* hitObj = mCps.GetTgHitObj();
    f32 hx = mCps.mGObjTg.mHitPos.x;
    f32 hy = mCps.mGObjTg.mHitPos.y;
    hitPos->x = hx;
    f32 hz = mCps.mGObjTg.mHitPos.z;
    hitPos->y = hy;
    hitPos->z = hz;
    if (hitObj == nullptr) {
        hitObj = mHeadSph.GetTgHitObj();
        hitPos->copy(mHeadSph.mGObjTg.mHitPos);
        if (hitObj == nullptr) {
            hitObj = mSph.GetTgHitObj();
            hitPos->copy(mSph.mGObjTg.mHitPos);
            if (hitObj == nullptr) {
                return;
            }
        }
    }
    daGy_HIO_c* hio = gy_hio();
    u32 type = gabi::load<u32>(gabi::ea(hitObj) + 0x10); /* the hit object's At type */
    if (type & 0x20) {
        health = 0;
        mDamageSpeed = hio->m14C;
    } else if (type & 0x4000 /* normal arrow */) {
        gy_seStart(this, 0x2834, 0x20);
        health = health - 2;
        mDamageSpeed = hio->m150;
    } else if (type & 0x80000 /* ice arrow */) {
        mFreezeType = 9;
        gy_seStart(this, 0x2834, 0x20);
        mIceTimer = 10;
        gy_removeWaves(this);
        goto freeze;
    } else if (type & 0x40000 /* fire arrow */) {
        gy_seStart(this, 0x2834, 0x20);
        health = health - 2;
        mDamageSpeed = hio->m150;
    } else if (type & 0x100000 /* light arrow */) {
        mFreezeType = 11;
        gy_seStart(this, 0x2834, 0x20);
        mLightTimer = 10;
        gy_removeWaves(this);
        goto freeze;
    } else if (type & 0x8000) {
        gy_seStart(this, 0x2834, 0x20);
        health = health - 2;
        mDamageSpeed = hio->m154;
    } else if (type & 0x40 /* boomerang */) {
        gy_seStart(this, 0x2833, 0x20);
        health = health - 2;
        mDamageSpeed = hio->m158;
    } else {
        goto freeze;
    }
    {
        mDoAud_onEnemyDamage_gy();
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        dPa_control_set(dComIfGp_getParticle(), 0, 0x10, hitPos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr,
                        nullptr);
        if (mFreezeType == 11) {
            gabi::Local<cXyz> sc;
            sc->y = 2.0f;
            sc->z = 2.0f;
            sc->x = 2.0f;
            dPa_control_set(dComIfGp_getParticle(), 0, 0xF, hitPos, &player->shape_angle, sc, 0xFF, nullptr, -1, nullptr,
                            nullptr, nullptr);
            gy_seStart(this, 0x2828, 0);
            return;
        }
        gabi::Local<cXyz> sc2;
        if (health <= 0) {
            sc2->y = 2.0f;
            sc2->z = 2.0f;
            sc2->x = 2.0f;
            dPa_control_set(dComIfGp_getParticle(), 0, 0xF, hitPos, &player->shape_angle, sc2, 0xFF, nullptr, -1,
                            nullptr, nullptr, nullptr);
            gy_seStart(this, 0x2828, 0);
            health = 0;
            s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
            mDoAud_monsSeStart_gy(0x48CE, &eyePos, gabi::load<u32>(gabi::ea(this) + 4), 0, reverb);
            if (mDamageSpeed == hio->m14C) {
                modeDeleteBombInit();
                return;
            }
            modeDeleteInit();
            return;
        }
        mPrevMode = mMode;
        s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
        mDoAud_monsSeStart_gy(0x48CD, &eyePos, gabi::load<u32>(gabi::ea(this) + 4), 0, reverb);
        modeDamageInit();
        return;
    }
freeze:
    if (mFreezeType == 9 || mFreezeType == 11) {
        modeDeleteInit();
    }
}
VERIFY(0x02168170, &daGy_c::checkTgHit);

/* a wave/splash emitter's "stop drawing" flag (JPABaseEmitter +0x254 bit 0) */
static inline void gy_emitterStop(JPABaseEmitter* e, bool stop) {
    u32 p = gabi::ea(e);
    u32 f = gabi::load<u32>(p + 0x254);
    gabi::store<u32>(p + 0x254, stop ? (f | 1) : (f & ~1u));
}

/* 02168640 */
void daGy_c::setWave() {
    WWHD_FUNC(0x02168640, void, this);
    daGy_HIO_c* hio = gy_hio();
    s8 anm = mAnmIdxNext;
    f32 scaleZ = hio->m024;
    f32 waveY;
    f32 splashRate;
    u32 mode;
    if (((u32)anm >= 5 && (u32)anm <= 6) || ((u32)anm >= 8 && (u32)anm <= 9)) {
        mode = mMode;
        waveY = 0.0f;
        splashRate = 0.0f;
    } else {
        mode = mMode;
        waveY = hio->m018;
        splashRate = hio->m010;
    }
    if (mode < 1 || (mode != 1 && mode == 8)) {
        waveY = 0.0f;
        splashRate = 0.0f;
    } else if (mode == 1) {
        f32 g = gabi::load<f32>(0x1047BD04); /* HD debug offset */
        f32 a = g + 0.7f;
        f32 b = g + 0.6f;
        waveY = gabi::fmuls_ppc(waveY, a);
        scaleZ = gabi::fmuls_ppc(hio->m024, b);
    }
    if (anm == 10) {
        splashRate = 0.0f;
        waveY = 0.0f;
    }
    if (mWave[0].mpEmitter != nullptr) {
        gy_emitterStop(mWave[0].mpEmitter, !(waveY > 0.0f));
    }
    if (mWave[1].mpEmitter != nullptr) {
        gy_emitterStop(mWave[1].mpEmitter, !(waveY > 0.0f));
    }
    mSplashPos.y = daSea_calcWave(mSplashPos.x, mSplashPos.z);
    mSplashAngle.y = current.angle.y;
    u32 w0 = gabi::ea(&mWave[0]);
    u32 w1 = gabi::ea(&mWave[1]);
    gabi::store<f32>(w1 + 0x8, waveY);
    gabi::store<f32>(w0 + 0x8, waveY);
    gabi::store<f32>(w0 + 0x18, scaleZ);
    gabi::store<f32>(w1 + 0x18, scaleZ);
    gabi::store<f32>(w1 + 0x14, hio->m020 + 1.0f);
    gabi::store<f32>(w0 + 0x14, 1.0f - hio->m020);
    f32 v38 = hio->m038, v34 = hio->m034, v2C = hio->m02C, v28 = hio->m028, v30 = hio->m030, v3C = hio->m03C;
    gabi::store<f32>(w0 + 0x24, v30);
    gabi::store<f32>(w1 + 0x20, v2C);
    gabi::store<f32>(w0 + 0x30, v3C);
    gabi::store<f32>(w0 + 0x20, v2C);
    gabi::store<f32>(w0 + 0x28, -v34);
    gabi::store<f32>(w1 + 0x24, v30);
    gabi::store<f32>(w0 + 0x1C, -v28);
    gabi::store<f32>(w1 + 0x28, v34);
    gabi::store<f32>(w1 + 0x1C, v28);
    gabi::store<f32>(w1 + 0x2C, v38);
    gabi::store<f32>(w1 + 0x30, v3C);
    gabi::store<f32>(w0 + 0x2C, v38);
    gabi::store<f32>(w1 + 0x10, hio->m01C);
    gabi::store<f32>(w0 + 0x10, hio->m01C);
    cLib_addCalc2(&mSplashRate, splashRate, 0.1f, 10.0f);
    if (mSplash.mpEmitter != nullptr) {
        fopAc_ac_c* player = dComIfGp_getPlayer(0);
        bool stop = true;
        if (mSplashRate > 0.1f && player->current.pos.y < 1500.0f) {
            stop = false;
        }
        gy_emitterStop(mSplash.mpEmitter, stop);
    }
    u32 sp = gabi::ea(&mSplash);
    gabi::store<f32>(sp + 0x8, mSplashRate);
    gabi::store<f32>(sp + 0xC, hio->m014);
}
VERIFY(0x02168640, &daGy_c::setWave);

/* the landing splash (particles 0x203B and 0x3C, or 0x3C alone) with the HD debug scale offsets */
static inline void gy_landSplash(daGy_c* i_this, f32 base, bool big, u32 se) {
    gabi::Local<cXyz> sc;
    f32 s = gabi::load<f32>(0x1047BBB0) + base;
    f32 sy = s + gabi::load<f32>(0x1047BBB4);
    sc->x = s;
    sc->z = s;
    sc->y = sy;
    if (big) {
        dPa_control_set(dComIfGp_getParticle(), 0, 0x203B, &i_this->current.pos, nullptr, sc, 0xFF, nullptr, -1, nullptr,
                        nullptr, nullptr);
    }
    dPa_control_set(dComIfGp_getParticle(), 0, 0x3C, &i_this->current.pos, nullptr, sc, 0xFF, nullptr, -1, nullptr,
                    nullptr, nullptr);
    gy_seStart(i_this, se, 0);
}

/* 021689C8 */
BOOL daGy_c::_execute() {
    WWHD_FUNC(0x021689C8, BOOL, this);
    mpCtrl = nullptr;
    gabi::Local<be<u32>> parent;
    if (parentActorID == (u32)-1 || !fopAcM_SearchByID_gy(parentActorID, parent) || *parent == 0) {
        fopAcM_delete(this);
        return TRUE;
    }
    daGyCtrl_c* ctrl;
    if (!fopAc_IsActor(gabi::at<void>(*parent)) || (ctrl = gabi::at<daGyCtrl_c>(*parent)) == nullptr ||
        (fpcM_GetName(ctrl) != 0xE5 && (ctrl == nullptr || fpcM_GetName(ctrl) != 0xE6))) {
        if (cLib_calcTimer_gy(&mA0C) != 0) {
            return TRUE;
        }
        fopAcM_delete(this);
        return TRUE;
    }
    shape_angle.x = current.angle.x;
    shape_angle.y = current.angle.y;
    mpCtrl = ctrl;
    shape_angle.z = current.angle.z;
    if (enemy_ice_gy(&mEnemyIce)) {
        J3DModel_setBaseTRMtx(mpMorf->getModel(), mDoMtx_stack_c::get());
        mpMorf->calc();
        mpMorf2->calc();
        return TRUE;
    }
    lineCheck(&current.pos, &mFA0);
    if (mMode != 0) {
        if (mLineHit) {
            modeDiveInit();
        }
        if (mMode != 0 && ctrl->mMode == 3) {
            modeDiveInit();
        }
    }
    m3D8.copy(ctrl->mSpawnPositions[mIndex]);
    mpMorf->play(nullptr, 0, 0);
    mpMorf->calc();
    mpMorf2->play(nullptr, 0, 0);
    mpMorf2->calc();
    if (mMode != 0) {
        setAtCollision();
        setCollision();
        checkTgHit();
    }
    if (gravity == 0.0f) {
        cLib_addCalc2(&mBodyY, mBodyYTarget, mBodyYScale, mBodyYStep);
    }
    cLib_addCalc2(&speedF, mAimSpeedF, 0.3f, 4.0f);
    setAnm();
    daGy_HIO_c* hio = gy_hio();
    if (hio->m097 != 0) {
        /* HIO: float on the water */
        gabi::Local<cXyz> pos;
        f32 x = current.pos.x;
        f32 y = current.pos.y + 1000.0f;
        f32 z = current.pos.z;
        pos->x = x;
        pos->z = z;
        pos->y = y;
        current.pos.y = dBgS_GetWaterHeight(pos);
        setMtx();
        setWave();
        return TRUE;
    }
    modeProcCall();
    fopAcM_posMoveF(this, &mStts.m_cc_move);
    current.pos.y = getWaterY();
    s8 anm = mAnmIdxNext;
    if (anm == 5) {
        f32 sy = speed.y;
        if (sy < -5.0f) {
            cLib_addCalcAngleS2(&current.angle.x, (s16)(gabi::load<s16>(0x1047BD4A) + 0x2000), 8, 0x400);
        } else if (sy > 5.0f) {
            cLib_addCalcAngleS2(&current.angle.x, (s16)(gabi::load<s16>(0x1047BD4C) - 0x1000), 8, 0x400);
        } else {
            cLib_addCalcAngleS2(&current.angle.x, 0, 8, 0x800);
        }
        if (gravity == 0.0f && mOldGravity < 0.0f) {
            mAnmIdxNext = 1;
            gy_landSplash(this, 2.0f, true, 0x58D0);
        }
    } else if (anm == 6 || anm == 4) {
        if (mMode == 4) {
            cLib_addCalcAngleS2(&current.angle.x, 0, 8, 0x800);
        }
        daSea_calcWave(current.pos.x, current.pos.z);
        if (gravity == 0.0f && mOldGravity < 0.0f) {
            gy_landSplash(this, 1.0f, false, 0x58D2);
        }
    } else if (anm == 7 || anm == 9) {
        daSea_calcWave(current.pos.x, current.pos.z);
        if (gravity == 0.0f && mOldGravity < 0.0f) {
            gy_landSplash(this, 2.0f, true, 0x58D0);
        }
    } else {
        cLib_addCalcAngleS2(&current.angle.x, 0, 8, 0x800);
    }
    if (mMode != 0) {
        mAcch.CrrPos(dComIfG_Bgsp());
    }
    gabi::store<u8>(gabi::ea(this) + 0x1C9, gabi::load<u8>(gabi::ea(this) + 0x326)); /* tevStr.mRoomNo = roomNo */
    gabi::store<u8>(gabi::ea(this) + 0x1CA, (u8)dBgS_GetPolyColor_gy(dComIfG_Bgsp(), gabi::at<void>(gabi::ea(&mAcch) + 0xE8)));
    setMtx();
    setWave();
    {
        u32 p = gabi::ea(this);
        f32 z = current.pos.z;
        f32 y = current.pos.y;
        gabi::store<f32>(p + 0x398, z); /* attention_info.position */
        eyePos.z = z;
        f32 x = current.pos.x;
        gabi::store<f32>(p + 0x390, x);
        eyePos.x = x;
        f32 g = gravity;
        gabi::store<f32>(p + 0x394, y + 160.0f);
        mOldGravity = g;
        eyePos.y = y + 60.0f;
    }
    if (mAnmIdxNext != 5 && mMode != 0) {
        gabi::Local<cXyz> move;
        cXyz_mi(&current.pos, move, &old.pos);
        f32 rate = std_sqrtf(PSVECSquareMag(move)) / 30.0f;
        if (rate > 0.0f) {
            if (!(rate < 1.0f)) {
                rate = 1.0f;
            }
        } else {
            rate = 0.0f;
        }
        s8 vol = (s8)gabi::ftoi(rate * 100.0f);
        mDoAud_seStart(0x50CE, &eyePos, vol, dComIfGp_getReverb(fopAcM_GetRoomNo(this)));
        if (mAnmIdxNext == 1) {
            f32 r = gabi::fmuls_ppc(rate, hio->m004);
            f32 lo = hio->m008;
            f32 d = gabi::fsubs_ppc(r, lo);
            f32 speedRate = d >= 0.0f ? r : lo; /* fsel */
            mpMorf->setPlaySpeed(speedRate);
            mpMorf2->setPlaySpeed(speedRate);
        }
    }
    settingTevStruct(dKy_getEnvlight(), 0, &current.pos, &tevStr);
    return TRUE;
}
VERIFY(0x021689C8, &daGy_c::_execute);

/* a function-local static (4 bytes, e.g. a GXColor) initialised from .data on first use */
static inline bool gy_initStatic(u32 guard, u32 dst, u32 src) {
    if (gabi::load<s32>(guard) != 0) return false;
    gabi::store<s32>(guard, 1);
    memcpy_g(gabi::at<void>(dst), gabi::at<void>(src), 4); /* 028FEAC0 -> memcpy */
    return true;
}
#define GY_DBG_COLOR0 0x101FDA44, 0x101FEBE8, 0x100108A0
#define GY_DBG_COLOR1 0x101FDA50, 0x101FEBF4, 0x100108A4
#define GY_DBG_COLOR2 0x101FDA48, 0x101FEBEC, 0x100108A8

/* 02169268: drawDebug (HD: the debug drawing is compiled out; only the initialisation of the
 * function-local colour statics of its inlined helpers is left) */
void daGy_c::drawDebug() {
    WWHD_FUNC(0x02169268, void, this);
    if (gy_initStatic(GY_DBG_COLOR0) && gy_initStatic(GY_DBG_COLOR0)) gy_initStatic(GY_DBG_COLOR0);
    if (gy_initStatic(GY_DBG_COLOR1) && gy_initStatic(GY_DBG_COLOR1)) gy_initStatic(GY_DBG_COLOR1);
    gy_initStatic(GY_DBG_COLOR2);
    if (dComIfGp_getShipActor_gy() != nullptr) {
        if (gy_initStatic(GY_DBG_COLOR0)) gy_initStatic(GY_DBG_COLOR0);
    }
    if (dComIfGp_getShipActor_gy() != nullptr) {
        if (gy_initStatic(GY_DBG_COLOR1)) gy_initStatic(GY_DBG_COLOR1);
    }
    dComIfGp_ea();
    if (gy_initStatic(GY_DBG_COLOR1)) gy_initStatic(GY_DBG_COLOR1);
    gy_initStatic(GY_DBG_COLOR0);
}
VERIFY(0x02169268, &daGy_c::drawDebug);

/* 021694E0 */
BOOL daGy_c::_draw() {
    WWHD_FUNC(0x021694E0, BOOL, this);
    daGy_HIO_c* hio = gy_hio();
    if (hio->mDebugOffset != 0) {
        drawDebug();
    }
    if ((mBodyY > hio->m0A0 || mAnmIdxNext == 5) && hio->m096 == 0) {
        mDoExt_McaMorf* morf = mpMorf;
        setLightTevColorType(dKy_getEnvlight(), morf->getModel(), &tevStr);
        if (mEnemyIce.mFreezeTimer > 20) {
            mDoExt_invisibleModel_entryDL_gy(mpMorf, -1, mInvisibleModel);
            return TRUE;
        }
        mpMorf->entryDL();
        dSnap_RegistFig(0xBC, this, 1.0f, 1.0f, 1.0f);
        mpMorf2->entryDL();
    }
    return TRUE;
}
VERIFY(0x021694E0, &daGy_c::_draw);

/* 021695C0 */
void daGy_c::modeCircleInit() {
    WWHD_FUNC(0x021695C0, void, this);
    daGy_HIO_c* hio = gy_hio();
    mMode = 1;
    s32 target = mpCtrl->mTarget;
    if (target == 0) {
        f32 r = cM_rndF((f32)(hio->m118 - hio->m116));
        mTimer = gabi::ftoi(r + (f32)hio->m116);
    } else if (target == 1) {
        f32 r = cM_rndF((f32)(hio->m114 - hio->m112));
        mTimer = gabi::ftoi(r + (f32)hio->m112);
    }
    mApproachRate = 0.0f;
}
VERIFY(0x021695C0, &daGy_c::modeCircleInit);

/* close in on the spawn point and face away from the controller's centre */
static inline void gy_approachHome(daGy_c* i_this, daGyCtrl_c* ctrl) {
    gabi::Local<cXyz> toHome;
    i_this->mAimSpeedF = 0.0f;
    cXyz_mi(&i_this->m3D8, toHome, &i_this->current.pos);
    cLib_addCalc2(&i_this->mApproachRate, 1.0f, 0.01f, 0.05f);
    gabi::Local<cXyz> step;
    cXyz_ml(toHome, step, i_this->mApproachRate);
    gabi::Local<cXyz> pos;
    cXyz_pl(&i_this->current.pos, pos, step);
    i_this->current.pos.copy(*pos);
    cLib_addCalcAngleS2(&i_this->current.angle.y, (s16)(ctrl->mSpawnAngles[i_this->mIndex] + 0x8000), 4, 0x400);
}
/* the horizontal distance to the spawn point (or another point) */
static inline f32 gy_homeDistXZ(daGy_c* i_this, cXyz* to = nullptr) {
    gabi::Local<cXyz> diff;
    cXyz_mi(&i_this->current.pos, diff, to != nullptr ? to : &i_this->m3D8);
    gabi::Local<cXyz> diffXZ;
    diffXZ->x = diff->x;
    diffXZ->y = 0.0f;
    diffXZ->z = diff->z;
    return std_sqrtf(PSVECSquareMag(diffXZ));
}

/* 02169798 */
void daGy_c::modeDive() {
    WWHD_FUNC(0x02169798, void, this);
    if (mAnmIdxNext == 5) {
        return;
    }
    daGyCtrl_c* ctrl = mpCtrl;
    daGy_HIO_c* hio = gy_hio();
    mAnmIdxNext = 1;
    if (ctrl->mMode == 3 || mLineHit) {
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &m3D8), 8, 0x400);
        mAimSpeedF = 0.0f;
        f32 y = hio->m0A0;
        mBodyYTarget = y + y;
        actor_status = actor_status & ~0x20u;
        mBodyYStep = hio->m0C8;
        gy_attnFlags(this) = gy_attnFlags(this) & ~4u;
        return;
    }
    actor_status = actor_status | 0x20;
    gy_attnFlags(this) = gy_attnFlags(this) | 4;
    mBodyYTarget = hio->m0A0;
    mBodyYStep = hio->m0C8;
    f32 dist = fopAcM_searchActorDistance(this, dComIfGp_getPlayer(0));
    if (ctrl->mPathFree[mIndex] != 0 && mBodyY < hio->m0A0 + 10.0f && dist > hio->m148) {
        modeCircleInit();
        return;
    }
    fopAc_ac_c* ship = dComIfGp_getShipActor_gy();
    f32 len = gy_homeDistXZ(this);
    if (ship == nullptr) {
        return;
    }
    if (len > hio->m144 || ship->speedF > hio->m06C) {
        setAimSpeedF();
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &m3D8), 8, 0x400);
        mApproachRate = 0.0f;
        return;
    }
    gy_approachHome(this, ctrl);
}
VERIFY(0x02169798, &daGy_c::modeDive);

/* 02169BEC */
void daGy_c::modeCircle() {
    WWHD_FUNC(0x02169BEC, void, this);
    if (mAnmIdxNext == 5) {
        return;
    }
    daGy_HIO_c* hio = gy_hio();
    daGyCtrl_c* ctrl = mpCtrl;
    mAnmIdxNext = 1;
    mBodyYTarget = hio->m0AC;
    mBodyYStep = hio->m0C4;
    if (ctrl->mPathFree[mIndex] == 0) {
        modeDiveInit();
        return;
    }
    fopAc_ac_c* ship = dComIfGp_getShipActor_gy();
    f32 len = gy_homeDistXZ(this);
    if (ship == nullptr) {
        return;
    }
    if (len > hio->m144 || ship->speedF > hio->m06C) {
        setAimSpeedF();
        if (fopAcM_searchActorDistance(this, dComIfGp_getPlayer(0)) < hio->m148) {
            modeDiveInit();
            return;
        }
        if (ship->speedF > hio->m070) {
            modeWithCircleInit();
            return;
        }
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &m3D8), 8, 0x400);
        mApproachRate = 0.0f;
        return;
    }
    gy_approachHome(this, ctrl);
    if (cLib_calcTimer_gy(&mTimer) != 0) {
        return;
    }
    if (gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0) & 0x1000000) {
        return;
    }
    s32 target = ctrl->mTarget;
    if (target == 0) {
        modeAttackPlayerInit();
    } else if (target == 1) {
        modeAttackInit();
    }
}
VERIFY(0x02169BEC, &daGy_c::modeCircle);

/* the head rammed a boat (process 0xA5): shake, sounds, swim back */
static inline bool gy_hitShip(daGy_c* i_this) {
    if (!i_this->mHeadSph.ChkCoHit()) return false;
    fopAc_ac_c* ac = i_this->mHeadSph.GetCoHitAc();
    if (ac == nullptr || fpcM_GetName(ac) != 0xA5) return false;
    return true;
}
static inline void gy_shipHitReaction(daGy_c* i_this) {
    gabi::Local<cXyz> dir;
    dir->z = 0.0f;
    dir->x = 0.0f;
    dir->y = 1.0f;
    dComIfGp_getVibration_StartShock(7, -0x21, dir);
    gy_seStart(i_this, 0x58D3, 0);
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(i_this));
    mDoAud_monsSeStart_gy(0x48CF, &i_this->eyePos, gabi::load<u32>(gabi::ea(i_this) + 4), 0, reverb);
    i_this->modeAttackBackInit();
}

/* 02169FB8 */
void daGy_c::modeAttack() {
    WWHD_FUNC(0x02169FB8, void, this);
    if (mpCtrl->mTarget == 0) {
        modeCircleInit();
        return;
    }
    daGy_HIO_c* hio = gy_hio();
    mBodyYTarget = hio->m0B0;
    mBodyYStep = hio->m0C4;
    if (cLib_calcTimer_gy(&mTimer) == 0) {
        modeCircleInit();
    }
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (fopAcM_searchActorDistance(this, dComIfGp_getPlayer(0)) < hio->m148) {
        mAimSpeedF = 0.0f;
    } else {
        mAimSpeedF = hio->m064;
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &player->current.pos), 8, 0x400);
    }
    if (gy_hitShip(this)) {
        gy_shipHitReaction(this);
    }
}
VERIFY(0x02169FB8, &daGy_c::modeAttack);

/* 0216A18C */
void daGy_c::modeAttackPlayer() {
    WWHD_FUNC(0x0216A18C, void, this);
    if (mpCtrl->mTarget == 1) {
        modeCircleInit();
        return;
    }
    daGy_HIO_c* hio = gy_hio();
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    u32 sub = mSubMode;
    if (sub == 0) {
        /* approach */
        mBodyYTarget = hio->m0B4;
        mBodyYStep = hio->m0C4;
        mAimSpeedF = hio->m058;
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &player->current.pos), 8, 0x400);
        gabi::Local<cXyz> diff;
        cXyz_mi(&mHeadPos, diff, &player->current.pos);
        gabi::Local<cXyz> diffXZ;
        diffXZ->x = diff->x;
        diffXZ->y = 0.0f;
        diffXZ->z = diff->z;
        if (std_sqrtf(PSVECSquareMag(diffXZ)) < hio->m148) {
            mAnmIdxNext = 10;
            mSubMode = mSubMode + 1;
        }
    } else if (sub == 1) {
        /* bite */
        mBodyYTarget = hio->m0B4;
        mBodyYStep = hio->m0C4;
        mAimSpeedF = hio->m05C;
        if (mAnmIdxNext == 10 && mpMorf->isStop()) {
            mAnmIdxNext = 2;
            setAnm();
            mAnmIdxNext = 10;
            mBiteCount = mBiteCount + 1;
        }
        if (mBiteCount > hio->m198) {
            mAnmIdxNext = 11;
            mSubMode = mSubMode + 1;
        }
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &player->current.pos), 8, 0x400);
    } else if (sub == 2) {
        /* the final bite */
        if (mHeadSph.ChkCoHit()) {
            fopAc_ac_c* ac = mHeadSph.GetCoHitAc();
            if (ac != nullptr && fpcM_GetName(ac) == 0xA8 && mHitPlayerShip == 0) {
                s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(this));
                mDoAud_monsSeStart_gy(0x48CF, &eyePos, gabi::load<u32>(gabi::ea(this) + 4), 0, reverb);
                gabi::Local<cXyz> dir;
                dir->z = 0.0f;
                dir->x = 0.0f;
                dir->y = 1.0f;
                dComIfGp_getVibration_StartShock(4, -0x21, dir);
                mHitPlayerShip = 1;
            }
        }
        f32 thr = gabi::load<f32>(0x1047BCF8) + 10.0f; /* HD debug offset */
        if (mpMorf->getFrame() > thr) {
            mBodyYTarget = hio->m0A0;
        } else {
            mBodyYTarget = hio->m0B4;
        }
        mBodyYStep = hio->m0C4;
        mAimSpeedF = hio->m060;
        if (mAnmIdxNext == 11 && mpMorf->isStop()) {
            mAnmIdxNext = 1;
            mHitPlayerShip = 0;
            modeDiveInit();
        }
    }
}
VERIFY(0x0216A18C, &daGy_c::modeAttackPlayer);

/* 0216A544 */
void daGy_c::modeAttackBack() {
    WWHD_FUNC(0x0216A544, void, this);
    daGy_HIO_c* hio = gy_hio();
    mAimSpeedF = 0.0f;
    mBodyYTarget = hio->m0A0;
    cLib_addCalcAngleS2(&mA10, 0, 4, 0x800);
    if (gy_homeDistXZ(this, &mA20) > hio->m144) {
        setAimSpeedF();
        if (mAnmIdxNext == 1) {
            modeWithCircleInit();
        }
        cLib_addCalcAngleS2(&current.angle.y, cLib_targetAngleY(&current.pos, &mA20), 8, 0x400);
    } else if (mAnmIdxNext == 1) {
        modeCircleInit();
    }
}
VERIFY(0x0216A544, &daGy_c::modeAttackBack);

/* 0216AA10 */
void daGy_c::modeWithCircle() {
    WWHD_FUNC(0x0216AA10, void, this);
    if (mAnmIdxNext == 5) {
        return;
    }
    daGy_HIO_c* hio = gy_hio();
    s32 index = mIndex;
    mBodyYTarget = hio->m0AC;
    mBodyYStep = hio->m0C4;
    if (mpCtrl->mPathFree[index] == 0) {
        modeDiveInit();
        return;
    }
    fopAc_ac_c* ship = dComIfGp_getShipActor_gy();
    if (ship == nullptr) {
        return;
    }
    if (ship->speedF < hio->mSpeedFast) {
        modeCircleInit();
    }
    cLib_addCalcAngleS2(&current.angle.y, ship->shape_angle.y, 8, 0x400);
    s16 angle = fopAcM_searchActorAngleY(ship, this);
    s32 dAngle = cLib_distanceAngleS(ship->shape_angle.y, angle);
    f32 dist = fopAcM_searchActorDistance(this, dComIfGp_getPlayer(0));
    if (dAngle < hio->m142 && dist > hio->m09C) {
        mAimSpeedF = ship->speedF - 5.0f;
    } else {
        mAimSpeedF = ship->speedF + hio->m040;
    }
    if (dAngle >= hio->m140) {
        return;
    }
    if (dAngle > hio->m142 && dist > hio->m09C) {
        /* jump out of the water next to the ship */
        speed.y = hio->m138;
        gy_seStart(this, 0x58CF, 0);
        mAnmIdxNext = 5;
        gy_deathSplash(this);
        modeWithAttackInit();
    }
}
VERIFY(0x0216AA10, &daGy_c::modeWithCircle);

/* 0216A6F0 */
void daGy_c::modeWithAttack() {
    WWHD_FUNC(0x0216A6F0, void, this);
    daGyCtrl_c* ctrl = mpCtrl;
    if (mAnmIdxNext != 5) {
        mAnmIdxNext = 3;
    }
    if (ctrl->mTarget == 0) {
        modeCircleInit();
        return;
    }
    daGy_HIO_c* hio = gy_hio();
    s16 angle = cLib_targetAngleY(&current.pos, &dComIfGp_getPlayer(0)->current.pos);
    f32 dist = fopAcM_searchActorDistance(this, dComIfGp_getPlayer(0));
    u32 sub = mSubMode;
    if (sub == 0) {
        mBodyYTarget = hio->m0AC;
        mBodyYStep = hio->m0C4;
        cLib_addCalcAngleS2(&current.angle.y, angle, 8, 0x400);
        mAimSpeedF = hio->m068;
        if (dist < hio->m044) {
            mSubMode = mSubMode + 1;
            mTimer = hio->m10E;
        }
    } else if (sub == 1) {
        mBodyYTarget = hio->m0B0;
        mBodyYStep = hio->m0C4;
        mAimSpeedF = hio->m068;
        if (gy_hitShip(this)) {
            gy_shipHitReaction(this);
            return;
        }
    } else if (sub == 2) {
        f32 y = hio->m0A0;
        f32 d = mBodyY - y;
        mBodyYTarget = y;
        mBodyYStep = hio->m0C4;
        if (std::fabs(d) < 10.0f) {
            fopAcM_delete(this);
        }
        return;
    } else {
        return;
    }
    if (cLib_calcTimer_gy(&mTimer) != 0) {
        return;
    }
    if (hio->m099 == 0) {
        modeWithCircleInit();
        return;
    }
    mSubMode = mSubMode + 1;
}
VERIFY(0x0216A6F0, &daGy_c::modeWithAttack);

/* pushed back from the player by the damage speed */
static inline void gy_damagePush(daGy_c* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    gabi::Local<cXyz> push;
    gabi::Local<cXyz> move;
    f32 sp = i_this->mDamageSpeed;
    move->x = 0.0f;
    push->x = 0.0f;
    push->y = 0.0f;
    push->z = -sp;
    move->y = 0.0f;
    move->z = 0.0f;
    mDoMtx_YrotS(mDoMtx_stack_c::get(), fopAcM_searchActorAngleY(i_this, player));
    PSMTXMultVec(mDoMtx_stack_c::get(), push, move);
    i_this->current.pos.x = i_this->current.pos.x + move->x;
    i_this->current.pos.y = i_this->current.pos.y + move->y;
    i_this->current.pos.z = i_this->current.pos.z + move->z;
    cLib_addCalc0(&i_this->mDamageSpeed, 1.0f, 2.0f);
}

/* 0216AC30 */
void daGy_c::modeDamage() {
    WWHD_FUNC(0x0216AC30, void, this);
    f32 sp = mDamageSpeed;
    mAimSpeedF = 0.0f;
    if (sp > 0.01f) {
        gy_damagePush(this);
    }
    if (mAnmIdxNext == 6 && mpMorf->isStop()) {
        s8 old = mAnmIdxOld;
        mAnmIdxNext = 1;
        if (old == 0) {
            modeDiveInit();
            return;
        }
        if ((u32)old == 6) {
            modeWithCircleInit();
            return;
        }
        modeCircleInit();
    }
}
VERIFY(0x0216AC30, &daGy_c::modeDamage);

/* 0216ADD8 */
void daGy_c::modeDelete() {
    WWHD_FUNC(0x0216ADD8, void, this);
    if (mLightTimer != -1) {
        if (cLib_calcTimer_gy(&mLightTimer) == 0) {
            mEnemyIce.mLightShrinkTimer = 1;
        }
        return;
    }
    if (mIceTimer != -1) {
        if (cLib_calcTimer_gy(&mIceTimer) == 0) {
            mEnemyIce.mFreezeDuration = gy_hio()->m00C;
        }
        return;
    }
    if (mAnmIdxNext == 8 && mpMorf->isStop()) {
        fopAcM_createDisappear(this, &current.pos, 10, 0, 0xFF);
        fopAcM_delete(this);
    }
}
VERIFY(0x0216ADD8, &daGy_c::modeDelete);

/* 0216AEC8 */
void daGy_c::modeDeleteBomb() {
    WWHD_FUNC(0x0216AEC8, void, this);
    if (mDamageSpeed > 0.01f) {
        gy_damagePush(this);
    }
    daGy_HIO_c* hio = gy_hio();
    s8 anm = mAnmIdxNext;
    if (anm == 7) {
        mBodyYTarget = hio->m168;
        if (!(mBodyY > hio->m168 + 10.0f)) {
            mAnmIdxNext = 9;
            mBodyYStep = hio->m0C4;
        }
        anm = mAnmIdxNext;
    }
    if (anm != 9) {
        return;
    }
    f32 bottom = hio->m16C;
    f32 y = mBodyY;
    if (!(y < bottom - 10.0f)) {
        if ((f32)(s32)mDisappearTimer == -1.0f) {
            mDisappearTimer = hio->m164;
        }
        mBodyYStep = hio->m178;
        y = mBodyY;
        mBodyYScale = hio->m17C;
        bottom = hio->m16C;
    }
    /* bob between bottom - m170 and bottom + m170 */
    f32 range = hio->m170;
    f32 low = bottom - range;
    f32 high = bottom + range;
    if (!(y > low + 10.0f)) {
        mBodyYTarget = high;
    } else if (!(y < high - 10.0f)) {
        mBodyYTarget = low;
    }
    if ((f32)(s32)mDisappearTimer != -1.0f && cLib_calcTimer_gy(&mDisappearTimer) == 0) {
        fopAcM_createDisappear(this, &current.pos, 10, 0, 0xFF);
        fopAcM_delete(this);
    }
}
VERIFY(0x0216AEC8, &daGy_c::modeDeleteBomb);
