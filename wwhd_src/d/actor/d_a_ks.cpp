/**
 * d_a_ks.cpp (WWHD)
 * Enemy - Morth
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ks.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * daKS_Execute (with the inlined action functions): d_a_ks_exec.cpp.
 */
#include "d/actor/d_a_ks.h"

#include <cmath>
#include "gc_api/d/d_com_inf_game.h"

const gc_tu_data gc_tu = {.safeString = KS_SAFESTRING_VTBL};
static s32 KS_ALL_COUNT;
static s32 KUTTUKU_ALL_COUNT;
static s32 HEAVY_IN;
static s32 GORON_COUNT;
static u8 ksPauseTimer;
static u8 item_tbl[4] = {10, 11, 12, 10};
static dCcD_SrcSph body_co_sph_src;
static s8 fire_j[10] = {0, -1, -1, -1, -1, -1, -1, -1, -1, -1};
static u32 pl_harituki_joint_dt[20] = {0x20,0x21,0x25,0x26,2,3,4,0x1E,0x29,0x1F,0x24,8,6,7,5,0xC,0xA,0xB,9,0xE};
static f32 fire_sc[10] = {1,1,1,1,1,1,1,1,1,1};



enum { fpcNm_GM_e = 0xCC, fpcNm_KS_e = 0xCD, fpcNm_TSUBO_e = 0x1C5 };
enum { DSNAP_TYPE_KS = 0xB3 };
enum { JA_SE_CM_KS_DIE = 0x587C };
enum {
    dRes_INDEX_KS_BCK_MABATAKI_e = 0xC,
    dRes_INDEX_KS_BDL_KS_BODY_e = 0xF,
    dRes_INDEX_KS_BDL_KS_EYE_e = 0x10,
    dRes_INDEX_KS_BRK_KS_BODY_e = 0x13,
    dRes_INDEX_KS_BRK_KS_EYE_e = 0x14,
    dRes_INDEX_KS_BTK_KS_EYE_e = 0x17,
};
enum {
    daDisItem_HEART_e = 0xA,
    daDisItem_MAGIC_e = 0xB,
    daDisItem_ARROW_e = 0xC,
    daDisItem_NONE13_e = 0xD,
};
/* cCcD_ObjAt::mType values */
enum : u32 {
    AT_TYPE_SWORD = 0x2,
    AT_TYPE_UNK8 = 0x8,
    AT_TYPE_FIRE = 0x200,
    AT_TYPE_SKULL_HAMMER = 0x10000,
    AT_TYPE_FIRE_ARROW = 0x40000,
    AT_TYPE_ICE_ARROW = 0x80000,
    AT_TYPE_LIGHT_ARROW = 0x100000,
    AT_TYPE_WIND = 0x200000,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 02041570 enemy_fire(enemyfire*), 02041C30 enemy_fire_remove(enemyfire*) (as in d_a_cc) */
static inline void enemy_fire(enemyfire_l* f) { ksMain_fire(f); }
static inline void enemy_fire_remove(enemyfire_l* f) { ksMain_removeFire(f); }
/* 025A9270 dPa_rippleEcallBack::end (remove) */
static inline void ripple_remove(void* r) { ksMain_rippleEnd(r); }
/* mDoExt_brkAnm / btkAnm frame (HD +4); entry with the anm's own frame (as in d_a_cc) */
static inline f32 anm_frame(void* anm) { return ksAnimation(anm)->mFrame; }
static inline void anm_setFrame(void* anm, f32 f) { ksAnimation(anm)->mFrame = f; }
// HD: mDoExt_brkAnm::remove / mDoExt_btkAnm::remove inline: clear the model data's tev-register
// (+0x48) and texture-SRT (+0x44) animation (as in d_a_cc)
static inline void brk_remove(J3DModel* model) { ksModelData(model)->mTevReg = 0; }
static inline void btk_remove(J3DModel* model) { ksModelData(model)->mTextureSRT = 0; }
/* camera_process_class* of the play object (play+0x5AF8); its view.mLookat.mEye at +0xDC */
static inline cXyz* ks_cameraEye() { return ksCameraEye(dComIfGp_getCamera(0)); }
/* dScnPly_ply_c::setPauseTimer(n): the pause timer byte 0x101EACB7 */
static inline void dScnPly_setPauseTimer(u8 n) { ksPauseTimer = n; }
/* 0200E814 cDT_NamePTbl::GetIndex(play+0x50AC, name, s32) */
static inline s32 dComIfGp_CharTbl_GetNameIndex(const char* name, s32 p) {
    return ksMain_characterIndex(dComIfGp_ea() + PLAY_NAMETBL, name, p);
}
/* 025E80D0 mDoExt_brkAnm::mDoExt_brkAnm (matcher: "mDoExt_brkAnm::init"), 025E8154 brkAnm::init,
 * 025E7C6C mDoExt_btkAnm::mDoExt_btkAnm (matcher: "init"), 025E7CE0 btkAnm::init (as in d_a_cc) */
// HD: constructors observe the previous outgoing stack argument at SP+8.
/* inlined into useHeapInit */
static void* new_brkAnm() {
    void* storage = operator_new(0x78);
    KsAnimationCallFrame frame;
    frame.mWords[2] = 0;
    return storage != nullptr ? ksMain_brkCtor(storage) : nullptr;
}
/* inlined into useHeapInit */
static void* new_btkAnm() {
    void* storage = operator_new(0x74);
    KsAnimationCallFrame frame;
    frame.mWords[2] = 1;
    return storage != nullptr ? ksMain_btkCtor(storage) : nullptr;
}
/* inlined into useHeapInit */
static BOOL ksBrkInit(void* animation,J3DModelData* model,void* key,s32 play,s32 mode,f32 rate,s16 start,s16 end,bool modify,s32 entry) {
    return ksMain_brkInit(animation,model,key,play,mode,start,end,modify,entry,rate);
}
/* inlined into useHeapInit */
static BOOL ksBtkInit(void* animation,J3DModelData* model,void* key,s32 play,s32 mode,f32 rate,s16 start,s16 end,bool modify,s32 entry) {
    return ksMain_btkInit(animation,model,key,play,mode,start,end,modify,entry,rate);
}
// HD: sead::SafeString comparison of the start stage name (play+0x5134) with a literal, inlined
// (as in d_a_bk_action): the virtual at vtable +0x14 is called twice on the literal's SafeString
// and once on the stage name's, then at most 0x40001 characters are compared
static inline void SafeString_vcall(SafeString* s) { ksAssureString(s); }
/* inlined into dead_eff_set, useHeapInit, daKS_Create */
static inline bool ksStageIs(const char* name) {
    SafeString a;
    a.mStringTop = gabi::guest_addr(name);
    a.__vtbl = KS_SAFESTRING_VTBL;
    u32 stage = ksStartStageAddress();
    SafeString b;
    b.__vtbl = KS_SAFESTRING_VTBL;
    b.mStringTop = stage;
    SafeString_vcall(&a);
    SafeString_vcall(&a);
    u32 pa = a.mStringTop;
    SafeString_vcall(&b);
    u32 pb = b.mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = ksStringByte(pa + i);
        u8 cb = ksStringByte(pb + i);
        if (ca != cb) return false;
        if (ca == 0) return true;
    }
    return false;
}
/* daPy_lk_c (the link player, play+0x5B34) */
static inline fopAc_ac_c* daPy_getPlayerLinkActorClass() { return dComIfGp_getLinkPlayer(); }

