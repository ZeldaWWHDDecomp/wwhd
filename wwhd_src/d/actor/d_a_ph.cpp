/**
 * d_a_ph.cpp (WWHD)
 * Enemy - Peahat / Seahat
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_ph.cpp) has only "Nonmatching" stubs for this actor: the functions here are
 * written from the WWHD code (cking.rpx), with the GameCube names and signatures, and verified
 * against it.
 */
#include "d/actor/d_a_ph.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/tu.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1003443C,
    .objAcch = {0x10034464, 0x10034484, 0x10034474},
};
#define PH_VTBL 0x10034494  /* ph_class vtable (HD virtual destructor) */
#define AAB_VTBL 0x10034454 /* this TU's cM3dGAab vtable */

enum { DSNAP_TYPE_PH = 0xB1, DSNAP_TYPE_SH = 0xBB };
enum { JA_SE_CM_PH_PROPELLER = 0x5857, JA_SE_CM_SH_PROPELLER = 0x5127 };

// HD: this translation unit uses the renderer's current model and matrix.
static s8 l_phFlameJoints[10] = {0, 1, 2, 3, -1, -1, -1, -1, -1, -1};
static s8 l_shFlameJoints[10] = {0, 1, 2, -1, -1, -1, -1, -1, -1, -1};
static s8 l_propellerFlameJoints[10] = {0, 1, 3, 4, 5, 7, 8, 9, 11, 12};
static f32 l_bodyFlameScale[10] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static f32 l_propellerFlameScale[10] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static dCcD_SrcSph body_sph_src;
static dCcD_SrcCyl at_cyl_src;
static dCcD_SrcCyl tg_cyl_src;
static f32 l_farPlaneScale;
static u8 l_boomerangReaction;
static u32 l_staticListHead;
static u32 l_staticConstructors;
static PhJointHitSourceHD l_phJointHits;
static PhJointHitSourceHD l_shJointHits;
static PhJ3DSysModelHD l_currentModel;
static Mtx34 l_currentMtx;

enum { PH_CHK_ALL, PH_CHK_WALL, PH_CHK_NONE };

static BOOL nodeCallBack_UP(J3DNode *node, int calcTiming);
static BOOL nodeCallBack_DW(J3DNode *node, int calcTiming);
static BOOL daPH_Draw(ph_class *i_this);
void anm_init(ph_class *i_this, int bckFileIdx, float morf, unsigned char loopMode, float playSpeed,
              int soundFileIdx, int part);
void puropera_sound(ph_class *i_this);
void puropera_kaiten(ph_class *i_this);
void fuwafuwa_set(ph_class *i_this);
BOOL ph_wall_hit_check(ph_class *i_this);
BOOL ph_hani_check(ph_class *i_this, float range, float height, unsigned char useHome);
void dead_item(ph_class *i_this);
void BG_check(ph_class *i_this);
void UP_draw_SUB(ph_class *i_this);
void DW_draw_SUB(ph_class *i_this);
static BOOL daPH_IsDelete(ph_class *);
static BOOL daPH_Delete(ph_class *i_this);
static enemyfire *enemyfire_ct(enemyfire *self);
static ph_class *ph_class_ct(ph_class *self);
static void __sinit_d_a_ph_cpp();
static void SafeString_dt(void *p, s32 flags);
static void ph_class_dt(ph_class *self, s32 flags);
static void ph_emptyVirtual(void *);
void fly_angle_set(ph_class *i_this, unsigned char mode);
BOOL hajiki_check(ph_class *i_this);
int sea_water_check(ph_class *i_this, unsigned char mode);
static BOOL useHeapInit(fopAc_ac_c *a_this);
static cPhs_State daPH_Create(fopAc_ac_c *a_this);
BOOL body_atari_check(ph_class *i_this);
void ph_wind_move(ph_class *i_this);
void ph_water_move(ph_class *i_this);
static BOOL daPH_Execute(ph_class *i_this);

static inline J3DModel *j3dSys_getModel() {
    return l_currentModel.mpModel;
}

static inline Mtx34 *J3DSys_mCurrentMtx() {
    return &l_currentMtx;
}

/* float -> u32 (GHS: fctiwz with the 2^31 adjustment) */
static inline u32 ftou(f32 f) {
    if (f < 2147483648.0f)
        return (u32)gabi::ftoi(f);
    return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}

static inline void ph_crr_pos(ph_class *i_this) {
    PSVECAdd(&i_this->current.pos, &i_this->m02E4, &i_this->current.pos);
    PSVECAdd(&i_this->old.pos, &i_this->m02E4, &i_this->old.pos);
    f32 o = i_this->m0380;
    i_this->current.pos.y = i_this->current.pos.y - o;
    i_this->old.pos.y = i_this->old.pos.y - o;
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    i_this->current.pos.y = i_this->current.pos.y + i_this->m0380;
    i_this->old.pos.y = i_this->old.pos.y + i_this->m0380;
    phNative_PSVECSubtract(&i_this->current.pos, &i_this->m02E4, &i_this->current.pos);
    phNative_PSVECSubtract(&i_this->old.pos, &i_this->m02E4, &i_this->old.pos);
}

// HD: the sound helper retains the native null checks.
static inline bool ph_seStart(ph_class *i_this, u32 id, u32 param) {
    if (i_this == nullptr || gabi::ea(&i_this->eyePos) == 0)
        return false;
    mDoAud_seStart(id, &i_this->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
    return true;
}

/* bounce off the player after a hit */
static inline BOOL ph_hajiki_set(ph_class *i_this, fopAc_ac_c *player) {
    i_this->current.angle.y =
        cM_atan2s(i_this->current.pos.x + i_this->m02E4.x - player->current.pos.x,
                  i_this->current.pos.z + i_this->m02E4.z - player->current.pos.z);
    i_this->m033F = PH_ACTION_BOUNCE;
    i_this->mAtCyl.OffAtSPrmBit(1);
    i_this->speedF = i_this->mType != PH_TYPE_PEAHAT ? 48.0f : 28.0f;
    i_this->m0346 = 10;
    return TRUE;
}

static inline void ph_ripple_end(ph_class *i_this) {
    dPa_rippleEcallBack_end((dPa_rippleEcallBack *)&i_this->mParticleCallBack);
}

static inline int ph_leave_water(ph_class *i_this) {
    if (i_this->m0341 != 0) {
        ph_ripple_end(i_this);
        i_this->m0398 = 0.0f;
        i_this->m0341 = 0;
    }
    return 0;
}

static inline u16 J3DModelData_getJointNum(J3DModelData *d) {
    return d->getJointNum();
}

/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index
 * checked against +4) */
static inline void setJointCallBack(J3DModelData *d, u16 i, u32 cb) {
    d->getJointNodePointer(i)->mCallBack = cb;
}

static inline s32 dComIfGp_CharTbl_GetIndex(const char *name, s32 p) {
    return phNameTableGetIndex(phNameTable(), name, p);
}

static inline s16 ph_id_angle(ph_class *i_this) {
    return (s16)((fopAcM_GetID(i_this) << 13) & 0x1FE000);
}

static inline void copy_bits(cXyz *dst, const cXyz *src) {
    const PhVectorWordsHD *source = phVectorWords(src);
    u32 x = source->x;
    u32 y = source->y;
    u32 z = source->z;
    PhVectorWordsHD *target = phVectorWords(dst);
    target->x = x;
    target->y = y;
    target->z = z;
}

/* the player's cut type (daPy_lk_c, u8 at +0x3AC) */
static inline u8 daPy_getCutType(fopAc_ac_c *player) {
    return ((cc_playerView *)player)->mCutType;
}

static inline void ph_cc_at_check(ph_class *i_this, CcAtInfo *atInfo) {
    atInfo->mpObj = (dCcD_GObjInf *)i_this->mBodySph.GetTgHitObj();
    phNative_cc_at_check(i_this, atInfo);
}

static inline void ph_set_hitpos(ph_class *i_this, cXyz *pos) {
    f32 x = i_this->mBodySph.mGObjTg.mHitPos.x;
    f32 z = i_this->mBodySph.mGObjTg.mHitPos.z;
    f32 y = i_this->mBodySph.mGObjTg.mHitPos.y;
    pos->z = z;
    pos->y = y;
    pos->x = x;
}

static inline void ph_hit_particle(u16 id, cXyz *pos, fopAc_ac_c *player, cXyz *scale) {
    dPa_control_set(dComIfGp_getParticle(), 0, id, pos, &player->shape_angle, scale, 0xFF, nullptr,
                    -1, nullptr, nullptr, nullptr);
}

/* the hit sound pair of the propeller blade: fopAcM_seStart + fopAcM_monsSeStart (inline null
 * checks) */
