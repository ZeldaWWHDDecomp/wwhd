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

#define SAFESTRING_VTBL 0x1003443C /* this TU's sead::SafeString vtable */
#define PH_VTBL 0x10034494         /* ph_class vtable (HD virtual destructor) */
#define AAB_VTBL 0x10034454        /* this TU's cM3dGAab vtable */

enum { DSNAP_TYPE_PH = 0xB1, DSNAP_TYPE_SH = 0xBB };
enum { JA_SE_CM_PH_PROPELLER = 0x5857, JA_SE_CM_SH_PROPELLER = 0x5127 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline BOOL enemy_ice(enemyice* ice) { return gabi::call<BOOL>(0x020402C8, ice); }
static inline void enemy_fire(enemyfire* fire) { gabi::call(0x02041570, fire); }
static inline void enemy_fire_remove(enemyfire* fire) { gabi::call(0x02041C30, fire); }
static inline void PSVECSubtract(const cXyz* a, const cXyz* b, cXyz* out) { gabi::call(0x028E8DAC, a, b, out); }
/* 025BEBB8 dSnap_RegistFig(type, actor, pos, angleY, sx, sy, sz) (the overload with a position) */
static inline void dSnap_RegistFig(s32 type, fopAc_ac_c* a, cXyz* pos, s16 angleY, f32 x, f32 y, f32 z) {
    gabi::call(0x025BEBB8, type, a, pos, angleY, x, y, z);
}
/* 0259138C mDoExt_invisibleModel::entryDL(McaMorf*, s8, ...) -- the matcher names it dMat_ice_c::entryDL */
static inline void mDoExt_invisibleModel_entryDL(mDoExt_McaMorf* morf, s32 p, void* inv) { gabi::call(0x0259138C, morf, p, inv); }
static inline dSv_event_c* dComIfGs_getEvent() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
/* J3D (HD): j3dSys.mModel 0x104B462C, J3DSys::mCurrentMtx 0x104B4868, joint block at model+0x2C */
struct J3DMtxBlock_l {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ gptr<Mtx34> mpMtx;
};
struct J3DModel_l {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_l> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ gptr<J3DModelData> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
};
static inline Mtx34* getAnmMtx(J3DModel_l* m, s32 jnt) {
    J3DMtxBlock_l* blk = m->mpMtxBlock;
    blk->mFlags |= 0x10;
    return gabi::at<Mtx34>(gabi::ea(blk->mpMtx.get()) + jnt * 0x30);
}
static inline J3DModel_l* j3dSys_getModel() { return gabi::at<J3DModel_l>(gabi::load<u32>(0x104B462C)); }
static inline Mtx34* J3DSys_mCurrentMtx() { return gabi::at<Mtx34>(0x104B4868); }

/* 023C9678 */
static BOOL nodeCallBack_UP(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x023C9678, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel_l* model = j3dSys_getModel();
        ph_class* i_this = gabi::at<ph_class>(model->mUserArea);
        J3DJoint* joint = J3DNode_toJoint(node);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            if (jntNo == 0) {
                gabi::Local<cXyz> offset;
                offset->x = 0.0f;
                offset->y = -80.0f;
                offset->z = 0.0f;
                MtxPosition(offset, &i_this->m02D8);
                mtx_copy(getAnmMtx(model, jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
            }
        }
    }
    return TRUE;
}
VERIFY(0x023C9678, nodeCallBack_UP);

/* 023C97BC */
static BOOL nodeCallBack_DW(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x023C97BC, BOOL, node, calcTiming);
    if (calcTiming == 0) {
        J3DModel_l* model = j3dSys_getModel();
        ph_class* i_this = gabi::at<ph_class>(model->mUserArea);
        J3DJoint* joint = J3DNode_toJoint(node);
        s32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != nullptr) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            gabi::Local<cXyz> offset;
            if (jntNo == 1) {
                offset->z = 0.0f;
                offset->x = 0.0f;
                offset->y = 0.0f;
                MtxPosition(offset, &i_this->m02C0);
                mtx_copy(getAnmMtx(model, jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
            } else if (jntNo == 0) {
                offset->z = 0.0f;
                offset->x = 0.0f;
                offset->y = 0.0f;
                MtxPosition(offset, &i_this->m02CC);
                mtx_copy(getAnmMtx(model, jntNo), calc_mtx());
                PSMTXCopy(calc_mtx(), J3DSys_mCurrentMtx());
            }
        }
    }
    return TRUE;
}
VERIFY(0x023C97BC, nodeCallBack_DW);

/* 023C9994 */
static BOOL daPH_Draw(ph_class* i_this) {
    WWHD_FUNC(0x023C9994, BOOL, i_this);
    J3DModel* bodyModel = i_this->mpBodyMorf->getModel();
    J3DModel* propModel = i_this->mpPropellerMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), bodyModel, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), propModel, &i_this->tevStr);

    dSnap_RegistFig(i_this->mType == 1 ? DSNAP_TYPE_SH : DSNAP_TYPE_PH, i_this, gabi::at<cXyz>(gabi::ea(i_this) + 0x390),
                    i_this->shape_angle.y, 1.0f, 1.0f, 1.0f);

    if (i_this->mEnemyIce.mFreezeTimer > 20) {
        /* frozen: draw the ice */
        mDoExt_invisibleModel_entryDL(i_this->mpBodyMorf, -1, i_this->mBodyInvisibleModel);
        if (i_this->m02FC.x != 0.0f && i_this->m037C == 0.0f) {
            mDoExt_invisibleModel_entryDL(i_this->mpPropellerMorf, -1, i_this->mPropellerInvisibleModel);
        }
        return TRUE;
    }

    dComIfGp_get(); /* HD: remnant of the shadow code */
    i_this->mpBodyMorf->entryDL();
    i_this->mpPropellerMorf->entryDL();
    gabi::Local<cXyz> shadowPos;
    shadowPos->x = i_this->current.pos.x;
    shadowPos->y = i_this->current.pos.y;
    shadowPos->z = i_this->current.pos.z;
    PSVECAdd(shadowPos, &i_this->m02E4, shadowPos);
    return TRUE;
}
VERIFY(0x023C9994, daPH_Draw);

/* 023C9AD8 */
void anm_init(ph_class* i_this, int bckFileIdx, float morf, unsigned char loopMode, float playSpeed, int soundFileIdx, int part) {
    WWHD_FUNC(0x023C9AD8, void, i_this, bckFileIdx, morf, loopMode, playSpeed, soundFileIdx, part);
    const char* arc;
    mDoExt_McaMorf* pMorf;
    if (i_this->mType == 0) {
        arc = STR(0x100344B4); /* "Ph" */
        if (part == 0) {
            i_this->m0374 = bckFileIdx;
        }
    } else {
        arc = STR(0x100344B7); /* "Sh" */
    }
    if (soundFileIdx >= 0) {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(arc, bckFileIdx, SAFESTRING_VTBL);
        void* sound = dComIfG_getObjectRes(arc, soundFileIdx, SAFESTRING_VTBL);
        pMorf = part == 0 ? i_this->mpPropellerMorf.get() : i_this->mpBodyMorf.get();
        pMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, sound);
    } else {
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(arc, bckFileIdx, SAFESTRING_VTBL);
        pMorf = part == 0 ? i_this->mpPropellerMorf.get() : i_this->mpBodyMorf.get();
        pMorf->setAnm(anm, loopMode, morf, playSpeed, 0.0f, -1.0f, nullptr);
    }
}
VERIFY(0x023C9AD8, anm_init);

/* float -> u32 (GHS: fctiwz with the 2^31 adjustment) */
static inline u32 ftou(f32 f) {
    if (f < 2147483648.0f) return (u32)gabi::ftoi(f);
    return (u32)gabi::ftoi(f - 2147483648.0f) + 0x80000000u;
}

/* 023C9E58 */
void puropera_sound(ph_class* i_this) {
    WWHD_FUNC(0x023C9E58, void, i_this);
    if (i_this->mType == 0) {
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
        if (vol > 100) vol = 100;
        fopAcM_seStart(i_this, JA_SE_CM_SH_PROPELLER, vol);
    }
}
VERIFY(0x023C9E58, puropera_sound);

/* 023CA054 */
void puropera_kaiten(ph_class* i_this) {
    WWHD_FUNC(0x023CA054, void, i_this);
    i_this->m033A = i_this->m033A + i_this->m0348;
    cLib_addCalcAngleS2(&i_this->m0348, i_this->m034A, 1, i_this->m034C);
    cLib_addCalcAngleS2(&i_this->m034C, 0x100, 1, 0x10);
    puropera_sound(i_this);
}
VERIFY(0x023CA054, puropera_kaiten);