/* 021A6F64 */
void draw_SUB(ks_class* i_this) {

    fopAc_ac_c* actor = i_this;

    J3DModel* pBodyModel = i_this->mpBodyMorf->getModel();
    J3DModel* pEyeModel = i_this->mpEyeMorf->getModel();

    cXyz local_24;
    cXyz_mi(ks_cameraEye(), &local_24, &actor->current.pos);
    f32 x = local_24.x;
    f32 z = local_24.z;
    f32 y = local_24.y;

    s16 iVar3 = cM_atan2s(x, z);
    s16 iVar4 = (s16)-cM_atan2s(y, std_sqrtf(gabi::fmadds(x, x, z * z)));

    f32 fVar1 = 0.0f;
    if (i_this->m2D0) {
        fVar1 = i_this->m304;
    }

    mDoMtx_stack_c::transS(actor->current.pos.x, actor->current.pos.y + fVar1, actor->current.pos.z);
    mDoMtx_stack_c::YrotM(iVar3);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), iVar4);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)(actor->shape_angle.z + i_this->m2FA));

    i_this->m2FA += i_this->m2FE;

    mDoMtx_stack_c::scaleM(1.0f, 1.0f, 1.0f);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), (s16)(actor->shape_angle.z - i_this->m2FA));
    J3DModel_setBaseTRMtx(pBodyModel, mDoMtx_stack_c::get());

    mDoMtx_stack_c::transS(actor->current.pos.x, actor->current.pos.y + fVar1, actor->current.pos.z);
    mDoMtx_stack_c::YrotM(actor->shape_angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), actor->shape_angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), actor->shape_angle.z);
    mDoMtx_stack_c::scaleM(1.0f, 1.0f, 1.0f); /* local_18.setall(1.0f) */
    J3DModel_setBaseTRMtx(pEyeModel, mDoMtx_stack_c::get());

    enemy_fire(&i_this->mEnemyFire);
}