static inline void ph_blade_se(ph_class *i_this, bool checkThis) {
    if ((checkThis && i_this == nullptr) || gabi::ea(&i_this->eyePos) == 0)
        return;
    mDoAud_seStart(0x5858, &i_this->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
    s8 roomNo = fopAcM_GetRoomNo(i_this);
    u32 pid = fopAcM_GetID(i_this);
    phNative_mDoAud_monsSeStart(0x485C, &i_this->eyePos, pid, 0, dComIfGp_getReverb(roomNo));
}

/* water_move start: drop into the water, blades stopped */
static inline void ph_water_start(ph_class *i_this) {
    i_this->mBodySph.OnCoSPrmBit(1);
    i_this->shape_angle.x = 0;
    i_this->shape_angle.z = 0;
    i_this->mBodySph.mTgType = 0xFF3DFEFF;
    i_this->current.angle.x = 0;
    i_this->current.angle.z = 0;
    for (int i = 0; i < 7; i++)
        i_this->mWobbleAngles[i] = 0;
    i_this->mAtCyl.OffAtSPrmBit(1);
}

static inline void ph_water_anm(ph_class *i_this) {
    f32 r = i_this->m02FC.x == 0.0f ? 200.0f : 100.0f;
    i_this->m0366 = (s16)gabi::ftoi(cM_rndF(r) + r);
    anm_init(i_this, 0x1F, 1.0f, 0, 1.0f, -1, 0);
    anm_init(i_this, 0x1A, 1.0f, 0, 1.0f, -1, 1);
}

static inline void ph_zero_wobble(ph_class *i_this) {
    for (int i = 0; i < 7; i++)
        i_this->mWobbleAngles[i] = 0;
}

/* fopAcM_monsSeStart (inline: checks eyePos, then the process id of a possibly NULL actor) */
static inline void ph_mons_se(ph_class *i_this, u32 id) {
    if (gabi::ea(&i_this->eyePos) == 0)
        return;
    u32 pid = fopAcM_GetID(i_this);
    s8 roomNo = fopAcM_GetRoomNo(i_this);
    phNative_mDoAud_monsSeStart(id, &i_this->eyePos, pid, 0, dComIfGp_getReverb(roomNo));
}

/* fopAcM_seStart (inline, HD null checks) */
static inline void ph_se(ph_class *i_this, u32 id, u32 param, bool checkThis) {
    if ((checkThis && i_this == nullptr) || gabi::ea(&i_this->eyePos) == 0)
        return;
    mDoAud_seStart(id, &i_this->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
}

/* the attack arm on (At bit + hit flag) */
static inline void ph_at_on(ph_class *i_this) {
    i_this->mAtCyl.OnAtSPrmBit(1);
    i_this->mAtCyl.mObjAt.mRPrm = 1;
}

/* ph_fujyou_move growth step: returns true when the body animation has ended */
static inline bool ph_fujyou_grow(ph_class *i_this) {
    cLib_addCalc2(&i_this->m02FC.x, i_this->m03A0 + 1.0f, 1.0f, i_this->m03A0 + 0.6f);
    f32 lim = i_this->m03A0 + 0.8f;
    f32 s = i_this->m02FC.x;
    mDoExt_McaMorf *morf = i_this->mpBodyMorf;
    i_this->m02FC.z = s;
    i_this->m02FC.y = s;
    if (s > lim) {
        f32 t = i_this->m03A0 + 1.0f;
        i_this->m02FC.x = t;
        i_this->m02FC.y = t;
        i_this->m02FC.z = t;
    }
    if (!morf->isStop())
        return false;
    i_this->m0384 = 0.0f;
    i_this->m034A = 0x1000;
    i_this->m034C = 0;
    ph_zero_wobble(i_this);
    i_this->m0346++;
    i_this->gravity = -3.0f;
    return true;
}

/* ph_fujyou_move rising step (propeller turning) */
static inline void ph_fujyou_rise(ph_class *i_this, f32 target, f32 scale, f32 step) {
    i_this->m033A += (s16)gabi::ftoi(i_this->m0384 * 1000.0f);
    puropera_sound(i_this);
    cLib_addCalc2(&i_this->m0384, target, scale, step);
    i_this->m035E += (s16)gabi::ftoi(i_this->m0384 * 200.0f);
    cLib_addCalcAngleS2(&i_this->current.angle.z, (s16)gabi::ftoi(cM_ssin(i_this->m035E) * 4000.0f),
                        1, 0x1000);
    i_this->m0356 += 2000;
    i_this->m0358 += 0x4B0;
    i_this->speed.y = i_this->m0384;
    i_this->m035A += 0x4B0;
    i_this->shape_angle.z = i_this->current.angle.z;
    i_this->m02F0.y = cM_ssin(i_this->m0356) * 10.0f;
    i_this->m02F0.x = cM_scos(i_this->m0358) * 20.0f;
    i_this->m02F0.z = cM_scos(i_this->m035A) * 20.0f;
}

static inline void ph_dead_se(ph_class *i_this) {
    ph_mons_se(i_this, i_this->mType == PH_TYPE_PEAHAT ? 0x485E : 0x48EF);
}

static inline void ph_to_water(ph_class *i_this, int water) {
    i_this->m033F = PH_ACTION_WATER;
    i_this->m0346 = water == 2 ? 0x3C : 0x46;
}

/* 023C9678 */
static BOOL nodeCallBack_UP(J3DNode *node, int calcTiming) {

    if (calcTiming == 0) {
        J3DModel *model = j3dSys_getModel();
        ph_class *i_this = (ph_class *)model->getUserArea();
        J3DJoint *joint = J3DNode_toJoint(node);
        s32 jntNo = phJointData(joint)->mJointNo;
        if (i_this != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            if (jntNo == 0) {
                cXyz offset;
                offset.x = 0.0f;
                offset.y = -80.0f;
                offset.z = 0.0f;
                MtxPosition(&offset, &i_this->m02D8);
                mtx_copy(model->getAnmMtx(jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
            }
        }
    }
    return TRUE;
}

/* 023C97BC */
static BOOL nodeCallBack_DW(J3DNode *node, int calcTiming) {

    if (calcTiming == 0) {
        J3DModel *model = j3dSys_getModel();
        ph_class *i_this = (ph_class *)model->getUserArea();
        J3DJoint *joint = J3DNode_toJoint(node);
        s32 jntNo = phJointData(joint)->mJointNo;
        if (i_this != nullptr) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            cXyz offset;
            if (jntNo == 1) {
                offset.z = 0.0f;
                offset.x = 0.0f;
                offset.y = 0.0f;
                MtxPosition(&offset, &i_this->m02C0);
                mtx_copy(model->getAnmMtx(jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
            } else if (jntNo == 0) {
                offset.z = 0.0f;
                offset.x = 0.0f;
                offset.y = 0.0f;
                MtxPosition(&offset, &i_this->m02CC);
                mtx_copy(model->getAnmMtx(jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
            }
        }
    }
    return TRUE;
}

/* 023C9994 */
static BOOL daPH_Draw(ph_class *i_this) {

    J3DModel *bodyModel = i_this->mpBodyMorf->getModel();
    J3DModel *propModel = i_this->mpPropellerMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), bodyModel, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), propModel, &i_this->tevStr);

    phNative_dSnap_RegistFig(i_this->mType == PH_TYPE_SEAHAT ? DSNAP_TYPE_SH : DSNAP_TYPE_PH,
                             i_this, &i_this->attention_info.position, i_this->shape_angle.y, 1.0f,
                             1.0f, 1.0f);

    if (i_this->mEnemyIce.mFreezeTimer > 20) {
        /* frozen: draw the ice */
        phNative_mDoExt_invisibleModel_entryDL(i_this->mpBodyMorf, -1,
                                               &i_this->mBodyInvisibleModel);
        if (i_this->m02FC.x != 0.0f && i_this->m037C == 0.0f) {
            phNative_mDoExt_invisibleModel_entryDL(i_this->mpPropellerMorf, -1,
                                                   &i_this->mPropellerInvisibleModel);
        }
        return TRUE;
    }

    dComIfGp_get(); /* HD: remnant of the shadow code */
    i_this->mpBodyMorf->entryDL();
    i_this->mpPropellerMorf->entryDL();
    cXyz shadowPos;
    shadowPos.x = i_this->current.pos.x;
    shadowPos.y = i_this->current.pos.y;
    shadowPos.z = i_this->current.pos.z;
    PSVECAdd(&shadowPos, &i_this->m02E4, &shadowPos);
    return TRUE;
}

/* 023C9AD8 */
void anm_init(ph_class *i_this, int bckFileIdx, float morf, unsigned char loopMode, float playSpeed,
              int soundFileIdx, int part) {

    const char *arc;
    mDoExt_McaMorf *pMorf;
    if (i_this->mType == PH_TYPE_PEAHAT) {
        arc = "Ph"; /* "Ph" */
        if (part == 0) {
            i_this->m0374 = bckFileIdx;
        }
    } else {
        arc = "Sh"; /* "Sh" */
    }
    if (soundFileIdx >= 0) {
        J3DAnmTransform *anm =
            (J3DAnmTransform *)dComIfG_getObjectRes(arc, bckFileIdx, gc_tu.safeString);
        void *sound = dComIfG_getObjectRes(arc, soundFileIdx, gc_tu.safeString);
        pMorf = part == 0 ? i_this->mpPropellerMorf.get() : i_this->mpBodyMorf.get();
        pMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, sound);
    } else {
        J3DAnmTransform *anm =
            (J3DAnmTransform *)dComIfG_getObjectRes(arc, bckFileIdx, gc_tu.safeString);
        pMorf = part == 0 ? i_this->mpPropellerMorf.get() : i_this->mpBodyMorf.get();
        pMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
    }
}

/* 023C9E58 */
void puropera_sound(ph_class *i_this) {

    if (i_this->mType == PH_TYPE_PEAHAT) {
        /* one sound per blade passing three angles */
        bool play = false;
        s16 flags = i_this->m0354;
        if (!(flags & 1)) {
            s16 a = i_this->m033A;
            if ((a < 0 ? -a : a) < 0x1000) {
                flags |= 1;
                play = true;
                i_this->m0354 = flags;
            }
        }
        if (!(flags & 2)) {
            s16 d = (s16)cLib_distanceAngleS(i_this->m033A, 0x6000);
            flags = i_this->m0354;
            if (d < 0x1000) {
                flags |= 2;
                play = true;
                i_this->m0354 = flags;
            }
        }
        bool sound = play;
        if (!(flags & 4)) {
            s16 d = (s16)cLib_distanceAngleS(i_this->m033A, -0x4000);
            if (d < 0x1000) {
                i_this->m0354 = i_this->m0354 | 4;
                sound = true;
            }
        }
        if (!sound) {
            return;
        }
        fopAcM_seStart(i_this, JA_SE_CM_PH_PROPELLER, 0);
        if ((i_this->m0354 & 7) == 7) {
            i_this->m0354 = 0;
        }
    } else {
        u32 vol = ftou((f32)i_this->m0348 / 81.0f);
        if (vol > 100)
            vol = 100;
        fopAcM_seStart(i_this, JA_SE_CM_SH_PROPELLER, vol);
    }
}

/* 023CA054 */
void puropera_kaiten(ph_class *i_this) {

    i_this->m033A = i_this->m033A + i_this->m0348;
    cLib_addCalcAngleS2(&i_this->m0348, i_this->m034A, 1, i_this->m034C);
    cLib_addCalcAngleS2(&i_this->m034C, 0x100, 1, 0x10);
    puropera_sound(i_this);
}

/* 023CA0BC */
void fuwafuwa_set(ph_class *i_this) {

    if (i_this->mType == PH_TYPE_PEAHAT) {
        i_this->m0356 += 700;
        i_this->m0358 += 200;
        i_this->m035A += 200;
        i_this->m02F0.y = cM_ssin(i_this->m0356) * 30.0f;
        i_this->m02F0.x = cM_scos(i_this->m0358) * 200.0f;
        i_this->m02F0.z = cM_scos(i_this->m035A) * 200.0f;
    } else {
        i_this->m0356 += (s16)gabi::ftoi(REG_F(12, 0) + 350.0f);
        i_this->m0358 += (s16)gabi::ftoi(REG_F(12, 1) + 100.0f);
        i_this->m035A += (s16)gabi::ftoi(REG_F(12, 1) + 100.0f);
        i_this->m02F0.y = cM_ssin(i_this->m0356) * (REG_F(12, 2) + 30.0f);
        i_this->m02F0.x = cM_scos(i_this->m0358) * (REG_F(12, 3) + 1000.0f);
        i_this->m02F0.z = cM_scos(i_this->m035A) * (REG_F(12, 3) + 1000.0f);
    }
}

/* 023CA258 */
void fly_angle_set(ph_class *i_this, unsigned char mode) {

    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    s16 stepY = 0x200;
    s16 targetZ = i_this->current.angle.z;
    s16 targetY = i_this->current.angle.y;
    bool wobble;
    switch (mode) {
    case 0:
        wobble = true;
        break;
    case 1:
    case 3:
        if (mode == 1) {
            f32 dz = i_this->current.pos.z - player->current.pos.z;
            f32 dx = i_this->current.pos.x - player->current.pos.x;
            f32 dy = i_this->current.pos.y - (player->current.pos.y + i_this->m0378);
            cLib_addCalcAngleS2(&i_this->current.angle.x,
                                cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz))), 1, 0x200);
        }
        targetY = fopAcM_searchPlayerAngleY(i_this);
        cLib_addCalcAngleS2(&i_this->current.angle.y, targetY, 1, 0x500);
        wobble = true;
        break;
    case 2: {
        f32 dz = i_this->current.pos.z - player->current.pos.z;
        f32 dx = i_this->current.pos.x - player->current.pos.x;
        f32 dist2 = gabi::fmadds(dx, dx, dz * dz);
        f32 dy = i_this->current.pos.y - (player->current.pos.y + i_this->m0378);
        cLib_addCalcAngleS2(&i_this->current.angle.x, cM_atan2s(dy, std_sqrtf(dist2)), 1, 0x200);
        dy = i_this->current.pos.y - player->current.pos.y;
        cLib_addCalcAngleS2(&i_this->shape_angle.x, cM_atan2s(dy, std_sqrtf(dist2)), 1, 0x200);
        stepY = 0x500;
        wobble = false;
        break;
    }
    case 4: {
        f32 dz = i_this->current.pos.z - player->current.pos.z;
        f32 dx = i_this->current.pos.x - player->current.pos.x;
        f32 dy = i_this->current.pos.y - (player->current.pos.y + i_this->m0378);
        cLib_addCalcAngleS2(&i_this->current.angle.x,
                            cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz))), 1, 0x200);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, i_this->current.angle.x, 1, 0x200);
        i_this->m035E += 3000;
        targetZ = (s16)gabi::ftoi(cM_ssin(i_this->m035E) * 7000.0f);
        wobble = false;
        break;
    }
    default:
        wobble = false;
        break;
    }
    if (wobble) {
        if (i_this->mType == PH_TYPE_PEAHAT) {
            i_this->m035C += 800;
        } else {
            i_this->m035C += 900;
        }
        cLib_addCalcAngleS2(&i_this->shape_angle.x,
                            (s16)gabi::ftoi(cM_ssin(i_this->m035C) * 4000.0f), 1, 0x200);
    }
    if (mode != 4) {
        cLib_addCalcAngleS2(&i_this->shape_angle.y, targetY, 1, stepY);
    }
    cLib_addCalcAngleS2(&i_this->shape_angle.z, targetZ, 1, 0x200);
}