/* 023CA0BC */
void fuwafuwa_set(ph_class* i_this) {
    WWHD_FUNC(0x023CA0BC, void, i_this);
    if (i_this->mType == 0) {
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
VERIFY(0x023CA0BC, fuwafuwa_set);

/* 023CB2E0 */
BOOL ph_wall_hit_check(ph_class* i_this) {
    WWHD_FUNC(0x023CB2E0, BOOL, i_this);
    if (!i_this->mAcch.ChkWallHit()) {
        return FALSE;
    }
    i_this->current.angle.y += 0x4000;
    i_this->m033F = 1;
    i_this->mAtCyl.OffAtSPrmBit(1);
    i_this->m0346 = 10;
    i_this->speedF = i_this->mType == 0 ? 28.0f : 48.0f;
    return TRUE;
}
VERIFY(0x023CB2E0, ph_wall_hit_check);

/* 023CB368 */
BOOL ph_hani_check(ph_class* i_this, float range, float height, unsigned char useHome) {
    WWHD_FUNC(0x023CB368, BOOL, i_this, range, height, useHome);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
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
        if (!(std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < range)) return FALSE;
    } else {
        f32 dz = i_this->m032C.z - player->current.pos.z;
        f32 dx = i_this->m032C.x - px;
        dy = i_this->m032C.y - py;
        if (!(std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < range)) return FALSE;
    }
    if (!(std_sqrtf(dy * dy) < height)) return FALSE;
    return TRUE;
}
VERIFY(0x023CB368, ph_hani_check);

/* 023CBAC0 */
void dead_item(ph_class* i_this) {
    WWHD_FUNC(0x023CBAC0, void, i_this);
    gabi::Local<cXyz> pos;
    pos->x = i_this->current.pos.x;
    pos->y = i_this->current.pos.y;
    pos->z = i_this->current.pos.z;
    PSVECAdd(pos, &i_this->m02E4, pos);
    u8 scale = (u8)gabi::ftoi(i_this->m039C + i_this->m039C + 5.0f);
    if (i_this->mType == 0) {
        fopAcM_createDisappear(i_this, pos, scale, 0, i_this->stealItemBitNo);
    } else {
        fopAcM_createDisappear(i_this, pos, scale, 0, 0xFF);
        /* Seahat kill counter */
        s32 n = dSv_event_getEventReg(dComIfGs_getEvent(), 0x7EFF) + 1;
        u8 v = n > 0xFF ? 0xFF : (u8)n;
        dSv_event_setEventReg(dComIfGs_getEvent(), 0x7EFF, v);
    }
    dComIfGs_onActor(i_this->setID, i_this->home.roomNo);
    fopAcM_delete(i_this);
}
VERIFY(0x023CBAC0, dead_item);

static inline void ph_crr_pos(ph_class* i_this) {
    PSVECAdd(&i_this->current.pos, &i_this->m02E4, &i_this->current.pos);
    PSVECAdd(&i_this->old.pos, &i_this->m02E4, &i_this->old.pos);
    f32 o = i_this->m0380;
    i_this->current.pos.y = i_this->current.pos.y - o;
    i_this->old.pos.y = i_this->old.pos.y - o;
    i_this->mAcch.CrrPos(dComIfG_Bgsp());
    i_this->current.pos.y = i_this->current.pos.y + i_this->m0380;
    i_this->old.pos.y = i_this->old.pos.y + i_this->m0380;
    PSVECSubtract(&i_this->current.pos, &i_this->m02E4, &i_this->current.pos);
    PSVECSubtract(&i_this->old.pos, &i_this->m02E4, &i_this->old.pos);
}

/* 023CBBE4 */
void BG_check(ph_class* i_this) {
    WWHD_FUNC(0x023CBBE4, void, i_this);
    if (i_this->mType == 0) {
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
VERIFY(0x023CBBE4, BG_check);

/* 023CBD80 */
void UP_draw_SUB(ph_class* i_this) {
    WWHD_FUNC(0x023CBD80, void, i_this);
    J3DModel* model = i_this->mpPropellerMorf->getModel();
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
    enemy_fire(&i_this->mPropellerEnemyFire);
}
VERIFY(0x023CBD80, UP_draw_SUB);

/* 023CBF44 */
void DW_draw_SUB(ph_class* i_this) {
    WWHD_FUNC(0x023CBF44, void, i_this);
    J3DModel* model = i_this->mpBodyMorf->getModel();
    PSMTXTrans(mDoMtx_stack_c::get(), i_this->current.pos.x + i_this->m02E4.x, i_this->current.pos.y + i_this->m02E4.y,
               i_this->current.pos.z + i_this->m02E4.z);
    mDoMtx_stack_c::YrotM(i_this->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->shape_angle.z);
    mDoMtx_stack_scaleM(i_this->scale.x, i_this->scale.y, i_this->scale.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    i_this->mpBodyMorf->calc();
    enemy_fire(&i_this->mBodyEnemyFire);
}
VERIFY(0x023CBF44, DW_draw_SUB);

/* 023CEEFC */
static BOOL daPH_IsDelete(ph_class*) {
    WWHD_FUNC(0x023CEEFC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x023CEEFC, daPH_IsDelete);

/* 023CEF04 */
static BOOL daPH_Delete(ph_class* i_this) {
    WWHD_FUNC(0x023CEF04, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, i_this->mType != 0 ? STR(0x100345AF) /* "Sh" */ : STR(0x100345AC) /* "Ph" */);
    dPa_rippleEcallBack_end((dPa_rippleEcallBack*)i_this->mParticleCallBack);
    enemy_fire_remove(&i_this->mBodyEnemyFire);
    enemy_fire_remove(&i_this->mPropellerEnemyFire);
    return TRUE;
}
VERIFY(0x023CEF04, daPH_Delete);

/* 023CF3C0: enemyfire::enemyfire (inline constructor, this TU's copy) */
static enemyfire* enemyfire_ct(enemyfire* self) {
    WWHD_FUNC(0x023CF3C0, enemyfire*, self);
    if (self == nullptr) {
        self = (enemyfire*)operator_new(sizeof(enemyfire));
        if (self == nullptr) return self;
    }
    if (gabi::ea(&self->mDirection) == 0) operator_new(0xC);
    dCcD_Stts_ct(&self->mStts);
    gabi::call(0x025166F0, &self->mSph); /* dCcD_Sph::dCcD_Sph */
    self->m228 = 1.0f;
    return self;
}
VERIFY(0x023CF3C0, enemyfire_ct);

/* 023CF44C: ph_class::ph_class (member constructors; HD: allocates when this == NULL) */
static ph_class* ph_class_ct(ph_class* self) {
    WWHD_FUNC(0x023CF44C, ph_class*, self);
    if (self == nullptr) {
        self = (ph_class*)operator_new(sizeof(ph_class));
        if (self == nullptr) return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = PH_VTBL;
    gabi::call(0x024EFE94, &self->mAcchCir); /* dBgS_AcchCir::dBgS_AcchCir */
    gabi::call(0x024F0474, &self->mAcch);    /* dBgS_Acch::dBgS_Acch */
    u32 acch = gabi::ea(&self->mAcch);       /* dBgS_ObjAcch */
    gabi::store<u32>(acch + 0x10, 0x10034464);
    gabi::store<u32>(acch + 0x20, 0x10034474);
    gabi::store<u32>(acch + 0x14, 0x10034484);
    gabi::store<u8>(acch + 0x18, 1);
    gabi::call(0x025A9084, self->mParticleCallBack); /* dPa_rippleEcallBack::dPa_rippleEcallBack */
    dCcD_Stts_ct(&self->mStts);
    gabi::call(0x025166F0, &self->mBodySph); /* dCcD_Sph::dCcD_Sph */
    dCcD_Cyl_ct(&self->mAtCyl, AAB_VTBL);
    dCcD_Cyl_ct(&self->mTgCyl, AAB_VTBL);
    /* enemyice */
    dCcD_Stts_ct(&self->mEnemyIce.mStts);
    dCcD_Cyl_ct(&self->mEnemyIce.mCyl, AAB_VTBL);
    gabi::call(0x024EFE94, &self->mEnemyIce.mBgAcchCir);
    gabi::call(0x024F0474, &self->mEnemyIce.mBgAcch);
    acch = gabi::ea(&self->mEnemyIce.mBgAcch);
    gabi::store<u32>(acch + 0x20, 0x10034474);
    gabi::store<u32>(acch + 0x10, 0x10034464);
    gabi::store<u8>(acch + 0x18, 1);
    gabi::store<u32>(acch + 0x14, 0x10034484);
    enemyfire_ct(&self->mBodyEnemyFire);
    enemyfire_ct(&self->mPropellerEnemyFire);
    gabi::call(0x025E895C, self->mPropellerInvisibleModel); /* mDoExt_invisibleModel::mDoExt_invisibleModel */
    gabi::call(0x025E895C, self->mBodyInvisibleModel);
    return self;
}
VERIFY(0x023CF44C, ph_class_ct);

/* 023CFD48: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_ph_cpp() {
    WWHD_FUNC(0x023CFD48, void, (u32)0);
    sinit_header_statics(0x1046CC1C, 0x101CE810);
}
VERIFY(0x023CFD48, __sinit_d_a_ph_cpp);

/* 023CFDDC: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x023CFDDC, void, p, flags);
    if (p != nullptr && (flags & 1)) operator_delete(p);
}
VERIFY(0x023CFDDC, SafeString_dt);

/* 023D0B08: ph_class deleting destructor (HD virtual destructor) */
static void ph_class_dt(ph_class* self, s32 flags) {
    WWHD_FUNC(0x023D0B08, void, self, flags);
    if (self == nullptr) return;
    gabi::call(0x025E89F8, self->mBodyInvisibleModel, 2); /* ~mDoExt_invisibleModel */
    gabi::call(0x025E89F8, self->mPropellerInvisibleModel, 2);
    gabi::call(0x02515AE8, &self->mPropellerEnemyFire.mSph, 2); /* dCcD_Sph::~dCcD_Sph */
    dCcD_Stts_dt(&self->mPropellerEnemyFire.mStts, 2);
    gabi::call(0x02515AE8, &self->mBodyEnemyFire.mSph, 2);
    dCcD_Stts_dt(&self->mBodyEnemyFire.mStts, 2);
    u32 acch = gabi::ea(&self->mEnemyIce.mBgAcch); /* ~dBgS_ObjAcch (inline) */
    gabi::store<u32>(acch + 0x20, 0x10034474);
    gabi::store<u32>(acch + 0x14, 0x10034484);
    gabi::call(0x024EFD9C, acch, 0);                                         /* dBgS_Acch::~dBgS_Acch */
    gabi::call(0x02018034, gabi::ea(&self->mEnemyIce.mBgAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir */
    dCcD_Cyl_dt(&self->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&self->mEnemyIce.mStts, 2);
    dCcD_Cyl_dt(&self->mTgCyl, 2);
    dCcD_Cyl_dt(&self->mAtCyl, 2);
    gabi::call(0x02515AE8, &self->mBodySph, 2);
    dCcD_Stts_dt(&self->mStts, 2);
    acch = gabi::ea(&self->mAcch);
    gabi::store<u32>(acch + 0x20, 0x10034474);
    gabi::store<u32>(acch + 0x14, 0x10034484);
    gabi::call(0x024EFD9C, acch, 0);
    gabi::call(0x02018034, gabi::ea(&self->mAcchCir) + 0x14, 2);
    gabi::call(0x025D50BC, self, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(self);
}
VERIFY(0x023D0B08, ph_class_dt);

/* 023D0C4C: empty virtual function in this TU's vtable (0x10034444) */
static void empty_virtual_023D0C4C(void*) {
    WWHD_FUNC(0x023D0C4C, void, (u32)0);
}
VERIFY(0x023D0C4C, empty_virtual_023D0C4C);

/* 023CA258 */
void fly_angle_set(ph_class* i_this, unsigned char mode) {
    WWHD_FUNC(0x023CA258, void, i_this, mode);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
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
            cLib_addCalcAngleS2(&i_this->current.angle.x, cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz))), 1, 0x200);
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
        cLib_addCalcAngleS2(&i_this->current.angle.x, cM_atan2s(dy, std_sqrtf(gabi::fmadds(dx, dx, dz * dz))), 1, 0x200);
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
        if (i_this->mType == 0) {
            i_this->m035C += 800;
        } else {
            i_this->m035C += 900;
        }
        cLib_addCalcAngleS2(&i_this->shape_angle.x, (s16)gabi::ftoi(cM_ssin(i_this->m035C) * 4000.0f), 1, 0x200);
    }
    if (mode != 4) {
        cLib_addCalcAngleS2(&i_this->shape_angle.y, targetY, 1, stepY);
    }
    cLib_addCalcAngleS2(&i_this->shape_angle.z, targetZ, 1, 0x200);
}
VERIFY(0x023CA258, fly_angle_set);

/* fopAcM_seStart with the HD null checks, returning whether the sound was started */
static inline bool ph_seStart(ph_class* i_this, u32 id, u32 param) {
    if (i_this == nullptr || gabi::ea(&i_this->eyePos) == 0) return false;
    mDoAud_seStart(id, &i_this->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
    return true;
}
/* bounce off the player after a hit */
static inline BOOL ph_hajiki_set(ph_class* i_this, fopAc_ac_c* player) {
    i_this->current.angle.y = cM_atan2s(i_this->current.pos.x + i_this->m02E4.x - player->current.pos.x,
                                        i_this->current.pos.z + i_this->m02E4.z - player->current.pos.z);
    i_this->m033F = 1;
    i_this->mAtCyl.OffAtSPrmBit(1);
    i_this->speedF = i_this->mType != 0 ? 48.0f : 28.0f;
    i_this->m0346 = 10;
    return TRUE;
}

/* 023CAFB0 */
BOOL hajiki_check(ph_class* i_this) {
    WWHD_FUNC(0x023CAFB0, BOOL, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    bool hit = false;
    if (i_this->mTgCyl.ChkTgHit()) {
        void* obj = i_this->mTgCyl.GetTgHitObj();
        if (obj != nullptr) {
            u32 type = gabi::load<u32>(gabi::ea(obj) + 0x10); /* At type */
            u32 se;
            if ((type & 0x2) || (type & 0x800) || (type & 0x14000400)) {
                se = 0x2803;
            } else if (type & 0x010000C0) {
                se = 0x2833;
            } else if (!(type & 0x00200000)) {
                se = 0x2834;
            } else {
                /* skull hammer: knocked down */
                i_this->m033F = 5;
                i_this->mAtCyl.OffAtSPrmBit(1);
                i_this->m0346 = 0x32;
                return TRUE;
            }
            if (ph_seStart(i_this, se, 0x42)) {
                type = gabi::load<u32>(gabi::ea(obj) + 0x10);
            }
            if (type & 0x2) {
                hit = true;
            }
        }
    }
    if (i_this->mTgCyl.mGObjAt.mRPrm & 1) {
        /* the propeller hit something: sparks */
        gabi::Local<cXyz> pos;
        pos->x = i_this->mTgCyl.mGObjAt.mHitPos.x;
        pos->y = i_this->mTgCyl.mGObjAt.mHitPos.y;
        pos->z = i_this->mTgCyl.mGObjAt.mHitPos.z;
        dPa_control_set(dComIfGp_getParticle(), 0, 0xC, pos, nullptr, nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
        fopAcM_seStart(i_this, 0x6817, 0);
        return ph_hajiki_set(i_this, player);
    }
    if ((i_this->mAtCyl.mGObjAt.mRPrm & 1) || hit) {
        return ph_hajiki_set(i_this, player);
    }
    return FALSE;
}
VERIFY(0x023CAFB0, hajiki_check);

static inline void ph_ripple_end(ph_class* i_this) { dPa_rippleEcallBack_end((dPa_rippleEcallBack*)i_this->mParticleCallBack); }

/* 023CB4C0: 0 out of the water, 1 in BG water, 2 in the sea */
int sea_water_check(ph_class* i_this, unsigned char mode) {
    WWHD_FUNC(0x023CB4C0, int, i_this, mode);
    gabi::store<u32>(gabi::ea(&i_this->m05BC.x), gabi::load<u32>(gabi::ea(&i_this->current.pos.x)));
    i_this->m05BC.y = i_this->m032C.y;
    gabi::store<u32>(gabi::ea(&i_this->m05BC.z), gabi::load<u32>(gabi::ea(&i_this->current.pos.z)));
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
        if (!(i_this->current.pos.y + i_this->m02E4.y < waterY + margin)) goto out;
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
        if (!(i_this->mAcch.m_flags & dBgS_Acch::WATER_IN)) goto out;
        waterY = gabi::load<f32>(gabi::ea(&i_this->mAcch) + 0x1BC); /* water height */
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
                    cLib_addCalc2(&i_this->current.pos.y, waterY - gabi::fmadds(s, 40.0f, 20.0f), 1.0f, speed);
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
        gabi::Local<cXyz> scale;
        gabi::Local<cXyz> splashPos;
        f32 splashScale;
        if (i_this->mType != 1) {
            scale->y = 1.0f;
            scale->x = 1.0f;
            scale->z = 1.0f;
            ph_ripple_end(i_this);
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &i_this->m05BC, nullptr, scale, 0xFF,
                            (dPa_levelEcallBack*)i_this->mParticleCallBack, -1, nullptr, nullptr, nullptr);
        } else {
            scale->x = 4.0f;
            scale->z = 4.0f;
            scale->y = 4.0f;
            dPa_control_c* pa = dComIfGp_getParticle();
            if (pa != nullptr) {
                PSVECScale(scale, scale, gabi::load<f32>(gabi::ea(pa) + 0xF8));
            }
            ph_ripple_end(i_this);
            dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &i_this->m05BC, nullptr, scale, 0xFF,
                            (dPa_levelEcallBack*)i_this->mParticleCallBack, -1, nullptr, nullptr, nullptr);
        }
        splashPos->z = i_this->m05BC.z;
        splashPos->y = i_this->m05BC.y;
        splashPos->x = i_this->m05BC.x;
        splashScale = 0.5f + i_this->m039C;
        i_this->m0370 = 0;
        i_this->m0341 = 1;
        gabi::store<f32>(gabi::ea(i_this) + 0x6D4, 0.0f); /* ripple callback rate */
        dPa_control_c* pa = dComIfGp_getParticle();
        if (pa != nullptr) {
            splashScale *= gabi::load<f32>(gabi::ea(pa) + 0xF8);
        }
        fopKyM_createWpillar(splashPos, splashScale, 0.5f, 0);
        if (i_this->m033F == 2) {
            fopAcM_seStart(i_this, 0x69FB, 0);
        } else if (i_this->m02FC.x != 0.0f && i_this->m037C == 0.0f) {
            fopAcM_seStart(i_this, 0x69F9, 0);
        }
    }
    return inSea ? 2 : 1;

out:
    if (i_this->m0341 != 0) {
        ph_ripple_end(i_this);
        i_this->m0398 = 0.0f;
        i_this->m0341 = 0;
    }
    return 0;
}
VERIFY(0x023CB4C0, sea_water_check);

/* 027F3F94 (the matcher names it __nw): returns the model data's joint tree; joint count (u16) at +8 */
static inline u16 J3DModelData_getJointNum(J3DModelData* d) { return gabi::load<u16>(gabi::ea(gabi::call<void*>(0x027F3F94, d)) + 8); }
/* J3DModelData::getJointNodePointer(i)->setCallBack(cb) (HD: nodes of 0x1C bytes from +8; index checked against +4) */
static inline void setJointCallBack(J3DModelData* d, u16 i, u32 cb) {
    u32 data = gabi::ea(d);
    u32 n = gabi::load<u32>(data + 4);
    u32 p = gabi::load<u32>(data + 8);
    if (i < n) p += i * 0x1C;
    gabi::store<u32>(p + 8, cb);
}
static inline BOOL mDoExt_invisibleModel_create(void* inv, J3DModel* model) { return gabi::call<BOOL>(0x025E8A48, inv, model); }
static inline u32 JntHit_create(J3DModel* model, u32 src, s16 num) { return gabi::call<u32>(0x02552B60, model, src, num); }

/* 023CEF6C */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x023CEF6C, BOOL, a_this);
    ph_class* i_this = (ph_class*)a_this;
    const char* arc = i_this->mType == 0 ? STR(0x100345B2) /* "Ph" */ : STR(0x100345B5) /* "Sh" */;
    u32 modelFlag = i_this->mType == 0 ? 0x80000 : 0;
    u32 dlFlag = i_this->mType == 0 ? 0x37441422 : 0x11020203;

    /* propeller */
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(arc, 0x23, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(arc, 0x1C, SAFESTRING_VTBL);
    i_this->mpPropellerMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f,
                                                     0, -1, 1, nullptr, modelFlag, dlFlag);
    if (i_this->mpPropellerMorf == nullptr || i_this->mpPropellerMorf->getModel() == nullptr) {
        return FALSE;
    }
    gabi::store<u32>(gabi::ea(i_this->mpPropellerMorf->getModel()) + 0xB8, gabi::ea(i_this)); /* setUserArea */
    for (u16 i = 0; i < J3DModelData_getJointNum(((J3DModel_l*)i_this->mpPropellerMorf->getModel())->mModelData); i++) {
        setJointCallBack(((J3DModel_l*)i_this->mpPropellerMorf->getModel())->mModelData, i, 0x023C9678 /* nodeCallBack_UP */);
    }

    /* body */
    bool peahat = i_this->mType == 0;
    arc = peahat ? STR(0x100345B2) : STR(0x100345B5);
    modelFlag = peahat ? 0x80000 : 0;
    dlFlag = peahat ? 0x37441422 : 0x11020203;
    modelData = (J3DModelData*)dComIfG_getObjectRes(arc, 0x22, SAFESTRING_VTBL);
    anm = (J3DAnmTransform*)dComIfG_getObjectRes(arc, 0x15, SAFESTRING_VTBL);
    i_this->mpBodyMorf = mDoExt_McaMorf::create(nullptr, modelData, nullptr, nullptr, anm, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1,
                                                1, nullptr, modelFlag, dlFlag);
    if (i_this->mpBodyMorf == nullptr || i_this->mpBodyMorf->getModel() == nullptr) {
        return FALSE;
    }
    gabi::store<u32>(gabi::ea(i_this->mpBodyMorf->getModel()) + 0xB8, gabi::ea(i_this));
    for (u16 i = 0; i < J3DModelData_getJointNum(((J3DModel_l*)i_this->mpBodyMorf->getModel())->mModelData); i++) {
        setJointCallBack(((J3DModel_l*)i_this->mpBodyMorf->getModel())->mModelData, i, 0x023C97BC /* nodeCallBack_DW */);
    }

    if (!mDoExt_invisibleModel_create(i_this->mPropellerInvisibleModel, i_this->mpPropellerMorf->getModel())) {
        return FALSE;
    }
    if (!mDoExt_invisibleModel_create(i_this->mBodyInvisibleModel, i_this->mpBodyMorf->getModel())) {
        return FALSE;
    }
    J3DModel* model = i_this->mpBodyMorf->getModel();
    u32 jntHit = JntHit_create(model, i_this->mType == 0 ? 0x101CE68C : 0x101CE6A4, 2);
    i_this->mpJntHit = jntHit;
    if (jntHit == 0) {
        return FALSE;
    }
    a_this->jntHit = jntHit;
    return TRUE;
}
VERIFY(0x023CEF6C, useHeapInit);

/* 0200E814 cDT_NamePTbl::GetIndex(name, 0) on play+0x50AC (dComIfGp_CharTbl) */
static inline s32 dComIfGp_CharTbl_GetIndex(const char* name, s32 p) {
    return gabi::call<s32>(0x0200E814, gabi::at<u8>(dComIfGp_ea() + PLAY_NAMETBL), name, p);
}
static inline s16 ph_id_angle(ph_class* i_this) { return (s16)((fopAcM_GetID(i_this) << 13) & 0x1FE000); }
static inline void copy_bits(cXyz* dst, const cXyz* src) {
    u32 x = gabi::load<u32>(gabi::ea(src)), y = gabi::load<u32>(gabi::ea(src) + 4), z = gabi::load<u32>(gabi::ea(src) + 8);
    gabi::store<u32>(gabi::ea(dst), x);
    gabi::store<u32>(gabi::ea(dst) + 4, y);
    gabi::store<u32>(gabi::ea(dst) + 8, z);
}

/* 023CF600 */
static cPhs_State daPH_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x023CF600, cPhs_State, a_this);
    ph_class* i_this = (ph_class*)a_this;
    /* HD: the archive is loaded before fopAcM_ct */
    u8 type = fopAcM_GetParam(a_this) & 0xFF;
    if (type == 0xFF) type = 0;
    i_this->mType = type;
    u32 heapSize = 0x2740;
    cPhs_State ret;
    if (REG_S(8, 9) != 0) {
        i_this->mType = 1;
        ret = dComIfG_resLoad(&i_this->mPhs, STR(0x100345E7) /* "Sh" */);
        heapSize = 0x3E40;
    } else if (type == 0) {
        ret = dComIfG_resLoad(&i_this->mPhs, STR(0x100345E4) /* "Ph" */);
    } else {
        ret = dComIfG_resLoad(&i_this->mPhs, STR(0x100345E7));
        heapSize = 0x3E40;
    }
    if (ret != cPhs_COMPLEATE_e) {
        return ret;
    }

    /* fopAcM_ct(a_this, ph_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) ph_class_ct(i_this);
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    if (!fopAcM_entrySolidHeap(a_this, 0x023CEF6C /* useHeapInit */, heapSize)) {
        return cPhs_ERROR_e;
    }

    u32 prm = fopAcM_GetParam(a_this);
    u8 range2 = prm >> 16;
    u8 range1 = prm >> 8;
    if (i_this->mType == 0) {
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
    if (range1 != 0xFF) i_this->m0390 = (f32)range1 * 100.0f;
    if (range2 != 0xFF) i_this->m0394 = (f32)range2 * 100.0f;
    gabi::store<u32>(gabi::ea(a_this) + 0x39C, 4); /* attention_info.flags = LOCKON_BATTLE */
    i_this->m038C = i_this->m0390 + 800.0f;
    u8 type2 = i_this->mType;
    if (type2 == 0) {
        a_this->itemTableIdx = dComIfGp_CharTbl_GetIndex(STR(0x100345DC) /* "p_hat" */, 0);
        gabi::store<u8>(gabi::ea(a_this) + 0x38A, 3); /* attention_info.distances[2] */
        a_this->stealItemLeft = 1;
        i_this->m0344 = 1;
    } else {
        a_this->itemTableIdx = dComIfGp_CharTbl_GetIndex(STR(0x100345D4) /* "sea_hat" */, 0);
        gabi::store<u8>(gabi::ea(a_this) + 0x38A, 0x2A);
        a_this->gbaName = 0x1F;
    }
    a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpBodyMorf->getModel()));
    i_this->mAcch.Set(&a_this->current.pos, &a_this->old.pos, a_this, 1, &i_this->mAcchCir, &a_this->speed);
    i_this->mStts.Init(0x50, 4, a_this);
    a_this->max_health = 1;
    a_this->health = 1;
    copy_bits(&i_this->m02FC, &a_this->scale);
    i_this->mBodySph.Set(gabi::at<dCcD_SrcSph>(0x101CE6D4));
    i_this->mBodySph.SetStts(&i_this->mStts);
    i_this->mBodySph.OffCoSPrmBit(1);
    i_this->mAtCyl.Set(gabi::at<dCcD_SrcCyl>(0x101CE788));
    i_this->mAtCyl.SetStts(&i_this->mStts);
    i_this->mTgCyl.Set(gabi::at<dCcD_SrcCyl>(0x101CE7CC));
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
        u32 jntTbl = i_this->mType == 1 ? 0x101CE770 : 0x101CE764;
        i_this->mBodyEnemyFire.mFlameJntIdxs[i] = gabi::load<s8>(jntTbl + i);
        f32 s = gabi::load<f32>(0x101CE714 + 4 * i);
        i_this->mBodyEnemyFire.mParticleScale[i] = s;
        i_this->mBodyEnemyFire.mParticleScale[i] = s + i_this->m039C;
    }
    i_this->mPropellerEnemyFire.mpMcaMorf = i_this->mpPropellerMorf;
    i_this->mPropellerEnemyFire.mpActor = a_this;
    for (int i = 0; i < 10; i++) {
        i_this->mPropellerEnemyFire.mFlameJntIdxs[i] = gabi::load<s8>(0x101CE77C + i);
        f32 s = gabi::load<f32>(0x101CE73C + 4 * i);
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

    if (i_this->mType == 1) {
        i_this->mAtCyl.mGObjAt.mSpl = 9;
        fopAcM_setCullSizeBox(a_this, -200.0f, -200.0f, -200.0f, 200.0f, 200.0f, 200.0f);
        f32 far = REG_F(8, 8) / gabi::load<f32>(0x1048D04C) + 10000.0f;
        f32 s = i_this->m03A0;
        i_this->m02FC.x = s;
        i_this->m02FC.y = s;
        i_this->m02FC.z = s;
        i_this->m0380 = 100.0f;
        i_this->m033F = 0;
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
            i_this->m033F = 3;
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
VERIFY(0x023CF600, daPH_Create);

/* 025192A8 cc_at_check(fopAc_ac_c*, CcAtInfo*) (SHARED-CANDIDATE, as in d_a_pt_action.cpp) */
struct CcAtInfo_l {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
static inline void cc_at_check(fopAc_ac_c* a, CcAtInfo_l* info) { gabi::call(0x025192A8, a, info); }
/* 025E1AA4 mDoAud_monsSeStart(id, pos, procId, param, reverb) */
static inline void mDoAud_monsSeStart(u32 id, cXyz* pos, u32 pid, u32 param, s32 reverb) {
    gabi::call(0x025E1AA4, id, pos, pid, param, reverb);
}
/* the player's cut type (daPy_lk_c, u8 at +0x3AC) */
static inline u8 daPy_getCutType(fopAc_ac_c* player) { return gabi::load<u8>(gabi::ea(player) + 0x3AC); }
static inline be<u8>& l_HD_101EACB7() { return *gabi::at<be<u8>>(0x101EACB7); }

static inline void ph_cc_at_check(ph_class* i_this, CcAtInfo_l* atInfo) {
    atInfo->mpObj = gabi::ea(i_this->mBodySph.GetTgHitObj());
    cc_at_check(i_this, atInfo);
}
static inline void ph_set_hitpos(ph_class* i_this, cXyz* pos) {
    f32 x = i_this->mBodySph.mGObjTg.mHitPos.x;
    f32 z = i_this->mBodySph.mGObjTg.mHitPos.z;
    f32 y = i_this->mBodySph.mGObjTg.mHitPos.y;
    pos->z = z;
    pos->y = y;
    pos->x = x;
}
static inline void ph_hit_particle(u16 id, cXyz* pos, fopAc_ac_c* player, cXyz* scale) {
    dPa_control_set(dComIfGp_getParticle(), 0, id, pos, &player->shape_angle, scale, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
}

/* 023CA5AC */
BOOL body_atari_check(ph_class* i_this) {
    WWHD_FUNC(0x023CA5AC, BOOL, i_this);
    gabi::Local<CcAtInfo_l> atInfo;
    for (u32 o = 0; o < sizeof(CcAtInfo_l); o += 4) gabi::store<u32>(gabi::ea(atInfo.get()) + o, 0);
    gabi::Local<cXyz> hitPos;
    gabi::Local<cXyz> scale1;
    gabi::Local<cXyz> scale2;
    atInfo->pParticlePos = 0;

    i_this->mStts.Move();
    if (i_this->m036C != 0 || !i_this->mBodySph.ChkTgHit()) {
        return FALSE;
    }
    i_this->m0340 = 0;
    void* hitObj = i_this->mBodySph.GetTgHitObj();
    if (hitObj == nullptr) {
        return FALSE;
    }
    i_this->m036C = 8;
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    u32 atType = gabi::load<u32>(gabi::ea(hitObj) + 0x10);
    bool noEffect = false;
    fopAc_ac_c* pl;

    switch (atType) {
    case 0x2: { /* sword */
        u8 cut = daPy_getCutType(player);
        if ((cut >= 5 && cut <= 10) || cut == 0xC || (cut >= 0xE && cut <= 0x10) || cut == 0x15 || cut == 0x17 ||
            (cut >= 0x19 && cut <= 0x1B) || cut == 0x1E || cut == 0x1F) {
            i_this->m0340 = 3;
        }
        i_this->m033F = 4;
        i_this->m0346 = 0x28;
        break;
    }
    case 0x20: /* bomb */
        i_this->m0340 = 8;
        i_this->m0346 = 0x2B;
        i_this->m033F = 4;
        break;
    case 0x40: /* boomerang */
        if (i_this->m033F != 2 && i_this->m02FC.x != 0.0f && i_this->m037C == 0.0f) {
            i_this->m0340 = 4;
            gabi::call(0x025E1FD8);
            l_HD_101EACB7() = 2;
            i_this->m0346 = 0x14;
            i_this->m033F = 2;
            return TRUE;
        }
        i_this->m0340 = 5;
        i_this->m033F = 4;
        i_this->m0346 = 0x28;
        break;
    case 0x200:
    case 0x40000: /* fire */
        i_this->mBodyEnemyFire.mFireDuration = 100;
        if (i_this->m02FC.x != 0.0f) {
            i_this->mPropellerEnemyFire.mFireDuration = 100;
        }
        i_this->m033F = 4;
        i_this->m0340 = 1;
        i_this->m0346 = 0x2B;
        break;
    case 0x4000:
        i_this->m0340 = 1;
        i_this->m0346 = 0x2B;
        i_this->m033F = 4;
        break;
    case 0x8000: /* grappling hook */
        if (i_this->mType != 0) {
            i_this->m0340 = 1;
            i_this->m0346 = 0x2B;
            i_this->m033F = 4;
            break;
        }
        i_this->m0340 = 9;
        if (i_this->m02FC.x != 0.0f && i_this->m037C == 0.0f) {
            l_HD_101EACB7() = 2;
            i_this->m033F = 2;
            i_this->m0346 = 0x14;
        }
        return FALSE;
    case 0x10000: /* skull hammer? */
        i_this->m033F = 4;
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
        enemy_fire_remove(&i_this->mBodyEnemyFire);
        enemy_fire_remove(&i_this->mPropellerEnemyFire);
        f32 s = i_this->m039C + 1.0f;
        i_this->m0340 = 1;
        i_this->mEnemyIce.mFreezeDuration = 200;
        i_this->m0346 = 0x2B;
        i_this->m0342 = 1;
        i_this->mEnemyIce.mParticleScale = s;
        i_this->m033F = 4;
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
        if (type == 1) i_this->mEnemyIce.mParticleScale = 4.0f;
        i_this->mEnemyIce.mYOffset = 0.0f;
        if (type == 1) i_this->mEnemyIce.mYOffset = 100.0f;
        gabi::store<u32>(gabi::ea(i_this) + 0x39C, 0); /* attention_info.flags */
        if (gabi::ea(&i_this->eyePos) != 0) {
            u32 se = i_this->mType == 0 ? 0x485E : 0x48EF;
            s8 roomNo = fopAcM_GetRoomNo(i_this);
            u32 pid = fopAcM_GetID(i_this);
            mDoAud_monsSeStart(se, &i_this->eyePos, pid, 0, dComIfGp_getReverb(roomNo));
        }
        i_this->m0340 = 1;
        i_this->m0346 = 0x2B;
        i_this->m033F = 4;
        break;
    }
    case 0x200000: /* hammer: knocked down */
        i_this->m0340 = 2;
        i_this->m033F = 5;
        i_this->m0346 = 0x32;
        i_this->mAtCyl.OffAtSPrmBit(1);
        return FALSE;
    case 0x8000000: /* hookshot? */
        if (i_this->mType != 0) {
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
                ph_cc_at_check(i_this, atInfo);
                i_this->m0343 = i_this->m0343 + 1;
                s8 n = (s8)i_this->m0344;
                i_this->health = health;
                if (n > 0) i_this->m0344 = n - 1;
            }
            dPa_control_set(dComIfGp_getParticle(), 0, 0x27B, gabi::at<cXyz>(gabi::ea(i_this) + 0x390), nullptr, nullptr, 0xFF,
                            nullptr, -1, nullptr, nullptr, nullptr);
            i_this->m033F = 1;
            i_this->m0346 = 10;
            i_this->mAtCyl.OffAtSPrmBit(1);
            return FALSE;
        }
        i_this->stealItemLeft = 0;
        i_this->m033F = 4;
        i_this->m0346 = 0x28;
        pl = dComIfGp_getPlayer(0);
        ph_set_hitpos(i_this, hitPos);
        ph_cc_at_check(i_this, atInfo);
        goto effect;
    default:
        i_this->m033F = 4;
        i_this->m0346 = 0x28;
        break;
    }
    if (noEffect) {
        return FALSE;
    }

    pl = dComIfGp_getPlayer(0);
    ph_set_hitpos(i_this, hitPos);
    ph_cc_at_check(i_this, atInfo);
effect:
    {
        u8 r = i_this->m0340;
        if (r != 3 && r != 6 && r != 7 && i_this->health > 0) {
            ph_hit_particle(0xD, hitPos, pl, nullptr);
            return TRUE;
        }
    }
    if (i_this->mType != 1) {
        scale1->x = 1.0f;
        scale2->z = 2.0f;
        scale1->y = 1.0f;
        scale2->x = 2.0f;
        scale2->y = 2.0f;
        scale1->z = 1.0f;
    } else {
        f32 s = REG_F(8, 9) + 3.25f;
        scale1->x = 1.25f;
        scale1->y = 1.25f;
        scale1->z = 1.25f;
        scale2->x = s;
        scale2->y = s;
        scale2->z = s;
    }
    ph_hit_particle(0x10, hitPos, pl, scale1);
    ph_hit_particle(0xF, hitPos, pl, scale2);
    return TRUE;
}
VERIFY(0x023CA5AC, body_atari_check);

/* 024EECAC dBgS::GetMtrlSndId(const cBgS_PolyInfo&) */
static inline u32 dBgS_GetMtrlSndId(dBgS* bgs, void* poly) { return gabi::call<u32>(0x024EECAC, bgs, poly); }
/* the hit sound pair of the propeller blade: fopAcM_seStart + fopAcM_monsSeStart (inline null checks) */
static inline void ph_blade_se(ph_class* i_this, bool checkThis) {
    if ((checkThis && i_this == nullptr) || gabi::ea(&i_this->eyePos) == 0) return;
    mDoAud_seStart(0x5858, &i_this->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
    s8 roomNo = fopAcM_GetRoomNo(i_this);
    u32 pid = fopAcM_GetID(i_this);
    mDoAud_monsSeStart(0x485C, &i_this->eyePos, pid, 0, dComIfGp_getReverb(roomNo));
}

/* 023CFDF0: blown by the Deku Leaf */
void ph_wind_move(ph_class* i_this) {
    WWHD_FUNC(0x023CFDF0, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
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
        if (v < 0.0f) v = 3.0f;
        i_this->speed.y = v;
        i_this->m0350 = 0;
        i_this->speedF = 30.0f;
        i_this->gravity = 0.0f;
        for (int i = 0; i < 7; i++) gabi::store<s16>(gabi::ea(i_this) + 0x472 + 2 * i, 0);
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
        cLib_addCalcAngleS2(&i_this->shape_angle.z, (s16)gabi::ftoi(cM_ssin(i_this->m035E) * 20000.0f), 1, 0x500);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, (s16)gabi::ftoi(cM_ssin(i_this->m035E) * 20000.0f), 1, 0x500);
        i_this->shape_angle.y += i_this->m0362;
        cLib_addCalc0(&i_this->speedF, 0.3f, 1.0f);
        cLib_addCalc0(&i_this->speed.y, 0.3f, 1.0f);
        if (i_this->speedF > 0.2f) break;
        i_this->speedF = 0.0f;
        i_this->m0360 = 0;
        i_this->m0346++;
        /* fallthrough */
    case 0x34:
        if (water) goto in_water;
        i_this->gravity = -0.3f;
        i_this->shape_angle.y += i_this->m0362;
        cLib_addCalcAngleS2(&i_this->m0362, 0, 1, 0x100);
        fly_angle_set(i_this, 0);
        if (!i_this->mAcch.ChkGroundHit()) break;
        {
            u32 snd = dBgS_GetMtrlSndId(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(i_this) + 0x5E8));
            if (gabi::ea(&i_this->eyePos) != 0) {
                mDoAud_seStart(0x5859, &i_this->eyePos, snd, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
            }
        }
        i_this->gravity = -3.0f;
        anm_init(i_this, 0x1F, 1.0f, 0, 1.0f, -1, 0);
        anm_init(i_this, 0x1A, 1.0f, 0, 1.0f, -1, 1);
        i_this->m0346++;
        break;
    case 0x35:
        if (water) goto in_water;
        cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 1, 0x500);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, i_this->current.angle.x, 1, 0x500);
        i_this->shape_angle.y += i_this->m0360;
        if (!i_this->mpBodyMorf->isStop()) break;
        if (i_this->m02FC.x == 0.0f) {
            i_this->speedF = 0.0f;
            i_this->m033F = 2;
            i_this->m0346 = 0x18;
            return;
        }
        i_this->m0366 = (s16)gabi::ftoi(cM_rndF(50.0f) + 50.0f);
        i_this->shape_angle.x = 0;
        i_this->m0346++;
        i_this->shape_angle.z = 0;
        break;
    case 0x36: {
        if (water) goto in_water;
        cLib_addCalc0(&i_this->speedF, 0.3f, 1.0f);
        s16 t = i_this->m0366;
        if (i_this->speedF > 0.1f) {
            i_this->shape_angle.y += i_this->m0360;
        }
        if (t != 0) break;
        i_this->speedF = 0.0f;
        if (i_this->m02FC.x == 0.0f) {
            i_this->m033F = 2;
            i_this->m0346 = 0x18;
        } else {
            i_this->m033F = 3;
            i_this->m0346 = 0x1E;
        }
        break;
    }
    default:
        break;
    }

    if (!body_atari_check(i_this)) return;
    i_this->shape_angle.x = 0;
    i_this->shape_angle.z = 0;
    if (i_this->m0340 != 2) return;
    if (i_this->mAcch.ChkGroundHit()) {
        i_this->m0360 = -1000;
        if (cM_rnd() < 0.5f) i_this->m0360 = 1000;
        i_this->speedF = 20.0f;
        if (i_this->m0374 != 0x1E) {
            anm_init(i_this, 0x1E, 1.0f, 0, 1.0f, -1, 0);
        }
        ph_blade_se(i_this, false);
        i_this->m0346 = 0x36;
    } else {
        i_this->m033F = 5;
        i_this->m0346 = 0x32;
    }
    return;

in_water:
    i_this->m033F = 6;
    i_this->m0346 = water == 2 ? 0x3C : 0x46;
}
VERIFY(0x023CFDF0, ph_wind_move);