/* 021A71EC */
static BOOL daKS_Draw(ks_class* i_this) {

    fopAc_ac_c* actor = i_this;

    cXyz local_24;
    cXyz_mi(&actor->current.pos, &local_24, ks_cameraEye());
    if (std_sqrtf(PSVECSquareMag(&local_24)) < REG0_F(10) + 100.0f) {
        return TRUE;
    }

    J3DModel* pBodyModel = i_this->mpBodyMorf->getModel();
    J3DModel* pEyeModel = i_this->mpEyeMorf->getModel();

    dSnap_RegistFig(DSNAP_TYPE_KS, actor, 1.0f, 1.0f, 1.0f);

    if (i_this->m2C8 == 6) {
        return TRUE;
    }

    if (i_this->m2D0 == 0 && i_this->m2CD == 0) {
        draw_SUB(i_this);
    }

    i_this->m2CD = 0;

    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &actor->current.pos, &actor->tevStr);
    setLightTevColorType(dKy_getEnvlight(), pBodyModel, &actor->tevStr);

    mDoExt_brkAnm* brk = i_this->mpBodyBrkAnm;
    mDoExt_brkAnm_entry(brk, J3DModel_getModelData(pBodyModel), anm_frame(brk));
    anm_setFrame(i_this->mpBodyBrkAnm, i_this->m320);
    i_this->mpBodyMorf->updateDL();
    brk_remove(pBodyModel);

    setLightTevColorType(dKy_getEnvlight(), pEyeModel, &actor->tevStr);

    mDoExt_btkAnm* btk = i_this->mpEyeBtkAnm;
    mDoExt_btkAnm_entry(btk, J3DModel_getModelData(pEyeModel), anm_frame(btk));
    anm_setFrame(i_this->mpEyeBtkAnm, (f32)(s16)i_this->m302);

    brk = i_this->mpEyeBrkAnm;
    mDoExt_brkAnm_entry(brk, J3DModel_getModelData(pEyeModel), anm_frame(brk));
    anm_setFrame(i_this->mpEyeBrkAnm, i_this->m320);
    i_this->mpEyeMorf->updateDL();

    brk_remove(pEyeModel);
    btk_remove(pEyeModel);

    return TRUE;
}

/* 021A73A4 */
BOOL tyaku_check(ks_class* i_this) {

    fopAc_ac_c* actor = i_this;

    if (i_this->mAcch.ChkGroundHit()) {
        return TRUE;
    }

    if (i_this->mAcch.ChkWaterIn()) {
        actor->speed.y = 0.0f;
        actor->gravity = 0.0f;
        return TRUE;
    }

    return FALSE;
}

/* 021A73E4 */
BOOL ks_kuttuki_check(ks_class* i_this) {

    dComIfGp_get(); /* daPy_getPlayerLinkActorClass() (unused) */
    if (i_this->mSph.ChkAtHit() && !(i_this->mSph.mGObjAt.mRPrm & 1) /* ChkAtShieldHit */) {
        fopAc_ac_c* hit_actor = i_this->mSph.GetAtHitAc();
        // HD: KUTTUKU_ALL_COUNT >= 0 && < 20 as one unsigned compare
        if (hit_actor && hit_actor == daPy_getPlayerLinkActorClass() && (u32)(s32)KUTTUKU_ALL_COUNT < 20 && GORON_COUNT == 0) {
            i_this->mSph.mObjTg.mSPrm &= ~1u; /* OffTgSetBit */
            i_this->mSph.mObjCo.mSPrm &= ~1u; /* ClrCoSet */
            i_this->mSph.ClrTgHit();

            if (i_this->m2CF) {
                i_this->m2CF = 0;
                ripple_remove(i_this->m52C);
            }

            i_this->mAction = KS_ATTACHED;
            i_this->mMode = 40;

            return TRUE;
        }
    }
    return FALSE;
}