/* 023CA5AC */
BOOL body_atari_check(ph_class *i_this) {

    CcAtInfo atInfo;
    for (int i = 0; i < 7; i++)
        phAttackWords(&atInfo)->mWords[i] = 0;
    cXyz hitPos;
    cXyz scale1;
    cXyz scale2;
    atInfo.pParticlePos = 0;

    i_this->mStts.Move();
    if (i_this->m036C != 0 || !i_this->mBodySph.ChkTgHit()) {
        return FALSE;
    }
    i_this->m0340 = 0;
    void *hitObj = i_this->mBodySph.GetTgHitObj();
    if (hitObj == nullptr) {
        return FALSE;
    }
    i_this->m036C = 8;
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    u32 atType = ((dCcD_GObjInf *)hitObj)->mAtType;
    fopAc_ac_c *pl;

    switch (atType) {
    case 0x2: { /* sword */
        u8 cut = daPy_getCutType(player);
        if ((cut >= 5 && cut <= 10) || cut == 0xC || (cut >= 0xE && cut <= 0x10) || cut == 0x15 ||
            cut == 0x17 || (cut >= 0x19 && cut <= 0x1B) || cut == 0x1E || cut == 0x1F) {
            i_this->m0340 = 3;
        }
        i_this->m033F = PH_ACTION_DAMAGE;
        i_this->m0346 = 0x28;
        break;
    }
    case 0x20: /* bomb */
        i_this->m0340 = 8;
        i_this->m0346 = 0x2B;
        i_this->m033F = PH_ACTION_DAMAGE;
        break;
    case 0x40: /* boomerang */
        if (i_this->m033F != PH_ACTION_SEPARATED && i_this->m02FC.x != 0.0f &&
            i_this->m037C == 0.0f) {
            i_this->m0340 = 4;
            phCancelBoomerangLock();
            l_boomerangReaction = 2;
            i_this->m0346 = 0x14;
            i_this->m033F = PH_ACTION_SEPARATED;
            return TRUE;
        }
        i_this->m0340 = 5;
        i_this->m033F = PH_ACTION_DAMAGE;
        i_this->m0346 = 0x28;
        break;
    case 0x200:
    case 0x40000: /* fire */
        i_this->mBodyEnemyFire.mFireDuration = 100;
        if (i_this->m02FC.x != 0.0f) {
            i_this->mPropellerEnemyFire.mFireDuration = 100;
        }
        i_this->m033F = PH_ACTION_DAMAGE;
        i_this->m0340 = 1;
        i_this->m0346 = 0x2B;
        break;
    case 0x4000:
        i_this->m0340 = 1;
        i_this->m0346 = 0x2B;
        i_this->m033F = PH_ACTION_DAMAGE;
        break;
    case 0x8000: /* grappling hook */
        if (i_this->mType != PH_TYPE_PEAHAT) {
            i_this->m0340 = 1;
            i_this->m0346 = 0x2B;
            i_this->m033F = PH_ACTION_DAMAGE;
            break;
        }
        i_this->m0340 = 9;
        if (i_this->m02FC.x != 0.0f && i_this->m037C == 0.0f) {
            l_boomerangReaction = 2;
            i_this->m033F = PH_ACTION_SEPARATED;
            i_this->m0346 = 0x14;
        }
        return FALSE;
    case 0x10000: /* skull hammer? */
        i_this->m033F = PH_ACTION_DAMAGE;
        i_this->m0346 = 0x2F;
        i_this->m0340 = 6;
        if (daPy_getCutType(player) == 0x11) {
            i_this->m0340 = 7;
            i_this->m0346 = 0x2B;
        }
        break;
    case 0x80000: { /* light arrow */
        PSVECAdd(&i_this->current.pos, &i_this->m02E4, &i_this->current.pos);
        i_this->m02E4.y = 0.0f;
        i_this->m02E4.x = 0.0f;
        i_this->m02F0.x = 0.0f;
        i_this->m02F0.z = 0.0f;
        i_this->m02E4.z = 0.0f;
        i_this->m02F0.y = 0.0f;
        phNative_enemy_fire_remove(&i_this->mBodyEnemyFire);
        phNative_enemy_fire_remove(&i_this->mPropellerEnemyFire);
        f32 s = i_this->m039C + 1.0f;
        i_this->m0340 = 1;
        i_this->mEnemyIce.mFreezeDuration = 200;
        i_this->m0346 = 0x2B;
        i_this->m0342 = 1;
        i_this->mEnemyIce.mParticleScale = s;
        i_this->m033F = PH_ACTION_DAMAGE;
        return FALSE;
    }
    case 0x100000: { /* ice arrow */
        PSVECAdd(&i_this->current.pos, &i_this->m02E4, &i_this->current.pos);
        u8 type = i_this->mType;
        i_this->m02F0.x = 0.0f;
        i_this->mEnemyIce.mParticleScale = 1.0f;
        i_this->mEnemyIce.mLightShrinkTimer = 1;
        i_this->m02F0.y = 0.0f;
        i_this->m02E4.z = 0.0f;
        i_this->m02F0.z = 0.0f;
        i_this->m02E4.y = 0.0f;
        i_this->m02E4.x = 0.0f;
        if (type == 1)
            i_this->mEnemyIce.mParticleScale = 4.0f;
        i_this->mEnemyIce.mYOffset = 0.0f;
        if (type == 1)
            i_this->mEnemyIce.mYOffset = 100.0f;
        i_this->attention_info.flags = 0; /* attention_info.flags */
        if (gabi::ea(&i_this->eyePos) != 0) {
            u32 se = i_this->mType == PH_TYPE_PEAHAT ? 0x485E : 0x48EF;
            s8 roomNo = fopAcM_GetRoomNo(i_this);
            u32 pid = fopAcM_GetID(i_this);
            phNative_mDoAud_monsSeStart(se, &i_this->eyePos, pid, 0, dComIfGp_getReverb(roomNo));
        }
        i_this->m0340 = 1;
        i_this->m0346 = 0x2B;
        i_this->m033F = PH_ACTION_DAMAGE;
        break;
    }
    case 0x200000: /* hammer: knocked down */
        i_this->m0340 = 2;
        i_this->m033F = PH_ACTION_WIND;
        i_this->m0346 = 0x32;
        i_this->mAtCyl.OffAtSPrmBit(1);
        return FALSE;
    case 0x8000000: /* hookshot? */
        if (i_this->mType != PH_TYPE_PEAHAT) {
            return FALSE;
        }
        if (i_this->m02FC.x != 0.0f && i_this->m037C == 0.0f) {
            /* knock off a propeller blade */
            i_this->stealItemLeft = (s8)i_this->m0344;
            if (i_this->m0374 != 0x1C) {
                anm_init(i_this, 0x1C, 5.0f, 2, 1.0f, -1, 0);
            }
            if (i_this->stealItemLeft > 0) {
                s8 health = i_this->health;
                i_this->health = 10;
                ph_cc_at_check(i_this, &atInfo);
                i_this->m0343 = i_this->m0343 + 1;
                s8 n = (s8)i_this->m0344;
                i_this->health = health;
                if (n > 0)
                    i_this->m0344 = n - 1;
            }
            dPa_control_set(dComIfGp_getParticle(), 0, 0x27B, &i_this->attention_info.position,
                            nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
            i_this->m033F = PH_ACTION_BOUNCE;
            i_this->m0346 = 10;
            i_this->mAtCyl.OffAtSPrmBit(1);
            return FALSE;
        }
        i_this->stealItemLeft = 0;
        i_this->m033F = PH_ACTION_DAMAGE;
        i_this->m0346 = 0x28;
        break;
    default:
        i_this->m033F = PH_ACTION_DAMAGE;
        i_this->m0346 = 0x28;
        break;
    }

    pl = dComIfGp_getPlayer(0);
    ph_set_hitpos(i_this, &hitPos);
    ph_cc_at_check(i_this, &atInfo);
    {
        u8 r = i_this->m0340;
        if (r != 3 && r != 6 && r != 7 && i_this->health > 0) {
            ph_hit_particle(0xD, &hitPos, pl, nullptr);
            return TRUE;
        }
    }
    if (i_this->mType != PH_TYPE_SEAHAT) {
        scale1.x = 1.0f;
        scale2.z = 2.0f;
        scale1.y = 1.0f;
        scale2.x = 2.0f;
        scale2.y = 2.0f;
        scale1.z = 1.0f;
    } else {
        f32 s = REG_F(8, 9) + 3.25f;
        scale1.x = 1.25f;
        scale1.y = 1.25f;
        scale1.z = 1.25f;
        scale2.x = s;
        scale2.y = s;
        scale2.z = s;
    }
    ph_hit_particle(0x10, &hitPos, pl, &scale1);
    ph_hit_particle(0xF, &hitPos, pl, &scale2);
    return TRUE;
}

/* 023CAFB0 */
BOOL hajiki_check(ph_class *i_this) {

    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    bool hit = false;
    if (i_this->mTgCyl.ChkTgHit()) {
        void *obj = i_this->mTgCyl.GetTgHitObj();
        if (obj != nullptr) {
            u32 type = ((dCcD_GObjInf *)obj)->mAtType; /* At type */
            u32 se;
            if ((type & 0x2) || (type & 0x800) || (type & 0x14000400)) {
                se = 0x2803;
            } else if (type & 0x010000C0) {
                se = 0x2833;
            } else if (!(type & 0x00200000)) {
                se = 0x2834;
            } else {
                /* skull hammer: knocked down */
                i_this->m033F = PH_ACTION_WIND;
                i_this->mAtCyl.OffAtSPrmBit(1);
                i_this->m0346 = 0x32;
                return TRUE;
            }
            if (ph_seStart(i_this, se, 0x42)) {
                type = ((dCcD_GObjInf *)obj)->mAtType;
            }
            if (type & 0x2) {
                hit = true;
            }
        }
    }
    if (i_this->mTgCyl.mGObjAt.mRPrm & 1) {
        /* the propeller hit something: sparks */
        cXyz pos;
        pos.x = i_this->mTgCyl.mGObjAt.mHitPos.x;
        pos.y = i_this->mTgCyl.mGObjAt.mHitPos.y;
        pos.z = i_this->mTgCyl.mGObjAt.mHitPos.z;
        dPa_control_set(dComIfGp_getParticle(), 0, 0xC, &pos, nullptr, nullptr, 0xFF, nullptr, -1,
                        nullptr, nullptr, nullptr);
        fopAcM_seStart(i_this, 0x6817, 0);
        return ph_hajiki_set(i_this, player);
    }
    if ((i_this->mAtCyl.mGObjAt.mRPrm & 1) || hit) {
        return ph_hajiki_set(i_this, player);
    }
    return FALSE;
}

/* 023CB2E0 */
BOOL ph_wall_hit_check(ph_class *i_this) {

    if (!i_this->mAcch.ChkWallHit()) {
        return FALSE;
    }
    i_this->current.angle.y += 0x4000;
    i_this->m033F = PH_ACTION_BOUNCE;
    i_this->mAtCyl.OffAtSPrmBit(1);
    i_this->m0346 = 10;
    i_this->speedF = i_this->mType == PH_TYPE_PEAHAT ? 28.0f : 48.0f;
    return TRUE;
}

/* 023CB368 */
BOOL ph_hani_check(ph_class *i_this, float range, float height, unsigned char useHome) {

    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    f32 py = player->current.pos.y;
    f32 px = player->current.pos.x;
    f32 dy;
    if (useHome == 0) {
        f32 y = i_this->current.pos.y + i_this->m02E4.y;
        f32 z = i_this->current.pos.z + i_this->m02E4.z;
        f32 x = i_this->current.pos.x + i_this->m02E4.x;
        f32 dz = z - player->current.pos.z;
        f32 dx = x - px;
        dy = y - py;
        if (!(std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < range))
            return FALSE;
    } else {
        f32 dz = i_this->m032C.z - player->current.pos.z;
        f32 dx = i_this->m032C.x - px;
        dy = i_this->m032C.y - py;
        if (!(std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < range))
            return FALSE;
    }
    if (!(std_sqrtf(dy * dy) < height))
        return FALSE;
    return TRUE;
}

/* 0 out of the water, 1 in BG water, 2 in the sea */
/* 023CB4C0 */
int sea_water_check(ph_class *i_this, unsigned char mode) {

    phVectorWords(&i_this->m05BC)->x = (u32)phVectorWords(&i_this->current.pos)->x;
    i_this->m05BC.y = i_this->m032C.y;
    phVectorWords(&i_this->m05BC)->z = (u32)phVectorWords(&i_this->current.pos)->z;
    PSVECAdd(&i_this->m05BC, &i_this->m02E4, &i_this->m05BC);
    f32 px = i_this->current.pos.x;
    f32 pz = i_this->current.pos.z;
    if ((mode == 0 || mode == 2) && i_this->gravity == 0.0f) {
        i_this->gravity = -5.0f;
    }

    bool inSea = false;
    int state;
    f32 waterY;
    if (daSea_ChkArea(px, pz)) {
        waterY = daSea_calcWave(i_this->current.pos.x, i_this->current.pos.z);
        if (fopAcM_searchPlayerDistance(i_this) > 50000.0f) {
            waterY = dComIfGp_getPlayer(0)->current.pos.y + 200.0f;
        }
        i_this->m05BC.y = waterY;
        f32 margin = i_this->m0346 == 3 ? 300.0f : 100.0f;
        if (!(i_this->current.pos.y + i_this->m02E4.y < waterY + margin)) {
            return ph_leave_water(i_this);
        }
        if (mode == 0 || mode == 2) {
            s16 t = i_this->m0370;
            i_this->gravity = 0.0f;
            i_this->m02F0.y = 0.0f;
            if (mode == 0) {
                i_this->speed.y = 0.0f;
                i_this->speed.x = 0.0f;
                i_this->speed.z = 0.0f;
            }
            i_this->m0370 = t + 1000;
            f32 s = cM_ssin(i_this->m0370);
            cLib_addCalc2(&i_this->current.pos.y, waterY - (s + s + 2.0f), 1.0f, i_this->m0398);
            cLib_addCalc2(&i_this->m0398, 100.0f, 1.0f, 30.0f);
        }
        state = 1;
        inSea = true;
    } else {
        if (!(i_this->mAcch.m_flags & dBgS_Acch::WATER_IN)) {
            return ph_leave_water(i_this);
        }
        waterY = i_this->mAcch.m_wtr.GetHeight(); /* water height */
        i_this->m05BC.y = waterY;
        if (!(i_this->current.pos.y + i_this->m02E4.y < waterY + 100.0f)) {
            state = 2;
        } else {
            if (mode == 0 || mode == 2) {
                s16 t = i_this->m0370;
                u8 type = i_this->mType;
                i_this->gravity = 0.0f;
                if (mode == 0) {
                    i_this->speed.y = 0.0f;
                    i_this->speed.x = 0.0f;
                    i_this->speed.z = 0.0f;
                }
                i_this->m0370 = t + 1000;
                f32 speed = i_this->m0398;
                f32 s = cM_ssin(i_this->m0370);
                if (type == 0) {
                    cLib_addCalc2(&i_this->current.pos.y, waterY - (s + s + 2.0f), 1.0f, speed);
                } else {
                    cLib_addCalc2(&i_this->current.pos.y, waterY - gabi::fmadds(s, 40.0f, 20.0f),
                                  1.0f, speed);
                }
                cLib_addCalc2(&i_this->m0398, 30.0f, 1.0f, 3.0f);
            }
            state = 1;
        }
    }
    {
        s16 m = i_this->m0346;
        if (m != 3 && m != 0x3D) {
            i_this->speedF = 0.0f;
        }
    }
    if (state == 1 && i_this->m0341 == 0) {
        /* entering the water: ripples and a splash */
        cXyz scale;
        cXyz splashPos;
        f32 splashScale;
        if (i_this->mType != PH_TYPE_SEAHAT) {
            scale.y = 1.0f;
            scale.x = 1.0f;
            scale.z = 1.0f;
            ph_ripple_end(i_this);
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &i_this->m05BC, nullptr, &scale, 0xFF,
                            (dPa_levelEcallBack *)&i_this->mParticleCallBack, -1, nullptr, nullptr,
                            nullptr);
        } else {
            scale.x = 4.0f;
            scale.z = 4.0f;
            scale.y = 4.0f;
            dPa_control_c *pa = dComIfGp_getParticle();
            if (pa != nullptr) {
                PSVECScale(&scale, &scale, phParticleScale(pa)->mSplashScale);
            }
            ph_ripple_end(i_this);
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &i_this->m05BC, nullptr, &scale, 0xFF,
                            (dPa_levelEcallBack *)&i_this->mParticleCallBack, -1, nullptr, nullptr,
                            nullptr);
        }
        splashPos.z = i_this->m05BC.z;
        splashPos.y = i_this->m05BC.y;
        splashPos.x = i_this->m05BC.x;
        splashScale = 0.5f + i_this->m039C;
        i_this->m0370 = 0;
        i_this->m0341 = 1;
        i_this->mParticleCallBack.mRate = 0.0f; /* ripple callback rate */
        dPa_control_c *pa = dComIfGp_getParticle();
        if (pa != nullptr) {
            splashScale *= phParticleScale(pa)->mSplashScale;
        }
        fopKyM_createWpillar(&splashPos, splashScale, 0.5f, 0);
        if (i_this->m033F == PH_ACTION_SEPARATED) {
            fopAcM_seStart(i_this, 0x69FB, 0);
        } else if (i_this->m02FC.x != 0.0f && i_this->m037C == 0.0f) {
            fopAcM_seStart(i_this, 0x69F9, 0);
        }
    }
    return inSea ? 2 : 1;
}