/* water_move start: drop into the water, blades stopped */
static inline void ph_water_start(ph_class* i_this) {
    i_this->mBodySph.OnCoSPrmBit(1);
    i_this->shape_angle.x = 0;
    i_this->shape_angle.z = 0;
    i_this->mBodySph.mTgType = 0xFF3DFEFF;
    i_this->current.angle.x = 0;
    i_this->current.angle.z = 0;
    for (int i = 0; i < 7; i++) gabi::store<s16>(gabi::ea(i_this) + 0x472 + 2 * i, 0);
    i_this->mAtCyl.OffAtSPrmBit(1);
}
static inline void ph_water_anm(ph_class* i_this) {
    f32 r = i_this->m02FC.x == 0.0f ? 200.0f : 100.0f;
    i_this->m0366 = (s16)gabi::ftoi(cM_rndF(r) + r);
    anm_init(i_this, 0x1F, 1.0f, 0, 1.0f, -1, 0);
    anm_init(i_this, 0x1A, 1.0f, 0, 1.0f, -1, 1);
}

/* 023D051C: floating in the water */
void ph_water_move(ph_class* i_this) {
    WWHD_FUNC(0x023D051C, void, i_this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    switch (i_this->m0346) {
    case 0x3C:
        ph_water_start(i_this);
        if (i_this->mType == 1) {
            i_this->m0366 = 150;
            i_this->mAtCyl.OnAtSPrmBit(1);
            i_this->mAtCyl.mObjAt.mRPrm = 1;
            anm_init(i_this, 0x17, 1.0f, 2, 1.0f, -1, 1);
        } else {
            ph_water_anm(i_this);
        }
        i_this->m0368 = 20;
        i_this->current.angle.y = cM_atan2s(i_this->current.pos.x + i_this->m02E4.x - player->current.pos.x,
                                            i_this->current.pos.z + i_this->m02E4.z - player->current.pos.z) + 0x8000;
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
        if (vol > 100) vol = 100;
        if (gabi::ea(&i_this->eyePos) != 0) {
            mDoAud_seStart(0x5128, &i_this->eyePos, vol, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
        }
        if (ph_hani_check(i_this, i_this->m038C, 25400.0f, 1)) {
            bool far = false;
            if (i_this->m0368 == 0) {
                f32 dz = i_this->m032C.z - i_this->current.pos.z;
                f32 dx = i_this->m032C.x - i_this->current.pos.x;
                far = std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) > i_this->m038C;
            }
            if (!far && i_this->m0366 != 0) break;
        }
        i_this->m033F = 3;
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
            i_this->m033F = 3;
            i_this->m0346 = i_this->mType == 1 ? 0x21 : 0x1E;
        }
        break;
    default:
        break;
    }
    body_atari_check(i_this);
    sea_water_check(i_this, 2);
}
VERIFY(0x023D051C, ph_water_move);