/* shock_damage_check inlined */
/* 021A74E4 */
BOOL body_atari_check(ks_class* i_this) {

    fopAc_ac_c* actor = i_this;
    dComIfGp_get(); /* daPy_getPlayerActorClass() (unused) */

    i_this->mStts.Move();

    BOOL tgHit = i_this->mSph.ChkTgHit();
    u32 play = dComIfGp_ea();
    if (tgHit) {
        fopAc_ac_c* player = ksPlayerAt(play);

        void* hitObj = i_this->mSph.GetTgHitObj();

        cXyz hitPos;
        cXyz* tgPos = i_this->mSph.GetTgHitPosP();
        hitPos.z = tgPos->z;
        hitPos.y = tgPos->y;
        hitPos.x = tgPos->x;

        if (!hitObj) {
            return FALSE;
        }

        actor->current.angle.y = fopAcM_searchPlayerAngleY(actor) + 0x8000;

        i_this->mAction = KS_DEAD;

        cXyz particleScale;
        switch (ksHitObject(hitObj)->mAtType) {
        case AT_TYPE_WIND: {
            // HD: blown off in the player's facing direction, with a random spread
            s16 spread = (s16)gabi::ftoi(cM_rndFX(4000.0f));
            actor->current.angle.y = player->shape_angle.y + spread;
            i_this->mAction = KS_WIND;
            i_this->mMode = 20;
            return FALSE;
        }
        case AT_TYPE_UNK8:
            actor->health = 0;
            i_this->mMode = 30;
            return FALSE;
        case AT_TYPE_SWORD: {
            if (i_this->mMode != 43 || i_this->m2CE) {
                dScnPly_setPauseTimer(2);
                actor->stealItemBitNo = 1;
            }
            f32 s = REG_F(8, 0) + 0.8f;
            particleScale.x = s;
            particleScale.z = s;
            particleScale.y = s;
            dComIfGp_particle_set(0xD, &hitPos, &player->shape_angle, &particleScale);
            break;
        }
        case AT_TYPE_SKULL_HAMMER: {
            u8 cut = ksPlayer(player)->mCutType; /* daPy_py_c::getCutType() */
            if (cut == 0x12 || cut == 0x13) {                  /* HAMMER_FRONTSWING, JUMPCUT_HAMMER */
                actor->speedF = 0.0f;
                actor->gravity = 0.0f;
                actor->speed.x = 0.0f;
                actor->speed.y = 0.0f;
                actor->speed.z = 0.0f;
                actor->health = 0;
                dScnPly_setPauseTimer(2);
                actor->stealItemBitNo = 1;
                i_this->mMode = 32;
                return TRUE;
            }
            break;
        }
        case AT_TYPE_FIRE:
        case AT_TYPE_FIRE_ARROW:
            i_this->mEnemyFire.mFireDuration = 100;
            break;
        case AT_TYPE_ICE_ARROW:
            i_this->mEnemyIce.mFreezeDuration = 200;
            i_this->m2D0 = 1;
            i_this->mEnemyIce.mParticleScale = 0.2f;
            i_this->mEnemyIce.mYOffset = 0.0f;
            enemy_fire_remove(&i_this->mEnemyFire);
            break;
        case AT_TYPE_LIGHT_ARROW:
            i_this->mEnemyIce.mLightShrinkTimer = 1;
            i_this->mEnemyIce.mParticleScale = 0.2f;
            i_this->mEnemyIce.mYOffset = 0.0f;
            i_this->m2D0 = 1;
            break;
        default: {
            f32 s = REG_F(8, 0) + 0.8f;
            particleScale.x = s;
            particleScale.z = s;
            particleScale.y = s;
            dComIfGp_particle_set(0xD, &hitPos, &player->shape_angle, &particleScale);
            break;
        }
        }

        actor->health = 0;
        i_this->mMode = 30;
        return TRUE;
    }

    /* shock_damage_check (inlined) */
    fopAc_ac_c* link = daPy_getPlayerLinkActorClass();
    if (ksPlayer(link)->mStateFlags & 0x20000) { /* checkHammerQuake() */
        const cXyz& swordTopPos = ksPlayer(link)->mSwordTopPos;
        f32 dz = (f32)swordTopPos.z - actor->current.pos.z; /* getSwordTopPos() */
        f32 dx = (f32)swordTopPos.x - actor->current.pos.x;
        f32 dy = (f32)swordTopPos.y - actor->current.pos.y;
        if (std_sqrtf(gabi::fmadds(dx, dx, dz * dz)) < 200.0f) {
            if (std_sqrtf(dy * dy) < 40.0f) {
                i_this->mAction = KS_DEAD;
                i_this->mMode = 32;
                return TRUE;
            }
        }
    }
    return FALSE;
}

/* 021A7A88 */
void speed_keisan(ks_class* i_this, s16 i_speed) {

    fopAc_ac_c* actor = i_this;

    cMtx_YrotS(calc_mtx(), i_speed);

    s16 sVar1 = (s16)gabi::ftoi(cM_ssin((u16)i_this->m2F0[0]) * i_this->m30C);

    cXyz local_14;
    local_14.z = 0.0f;
    local_14.x = (f32)sVar1;
    local_14.y = 0.0f;

    cXyz local_8;
    MtxPosition(&local_14, &local_8);

    actor->current.pos.x += local_8.x;
    actor->current.pos.z += local_8.z;
}

/* gm_birth_delet inlined */
/* 021A7B4C */
void dead_eff_set(ks_class* i_this, cXyz* i_pos) {

    fopAc_ac_c* actor = i_this;
    u8 drop_type;
    if (ksStageIs("GanonK") && actor->stealItemBitNo != 0) {
        if (dComIfGs_getLife() <= 8) { /* dComIfGs_getLife() */
            drop_type = daDisItem_HEART_e;
        } else if (cM_rndF(1.0f) < 0.5f) {
            KsSaveView* sv = ksSave();
            u8 arrows = (u8)sv->mArrows;
            if (arrows == 0) {
                drop_type = daDisItem_ARROW_e;
            } else if ((s32)(u8)sv->mMagic < (s32)((u8)sv->mMaxMagic >> 1)) {
                drop_type = daDisItem_MAGIC_e;
            } else if (arrows < 10) {
                drop_type = daDisItem_ARROW_e;
            } else {
                /* static u8 item_tbl[] = { HEART, MAGIC, ARROW, HEART } (0x101B88BC) */
                drop_type = item_tbl[gabi::ftoi(cM_rndF(2.99f))];
            }
        } else {
            drop_type = daDisItem_NONE13_e;
        }

        fopAcM_createDisappear(actor, i_pos, 3, drop_type, 0xFF);
    } else {
        fopAcM_seStart(actor, JA_SE_CM_KS_DIE, 0);

        dComIfGp_particle_setSimple(0x8068 /* ID_IT_SN_O_KUROBOU_SIBOU00 */, i_pos);

        /* gm_birth_delet(i_this) */
        if (i_this->mGmID) {
            fopAc_ac_c* i_gm = fopAcM_SearchByID(i_this->mGmID);
            if (i_gm && fopAc_IsActor(i_gm) && i_gm && fpcM_GetName(i_gm) == fpcNm_GM_e) {
                be<s16>& m31E = ksGm(i_gm)->m31E; /* gm_class::m31E */
                if (m31E > 0) {
                    m31E = m31E - 1;
                }
            }
        }
    }

    fopAcM_delete(actor);
}