/* ph_fly_move (Peahat, m033F == 0) */
/* inlined into daPH_Execute */
static inline int ph_fly_move(ph_class *i_this) {
    switch (i_this->m0346) {
    case 0:
        anm_init(i_this, 0x1C, 5.0f, 2, 1.0f, -1, 0);
        anm_init(i_this, 0x15, 5.0f, 2, 1.0f, -1, 1);
        i_this->mBodySph.mTgType = 0x081DC060;
        i_this->m034A = 0x1000;
        i_this->m034C = 0;
        i_this->m0346++;
        /* fallthrough */
    case 1:
        cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 1, 0x200);
        cLib_addCalc0(&i_this->speedF, 0.2f, 0.5f);
        i_this->speed.y = 0.0f;
        fly_angle_set(i_this, 0);
        fuwafuwa_set(i_this);
        if (i_this->m0364 == 0 && ph_hani_check(i_this, i_this->m0390, i_this->m0394, 0)) {
            i_this->m0346++;
            i_this->m0378 = 150.0f;
        }
        break;
    case 2:
        cLib_addCalc2(&i_this->speedF, 8.0f, 0.5f, 1.0f);
        fly_angle_set(i_this, 1);
        fuwafuwa_set(i_this);
        if (ph_hani_check(i_this, 250.0f, 25400.0f, 0)) {
            i_this->m034A = 0x2000;
            i_this->m034C = 0;
            i_this->m0364 = 20;
            i_this->m0346++;
        }
        break;
    case 3:
        i_this->m0352 = fopAcM_searchPlayerAngleY(i_this);
        i_this->m02F0.x = 0.0f;
        i_this->m02F0.y = 0.0f;
        i_this->m0346++;
        i_this->m02F0.z = 0.0f;
        i_this->m0378 = 80.0f;
        /* fallthrough */
    case 4:
        cLib_addCalc2(&i_this->speedF, 16.0f, 1.0f, 2.0f);
        fly_angle_set(i_this, 2);
        cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->m0352, 1, 0x500);
        cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 1, 0x500);
        if (i_this->m0364 == 0) {
            i_this->m034A = 0x1000;
            i_this->m0378 = 150.0f;
            i_this->m034C = 0;
            i_this->m0364 = (s16)gabi::ftoi(60.0f + cM_rndF(60.0f));
            ph_at_on(i_this);
            ph_zero_wobble(i_this);
            i_this->m0346 = 1;
        }
        break;
    }
    puropera_kaiten(i_this);
    if (sea_water_check(i_this, 1) == 0)
        return PH_CHK_ALL;
    if (!(i_this->current.pos.y + i_this->m02E4.y < i_this->m05BC.y + 200.0f))
        return PH_CHK_ALL;
    i_this->current.angle.x = 0;
    return body_atari_check(i_this) ? PH_CHK_NONE : PH_CHK_WALL;
}

/* ph_fly_sea_move (Seahat, m033F == 0) */
/* inlined into daPH_Execute */
static inline int ph_fly_sea_move(ph_class *i_this) {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    fopAc_ac_c *ship = dComIfGp_getPlayer(2);
    if (REG_S(8, 3) != 0)
        return PH_CHK_NONE;

    bool attackCheck;
    switch (i_this->m0346) {
    case 0:
        ph_zero_wobble(i_this);
        i_this->current.angle.z = 0;
        if (i_this->m0374 != 0x1C) {
            anm_init(i_this, 0x1C, 5.0f, 2, 1.0f, -1, 0);
            anm_init(i_this, 0x15, 5.0f, 2, 1.0f, -1, 1);
        }
        i_this->m034A = 0x1000;
        i_this->m034C = 0;
        i_this->m0346++;
        i_this->m0378 = 150.0f;
        i_this->mBodySph.mTgType = 0x081DC060;
        ph_at_on(i_this);
        /* fallthrough */
    case 1:
        if (dComIfGp_getPlayerStatus0(0) & 0x10000) {
            s16 t = i_this->m0372;
            bool check = false;
            if (t == 0) {
                check = true;
            } else if (t > 0) {
                t--;
                i_this->m0372 = t;
                check = t == 0;
            }
            if (check && ph_hani_check(i_this, i_this->m0390, i_this->m0394, 1)) {
                i_this->m034A = 0x2000;
                i_this->m0378 = 150.0f;
                i_this->m034C = 0;
                i_this->m0346++;
            }
        }
        cLib_addCalc0(&i_this->speedF, 1.0f, REG_F(12, 6) + 5.0f);
        cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 1, 0x200);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x200);
        fuwafuwa_set(i_this);
        i_this->m0352 = fopAcM_searchPlayerAngleY(i_this);
        attackCheck = (u32)(i_this->m0346 - 4) < 3;
        break;
    case 2:
        i_this->m0364 = 0x39;
        i_this->m0352 = fopAcM_searchPlayerAngleY(i_this);
        i_this->shape_angle.z = 0;
        i_this->m02F0.x = 0.0f;
        i_this->m0346++;
        i_this->m0378 = 80.0f;
        i_this->m02F0.y = 0.0f;
        i_this->m02F0.z = 0.0f;
        /* fallthrough */
    case 3:
        if (!ph_hani_check(i_this, i_this->m038C, 25400.0f, 1)) {
            i_this->m0346 = 4;
            attackCheck = true;
            break;
        }
        if (i_this->m0368 == 0) {
            f32 dz = i_this->m032C.z - i_this->current.pos.z;
            f32 dx = i_this->m032C.x - i_this->current.pos.x;
            if (std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) > i_this->m038C) {
                i_this->m0368 = 0x1E;
                i_this->m0346 = 4;
                attackCheck = true;
                break;
            }
        }
        if (!(dComIfGp_getPlayerStatus0(0) & 0x10000)) {
            i_this->m0372 = 0x3C;
            i_this->m0346 = 0;
            if (fopAcM_searchActorDistance(i_this, ship) < REG_F(12, 7) + 200.0f) {
                i_this->m0346 = 7;
                attackCheck = false;
                break;
            }
            attackCheck = (u32)(i_this->m0346 - 4) < 3;
            break;
        }
        if (i_this->m0364 == 0) {
            i_this->m0372 = 0x3C;
            i_this->m0346 = 0;
            attackCheck = false;
            break;
        }
        cLib_addCalc2(&i_this->speedF, 98.0f, 1.0f, 10.0f);
        if (i_this->m0366 != 0) {
            attackCheck = (u32)(i_this->m0346 - 4) < 3;
            break;
        }
        i_this->m0366 = 15;
        i_this->m0341 = 0;
        attackCheck = (u32)(i_this->m0346 - 4) < 3;
        break;
    case 4:
        i_this->m0364 = 0x1E;
        i_this->current.angle.z = 0;
        i_this->m034A = 0x1000;
        i_this->m0378 = 150.0f;
        i_this->m034C = 0;
        i_this->m0346++;
        /* fallthrough */
    case 5:
        cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 1, 0x200);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x200);
        cLib_addCalc0(&i_this->speedF, 1.0f, REG_F(12, 6) + 5.0f);
        i_this->m0352 = fopAcM_searchPlayerAngleY(i_this);
        if (i_this->m0364 != 0) {
            attackCheck = (u32)(i_this->m0346 - 4) < 3;
            break;
        }
        i_this->m0346++;
        attackCheck = (u32)(i_this->m0346 - 4) < 3;
        break;
    case 6: {
        cLib_addCalc2(&i_this->speedF, 150.0f, 0.5f, 1.0f);
        f32 dx = i_this->m032C.x - i_this->current.pos.x;
        f32 dz = i_this->m032C.z - i_this->current.pos.z;
        i_this->m0352 = cM_atan2s(dx, dz);
        if (!(std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < 50.0f)) {
            attackCheck = (u32)(i_this->m0346 - 4) < 3;
            break;
        }
        i_this->m0372 = 0x3C;
        i_this->m0346 = 0;
        attackCheck = false;
        break;
    }
    case 7:
        ph_zero_wobble(i_this);
        i_this->current.angle.z = 0;
        if (i_this->m0374 != 0x1C) {
            anm_init(i_this, 0x1C, 5.0f, 2, 1.0f, -1, 0);
            anm_init(i_this, 0x15, 5.0f, 2, 1.0f, -1, 1);
        }
        i_this->speedF = 98.0f;
        i_this->m0352 =
            cM_atan2s(i_this->m032C.x - ship->current.pos.x, i_this->m032C.z - ship->current.pos.z);
        i_this->m0346++;
        /* fallthrough */
    case 8:
        cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 1, 0x200);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x200);
        cLib_addCalc0(&i_this->speedF, 1.0f, REG_F(12, 6) + 5.0f);
        if (!(i_this->speedF < 1.0f)) {
            attackCheck = (u32)(i_this->m0346 - 4) < 3;
            break;
        }
        if (ph_hani_check(i_this, i_this->m038C, 25400.0f, 1)) {
            i_this->m0346 = 0;
            i_this->speedF = 0.0f;
            attackCheck = false;
        } else {
            i_this->m0346 = 4;
            attackCheck = true;
        }
        break;
    default:
        attackCheck = (u32)(i_this->m0346 - 4) < 3;
        break;
    }

    if (attackCheck && (dComIfGp_getPlayerStatus0(0) & 0x10000) &&
        ph_hani_check(i_this, i_this->m0390, i_this->m0394, 1)) {
        i_this->m034A = 0x2000;
        i_this->m0378 = 150.0f;
        i_this->m034C = 0;
        i_this->m0346 = 2;
    }

    puropera_kaiten(i_this);
    cLib_addCalcAngleS2(&i_this->current.angle.y, i_this->m0352, 1, 0x500);
    cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 1, 0x500);
    sea_water_check(i_this, 1);
    f32 targetY = i_this->m05BC.y + 500.0f + REG_F(12, 8);
    if (i_this->m0346 == 3) {
        cLib_addCalcAngleS2(&i_this->shape_angle.x, (s16)gabi::ftoi(REG_F(12, 11) + 4096.0f), 1,
                            0x800);
        targetY = player->current.pos.y;
    }
    cLib_addCalc2(&i_this->current.pos.y, targetY, 1.0f, REG_F(12, 10) + 50.0f);
    return PH_CHK_ALL;
}

/* ph_hane_move (m033F == 1): knocked back, spinning */
/* inlined into daPH_Execute */
static inline void ph_hane_move(ph_class *i_this) {
    switch (i_this->m0346) {
    case 0xA:
        i_this->m034A = 0x1000;
        i_this->m0378 = 100.0f;
        i_this->m034C = 0;
        i_this->m036E = 0;
        i_this->m0364 = 15;
        i_this->m035E = 0;
        i_this->m0346++;
        /* fallthrough */
    case 0xB:
        i_this->shape_angle.y += 0x500;
        cLib_addCalc0(&i_this->speedF, 0.5f, 1.0f);
        fly_angle_set(i_this, 4);
        if (i_this->m0364 == 0 && i_this->speedF < 0.1f) {
            i_this->m033F = PH_ACTION_FLY;
            if (i_this->mType == PH_TYPE_PEAHAT) {
                i_this->m0364 = 0x3C;
                ph_at_on(i_this);
                ph_zero_wobble(i_this);
                i_this->m0346 = 1;
            } else {
                i_this->m0346 = 0;
            }
        }
        break;
    }
    body_atari_check(i_this);
    puropera_kaiten(i_this);
    if (i_this->mType == PH_TYPE_SEAHAT) {
        sea_water_check(i_this, 1);
    }
}