/* ======================================================================================
 * daPH_Execute (023CC070) and the moves GHS inlined into it: ph_fly_move, ph_fly_sea_move,
 * ph_hane_move, ph_bunri_move, ph_fujyou_move, ph_damage_dead_move.
 * ====================================================================================== */

/* 025DA088 fopAcM_setGbaName(actor, itemNo, gbaName0, gbaName1) */
static inline void fopAcM_setGbaName(fopAc_ac_c* a, u8 itemNo, u8 n0, u8 n1) { gabi::call(0x025DA088, a, itemNo, n0, n1); }
static inline u32 dComIfGp_getPlayerStatus0() { return gabi::load<u32>(dComIfGp_ea() + PLAY_PLAYER_STATUS0); }
static inline void ph_zero_wobble(ph_class* i_this) {
    for (int i = 0; i < 7; i++) gabi::store<s16>(gabi::ea(i_this) + 0x472 + 2 * i, 0);
}
/* fopAcM_monsSeStart (inline: checks eyePos, then the process id of a possibly NULL actor) */
static inline void ph_mons_se(ph_class* i_this, u32 id) {
    if (gabi::ea(&i_this->eyePos) == 0) return;
    u32 pid = fopAcM_GetID(i_this);
    s8 roomNo = fopAcM_GetRoomNo(i_this);
    mDoAud_monsSeStart(id, &i_this->eyePos, pid, 0, dComIfGp_getReverb(roomNo));
}
/* fopAcM_seStart (inline, HD null checks) */
static inline void ph_se(ph_class* i_this, u32 id, u32 param, bool checkThis) {
    if ((checkThis && i_this == nullptr) || gabi::ea(&i_this->eyePos) == 0) return;
    mDoAud_seStart(id, &i_this->eyePos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
}
/* the attack arm on (At bit + hit flag) */
static inline void ph_at_on(ph_class* i_this) {
    i_this->mAtCyl.OnAtSPrmBit(1);
    i_this->mAtCyl.mObjAt.mRPrm = 1;
}

enum { PH_CHK_ALL, PH_CHK_WALL, PH_CHK_NONE };

/* ph_fly_move (Peahat, m033F == 0) */
static inline int ph_fly_move(ph_class* i_this) {
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
    if (sea_water_check(i_this, 1) == 0) return PH_CHK_ALL;
    if (!(i_this->current.pos.y + i_this->m02E4.y < i_this->m05BC.y + 200.0f)) return PH_CHK_ALL;
    i_this->current.angle.x = 0;
    return body_atari_check(i_this) ? PH_CHK_NONE : PH_CHK_WALL;
}

/* ph_fly_sea_move (Seahat, m033F == 0) */
static inline int ph_fly_sea_move(ph_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    fopAc_ac_c* ship = dComIfGp_getPlayer(2);
    if (REG_S(8, 3) != 0) return PH_CHK_NONE;

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
        if (dComIfGp_getPlayerStatus0() & 0x10000) {
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
        goto attack_range;
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
        if (!(dComIfGp_getPlayerStatus0() & 0x10000)) {
            i_this->m0372 = 0x3C;
            i_this->m0346 = 0;
            if (fopAcM_searchActorDistance(i_this, ship) < REG_F(12, 7) + 200.0f) {
                i_this->m0346 = 7;
                attackCheck = false;
                break;
            }
            goto attack_range;
        }
        if (i_this->m0364 == 0) {
            i_this->m0372 = 0x3C;
            i_this->m0346 = 0;
            attackCheck = false;
            break;
        }
        cLib_addCalc2(&i_this->speedF, 98.0f, 1.0f, 10.0f);
        if (i_this->m0366 != 0) goto attack_range;
        i_this->m0366 = 15;
        i_this->m0341 = 0;
        goto attack_range;
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
        if (i_this->m0364 != 0) goto attack_range;
        i_this->m0346++;
        goto attack_range;
    case 6: {
        cLib_addCalc2(&i_this->speedF, 150.0f, 0.5f, 1.0f);
        f32 dx = i_this->m032C.x - i_this->current.pos.x;
        f32 dz = i_this->m032C.z - i_this->current.pos.z;
        i_this->m0352 = cM_atan2s(dx, dz);
        if (!(std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < 50.0f)) goto attack_range;
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
        i_this->m0352 = cM_atan2s(i_this->m032C.x - ship->current.pos.x, i_this->m032C.z - ship->current.pos.z);
        i_this->m0346++;
        /* fallthrough */
    case 8:
        cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 1, 0x200);
        cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x200);
        cLib_addCalc0(&i_this->speedF, 1.0f, REG_F(12, 6) + 5.0f);
        if (!(i_this->speedF < 1.0f)) goto attack_range;
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
    attack_range:
        attackCheck = (u32)(i_this->m0346 - 4) < 3;
        break;
    }

    if (attackCheck && (dComIfGp_getPlayerStatus0() & 0x10000) && ph_hani_check(i_this, i_this->m0390, i_this->m0394, 1)) {
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
        cLib_addCalcAngleS2(&i_this->shape_angle.x, (s16)gabi::ftoi(REG_F(12, 11) + 4096.0f), 1, 0x800);
        targetY = player->current.pos.y;
    }
    cLib_addCalc2(&i_this->current.pos.y, targetY, 1.0f, REG_F(12, 10) + 50.0f);
    return PH_CHK_ALL;
}