/* 021A7E68 */
static void* tsubo_search(void* param_1, void* i_data) {

    ks_class* i_this = (ks_class*)i_data;
    fopAc_ac_c* actor = i_this;
    // HD: the GameCube's `r0 < 100` is gone; the name check tests the pointer
    if (fopAc_IsActor(param_1) && param_1 != nullptr && fpcM_GetName(param_1) == fpcNm_TSUBO_e) {
        fopAc_ac_c* tsubo_actor = (fopAc_ac_c*)param_1;

        if (std::fabs((f32)(tsubo_actor->current.pos.x - actor->current.pos.x)) < 20.0f &&
            std::fabs((f32)(tsubo_actor->current.pos.y - actor->current.pos.y)) < 20.0f &&
            std::fabs((f32)(tsubo_actor->current.pos.z - actor->current.pos.z)) < 20.0f) {
            i_this->mKsID = fopAcM_GetID(tsubo_actor);
        }
    }
    return nullptr;
}

/* 021A7F24 */
void BG_check(ks_class* i_this) {

    fopAc_ac_c* actor = i_this;

    // HD: no mAcchCir.SetWall(30, 30) here (daKS_Create sets the wall once)
    f32 h = i_this->m304;
    actor->current.pos.y = actor->current.pos.y - h;
    actor->old.pos.y = actor->old.pos.y - h;

    i_this->mAcch.CrrPos(dComIfG_Bgsp());

    h = i_this->m304;
    actor->current.pos.y = actor->current.pos.y + h;
    actor->old.pos.y = actor->old.pos.y + h;
}

/* 021A9EC0 */
static BOOL daKS_IsDelete(ks_class*) {

    return TRUE;
}

/* 021A9EC8 */
static BOOL daKS_Delete(ks_class* i_this) {

    dComIfG_resDelete(&i_this->mPhs, "Ks");

    ripple_remove(i_this->m52C);

    enemy_fire_remove(&i_this->mEnemyFire);

    KS_ALL_COUNT = KS_ALL_COUNT - 1;
    if (KS_ALL_COUNT == 0) {
        dComIfGp_get(); /* daPy_getPlayerLinkActorClass()->offHeavyState(): HD inline, no effect */

        KUTTUKU_ALL_COUNT = 0;
        GORON_COUNT = 0;
        HEAVY_IN = FALSE;
    }

    return TRUE;
}

/* 021A9F48 */
static BOOL useHeapInit(fopAc_ac_c* i_act) {

    ks_class* i_this = (ks_class*)i_act;
    J3DModel* bodyModel;
    J3DModel* eyeModel;

    i_this->mpBodyMorf = mDoExt_McaMorf::create(
        nullptr, (J3DModelData*)dComIfG_getObjectRes("Ks", dRes_INDEX_KS_BDL_KS_BODY_e, KS_SAFESTRING_VTBL), nullptr, nullptr,
        nullptr, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, 0, nullptr, 0, 0x11020203);
    if (i_this->mpBodyMorf == nullptr || i_this->mpBodyMorf->getModel() == nullptr)
        return FALSE;
    bodyModel = i_this->mpBodyMorf->getModel();

    i_this->mpBodyBrkAnm = (mDoExt_brkAnm*)new_brkAnm();
    if (i_this->mpBodyBrkAnm == nullptr)
        return FALSE;

    {
        void* key = dComIfG_getObjectRes("Ks", dRes_INDEX_KS_BRK_KS_BODY_e, KS_SAFESTRING_VTBL);
        if (ksBrkInit( i_this->mpBodyBrkAnm, J3DModel_getModelData(bodyModel), key, TRUE,
                            J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0) == 0)
            return FALSE;
    }

    {
        J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes("Ks", dRes_INDEX_KS_BDL_KS_EYE_e, KS_SAFESTRING_VTBL);
        J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes("Ks", dRes_INDEX_KS_BCK_MABATAKI_e, KS_SAFESTRING_VTBL);
        i_this->mpEyeMorf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, 1,
                                                   nullptr, 0, 0x11020203);
    }
    if (i_this->mpEyeMorf == nullptr || i_this->mpEyeMorf->getModel() == nullptr)
        return FALSE;
    eyeModel = i_this->mpEyeMorf->getModel();

    i_this->mpEyeBtkAnm = (mDoExt_btkAnm*)new_btkAnm();
    if (i_this->mpEyeBtkAnm == nullptr)
        return FALSE;

    {
        void* key = dComIfG_getObjectRes("Ks", dRes_INDEX_KS_BTK_KS_EYE_e, KS_SAFESTRING_VTBL);
        if (ksBtkInit( i_this->mpEyeBtkAnm, J3DModel_getModelData(eyeModel), key, TRUE,
                            J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0) == 0)
            return FALSE;
    }

    i_this->mpEyeBrkAnm = (mDoExt_brkAnm*)new_brkAnm();
    if (i_this->mpEyeBrkAnm == nullptr)
        return FALSE;

    {
        void* key = dComIfG_getObjectRes("Ks", dRes_INDEX_KS_BRK_KS_EYE_e, KS_SAFESTRING_VTBL);
        if (ksBrkInit( i_this->mpEyeBrkAnm, J3DModel_getModelData(eyeModel), key, TRUE,
                            J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0) == 0)
            return FALSE;
    }

    // HD: in Ganon's Tower the two model packets are set up (0207FD38)
    if (ksStageIs("GanonK")) {
        ksMain_packetInit(i_this->mPacket[0], 0);
        ksMain_packetInit(i_this->mPacket[1], 0);
    }
    return TRUE;
}