/* ph_bunri_move (m033F == 2): the propeller has come off, the body hops on the ground */
/* inlined into daPH_Execute */
static inline void ph_bunri_move(ph_class *i_this) {
    dComIfGp_get(); /* HD: unused */
    s16 mode = i_this->m0346;
    bool forceDecrement = false;
    switch (mode) {
    case 0x14:
        i_this->mAtCyl.OffAtSPrmBit(1);
        i_this->m037C = 1.0f;
        i_this->mBodySph.OnCoSPrmBit(1);
        i_this->mBodySph.mTgType = 0xFF3DFEFF;
        phVectorWords(&i_this->m02F0)->x = (u32)phVectorWords(&i_this->m02E4)->x;
        phVectorWords(&i_this->m02F0)->z = (u32)phVectorWords(&i_this->m02E4)->z;
        i_this->m02F0.y = 0.0f;
        i_this->mTgCyl.OffTgSPrmBit(1);
        if (i_this->mType == PH_TYPE_PEAHAT) {
            i_this->m0350 = 900;
            if (i_this != nullptr && gabi::ea(&i_this->eyePos) != 0) {
                mDoAud_seStart(0x585A, &i_this->eyePos, 0,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
                ph_mons_se(i_this, 0x485C);
            }
        } else {
            i_this->m0350 = 150;
            if (i_this != nullptr && gabi::ea(&i_this->eyePos) != 0) {
                mDoAud_seStart(0x69FA, &i_this->eyePos, 0,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
                ph_mons_se(i_this, 0x48F0);
            }
        }
        i_this->speedF = 0.0f;
        anm_init(i_this, 0x1B, 1.0f, 0, 1.0f, -1, 0);
        anm_init(i_this, 0x14, 1.0f, 0, 2.0f, -1, 1);
        mode = i_this->m0346 + 1;
        i_this->m0346 = mode;
        /* fallthrough */
    case 0x15:
        if (!i_this->mpBodyMorf->isStop())
            break;
        copy_bits(&i_this->m0308, &i_this->current.pos);
        i_this->speed.y = 10.0f;
        mode = i_this->m0346 + 1;
        i_this->m0346 = mode;
        i_this->gravity = i_this->mType == PH_TYPE_SEAHAT ? -10.0f : -3.0f;
        /* fallthrough */
    case 0x16:
        if (!i_this->mAcch.ChkGroundHit())
            break;
        anm_init(i_this, 0x13, 1.0f, 0, 1.0f, -1, 1);
        {
            u32 snd =
                phNative_dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo *)&i_this->mAcch.m_gnd);
            ph_se(i_this, 0x5859, snd, true);
        }
        mode = i_this->m0346 + 1;
        i_this->m0346 = mode;
        break;
    case 0x17:
        if (!i_this->mpBodyMorf->isStop())
            break;
        i_this->m0346 = mode + 1;
        /* fallthrough */
    case 0x18:
        if (i_this->m0350 == 0) {
            i_this->m033F = PH_ACTION_RISE;
            i_this->m0346 = 0x1E;
            forceDecrement = true;
            break;
        }
        {
            f32 dz = i_this->m0308.z - i_this->current.pos.z;
            f32 dx = i_this->m0308.x - i_this->current.pos.x;
            f32 angle = (f32)(s16)i_this->current.angle.y;
            i_this->mBodySph.mTgType = 0xFF3DFEFF;
            i_this->current.angle.y = (s16)gabi::ftoi(angle + cM_rndFX(16384.0f));
            if (!(std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < 200.0f)) {
                /* too far from where it landed: turn back */
                s16 back = cM_atan2s(dx, dz);
                if ((s16)cLib_distanceAngleS(i_this->current.angle.y, back) < 0x2000) {
                    i_this->current.angle.y = cM_atan2s(dx, dz);
                } else {
                    s16 a = i_this->current.angle.y;
                    s16 p = a + 0x2000;
                    s16 m = a - 0x2000;
                    back = cM_atan2s(dx, dz);
                    s16 dp = (s16)cLib_distanceAngleS(p, back);
                    back = cM_atan2s(dx, dz);
                    s16 dm = (s16)cLib_distanceAngleS(m, back);
                    i_this->current.angle.y = dp < dm ? p : m;
                }
            }
        }
        anm_init(i_this, 0x18, 2.0f, 0, 1.0f, -1, 1);
        i_this->m0346 = 0x19;
        /* fallthrough */
    case 0x19:
        if (i_this->mpBodyMorf->getFrame() < 7.0f) {
            cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 1, 0x1000);
        }
        if (!i_this->mpBodyMorf->checkFrame(7.0f)) {
            mode = i_this->m0346;
            break;
        }
        i_this->speedF = 5.0f;
        i_this->gravity = -3.0f;
        i_this->speed.y = 20.0f;
        ph_mons_se(i_this, 0x485F);
        mode = i_this->m0346 + 1;
        i_this->m0346 = mode;
        break;
    case 0x1A:
        if (!i_this->mAcch.ChkGroundHit())
            break;
        {
            u32 snd =
                phNative_dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo *)&i_this->mAcch.m_gnd);
            ph_se(i_this, 0x585B, snd, true);
        }
        i_this->speedF = 0.0f;
        i_this->m0346 = 0x18;
        forceDecrement = true;
        break;
    default:
        break;
    }
    if (forceDecrement || mode >= 0x18) {
        if (i_this->m0350 > 0)
            i_this->m0350--;
    }
    cLib_addCalcAngleS2(&i_this->current.angle.x, 0, 1, 0x500);
    cLib_addCalcAngleS2(&i_this->shape_angle.x, i_this->current.angle.x, 1, 0x500);
    cLib_addCalcAngleS2(&i_this->current.angle.z, 0, 1, 0x500);
    cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 1, 0x500);
    if (body_atari_check(i_this)) {
        s16 save = i_this->m0346;
        if (i_this->m0340 == 2) {
            ph_blade_se(i_this, false);
            s16 a = fopAcM_searchPlayerAngleY(i_this);
            i_this->m0346 = save;
            i_this->current.angle.y = a + 0x8000;
            i_this->speedF = 30.0f;
            i_this->m033F = PH_ACTION_SEPARATED;
        }
    }
}

/* ph_fujyou_move (m033F == 3): the propeller regrows and the Peahat takes off again */
/* inlined into daPH_Execute */
static inline void ph_fujyou_move(ph_class *i_this) {
    dComIfGp_get(); /* HD: unused */
    switch (i_this->m0346) {
    case 0x1E:
        ph_se(i_this, 0x585C, 0, true);
        {
            f32 y = i_this->current.pos.y;
            i_this->mAtCyl.OnAtSPrmBit(1);
            i_this->mBodySph.OffCoSPrmBit(1);
            i_this->mAtCyl.mObjAt.mRPrm = 1;
            i_this->mTgCyl.OnTgSPrmBit(1);
            i_this->m0388 = y + 200.0f;
        }
        anm_init(i_this, 0x1D, 5.0f, 0, 1.0f, -1, 0);
        anm_init(i_this, 0x16, 5.0f, 0, 1.0f, -1, 1);
        i_this->m0346++;
        /* fallthrough */
    case 0x1F:
        ph_fujyou_grow(i_this);
        break;
    case 0x20:
        ph_fujyou_rise(i_this, 4.0f, 0.03f, 0.05f);
        if (i_this->current.pos.y > i_this->m0388) {
            ph_at_on(i_this);
            ph_zero_wobble(i_this);
            i_this->mBodySph.mTgType = 0x081DC060;
            i_this->current.angle.z = 0;
            i_this->m0346 = 1;
            i_this->m033F = PH_ACTION_FLY;
            i_this->gravity = -3.0f;
        }
        break;
    case 0x21:
        ph_se(i_this, 0x69FC, 0, true);
        i_this->mBodySph.OffCoSPrmBit(1);
        anm_init(i_this, 0x1D, 5.0f, 0, 1.0f, -1, 0);
        anm_init(i_this, 0x16, 5.0f, 0, 1.0f, -1, 1);
        i_this->m0346++;
        /* fallthrough */
    case 0x22:
        ph_fujyou_grow(i_this);
        break;
    case 0x23:
        ph_fujyou_rise(i_this, 6.0f, 0.3f, 0.5f);
        if (std::fabs(i_this->current.pos.y - (i_this->m05BC.y + 500.0f)) < 10.0f) {
            i_this->m033F = PH_ACTION_FLY;
            i_this->m0346 = 4;
        }
        break;
    case 0x24:
        i_this->m033A += (s16)gabi::ftoi(i_this->m0384 * 1000.0f);
        puropera_sound(i_this);
        i_this->speed.y = 70.0f;
        if (i_this->current.pos.y > i_this->m05BC.y + 500.0f) {
            i_this->m0346 = 0;
            i_this->m033F = PH_ACTION_FLY;
        }
        break;
    }

    if (i_this->mType != PH_TYPE_SEAHAT) {
        if (sea_water_check(i_this, 1) != 0 &&
            i_this->current.pos.y + i_this->m02E4.y < i_this->m05BC.y + 100.0f) {
            i_this->gravity = 0.0f;
        }
    }
    if (!body_atari_check(i_this) && hajiki_check(i_this)) {
        anm_init(i_this, 0x1C, 5.0f, 2, 1.0f, -1, 0);
        f32 s = i_this->m03A0 + 1.0f;
        i_this->m02FC.x = s;
        i_this->m02FC.y = s;
        i_this->m02FC.z = s;
    }
    if (i_this->mType != PH_TYPE_SEAHAT) {
        sea_water_check(i_this, 1);
        return;
    }
    if (sea_water_check(i_this, 1) != 0 && i_this->m0341 != 0) {
        f32 y = gabi::fnmsubs(i_this->m039C, 10.0f, i_this->m05BC.y);
        if (i_this->current.pos.y < y) {
            i_this->current.pos.y = y;
        }
    }
}

/* 023CBAC0 */
void dead_item(ph_class *i_this) {

    cXyz pos;
    pos.x = i_this->current.pos.x;
    pos.y = i_this->current.pos.y;
    pos.z = i_this->current.pos.z;
    PSVECAdd(&pos, &i_this->m02E4, &pos);
    u8 scale = (u8)gabi::ftoi(i_this->m039C + i_this->m039C + 5.0f);
    if (i_this->mType == PH_TYPE_PEAHAT) {
        fopAcM_createDisappear(i_this, &pos, scale, 0, i_this->stealItemBitNo);
    } else {
        fopAcM_createDisappear(i_this, &pos, scale, 0, 0xFF);
        /* Seahat kill counter */
        s32 n = dSv_event_getEventReg(dComIfGs_getEvent(), 0x7EFF) + 1;
        u8 v = n > 0xFF ? 0xFF : (u8)n;
        dSv_event_setEventReg(dComIfGs_getEvent(), 0x7EFF, v);
    }
    dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
    fopAcM_delete(i_this);
}

/* ph_damage_dead_move (m033F == 4). Returns true when the live Peahat goes on to the water check */
/* inlined into daPH_Execute */
static inline bool ph_damage_dead_move(ph_class *i_this) {
    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    bool die = false;
    switch (i_this->m0346) {
    case 0x28:
        i_this->current.angle.y = fopAcM_searchPlayerAngleY(i_this) + 0x8000;
        if (i_this->health <= 0 || i_this->m0340 == 3) {
            i_this->m034E = 0x2000;
            i_this->speedF = 25.0f;
        } else {
            i_this->m034E = 0x1000;
            i_this->speedF = 15.0f;
        }
        if (i_this->m0341 != 0) {
            i_this->m034E = -0x8000;
        }
        if (gabi::ea(&i_this->eyePos) != 0) {
            if (i_this->health > 0) {
                ph_mons_se(i_this, 0x485D);
            } else {
                ph_dead_se(i_this);
            }
        }
        i_this->m0364 = 0;
        if (i_this->m02FC.x != 0.0f) {
            anm_init(i_this, 0x1E, 1.0f, 0, 1.0f, -1, 0);
        }
        anm_init(i_this, 0x14, 1.0f, 0, 1.0f, -1, 1);
        i_this->m0346++;
        /* fallthrough */
    case 0x29:
        cLib_addCalc0(&i_this->speedF, 0.5f, 1.0f);
        if (i_this->health <= 0 && i_this->speedF < 5.0f) {
            cLib_addCalc0(&i_this->m02FC.x, 0.3f, i_this->m03A0 + 1.0f);
            f32 s = i_this->m02FC.x;
            i_this->m02FC.z = s;
            i_this->m02FC.y = s;
        }
        i_this->shape_angle.y += i_this->m034E;
        cLib_addCalcAngleS2(&i_this->m034E, 0, 1, 0x200);
        if (!i_this->mpBodyMorf->isStop())
            break;
        if (i_this->health <= 0) {
            die = true;
            break;
        }
        i_this->speedF = 0.0f;
        i_this->m0346 = 0x18;
        i_this->m033F = PH_ACTION_SEPARATED;
        break;
    case 0x2A:
        if (i_this->m0364 != 0)
            break;
        i_this->m033F = PH_ACTION_SEPARATED;
        i_this->m0346 = 0x18;
        break;
    case 0x2B: {
        s16 a = cM_atan2s(i_this->current.pos.x + i_this->m02E4.x - player->current.pos.x,
                          i_this->current.pos.z + i_this->m02E4.z - player->current.pos.z);
        i_this->m035E = 0;
        i_this->mAtCyl.OffAtSPrmBit(1);
        i_this->speedF = 25.0f;
        i_this->health = 0;
        i_this->current.angle.y = a;
        ph_dead_se(i_this);
        anm_init(i_this, 0x14, 1.0f, 0, 1.0f, -1, 1);
        f32 s = i_this->m039C + 1.0f;
        i_this->m0320.x = s;
        i_this->m0320.y = s;
        i_this->m0320.z = s;
        if (i_this->m0342 != 0) {
            /* frozen by a light arrow */
            i_this->speed.x = 0.0f;
            i_this->speed.y = 0.0f;
            i_this->speed.z = 0.0f;
            i_this->speedF = 0.0f;
            i_this->m0364 = 0;
            i_this->m0346 = 0x2E;
        } else if (!i_this->mAcch.ChkGroundHit()) {
            i_this->speed.y = 0.0f;
            i_this->gravity = 0.0f;
            i_this->m0346 = 0x2C;
            i_this->m0364 = 0x3C;
            if (i_this->m0340 == 7)
                i_this->m0364 = 0;
            anm_init(i_this, 0x1E, 1.0f, 0, 1.0f, -1, 0);
            if (i_this->m0340 == 6) {
                i_this->m0364 = 0;
                i_this->speedF = 15.0f;
                i_this->speed.y = 20.0f;
                i_this->m0346 = 0x2D;
                i_this->gravity = -5.0f;
            }
        } else {
            i_this->m0346 = 0x2E;
        }
        break;
    }
    case 0x2C:
        cLib_addCalc0(&i_this->speedF, 0.5f, 0.5f);
        i_this->shape_angle.y += 0x500;
        fly_angle_set(i_this, 4);
        if (i_this->m0364 == 0 || i_this->mpPropellerMorf->isStop()) {
            die = true;
            break;
        }
        break;
    case 0x2D:
        if (i_this->m0364 == 1) {
            i_this->gravity = -5.0f;
            i_this->speed.y = 23.0f;
            if (i_this->m035E > 1)
                i_this->speed.y = 15.0f;
            f32 b = i_this->m039C;
            i_this->speedF = 5.0f;
            i_this->m0320.x = b + 0.8f;
            i_this->m0320.y = b + 1.2f;
        }
        if (i_this->m0364 == 0 && i_this->mAcch.ChkGroundHit()) {
            f32 b = i_this->m039C;
            i_this->speedF = 0.0f;
            i_this->m0320.x = b + 1.2f;
            i_this->m0320.y = b + 0.8f;
            i_this->m0364 = 3;
            s16 n = i_this->m035E + 1;
            i_this->m035E = n;
            if (n > 2)
                dead_item(i_this);
        }
        cLib_addCalc2(&i_this->scale.x, i_this->m0320.x, 0.5f, 0.5f);
        cLib_addCalc2(&i_this->scale.y, i_this->m0320.y, 0.5f, 0.5f);
        break;
    case 0x2E:
        cLib_addCalc0(&i_this->speedF, 0.5f, 0.5f);
        i_this->shape_angle.y += 0x500;
        if (i_this->m0364 == 0) {
            die = true;
            break;
        }
        break;
    case 0x2F:
        i_this->shape_angle.x = 0;
        i_this->speedF = 0.0f;
        i_this->gravity = -3.0f;
        i_this->shape_angle.z = 0;
        i_this->health = 0;
        i_this->mBodySph.OffCoSPrmBit(1);
        i_this->mAtCyl.OffAtSPrmBit(1);
        i_this->mBodySph.OffTgSPrmBit(1);
        i_this->current.angle.x = 0;
        i_this->current.angle.z = 0;
        i_this->m035E = 0;
        i_this->attention_info.flags = 0; /* attention_info.flags */
        if (i_this->m02FC.x != 0.0f) {
            anm_init(i_this, 0x1E, 1.0f, 0, 1.0f, -1, 0);
        }
        anm_init(i_this, 0x14, 1.0f, 0, 1.0f, -1, 1);
        i_this->m0380 = 10.0f;
        ph_dead_se(i_this);
        i_this->m0346++;
        /* fallthrough */
    case 0x30:
        cLib_addCalc2(&i_this->scale.y, 0.1f, 1.0f, 0.5f);
        cLib_addCalc2(&i_this->scale.x, 1.7f, 1.0f, 0.7f);
        if (i_this->m02FC.x != 0.0f) {
            copy_bits(&i_this->m02FC, &i_this->scale);
        }
        if (!(i_this->scale.y < 0.1f))
            break;
        i_this->scale.y = 0.1f;
        i_this->scale.x = 1.7f;
        i_this->m0364 = 0x14;
        i_this->m0346++;
        break;
    case 0x31:
        if (i_this->m0364 != 0)
            break;
        die = true;
        break;
    default:
        break;
    }
    if (die) {
        dead_item(i_this);
    }
    i_this->current.angle.x = 0;
    i_this->current.angle.z = 0;
    cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x500);
    cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 1, 0x500);
    return i_this->health > 0;
}