/* ph_hane_move (m033F == 1): knocked back, spinning */
static inline void ph_hane_move(ph_class* i_this) {
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
            i_this->m033F = 0;
            if (i_this->mType == 0) {
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
    if (i_this->mType == 1) {
        sea_water_check(i_this, 1);
    }
}

/* ph_bunri_move (m033F == 2): the propeller has come off, the body hops on the ground */
static inline void ph_bunri_move(ph_class* i_this) {
    dComIfGp_get(); /* HD: unused */
    s16 mode = i_this->m0346;
    switch (mode) {
    case 0x14:
        i_this->mAtCyl.OffAtSPrmBit(1);
        i_this->m037C = 1.0f;
        i_this->mBodySph.OnCoSPrmBit(1);
        i_this->mBodySph.mTgType = 0xFF3DFEFF;
        gabi::store<u32>(gabi::ea(&i_this->m02F0.x), gabi::load<u32>(gabi::ea(&i_this->m02E4.x)));
        gabi::store<u32>(gabi::ea(&i_this->m02F0.z), gabi::load<u32>(gabi::ea(&i_this->m02E4.z)));
        i_this->m02F0.y = 0.0f;
        i_this->mTgCyl.OffTgSPrmBit(1);
        if (i_this->mType == 0) {
            i_this->m0350 = 900;
            if (i_this != nullptr && gabi::ea(&i_this->eyePos) != 0) {
                mDoAud_seStart(0x585A, &i_this->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
                ph_mons_se(i_this, 0x485C);
            }
        } else {
            i_this->m0350 = 150;
            if (i_this != nullptr && gabi::ea(&i_this->eyePos) != 0) {
                mDoAud_seStart(0x69FA, &i_this->eyePos, 0, dComIfGp_getReverb(fopAcM_GetRoomNo(i_this)));
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
        if (!i_this->mpBodyMorf->isStop()) goto end;
        copy_bits(&i_this->m0308, &i_this->current.pos);
        i_this->speed.y = 10.0f;
        mode = i_this->m0346 + 1;
        i_this->m0346 = mode;
        i_this->gravity = i_this->mType == 1 ? -10.0f : -3.0f;
        /* fallthrough */
    case 0x16:
        if (!i_this->mAcch.ChkGroundHit()) goto end;
        anm_init(i_this, 0x13, 1.0f, 0, 1.0f, -1, 1);
        {
            u32 snd = dBgS_GetMtrlSndId(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(i_this) + 0x5E8));
            ph_se(i_this, 0x5859, snd, true);
        }
        mode = i_this->m0346 + 1;
        i_this->m0346 = mode;
        goto count;
    case 0x17:
        if (!i_this->mpBodyMorf->isStop()) goto end;
        i_this->m0346 = mode + 1;
        /* fallthrough */
    case 0x18:
        if (i_this->m0350 == 0) {
            i_this->m033F = 3;
            i_this->m0346 = 0x1E;
            goto decrement;
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
            goto end;
        }
        i_this->speedF = 5.0f;
        i_this->gravity = -3.0f;
        i_this->speed.y = 20.0f;
        ph_mons_se(i_this, 0x485F);
        mode = i_this->m0346 + 1;
        i_this->m0346 = mode;
        goto count;
    case 0x1A:
        if (!i_this->mAcch.ChkGroundHit()) goto end;
        {
            u32 snd = dBgS_GetMtrlSndId(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(i_this) + 0x5E8));
            ph_se(i_this, 0x585B, snd, true);
        }
        i_this->speedF = 0.0f;
        i_this->m0346 = 0x18;
        goto decrement;
    default:
        goto end;
    }
end:
count:
    if (mode >= 0x18) {
    decrement:
        if (i_this->m0350 > 0) i_this->m0350--;
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
            i_this->m033F = 2;
        }
    }
}

/* ph_fujyou_move growth step: returns true when the body animation has ended */
static inline bool ph_fujyou_grow(ph_class* i_this) {
    cLib_addCalc2(&i_this->m02FC.x, i_this->m03A0 + 1.0f, 1.0f, i_this->m03A0 + 0.6f);
    f32 lim = i_this->m03A0 + 0.8f;
    f32 s = i_this->m02FC.x;
    mDoExt_McaMorf* morf = i_this->mpBodyMorf;
    i_this->m02FC.z = s;
    i_this->m02FC.y = s;
    if (s > lim) {
        f32 t = i_this->m03A0 + 1.0f;
        i_this->m02FC.x = t;
        i_this->m02FC.y = t;
        i_this->m02FC.z = t;
    }
    if (!morf->isStop()) return false;
    i_this->m0384 = 0.0f;
    i_this->m034A = 0x1000;
    i_this->m034C = 0;
    ph_zero_wobble(i_this);
    i_this->m0346++;
    i_this->gravity = -3.0f;
    return true;
}
/* ph_fujyou_move rising step (propeller turning) */
static inline void ph_fujyou_rise(ph_class* i_this, f32 target, f32 scale, f32 step) {
    i_this->m033A += (s16)gabi::ftoi(i_this->m0384 * 1000.0f);
    puropera_sound(i_this);
    cLib_addCalc2(&i_this->m0384, target, scale, step);
    i_this->m035E += (s16)gabi::ftoi(i_this->m0384 * 200.0f);
    cLib_addCalcAngleS2(&i_this->current.angle.z, (s16)gabi::ftoi(cM_ssin(i_this->m035E) * 4000.0f), 1, 0x1000);
    i_this->m0356 += 2000;
    i_this->m0358 += 0x4B0;
    i_this->speed.y = i_this->m0384;
    i_this->m035A += 0x4B0;
    i_this->shape_angle.z = i_this->current.angle.z;
    i_this->m02F0.y = cM_ssin(i_this->m0356) * 10.0f;
    i_this->m02F0.x = cM_scos(i_this->m0358) * 20.0f;
    i_this->m02F0.z = cM_scos(i_this->m035A) * 20.0f;
}

/* ph_fujyou_move (m033F == 3): the propeller regrows and the Peahat takes off again */
static inline void ph_fujyou_move(ph_class* i_this) {
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
            i_this->m033F = 0;
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
            i_this->m033F = 0;
            i_this->m0346 = 4;
        }
        break;
    case 0x24:
        i_this->m033A += (s16)gabi::ftoi(i_this->m0384 * 1000.0f);
        puropera_sound(i_this);
        i_this->speed.y = 70.0f;
        if (i_this->current.pos.y > i_this->m05BC.y + 500.0f) {
            i_this->m0346 = 0;
            i_this->m033F = 0;
        }
        break;
    }

    if (i_this->mType != 1) {
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
    if (i_this->mType != 1) {
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

static inline void ph_dead_se(ph_class* i_this) { ph_mons_se(i_this, i_this->mType == 0 ? 0x485E : 0x48EF); }

/* ph_damage_dead_move (m033F == 4). Returns true when the live Peahat goes on to the water check */
static inline bool ph_damage_dead_move(ph_class* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
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
        if (!i_this->mpBodyMorf->isStop()) break;
        if (i_this->health <= 0) goto dead;
        i_this->speedF = 0.0f;
        i_this->m0346 = 0x18;
        i_this->m033F = 2;
        break;
    case 0x2A:
        if (i_this->m0364 != 0) break;
        i_this->m033F = 2;
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
            if (i_this->m0340 == 7) i_this->m0364 = 0;
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
        if (i_this->m0364 == 0 || i_this->mpPropellerMorf->isStop()) goto dead;
        break;
    case 0x2D:
        if (i_this->m0364 == 1) {
            i_this->gravity = -5.0f;
            i_this->speed.y = 23.0f;
            if (i_this->m035E > 1) i_this->speed.y = 15.0f;
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
            if (n > 2) dead_item(i_this);
        }
        cLib_addCalc2(&i_this->scale.x, i_this->m0320.x, 0.5f, 0.5f);
        cLib_addCalc2(&i_this->scale.y, i_this->m0320.y, 0.5f, 0.5f);
        break;
    case 0x2E:
        cLib_addCalc0(&i_this->speedF, 0.5f, 0.5f);
        i_this->shape_angle.y += 0x500;
        if (i_this->m0364 == 0) goto dead;
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
        gabi::store<u32>(gabi::ea(i_this) + 0x39C, 0); /* attention_info.flags */
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
        if (!(i_this->scale.y < 0.1f)) break;
        i_this->scale.y = 0.1f;
        i_this->scale.x = 1.7f;
        i_this->m0364 = 0x14;
        i_this->m0346++;
        break;
    case 0x31:
        if (i_this->m0364 != 0) break;
        goto dead;
    default:
        break;
    }
    goto done;
dead:
    dead_item(i_this);
done:
    i_this->current.angle.x = 0;
    i_this->current.angle.z = 0;
    cLib_addCalcAngleS2(&i_this->shape_angle.x, 0, 1, 0x500);
    cLib_addCalcAngleS2(&i_this->shape_angle.z, i_this->current.angle.z, 1, 0x500);
    return i_this->health > 0;
}

static inline void ph_to_water(ph_class* i_this, int water) {
    i_this->m033F = 6;
    i_this->m0346 = water == 2 ? 0x3C : 0x46;
}

/* 023CC070 */
static BOOL daPH_Execute(ph_class* i_this) {
    WWHD_FUNC(0x023CC070, BOOL, i_this);
    fopAc_ac_c* a_this = i_this;
    if (i_this->mType == 0) {
        fopAcM_setGbaName(a_this, 0x2D, 8, 0x25);
    } else {
        fopAcM_setGbaName(a_this, 0x2D, 0x1F, 0x27);
    }

    if (enemy_ice(&i_this->mEnemyIce)) {
        J3DModel_setBaseTRMtx(i_this->mpBodyMorf->getModel(), mDoMtx_stack_c::get());
        i_this->mpBodyMorf->calc();
        if (i_this->m02FC.x != 0.0f && i_this->m037C == 0.0f) {
            f32 sxz = i_this->mEnemyIce.mScaleXZ;
            if (i_this->mType == 1) {
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
        be<s16>* t = gabi::at<be<s16>>(gabi::ea(i_this) + 0x480 + 2 * i);
        if (*t != 0) *t = *t - 1;
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
    case 0:
        if (i_this->mType == 0) {
            chk = ph_fly_move(i_this);
        } else {
            chk = ph_fly_sea_move(i_this);
        }
        if (chk != PH_CHK_NONE) {
            if (chk == PH_CHK_ALL && body_atari_check(i_this)) break;
            if (ph_wall_hit_check(i_this)) break;
            hajiki_check(i_this);
        }
        break;
    case 1:
        ph_hane_move(i_this);
        break;
    case 2: {
        ph_bunri_move(i_this);
        int water = sea_water_check(i_this, 1);
        if (water != 0) ph_to_water(i_this, water);
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

    gabi::Local<cXyz> local;
    gabi::Local<cXyz> world;
    bool propFlying = false;
    if (i_this->m037C == 0.0f) {
        copy_bits(&i_this->m0314, &i_this->m02C0);
    } else if (i_this->m02FC.x != 0.0f) {
        /* the propeller flies off */
        f32 height = 1000.0f;
        puropera_kaiten(i_this);
        cLib_addCalc2(&i_this->m037C, 10.0f, 0.5f, 1.0f);
        if (i_this->mType == 1) {
            cLib_addCalc2(&i_this->m037C, 30.0f, 1.0f, 5.0f);
            height = 2000.0f;
            i_this->m034A = 0x2000;
        }
        local->x = 0.0f;
        local->y = 0.0f;
        local->z = -i_this->m037C;
        MtxPosition(local, world);
        PSVECAdd(&i_this->m0314, world, &i_this->m0314);
        f32 y = i_this->m0314.y + i_this->m037C;
        i_this->m0314.y = y;
        if (y > a_this->current.pos.y + height) {
            i_this->m02FC.x = 0.0f;
            i_this->m02FC.y = 0.0f;
            i_this->m02FC.z = 0.0f;
            local->x = 0.0f;
            local->y = 0.0f;
            i_this->m037C = 0.0f;
            propFlying = true;
        }
    }
    if (!propFlying) {
        local->y = 0.0f;
        local->x = 0.0f;
    }
    local->z = a_this->speedF;
    MtxPosition(local, world);
    a_this->speed.x = world->x;
    a_this->speed.z = world->z;

    u8 st = i_this->m033F;
    f32 vy;
    if (st == 2 || st == 3 || st == 5 || st == 4) {
        vy = a_this->speed.y + a_this->gravity;
        if (vy < -20.0f) vy = -20.0f;
    } else {
        vy = world->y;
    }
    a_this->speed.y = vy;
    fopAcM_posMove(a_this, &i_this->mStts.m_cc_move);
    if (i_this->m033F == 1 && i_this->m0341 != 0) {
        f32 y = i_this->m05BC.y + 40.0f;
        if (a_this->current.pos.y < y) a_this->current.pos.y = y;
    }

    if (i_this->mType == 0) {
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

    if (i_this->m033F != 4) {
        if (i_this->mType == 0) {
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
            gabi::Local<cXyz> c;
            c->x = i_this->m02CC.x;
            c->z = i_this->m02CC.z;
            c->y = i_this->m02CC.y - (REG_F(8, 10) + 150.0f);
            i_this->mTgCyl.mCyl.SetC(c);
            i_this->mTgCyl.mCyl.SetH(REG_F(8, 12) + 500.0f);
            i_this->mTgCyl.mCyl.SetR(REG_F(8, 13) + 500.0f);
            dComIfG_Ccsp_Set(&i_this->mTgCyl);
            f32 d = REG_F(8, 10) + 150.0f;
            c->x = i_this->m02CC.x;
            c->z = i_this->m02CC.z;
            c->y = i_this->m02CC.y - d;
            i_this->mAtCyl.mCyl.SetC(c);
            i_this->mAtCyl.mCyl.SetH(REG_F(8, 14) + 350.0f);
            i_this->mAtCyl.mCyl.SetR(REG_F(8, 15) + 250.0f);
            dComIfG_Ccsp_Set(&i_this->mAtCyl);
            i_this->mBodySph.mSph.SetC(&i_this->m02CC);
            i_this->mBodySph.mSph.SetR(gabi::fmadds(i_this->m039C, REG_F(8, 17) + 40.0f, REG_F(8, 16) + 70.0f));
            dComIfG_Ccsp_Set(&i_this->mBodySph);
        }
    }

    cXyz* attnPos = gabi::at<cXyz>(gabi::ea(i_this) + 0x390); /* attention_info.position */
    attnPos->x = i_this->m02C0.x;
    f32 ay = i_this->m02C0.y;
    attnPos->y = ay;
    attnPos->z = i_this->m02C0.z;
    if (i_this->mType == 1) {
        attnPos->y = ay - 100.0f;
    }
    BG_check(i_this);
    return TRUE;
}
VERIFY(0x023CC070, daPH_Execute);