/* enemyfire::enemyfire (this TU's inline copy; HD allocates when this == NULL) */
/* 021AA2D8 */
static enemyfire_l* enemyfire_ct(enemyfire_l* p) {

    if (p == nullptr) {
        p = (enemyfire_l*)operator_new(0x22C);
        if (p == nullptr) return p;
    }
    if (gabi::ea(&p->mDirection) == 0) /* cXyz::cXyz(): `new` when NULL */
        operator_new(0xC);
    dCcD_Stts_ct(&p->mStts);
    ksMain_sphereCtor(&p->mSph); /* dCcD_Sph::dCcD_Sph */
    p->m228 = 1.0f;
    return p;
}

/* 021AA364 */
static ks_class* ks_class_ct(ks_class* p) {

    if (p == nullptr) {
        p = (ks_class*)operator_new(0xF08);
        if (p == nullptr) return p;
    }
    fopAc_ac_c_ct(p);
    p->__vtbl = KS_VTBL;
    ksMain_packetCtor(p->mPacket[0]); /* mDoExt_J3DModelPacketS (HD) */
    ksMain_packetCtor(p->mPacket[1]);
    dBgS_AcchCir_ct(&p->mAcchCir);
    ksAcchCtor(&p->mAcch);
    ksMain_rippleCtor(p->m52C); /* dPa_rippleEcallBack::dPa_rippleEcallBack */
    dCcD_Stts_ct(&p->mStts);
    ksMain_sphereCtor(&p->mSph); /* dCcD_Sph::dCcD_Sph */
    /* enemyice::enemyice (inline) */
    dCcD_Stts_ct(&p->mEnemyIce.mStts);
    dCcD_Cyl_ct(&p->mEnemyIce.mCyl, KS_ICE_CYLINDER_VTABLE);
    dBgS_AcchCir_ct(&p->mEnemyIce.mBgAcchCir);
    ksAcchCtor(&p->mEnemyIce.mBgAcch);
    ksMain_fireCtor(&p->mEnemyFire); /* enemyfire::enemyfire */
    return p;
}