/* blown by the Deku Leaf */
/* 023CFDF0 */
void ph_wind_move(ph_class *i_this) {

    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    int water = sea_water_check(i_this, 1);
    switch (i_this->m0346) {
    case 0x32: {
        ph_blade_se(i_this, true);
        i_this->mAtCyl.OffAtSPrmBit(1);
        i_this->mTgCyl.OffTgSPrmBit(1);
        i_this->mBodySph.OnCoSPrmBit(1);
        i_this->mBodySph.mTgType = 0xFF3DFEFF;
        i_this->current.angle.y = fopAcM_searchPlayerAngleY(i_this) + 0x8000;
        f32 dy = i_this->current.pos.y + i_this->m02E4.y - player->current.pos.y;
        f32 v = gabi::fnmsubs(std_sqrtf(dy * dy), 0.1f, 10.0f);
        if (v < 0.0f)
            v = 3.0f;
        i_this->speed.y = v;
        i_this->m0350 = 0;
        i_this->speedF = 30.0f;
        i_this->gravity = 0.0f;
        for (int i = 0; i < 7; i++)
            i_this->mWobbleAngles[i] = 0;
        i_this->m02F0.x = 0.0f;
        i_this->m02F0.y = 0.0f;
        i_this->current.angle.x = 0;
        i_this->current.angle.z = 0;
        i_this->m0362 = -0x1000;
        i_this->m0360 = -10000;
        i_this->m02F0.z = 0.0f;
        if (cM_rnd() < 0.5f) {
            i_this->m0360 = 10000;
            i_this->m0362 = 0x1000;
        }
        anm_init(i_this, 0x1E, 1.0f, 0, 1.0f, -1, 0);
        anm_init(i_this, 0x19, 1.0f, 0, 1.0f, -1, 1);
        i_this->m0346++;
    }
        /* fallthrough */
    case 0x33:
        i_this->m035E += i_this->m0360;
        cLib_addCalcAngleS2(&i_this->shape_angle.z,
                            (s16)gabi::ftoi(cM_ssin(i_this->m035E) * 20000.0f), 1, 0x500);
        cLib_addCalcAngleS2(&i_this->shape_angle.x,
                            (s16)gabi::ftoi(cM_ssin(i_this->m035E) * 20000.0f), 1, 0x500);
        i_this->shape_angle.y += i_this->m0362;
        cLib_addCalc0(&i_this->speedF, 0.3f, 1.0f);
        cLib_addCalc0(&i_this->speed.y, 0.3f, 1.0f);
        if (i_this->speedF > 0.2f)
            break;
        i_this->speedF = 0.0f;
        i_this->m0360 = 0;
        i_this->m0346++;
        /* fallthrough */
    case 0x34:
        if (water) {
            i_this->m033F = PH_ACTION_WATER;
            i_this->m0346 = water == 2 ? 0x3C : 0x46;
            return;
        }
        i_this->gravity = -0.3f;
        i_this->shape_angle.y += i_this->m0362;
        cLib_addCalcAngleS2(&i_this->m0362, 0, 1, 0x100);
        fly_angle_set(i_this, 0);
        if (!i_this->mAcch.ChkGroundHit())
            break;
        {
            u32 snd =
                phNative_dBgS_GetMtrlSndId(dComIfG_Bgsp(), (cBgS_PolyInfo *)&i_this->mAcch.m_gnd);
            if (gabi::ea(&i_this->eyePos) != 0) {
                mDoAud_seStart(0x5859, &i_this->eyePos, snd,
                               dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
            }
        }
        i_this->gravity = -3.0f;
        anm_init(i_this, 0x1F, 1.0f, 0, 1.0f, -1, 0);
        anm_init(i_this, 0x1A, 1.0f, 0, 1.0f, -1, 1);
        i_this->m0346++;
        break;
    case 0x35:
        if (water) {
            i_this->m033F = PH_ACTION_WATER;
            i_this->m0346 = water == 2 ? 0x3C : 0x46;
            return;
        }
        cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 1, 0x500);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, i_this->current.angle.x, 1, 0x500);
        i_this->shape_angle.y += i_this->m0360;
        if (!i_this->mpBodyMorf->isStop())
            break;
        if (i_this->m02FC.x == 0.0f) {
            i_this->speedF = 0.0f;
            i_this->m033F = PH_ACTION_SEPARATED;
            i_this->m0346 = 0x18;
            return;
        }
        i_this->m0366 = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
        i_this->shape_angle.x = 0;
        i_this->m0346++;
        i_this->shape_angle.z = 0;
        break;
    case 0x36: {
        if (water) {
            i_this->m033F = PH_ACTION_WATER;
            i_this->m0346 = water == 2 ? 0x3C : 0x46;
            return;
        }
        cLib_addCalc0(&i_this->speedF, 0.3f, 1.0f);
        s16 t = i_this->m0366;
        if (i_this->speedF > 0.1f) {
            i_this->shape_angle.y += i_this->m0360;
        }
        if (t != 0)
            break;
        i_this->speedF = 0.0f;
        if (i_this->m02FC.x == 0.0f) {
            i_this->m033F = PH_ACTION_SEPARATED;
            i_this->m0346 = 0x18;
        } else {
            i_this->m033F = PH_ACTION_RISE;
            i_this->m0346 = 0x1E;
        }
        break;
    }
    default:
        break;
    }

    if (!body_atari_check(i_this))
        return;
    i_this->shape_angle.x = 0;
    i_this->shape_angle.z = 0;
    if (i_this->m0340 != 2)
        return;
    if (i_this->mAcch.ChkGroundHit()) {
        i_this->m0360 = -1000;
        if (cM_rnd() < 0.5f)
            i_this->m0360 = 1000;
        i_this->speedF = 20.0f;
        if (i_this->m0374 != 0x1E) {
            anm_init(i_this, 0x1E, 1.0f, 0, 1.0f, -1, 0);
        }
        ph_blade_se(i_this, false);
        i_this->m0346 = 0x36;
    } else {
        i_this->m033F = PH_ACTION_WIND;
        i_this->m0346 = 0x32;
    }
    return;
}

/* floating in the water */
/* 023D051C */
void ph_water_move(ph_class *i_this) {

    fopAc_ac_c *player = dComIfGp_getPlayer(0);
    switch (i_this->m0346) {
    case 0x3C:
        ph_water_start(i_this);
        if (i_this->mType == PH_TYPE_SEAHAT) {
            i_this->m0366 = 150;
            i_this->mAtCyl.OnAtSPrmBit(1);
            i_this->mAtCyl.mObjAt.mRPrm = 1;
            anm_init(i_this, 0x17, 1.0f, 2, 1.0f, -1, 1);
        } else {
            ph_water_anm(i_this);
        }
        i_this->m0368 = 20;
        i_this->current.angle.y =
            cM_atan2s(i_this->current.pos.x + i_this->m02E4.x - player->current.pos.x,
                      i_this->current.pos.z + i_this->m02E4.z - player->current.pos.z) +
            0x8000;
        i_this->m0346++;
        /* fallthrough */
    case 0x3D: {
        if (i_this->m0364 == 0) {
            i_this->m0341 = 0;
            i_this->m0364 = 15;
        }
        cLib_addCalcAngleS2(&i_this->shape_angle.y, i_this->current.angle.y, 1, 0x700);
        cLib_addCalc2(&i_this->speedF, 30.0f, 1.0f, 10.0f);
        u32 vol = ftou(i_this->speedF * 3.4f);
        if (vol > 100)
            vol = 100;
        if (gabi::ea(&i_this->eyePos) != 0) {
            mDoAud_seStart(0x5128, &i_this->eyePos, vol,
                           dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
        }
        if (ph_hani_check(i_this, i_this->m038C, 25400.0f, 1)) {
            bool far = false;
            if (i_this->m0368 == 0) {
                f32 dz = i_this->m032C.z - i_this->current.pos.z;
                f32 dx = i_this->m032C.x - i_this->current.pos.x;
                far = std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) > i_this->m038C;
            }
            if (!far && i_this->m0366 != 0)
                break;
        }
        i_this->m033F = PH_ACTION_RISE;
        i_this->m0346 = 0x21;
        break;
    }
    case 0x46:
        ph_water_start(i_this);
        ph_water_anm(i_this);
        i_this->m0346++;
        /* fallthrough */
    case 0x47:
        if (i_this->m0366 == 0) {
            i_this->m033F = PH_ACTION_RISE;
            i_this->m0346 = i_this->mType == PH_TYPE_SEAHAT ? 0x21 : 0x1E;
        }
        break;
    default:
        break;
    }
    body_atari_check(i_this);
    sea_water_check(i_this, 2);
}

/* 023CBBE4 */
void BG_check(ph_class *i_this) {

    if (i_this->mType == PH_TYPE_PEAHAT) {
        f32 r = 40.0f;
        if (i_this->m02FC.x != 0.0f && i_this->m037C == 0.0f) {
            r = 100.0f;
        }
        i_this->mAcchCir.SetWall(40.0f, r);
    } else {
        i_this->mAcchCir.SetWall(50.0f, 500.0f);
    }
    ph_crr_pos(i_this);
}