/* 021AA4C8 */
static cPhs_State daKS_Create(fopAc_ac_c* i_this) {

    ks_class* a_this = (ks_class*)i_this;
    /* fopAcM_ct(i_this, ks_class) */
    if (!fopAcM_CheckCondition(i_this, fopAcCnd_INIT_e)) {
        if (i_this != nullptr) {
            ksMain_actorCtor(i_this); /* ks_class::ks_class */
        }
        fopAcM_OnCondition(i_this, fopAcCnd_INIT_e);
    }

    cPhs_State res = dComIfG_resLoad(&a_this->mPhs, "Ks");
    if (res != cPhs_COMPLEATE_e) {
        return res;
    }
    if (!fopAcM_entrySolidHeap(i_this, gabi::guest_fn((const void*)&useHeapInit), 0x1060)) {
        return cPhs_ERROR_e;
    }

    u32 prm = fopAcM_GetParam(i_this);
    a_this->m2C8 = prm;
    a_this->m2C9 = prm >> 8;
    a_this->m2CA = prm >> 0x10;

    if (a_this->m2C8 == 0xff) {
        a_this->m2C8 = 0;
    }

    if (a_this->m2C8 <= 1 && (a_this->m2C9 > 0x15 || a_this->m2C9 == 0)) {
        a_this->m2C9 = 1;
    }

    if (a_this->m2CA == 0xff) {
        a_this->m2CA = 0;
    }

    a_this->m318 = (f32)(u8)a_this->m2CA * 10.0f;

    if (a_this->m2C8 <= 1) {
        u32 param = 2;
        if (a_this->m2C8 == 0) {
            param = 3;
        }

        for (int i = 0; i < a_this->m2C9; i++) {
            cXyz local_4c;
            local_4c.y = i_this->current.pos.y;
            local_4c.x = i_this->current.pos.x;
            local_4c.z = i_this->current.pos.z;
            if (i != 0) {
                if (i_this->current.angle.x != 0) {
                    f32 y = local_4c.y + cM_rndFX(120.0f);
                    f32 ground_y = a_this->mAcch.GetGroundH();
                    local_4c.y = y;
                    if (y < ground_y) {
                        local_4c.y = ground_y + cM_rndF(120.0f);
                    }
                } else {
                    local_4c.x = local_4c.x + cM_rndFX(40.0f);
                    local_4c.z = local_4c.z + cM_rndFX(40.0f);
                }
            }
            /* i_this->shape_angle = i_this->current.angle; i_this->shape_angle.x = 0; */
            i_this->shape_angle.x = 0;
            i_this->shape_angle.y = i_this->current.angle.y;
            i_this->shape_angle.z = i_this->current.angle.z;

            fopAcM_create(fpcNm_KS_e, param, &local_4c, fopAcM_GetRoomNo(i_this), &i_this->shape_angle, &i_this->scale, 0, 0);
        }

        return cPhs_ERROR_e;
    }

    if (KS_ALL_COUNT == 0) {
        GORON_COUNT = 0;
        KUTTUKU_ALL_COUNT = 0;
        HEAVY_IN = FALSE;
    }

    KS_ALL_COUNT = KS_ALL_COUNT + 1;

    i_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(a_this->mpBodyMorf->getModel()));

    fopAcM_setCullSizeBox(i_this, -20.0f, -20.0f, -20.0f, 20.0f, 20.0f, 20.0f);

    i_this->attention_info.flags = 0; /* attention_info.flags */

    a_this->mAcch.Set(&i_this->current.pos, &i_this->old.pos, i_this, 1, &a_this->mAcchCir, &i_this->speed, nullptr, nullptr);

    // HD: the wall check is set once here; larger in stage TF_02
    if (ksStageIs("TF_02")) {
        a_this->mAcchCir.SetWall(30.0f, REG_F(10, 8) + 50.0f);
    } else {
        a_this->mAcchCir.SetWall(30.0f, 30.0f);
    }

    a_this->mStts.Init(2, 1, i_this);

    i_this->max_health = 1;
    i_this->health = 1;

    a_this->m304 = 25.0f;

    f32 t = cM_rndFX(25.0f) + 50.0f;
    a_this->m2DC.x = 1.0f;
    a_this->m2DC.y = 1.0f;
    a_this->m2DC.z = 1.0f;
    a_this->m2E8[0] = (s16)gabi::ftoi(t);

    a_this->m2FE = (fopAcM_GetID(i_this) & 7) * 0x32 + 1000;

    a_this->mSph.Set(&body_co_sph_src);
    a_this->mSph.SetStts(&a_this->mStts);

    a_this->mEnemyIce.mpActor = i_this;
    a_this->mEnemyIce.mWallRadius = 20.0f;
    a_this->mEnemyIce.mCylHeight = 20.0f;
    a_this->mEnemyIce.m1B0 = 1;

    a_this->mEnemyFire.mpMcaMorf = a_this->mpBodyMorf;
    a_this->mEnemyFire.mpActor = i_this;

    // HD: verified flame joint and scale tables are shared with the constructor.
    for (int i = 0; i < 10; i++) {
        a_this->mEnemyFire.mFlameJntIdxs[i] = fire_j[i];
        a_this->mEnemyFire.mParticleScale[i] = fire_sc[i];
    }

    i_this->itemTableIdx = dComIfGp_CharTbl_GetNameIndex("kuro_s", 0);

    if (a_this->m2C8 == 4 || a_this->m2C8 == 5) {
        a_this->mGmID = i_this->parentActorID; /* fopAcM_GetLinkId */
        if (a_this->mGmID == fpcM_ERROR_PROCESS_ID_e) {
            return cPhs_ERROR_e;
        }

        fopAc_ac_c* gm_actor = fopAcM_SearchByID(a_this->mGmID);
        if (gm_actor && fopAc_IsActor(gm_actor) && gm_actor && fpcM_GetName(gm_actor) == fpcNm_GM_e && a_this->m2C8 == 5) {
            i_this->current.angle.y = (s16)gabi::ftoi((f32)(s16)i_this->current.angle.y + cM_rndFX(0x2000));
            i_this->speedF = cM_rndF(6.0f) + 34.0f;
            i_this->speed.y = cM_rndF(8.0f) + 22.0f;
        }
    }

    a_this->m31C = 20.0f;

    if (a_this->m2C8 == 6) {
        a_this->mSph.mObjTg.mSPrm &= ~1u; /* OffTgSetBit */
        a_this->mSph.mObjCo.mSPrm &= ~1u; /* OffCoSetBit */
        a_this->mSph.ClrTgHit();

        a_this->mAction = KS_POT_SEARCH;
        a_this->mMode = 50;

        return res;
    }

    if (a_this->m2C8 == 7) {
        i_this->actor_status |= 0x4000; /* fopAcStts_UNK4000_e */

        a_this->mAction = KS_BIRTH;
        a_this->mMode = 60;

        return res;
    }

    if (a_this->m2C8 == 2) {
        a_this->mAction = KS_MOVE;
        a_this->mMode = 3;

        i_this->gravity = -3.0f;

        BG_check(a_this);

        if (!a_this->mAcch.ChkGroundHit()) {
            i_this->gravity = 0.0f;
        }
    }

    return res;
}

/* __sinit_d_a_ks_cpp (header statics only) */
/* 021AAD90 */
static void __sinit_d_a_ks_cpp() {

    sinit_header_statics(KS_HEADER_STATIC_OBJECT, KS_HEADER_STATIC_FLAG);
}

/* sead::SafeString deleting destructor (this TU's copy) */
/* 021AAE24 */
static void SafeString_dt(SafeString* p, s32 flags) {

    if (p == nullptr) return;
    if (flags & 1) operator_delete(p);
}

/* ks_class deleting destructor */
/* 021AAE38 */
static void ks_class_dt(ks_class* p, s32 flags) {

    if (p == nullptr) return;
    /* ~enemyfire */
    ksMain_sphereDtor(&p->mEnemyFire.mSph, 2);  /* dCcD_Sph::~dCcD_Sph */
    ksMain_statusDtor(&p->mEnemyFire.mStts, 2); /* dCcD_Stts::~dCcD_Stts */
    /* ~enemyice */
    p->mEnemyIce.mBgAcch.dBgS_GrpPassChk::__vtbl = KS_ACCH_VTABLE;
    p->mEnemyIce.mBgAcch.dBgS_PolyPassChk::__vtbl = KS_GROUND_VTABLE;
    ksMain_acchDtor(&p->mEnemyIce.mBgAcch, 0);                                    /* dBgS_Acch::~dBgS_Acch */
    ksMain_circleDtor(&p->mEnemyIce.mBgAcchCir.m_cir, 2); /* cM3dGCir::~cM3dGCir */
    dCcD_Cyl_dt(&p->mEnemyIce.mCyl, 2);
    dCcD_Stts_dt(&p->mEnemyIce.mStts, 2);
    ksMain_sphereDtor(&p->mSph, 2);
    dCcD_Stts_dt(&p->mStts, 2);
    p->mAcch.dBgS_GrpPassChk::__vtbl = KS_ACCH_VTABLE;
    p->mAcch.dBgS_PolyPassChk::__vtbl = KS_GROUND_VTABLE;
    ksMain_acchDtor(&p->mAcch, 0);
    ksMain_circleDtor(&p->mAcchCir.m_cir, 2);
    ksMain_packetDtor(p->mPacket[1], 2); /* mDoExt_J3DModelPacketS::~ */
    ksMain_packetDtor(p->mPacket[0], 2);
    ksMain_actorDtor(p, 0); /* fopAc_ac_c::~fopAc_ac_c */
    if (flags & 1) operator_delete(p);
}

/* sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
/* 021AAF4C */
static void SafeString_assureTerminationImpl(SafeString*) {

}

/* WWHD glue
str 10013794 in 021A9EC8
str 10013797 in 021A9F48
str 100137C0 in 021AA4C8
str 100136FC in 021A7B4C
str 1001379C in 021A9F48
str 100137B8 in 021AA4C8
str 100137C4 in 021AA4C8
data KS_ALL_COUNT 101B883C
data KUTTUKU_ALL_COUNT 101B8840
data HEAVY_IN 101B8844
data GORON_COUNT 101B8848
data ksPauseTimer 101EACB7
data item_tbl 101B88BC
data body_co_sph_src 101B88C0
data fire_j 101B8928
data fire_sc 101B8900
data pl_harituki_joint_dt 101B886C
ext 021AA2D8 enemyfire_l* ksMain_fireCtor(enemyfire_l* fire)
ext 021AA364 ks_class* ksMain_actorCtor(fopAc_ac_c* actor)
ext 025E80D0 void* ksMain_brkCtor(void* storage)
ext 025E7C6C void* ksMain_btkCtor(void* storage)
ext 025E8154 BOOL ksMain_brkInit(void* animation,J3DModelData* model,void* key,s32 play,s32 mode,s16 start,s16 end,bool modify,s32 entry,f32 rate)
ext 025E7CE0 BOOL ksMain_btkInit(void* animation,J3DModelData* model,void* key,s32 play,s32 mode,s16 start,s16 end,bool modify,s32 entry,f32 rate)
ext 02041570 void ksMain_fire(enemyfire_l* fire)
ext 02041C30 void ksMain_removeFire(enemyfire_l* fire)
ext 025A9270 void ksMain_rippleEnd(void* ripple)
ext 0200E814 s32 ksMain_characterIndex(u32 table, const char* name, s32 mode)
ext 0207FD38 void ksMain_packetInit(void* packet, s32 mode)
ext 025166F0 void ksMain_sphereCtor(dCcD_Sph* sphere)
ext 02080404 void ksMain_packetCtor(void* packet)
ext 025A9084 void ksMain_rippleCtor(void* ripple)
ext 02515AE8 void ksMain_sphereDtor(dCcD_Sph* sphere, s32 flags)
ext 02515860 void ksMain_statusDtor(dCcD_Stts* status, s32 flags)
ext 024EFD9C void ksMain_acchDtor(dBgS_ObjAcch* acch, s32 flags)
ext 02018034 void ksMain_circleDtor(void* circle, s32 flags)
ext 02082DDC void ksMain_packetDtor(void* packet, s32 flags)
ext 025D50BC void ksMain_actorDtor(ks_class* actor, s32 flags)
*/