/* 023CBD80 */
void UP_draw_SUB(ph_class *i_this) {

    J3DModel *model = i_this->mpPropellerMorf->getModel();
    PSMTXTrans(mDoMtx_stack_c::get(), i_this->m0314.x, i_this->m0314.y, i_this->m0314.z);
    s16 mode = i_this->m0346;
    if (mode == 4 || mode == 0x33 || mode == 3) {
        mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
        mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
        mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    } else {
        mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
    }
    mDoMtx_stack_c::YrotM(i_this->m033A);
    mDoMtx_stack_scaleM(i_this->m02FC.x, i_this->m02FC.y, i_this->m02FC.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    i_this->mpPropellerMorf->calc();
    phNative_enemy_fire(&i_this->mPropellerEnemyFire);
}

/* 023CBF44 */
void DW_draw_SUB(ph_class *i_this) {

    J3DModel *model = i_this->mpBodyMorf->getModel();
    PSMTXTrans(mDoMtx_stack_c::get(), i_this->current.pos.x + i_this->m02E4.x,
               i_this->current.pos.y + i_this->m02E4.y, i_this->current.pos.z + i_this->m02E4.z);
    mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    mDoMtx_stack_scaleM(i_this->scale.x, i_this->scale.y, i_this->scale.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    i_this->mpBodyMorf->calc();
    phNative_enemy_fire(&i_this->mBodyEnemyFire);
}

/* 023CC070 */
static BOOL daPH_Execute(ph_class *i_this) {

    fopAc_ac_c *a_this = i_this;
    if (i_this->mType == PH_TYPE_PEAHAT) {
        phNative_fopAcM_setGbaName(a_this, 0x2D, 8, 0x25);
    } else {
        phNative_fopAcM_setGbaName(a_this, 0x2D, 0x1F, 0x27);
    }

    if (phNative_enemy_ice(&i_this->mEnemyIce)) {
        J3DModel_setBaseTRMtx(i_this->mpBodyMorf->getModel(), mDoMtx_stack_c::get());
        i_this->mpBodyMorf->calc();
        if (i_this->m02FC.x != 0.0f && i_this->m037C == 0.0f) {
            f32 sxz = i_this->mEnemyIce.mScaleXZ;
            if (i_this->mType == PH_TYPE_SEAHAT) {
                f32 k = i_this->m03A0;
                f32 sy = i_this->mEnemyIce.mScaleY;
                copy_bits(&i_this->m0314, &i_this->m02C0);
                f32 x = k * sxz;
                i_this->m02FC.x = x;
                i_this->m02FC.y = k * sy;
                i_this->m02FC.z = x;
            } else {
                f32 sy = i_this->mEnemyIce.mScaleY;
                f32 x = a_this->scale.x;
                i_this->m02FC.x = x;
                f32 y = a_this->scale.y;
                i_this->m02FC.y = y;
                f32 z = a_this->scale.z;
                i_this->m02FC.x = x * sxz;
                i_this->m02FC.y = y * sy;
                i_this->m02FC.z = z * sxz;
                copy_bits(&i_this->m0314, &i_this->m02C0);
            }
            UP_draw_SUB(i_this);
        }
        return TRUE;
    }

    /* eyePos: the body joint, lowered */
    a_this->eyePos.x = i_this->m02C0.x;
    f32 ey = i_this->m02C0.y;
    a_this->eyePos.y = ey;
    a_this->eyePos.z = i_this->m02C0.z;
    ey = ey - 30.0f;
    a_this->eyePos.y = ey;
    a_this->eyePos.y = gabi::fnmsubs(50.0f, i_this->m039C, ey);

    for (int i = 0; i < 5; i++) {
        be<s16> *t = &i_this->mTimers[i];
        if (*t != 0)
            *t = *t - 1;
    }
    /* debug overrides */
    f32 r = REG_F(8, 0);
    if (r != 0.0f) {
        f32 s = r + 8.0f;
        i_this->m039C = s;
        a_this->scale.x = s + 1.0f;
        a_this->scale.y = s + 1.0f;
        a_this->scale.z = s + 1.0f;
    }
    r = REG_F(8, 1);
    if (r != 0.0f) {
        i_this->m03A0 = r + 5.0f;
    }

    int chk;
    switch (i_this->m033F) {
    case PH_ACTION_FLY:
        if (i_this->mType == PH_TYPE_PEAHAT) {
            chk = ph_fly_move(i_this);
        } else {
            chk = ph_fly_sea_move(i_this);
        }
        if (chk != PH_CHK_NONE) {
            if (chk == PH_CHK_ALL && body_atari_check(i_this))
                break;
            if (ph_wall_hit_check(i_this))
                break;
            hajiki_check(i_this);
        }
        break;
    case PH_ACTION_BOUNCE:
        ph_hane_move(i_this);
        break;
    case PH_ACTION_SEPARATED: {
        ph_bunri_move(i_this);
        int water = sea_water_check(i_this, 1);
        if (water != 0)
            ph_to_water(i_this, water);
        break;
    }
    case 3:
        ph_fujyou_move(i_this);
        break;
    case 4: {
        if (ph_damage_dead_move(i_this)) {
            u8 inWater = i_this->m0341;
            body_atari_check(i_this);
            int water = sea_water_check(i_this, 0);
            if (water != 0 && inWater == 0 && i_this->m0341 != 0) {
                ph_to_water(i_this, water);
            }
        }
        break;
    }
    case 5:
        ph_wind_move(i_this);
        break;
    case 6:
        ph_water_move(i_this);
        break;
    }

    cMtx_YrotS(calc_mtx(), a_this->current.angle.y);
    cMtx_XrotM(calc_mtx(), a_this->current.angle.x);

    cXyz local;
    cXyz world;
    bool propFlying = false;
    if (i_this->m037C == 0.0f) {
        copy_bits(&i_this->m0314, &i_this->m02C0);
    } else if (i_this->m02FC.x != 0.0f) {
        /* the propeller flies off */
        f32 height = 1000.0f;
        puropera_kaiten(i_this);
        cLib_addCalc2(&i_this->m037C, 10.0f, 0.5f, 1.0f);
        if (i_this->mType == PH_TYPE_SEAHAT) {
            cLib_addCalc2(&i_this->m037C, 30.0f, 1.0f, 5.0f);
            height = 2000.0f;
            i_this->m034A = 0x2000;
        }
        local.x = 0.0f;
        local.y = 0.0f;
        local.z = -i_this->m037C;
        MtxPosition(&local, &world);
        PSVECAdd(&i_this->m0314, &world, &i_this->m0314);
        f32 y = i_this->m0314.y + i_this->m037C;
        i_this->m0314.y = y;
        if (y > a_this->current.pos.y + height) {
            i_this->m02FC.x = 0.0f;
            i_this->m02FC.y = 0.0f;
            i_this->m02FC.z = 0.0f;
            local.x = 0.0f;
            local.y = 0.0f;
            i_this->m037C = 0.0f;
            propFlying = true;
        }
    }
    if (!propFlying) {
        local.y = 0.0f;
        local.x = 0.0f;
    }
    local.z = a_this->speedF;
    MtxPosition(&local, &world);
    a_this->speed.x = world.x;
    a_this->speed.z = world.z;

    u8 st = i_this->m033F;
    f32 vy;
    if (st == 2 || st == 3 || st == 5 || st == 4) {
        vy = a_this->speed.y + a_this->gravity;
        if (vy < -20.0f)
            vy = -20.0f;
    } else {
        vy = world.y;
    }
    a_this->speed.y = vy;
    fopAcM_posMove(a_this, &i_this->mStts.m_cc_move);
    if (i_this->m033F == PH_ACTION_BOUNCE && i_this->m0341 != 0) {
        f32 y = i_this->m05BC.y + 40.0f;
        if (a_this->current.pos.y < y)
            a_this->current.pos.y = y;
    }

    if (i_this->mType == PH_TYPE_PEAHAT) {
        cLib_addCalc2(&i_this->m02E4.x, i_this->m02F0.x, 0.3f, 3.0f);
        cLib_addCalc2(&i_this->m02E4.y, i_this->m02F0.y, 0.3f, 3.0f);
        cLib_addCalc2(&i_this->m02E4.z, i_this->m02F0.z, 0.3f, 3.0f);
    } else {
        cLib_addCalc2(&i_this->m02E4.x, i_this->m02F0.x, 1.0f, 30.0f);
        cLib_addCalc2(&i_this->m02E4.y, i_this->m02F0.y, 1.0f, 30.0f);
        cLib_addCalc2(&i_this->m02E4.z, i_this->m02F0.z, 1.0f, 30.0f);
        cLib_addCalc2(&a_this->current.pos.x, a_this->current.pos.x + i_this->m02E4.x, 1.0f, 30.0f);
        cLib_addCalc2(&a_this->current.pos.y, a_this->current.pos.y + i_this->m02E4.y, 1.0f, 30.0f);
        cLib_addCalc2(&a_this->current.pos.z, a_this->current.pos.z + i_this->m02E4.z, 1.0f, 30.0f);
        cLib_addCalc0(&i_this->m02E4.x, 1.0f, 30.0f);
        cLib_addCalc0(&i_this->m02E4.y, 1.0f, 30.0f);
        cLib_addCalc0(&i_this->m02E4.z, 1.0f, 30.0f);
    }
    i_this->mpBodyMorf->play(nullptr, 0, 0);
    i_this->mpPropellerMorf->play(nullptr, 0, 0);
    DW_draw_SUB(i_this);
    UP_draw_SUB(i_this);

    if (i_this->m033F != PH_ACTION_DAMAGE) {
        if (i_this->mType == PH_TYPE_PEAHAT) {
            i_this->mAtCyl.mCyl.SetC(&i_this->m02D8);
            i_this->mAtCyl.mCyl.SetH(60.0f);
            i_this->mAtCyl.mCyl.SetR(35.0f);
            dComIfG_Ccsp_Set(&i_this->mAtCyl);
            i_this->mTgCyl.mCyl.SetC(&i_this->m02D8);
            i_this->mTgCyl.mCyl.SetH(60.0f);
            i_this->mTgCyl.mCyl.SetR(100.0f);
            dComIfG_Ccsp_Set(&i_this->mTgCyl);
            i_this->mBodySph.mSph.SetC(&i_this->m02CC);
            i_this->mBodySph.mSph.SetR(40.0f);
            dComIfG_Ccsp_Set(&i_this->mBodySph);
        } else {
            cXyz c;
            c.x = i_this->m02CC.x;
            c.z = i_this->m02CC.z;
            c.y = i_this->m02CC.y - (REG_F(8, 10) + 150.0f);
            i_this->mTgCyl.mCyl.SetC(&c);
            i_this->mTgCyl.mCyl.SetH(REG_F(8, 12) + 500.0f);
            i_this->mTgCyl.mCyl.SetR(REG_F(8, 13) + 500.0f);
            dComIfG_Ccsp_Set(&i_this->mTgCyl);
            f32 d = REG_F(8, 10) + 150.0f;
            c.x = i_this->m02CC.x;
            c.z = i_this->m02CC.z;
            c.y = i_this->m02CC.y - d;
            i_this->mAtCyl.mCyl.SetC(&c);
            i_this->mAtCyl.mCyl.SetH(REG_F(8, 14) + 350.0f);
            i_this->mAtCyl.mCyl.SetR(REG_F(8, 15) + 250.0f);
            dComIfG_Ccsp_Set(&i_this->mAtCyl);
            i_this->mBodySph.mSph.SetC(&i_this->m02CC);
            i_this->mBodySph.mSph.SetR(
                gabi::fmadds(i_this->m039C, REG_F(8, 17) + 40.0f, REG_F(8, 16) + 70.0f));
            dComIfG_Ccsp_Set(&i_this->mBodySph);
        }
    }

    cXyz *attnPos = &i_this->attention_info.position; /* attention_info.position */
    attnPos->x = i_this->m02C0.x;
    f32 ay = i_this->m02C0.y;
    attnPos->y = ay;
    attnPos->z = i_this->m02C0.z;
    if (i_this->mType == PH_TYPE_SEAHAT) {
        attnPos->y = ay - 100.0f;
    }
    BG_check(i_this);
    return TRUE;
}

/* 023CEEFC */
static BOOL daPH_IsDelete(ph_class *) {

    return TRUE;
}

/* 023CEF04 */
static BOOL daPH_Delete(ph_class *i_this) {

    dComIfG_resDelete(&i_this->mPhs, i_this->mType != PH_TYPE_PEAHAT ? "Sh" : "Ph");
    dPa_rippleEcallBack_end((dPa_rippleEcallBack *)&i_this->mParticleCallBack);
    phNative_enemy_fire_remove(&i_this->mBodyEnemyFire);
    phNative_enemy_fire_remove(&i_this->mPropellerEnemyFire);
    return TRUE;
}

/* 023CEF6C */
static BOOL useHeapInit(fopAc_ac_c *a_this) {

    ph_class *i_this = (ph_class *)a_this;
    const char *arc = i_this->mType == PH_TYPE_PEAHAT ? "Ph" : "Sh";
    u32 modelFlag = i_this->mType == PH_TYPE_PEAHAT ? 0x80000 : 0;
    u32 dlFlag = i_this->mType == PH_TYPE_PEAHAT ? 0x37441422 : 0x11020203;

    /* propeller */
    J3DModelData *modelData = (J3DModelData *)dComIfG_getObjectRes(arc, 0x23, gc_tu.safeString);
    J3DAnmTransform *anm = (J3DAnmTransform *)dComIfG_getObjectRes(arc, 0x1C, gc_tu.safeString);
    i_this->mpPropellerMorf =
        mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP,
                               1.0f, 0, -1, 1, nullptr, modelFlag, dlFlag);
    if (i_this->mpPropellerMorf == nullptr || i_this->mpPropellerMorf->getModel() == nullptr) {
        return FALSE;
    }
    i_this->mpPropellerMorf->getModel()->mUserArea = gabi::ea(i_this); /* setUserArea */
    for (u16 i = 0; i < J3DModelData_getJointNum(
                            ((J3DModel *)i_this->mpPropellerMorf->getModel())->mModelData);
         i++) {
        setJointCallBack(((J3DModel *)i_this->mpPropellerMorf->getModel())->mModelData, i,
                         0x023C9678 /* nodeCallBack_UP */);
    }

    /* body */
    bool peahat = i_this->mType == PH_TYPE_PEAHAT;
    arc = peahat ? "Ph" : "Sh";
    modelFlag = peahat ? 0x80000 : 0;
    dlFlag = peahat ? 0x37441422 : 0x11020203;
    modelData = (J3DModelData *)dComIfG_getObjectRes(arc, 0x22, gc_tu.safeString);
    anm = (J3DAnmTransform *)dComIfG_getObjectRes(arc, 0x15, gc_tu.safeString);
    i_this->mpBodyMorf =
        mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP,
                               1.0f, 0, -1, 1, nullptr, modelFlag, dlFlag);
    if (i_this->mpBodyMorf == nullptr || i_this->mpBodyMorf->getModel() == nullptr) {
        return FALSE;
    }
    i_this->mpBodyMorf->getModel()->mUserArea = gabi::ea(i_this);
    for (u16 i = 0;
         i < J3DModelData_getJointNum(((J3DModel *)i_this->mpBodyMorf->getModel())->mModelData);
         i++) {
        setJointCallBack(((J3DModel *)i_this->mpBodyMorf->getModel())->mModelData, i,
                         0x023C97BC /* nodeCallBack_DW */);
    }

    if (!phNative_mDoExt_invisibleModel_create(&i_this->mPropellerInvisibleModel,
                                               i_this->mpPropellerMorf->getModel())) {
        return FALSE;
    }
    if (!phNative_mDoExt_invisibleModel_create(&i_this->mBodyInvisibleModel,
                                               i_this->mpBodyMorf->getModel())) {
        return FALSE;
    }
    J3DModel *model = i_this->mpBodyMorf->getModel();
    u32 jntHit = phNative_JntHit_create(
        model, gabi::ea(i_this->mType == PH_TYPE_PEAHAT ? &l_phJointHits : &l_shJointHits), 2);
    i_this->mpJntHit = jntHit;
    if (jntHit == 0) {
        return FALSE;
    }
    a_this->jntHit = jntHit;
    return TRUE;
}

/* 023CF600 */
static cPhs_State daPH_Create(fopAc_ac_c *a_this) {

    ph_class *i_this = (ph_class *)a_this;
    /* HD: the archive is loaded before fopAcM_ct */
    u8 type = fopAcM_GetParam(a_this) & 0xFF;
    if (type == 0xFF)
        type = 0;
    i_this->mType = type;
    u32 heapSize = 0x2740;
    cPhs_State ret;
    if (REG_S(8, 9) != 0) {
        i_this->mType = PH_TYPE_SEAHAT;
        ret = dComIfG_resLoad(&i_this->mPhs, "Sh");
        heapSize = 0x3E40;
    } else if (type == 0) {
        ret = dComIfG_resLoad(&i_this->mPhs, "Ph");
    } else {
        ret = dComIfG_resLoad(&i_this->mPhs, "Sh");
        heapSize = 0x3E40;
    }
    if (ret != cPhs_COMPLEATE_e) {
        return ret;
    }

    /* fopAcM_ct(a_this, ph_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr)
            ph_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    if (!fopAcM_entrySolidHeap(a_this, 0x023CEF6C /* useHeapInit */, heapSize)) {
        return cPhs_ERROR_e;
    }

    u32 prm = fopAcM_GetParam(a_this);
    u8 range2 = prm >> 16;
    u8 range1 = prm >> 8;
    if (i_this->mType == PH_TYPE_PEAHAT) {
        i_this->m0390 = 1000.0f;
        i_this->m0394 = 500.0f;
    } else {
        i_this->m0390 = 12000.0f;
        i_this->m0394 = 6000.0f;
        i_this->m039C = 8.0f;
        a_this->scale.x = 9.0f;
        a_this->scale.y = 9.0f;
        i_this->m03A0 = 5.0f;
        a_this->scale.z = 9.0f;
    }
    if (range1 != 0xFF)
        i_this->m0390 = (f32)range1 * 100.0f;
    if (range2 != 0xFF)
        i_this->m0394 = (f32)range2 * 100.0f;
    a_this->attention_info.flags =
        fopAc_Attn_LOCKON_BATTLE_e; /* attention_info.flags = LOCKON_BATTLE */
    i_this->m038C = i_this->m0390 + 800.0f;
    u8 type2 = i_this->mType;
    if (type2 == 0) {
        a_this->itemTableIdx = dComIfGp_CharTbl_GetIndex("p_hat", 0);
        a_this->attention_info.distances[2] = 3; /* attention_info.distances[2] */
        a_this->stealItemLeft = 1;
        i_this->m0344 = 1;
    } else {
        a_this->itemTableIdx = dComIfGp_CharTbl_GetIndex("sea_hat", 0);
        a_this->attention_info.distances[2] = 0x2A;
        a_this->gbaName = 0x1F;
    }
    a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpBodyMorf->getModel()));
    i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir,
                      &a_this->speed);
    i_this->mStts.Init(0x50, 4, a_this);
    a_this->max_health = 1;
    a_this->health = 1;
    copy_bits(&i_this->m02FC, &a_this->scale);
    i_this->mBodySph.Set(&body_sph_src);
    i_this->mBodySph.SetStts(&i_this->mStts);
    i_this->mBodySph.OffCoSPrmBit(1);
    i_this->mAtCyl.Set(&at_cyl_src);
    i_this->mAtCyl.SetStts(&i_this->mStts);
    i_this->mTgCyl.Set(&tg_cyl_src);
    i_this->mTgCyl.SetStts(&i_this->mStts);
    i_this->m0356 = ph_id_angle(i_this);
    i_this->m0358 = ph_id_angle(i_this);
    i_this->m035A = ph_id_angle(i_this);
    i_this->m035C = ph_id_angle(i_this);

    i_this->mEnemyIce.mpActor = a_this;
    i_this->mEnemyIce.m1B0 = 1;
    i_this->mEnemyIce.mWallRadius = 40.0f;
    i_this->mEnemyIce.mCylHeight = 40.0f;
    i_this->mBodyEnemyFire.mpMcaMorf = i_this->mpBodyMorf;
    i_this->mBodyEnemyFire.mpActor = a_this;
    for (int i = 0; i < 10; i++) {
        be<s8> *jntTbl = i_this->mType == PH_TYPE_SEAHAT ? l_shFlameJoints : l_phFlameJoints;
        i_this->mBodyEnemyFire.mFlameJntIdxs[i] = jntTbl[i];
        f32 s = l_bodyFlameScale[i];
        i_this->mBodyEnemyFire.mParticleScale[i] = s;
        i_this->mBodyEnemyFire.mParticleScale[i] = s + i_this->m039C;
    }
    i_this->mPropellerEnemyFire.mpMcaMorf = i_this->mpPropellerMorf;
    i_this->mPropellerEnemyFire.mpActor = a_this;
    for (int i = 0; i < 10; i++) {
        i_this->mPropellerEnemyFire.mFlameJntIdxs[i] = l_propellerFlameJoints[i];
        f32 s = l_propellerFlameScale[i];
        i_this->mPropellerEnemyFire.mParticleScale[i] = s;
        i_this->mPropellerEnemyFire.mParticleScale[i] = s + i_this->m039C;
    }

    i_this->m0380 = 30.0f;
    a_this->gravity = -3.0f;
    BG_check(i_this);
    copy_bits(&i_this->m032C, &a_this->current.pos);
    a_this->gravity = 0.0f;
    anm_init(i_this, 0x1C, 5.0f, 2, 1.0f, -1, 0);
    anm_init(i_this, 0x15, 5.0f, 2, 1.0f, -1, 1);

    if (i_this->mType == PH_TYPE_SEAHAT) {
        i_this->mAtCyl.mGObjAt.mSpl = 9;
        fopAcM_setCullSizeBox(a_this, -200.0f, -200.0f, -200.0f, 200.0f, 200.0f, 200.0f);
        f32 far = REG_F(8, 8) / (f32)l_farPlaneScale + 10000.0f;
        f32 s = i_this->m03A0;
        i_this->m02FC.x = s;
        i_this->m02FC.y = s;
        i_this->m02FC.z = s;
        i_this->m0380 = 100.0f;
        i_this->m033F = PH_ACTION_FLY;
        i_this->m0341 = 1;
        a_this->actor_status &= ~0x80000;
        i_this->m0346 = 0;
        a_this->cullSizeFar = far;
    } else {
        fopAcM_setCullSizeBox(a_this, -80.0f, -80.0f, -80.0f, 80.0f, 80.0f, 80.0f);
        i_this->mBodySph.mTgType = 0x081DC060;
        if (i_this->mAcch.ChkGroundHit()) {
            i_this->m02FC.x = 0.0f;
            i_this->m02FC.y = 0.0f;
            i_this->m033F = PH_ACTION_RISE;
            i_this->m0346 = 0x1E;
            i_this->m02FC.z = 0.0f;
        }
    }
    i_this->mpBodyMorf->play(nullptr, 0, 0);
    i_this->mpPropellerMorf->play(nullptr, 0, 0);
    DW_draw_SUB(i_this);
    copy_bits(&i_this->m0314, &i_this->m02C0);
    UP_draw_SUB(i_this);
    return ret;
}

// HD: compiler-generated and template code of this translation unit.

/* enemyfire::enemyfire (inline constructor, this TU's copy) */
/* 023CF3C0 */
static enemyfire *enemyfire_ct(enemyfire *self) {

    if (self == nullptr) {
        self = (enemyfire *)operator_new(sizeof(enemyfire));
        if (self == nullptr)
            return self;
    }
    if (gabi::ea(&self->mDirection) == 0)
        operator_new(0xC);
    dCcD_Stts_ct(&self->mStts);
    phSphereConstruct(&self->mSph); /* dCcD_Sph::dCcD_Sph */
    self->mLight.mHD20 = 1.0f;
    return self;
}

/* ph_class::ph_class (member constructors; HD: allocates when this == NULL) */
/* 023CF44C */
static ph_class *ph_class_ct(ph_class *self) {

    if (self == nullptr) {
        self = (ph_class *)operator_new(sizeof(ph_class));
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = PH_VTBL;
    phAcchCirConstruct(&self->mAcchCir); /* dBgS_AcchCir::dBgS_AcchCir */
    phAcchConstruct(&self->mAcch);       /* dBgS_Acch::dBgS_Acch */
    dBgS_Acch *acch = &self->mAcch;      /* dBgS_ObjAcch */
    acch->cBgS_Chk::__vtbl = gc_tu.objAcch.v10;
    acch->dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    acch->dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    acch->mbObjThrough = 1;
    phRippleConstruct(&self->mParticleCallBack); /* dPa_rippleEcallBack::dPa_rippleEcallBack */
    dCcD_Stts_ct(&self->mStts);
    phSphereConstruct(&self->mBodySph); /* dCcD_Sph::dCcD_Sph */
    dCcD_Cyl_ct(&self->mAtCyl, AAB_VTBL);
    dCcD_Cyl_ct(&self->mTgCyl, AAB_VTBL);
    /* enemyice */
    dCcD_Stts_ct(&self->mEnemyIce.mStts);
    dCcD_Cyl_ct(&self->mEnemyIce.mCyl, AAB_VTBL);
    phAcchCirConstruct(&self->mEnemyIce.mBgAcchCir);
    phAcchConstruct(&self->mEnemyIce.mBgAcch);
    acch = &self->mEnemyIce.mBgAcch;
    acch->dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    acch->cBgS_Chk::__vtbl = gc_tu.objAcch.v10;
    acch->mbObjThrough = 1;
    acch->dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    enemyfire_ct(&self->mBodyEnemyFire);
    enemyfire_ct(&self->mPropellerEnemyFire);
    phInvisibleConstruct(
        &self->mPropellerInvisibleModel); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    phInvisibleConstruct(&self->mBodyInvisibleModel);
    return self;
}

/* static initialisation of the translation unit (header statics only) */
/* 023CFD48 */
static void __sinit_d_a_ph_cpp() {

    sinit_header_statics(gabi::ea(&l_staticListHead), gabi::ea(&l_staticConstructors));
}

/* sead::SafeString deleting destructor (this TU's copy) */
/* 023CFDDC */
static void SafeString_dt(void *p, s32 flags) {

    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* ph_class deleting destructor (HD virtual destructor) */
/* 023D0B08 */
static void ph_class_dt(ph_class *self, s32 flags) {

    if (self == nullptr)
        return;
    phInvisibleDestroy(&self->mBodyInvisibleModel, 2); /* ~mDoExt_invisibleModel */
    phInvisibleDestroy(&self->mPropellerInvisibleModel, 2);
    phSphereDestroy(&self->mPropellerEnemyFire.mSph, 2); /* dCcD_Sph::~dCcD_Sph */
    dCcD_Stts_dt(&self->mPropellerEnemyFire.mStts, 2);
    phSphereDestroy(&self->mBodyEnemyFire.mSph, 2);
    dCcD_Stts_dt(&self->mBodyEnemyFire.mStts, 2);
    dBgS_Acch *acch = &self->mEnemyIce.mBgAcch; /* ~dBgS_ObjAcch (inline) */
    acch->dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    acch->dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    phAcchDestroy(gabi::ea(acch), 0);                                /* dBgS_Acch::~dBgS_Acch */
    phCircleDestroy(gabi::ea(&self->mEnemyIce.mBgAcchCir.m_cir), 2); /* cM3dGCir::~cM3dGCir */
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    dCcD_Cyl_dt(&self->mTgCyl, 2);
    dCcD_Cyl_dt(&self->mAtCyl, 2);
    phSphereDestroy(&self->mBodySph, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    acch = &self->mAcch;
    acch->dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
    acch->dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
    phAcchDestroy(gabi::ea(acch), 0);
    phCircleDestroy(gabi::ea(&self->mAcchCir.m_cir), 2);
    phActorDestroy(self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1)
        operator_delete(self);
}

// HD: empty SafeString virtual entry.
/* 023D0C4C */
static void ph_emptyVirtual(void *) {}

// clang-format off
/* WWHD glue
data l_phFlameJoints 101CE764
data l_shFlameJoints 101CE770
data l_propellerFlameJoints 101CE77C
data l_bodyFlameScale 101CE714
data l_propellerFlameScale 101CE73C
data body_sph_src 101CE6D4
data at_cyl_src 101CE788
data tg_cyl_src 101CE7CC
data l_farPlaneScale 1048D04C
data l_boomerangReaction 101EACB7
data l_staticListHead 1046CC1C
data l_staticConstructors 101CE810
data l_phJointHits 101CE68C
data l_shJointHits 101CE6A4
str 100344B4 "Ph" in 023C9AD8
str 100344B7 "Sh" in 023C9AD8
str 100345AF "Sh" in 023CEF04
str 100345AC "Ph" in 023CEF04
str 100345B2 "Ph" in 023CEF6C
str 100345B5 "Sh" in 023CEF6C
str 100345E7 "Sh" in 023CF600
str 100345E4 "Ph" in 023CF600
str 100345DC "p_hat" in 023CF600
str 100345D4 "sea_hat" in 023CF600
data l_currentModel 104B462C
data l_currentMtx 104B4868
ext 020402C8 BOOL phNative_enemy_ice(enemyice* ice)
ext 02041570 void phNative_enemy_fire(enemyfire* fire)
ext 02041C30 void phNative_enemy_fire_remove(enemyfire* fire)
ext 028E8DAC void phNative_PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out)
ext 025BEBB8 void phNative_dSnap_RegistFig(s32 type, fopAc_ac_c* a, cXyz* pos, s16 angleY, f32 x, f32 y, f32 z)
ext 0259138C void phNative_mDoExt_invisibleModel_entryDL(mDoExt_McaMorf* morf, s32 p, void* inv)
ext 025E8A48 BOOL phNative_mDoExt_invisibleModel_create(void* inv, J3DModel* model)
ext 02552B60 u32 phNative_JntHit_create(J3DModel* model, u32 src, s16 num)
ext 0200E814 s32 phNameTableGetIndex(void* table, const char* name, s32 p)
ext 025192A8 void phNative_cc_at_check(fopAc_ac_c* a, CcAtInfo* info)
ext 025E1AA4 void phNative_mDoAud_monsSeStart(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb)
ext 024EECAC u32 phNative_dBgS_GetMtrlSndId(dBgS* bgs, void* poly)
ext 025DA088 void phNative_fopAcM_setGbaName(fopAc_ac_c* a, u8 itemNo, u8 n0, u8 n1)
ext 025166F0 void phSphereConstruct(dCcD_Sph* sphere)
ext 024EFE94 void phAcchCirConstruct(dBgS_AcchCir* cir)
ext 024F0474 void phAcchConstruct(dBgS_Acch* acch)
ext 025A9084 void phRippleConstruct(void* callback)
ext 025E895C void phInvisibleConstruct(void* model)
ext 025E89F8 void phInvisibleDestroy(void* model, s32 flags)
ext 02515AE8 void phSphereDestroy(dCcD_Sph* sphere, s32 flags)
ext 024EFD9C void phAcchDestroy(u32 acch, s32 flags)
ext 02018034 void phCircleDestroy(u32 circle, s32 flags)
ext 025D50BC void phActorDestroy(fopAc_ac_c* actor, s32 flags)
ext 025E1FD8 void phCancelBoomerangLock()
*/
// clang-format on
