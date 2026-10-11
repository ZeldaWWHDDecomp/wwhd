/**
 * d_a_sk.cpp (WWHD)
 * Swaying vine/tentacle ("turu") with a jointed hit body.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_sk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_sk.h"
const gc_tu_data gc_tu = {.safeString = 0x1003CE74};
enum { dRes_INDEX_SK_BDL_TURU_00_e = 3 };
enum { TEV_TYPE_BG0_PLIGHT = 0x5B };
/* HD sound ids */
enum {
    JA_SE_OBJ_SHOKU_LIFT_MOVE = 0x3823,
    JA_SE_LK_SW_HIT_S = 0x2803,
    JA_SE_LK_W_WEP_HIT = 0x2833,
    JA_SE_LK_HAMMER_HIT = 0x2855,
    JA_SE_LK_MS_WEP_HIT = 0x2834,
};

// HD: checked packed collision and joint-hit sources.
static const u32 body_co_sph_src[] = {0, 8192, 16777216, 15, 4280155839, 3, 0, 100665600, 0, 0, 0, 0, 0, 0, 0, 1097859072};
static const u32 search_data[] = {524289, 1124859904, 270338364, 524290, 1119092736, 270338364, 524291, 1112014848, 270338364};
static const f32 joint_offsets[] = {0.0f, -70.0f, 0.0f, 0.0f, -200.0f};
static const f32 sphere_radii[] = {220.0f, 180.0f, 120.0f, 70.0f};
// HD: four collision spheres and an actor virtual table.
sk_class::sk_class() {
    fopAc_ac_c_ct(this);
    __vtbl = 0x1003CE8C;
    dCcD_Stts_ct(&mStts);
    Sk_constructArray(m348, 4, sizeof(dCcD_Sph), 0x025166F0);
}

// HD: only joints 1..4, the rotations/positions of joint j are stored at [j - 1], the
// offsets come from a table {0, -70, 0, 0, -200} (GameCube: 180, 250, 250, 180 for joints 4..1)
/* 024884FC */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    if (calcTiming == J3DNodeCBCalcTiming_In) {
        SkJointData* joint = Sk_jointData(node);
        J3DModel* model = j3dSys.getModel();
        sk_class* i_this = static_cast<sk_class*>(model->getUserArea());
        u32 jntNo = joint->mJntNo;
        if (i_this != NULL && jntNo - 1 < 4) {
            PSMTXCopy(model->getAnmMtx(jntNo), calc_mtx());
            cMtx_YrotM(calc_mtx(), i_this->m2C2[jntNo - 1].y);
            cMtx_XrotM(calc_mtx(), i_this->m2C2[jntNo - 1].x);
            cMtx_ZrotM(calc_mtx(), i_this->m2C2[jntNo - 1].z);
            gabi::Local<cXyz> offset;
            f32 zero = joint_offsets[0];
            offset->y = zero;
            offset->x = zero;
            offset->z = joint_offsets[jntNo];
            MtxPosition(offset, &i_this->m2DC[jntNo - 1]);
            /* model->setAnmMtx(jntNo, *calc_mtx) */
            Mtx34* src = calc_mtx();
            Mtx34* dst = model->getAnmMtx(jntNo);
            mtx_copy(dst, src);
            PSMTXCopy(calc_mtx(), J3DSys::mCurrentMtx); /* J3DSys::mCurrentMtx */
        }
    }
    return TRUE;
}

/* 02488668 */
static BOOL daSk_Draw(sk_class* i_this) {
    J3DModel* model = i_this->mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0_PLIGHT, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    i_this->mpMorf->entryDL();
    return TRUE;
}

/* dousa_move (inlined in Execute). HD: a different motion: joints 1..3 swing in X and Y with an
 * amplitude growing along the vine, Z stays 0 */
static inline void dousa_move(sk_class* i_this) {
    for (s32 i = 0; i < 3; i++) {
        f32 amp = gabi::fmadds((f32)i, 1000.0f, 1500.0f);
        i_this->m2C2[i].x = (s16)gabi::ftoi(cM_ssin(i_this->m2BE * 1800 - i * 16000) * amp);
        i_this->m2C2[i].y = (s16)gabi::ftoi(cM_ssin(i_this->m2BE * 1400 - i * 16000) * amp);
        i_this->m2C2[i].z = 0;
    }

    s16 angle = cLib_distanceAngleS(i_this->m2C2[1].z, 0x1000);
    if (angle < 0x1000) {
        if (!i_this->m2BC) {
            fopAcM_seStart(i_this, JA_SE_OBJ_SHOKU_LIFT_MOVE, 0);
            i_this->m2BC = true;
        }
    } else {
        i_this->m2BC = false;
    }
}

/* body_atari_check (inlined in Execute) */
static inline void body_atari_check(sk_class* i_this) {
    if (i_this->m2C0 > 0) {
        i_this->m2C0 -= 1;
        return;
    }

    for (s32 i = 0; i < 4; i++) {
        if (i_this->m348[i].ChkTgHit()) {
            SkHitObject* hitObj = static_cast<SkHitObject*>(i_this->m348[i].GetTgHitObj());
            if (hitObj == NULL) {
                break;
            }
            i_this->m2C0 = 8;

            switch (hitObj->GetAtType()) {
            case AT_TYPE_SWORD:
            case AT_TYPE_MACHETE:
            case AT_TYPE_UNK800:
            case AT_TYPE_DARKNUT_SWORD:
            case AT_TYPE_MOBLIN_SPEAR:
            case AT_TYPE_PGANON_SWORD:
                fopAcM_seStart(i_this, JA_SE_LK_SW_HIT_S, 0x44);
                break;

            case AT_TYPE_BOOMERANG:
            case AT_TYPE_BOKO_STICK:
            case AT_TYPE_UNK2000:
            case AT_TYPE_STALFOS_MACE:
                fopAcM_seStart(i_this, JA_SE_LK_W_WEP_HIT, 0x44);
                break;

            case AT_TYPE_SKULL_HAMMER:
                fopAcM_seStart(i_this, JA_SE_LK_HAMMER_HIT, 0x44);
                break;

            case AT_TYPE_FIRE_ARROW:
            case AT_TYPE_ICE_ARROW:
            case AT_TYPE_LIGHT_ARROW:
            case AT_TYPE_NORMAL_ARROW:
            case AT_TYPE_GRAPPLING_HOOK:
                fopAcM_seStart(i_this, JA_SE_LK_MS_WEP_HIT, 0x44);
                break;
            }
        }
    }
}

/* 024886D0 */
static BOOL daSk_Execute(sk_class* i_this) {
    dousa_move(i_this);

    mDoExt_McaMorf* morf = i_this->mpMorf;
    f32 sy = i_this->scale.y;
    f32 sx = i_this->scale.x;
    i_this->m2BE += 1;
    J3DModel* model = morf->getModel();
    f32 sz = i_this->scale.z;
    model->mBaseScale.x = sx;
    model->mBaseScale.y = sy;
    model->mBaseScale.z = sz;
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->current.angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->current.angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    i_this->mpMorf->calc();

    for (s32 i = 0; i < 4; i++) {
        i_this->m348[i].SetC(&i_this->m2DC[i]);
        i_this->m348[i].SetR(sphere_radii[i]); /* {220, 180, 120, 70} */
        dComIfG_Ccsp_Set(&i_this->m348[i]);
    }
    body_atari_check(i_this);
    return TRUE;
}

/* 02488B80 */
static BOOL daSk_IsDelete(sk_class*) {
    return TRUE;
}

// HD: ordinary resource deletion replaces resDeleteDemo.
/* 02488B88 */
static BOOL daSk_Delete(sk_class* i_this) {
    dComIfG_resDelete(&i_this->mPhase, "Sk");
    return TRUE;
}

// HD: joint zero is excluded from the callback installation.
/* 02488BB8 */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    sk_class* i_this = (sk_class*)a_this;
    J3DModelData* pModelData = (J3DModelData*)dComIfG_getObjectRes("Sk", dRes_INDEX_SK_BDL_TURU_00_e);

    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, pModelData, NULL, NULL, NULL, -1, 1.0f, 0, -1, 1, NULL, 0x80000, 0x11000022);
    if (i_this->mpMorf == NULL || i_this->mpMorf->getModel() == NULL) {
        return FALSE;
    }

    i_this->mpMorf->getModel()->setUserArea((uintptr_t)i_this);

    for (u16 i = 0; i < i_this->mpMorf->getModel()->getModelData()->getJointNum(); i++) {
        if (i != 0) {
            i_this->mpMorf->getModel()->getModelData()->getJointNodePointer(i)->setCallBack(nodeCallBack);
        }
    }

    i_this->m924 = JntHit_create(i_this->mpMorf->getModel(), search_data, 3);
    if (i_this->m924 != 0) {
        i_this->jntHit = i_this->m924;
    } else {
        return FALSE;
    }
    return TRUE;
}

// HD: initialize the four spheres, then run Execute once for the initial pose.
/* 02488D58 */
static cPhs_State daSk_Create(fopAc_ac_c* a_this) {
    sk_class* i_this = (sk_class*)a_this;
    fopAcM_ct(a_this, sk_class);
    cPhs_State PVar2 = dComIfG_resLoad(&i_this->mPhase, "Sk");
    if (PVar2 == cPhs_COMPLEATE_e) {
        i_this->m2B4 = fopAcM_GetParam(i_this);
        if (i_this->m2B4 == 0xff) {
            i_this->m2B4 = 0;
        }

        i_this->m2B5 = fopAcM_GetParam(i_this) >> 0x18;
        if (i_this->m2B5 != 0xff && fopAcM_isSwitch(a_this, i_this->m2B5)) {
            return cPhs_ERROR_e;
        }

        if (!fopAcM_entrySolidHeap(a_this, useHeapInit, 0xD00)) {
            return cPhs_ERROR_e;
        }

        /* fopAcM_SetMtx(a_this, model->getBaseTRMtx()) */
        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));
        fopAcM_SetMin(a_this, -800.0f, -200.0f, -400.0f); /* fopAcM_SetMin */
        fopAcM_SetMax(a_this, 500.0f, 1000.0f, 1000.0f);  /* fopAcM_SetMax */
        a_this->attention_info.flags = 0;              /* attention_info.flags */
        i_this->m2BE = (s16)((u8)fopAcM_GetID(a_this) << 0xC);

        for (s32 i = 0; i < 4; i++) {
            i_this->m348[i].Set(reinterpret_cast<const dCcD_SrcSph*>(body_co_sph_src));
            i_this->m348[i].SetStts(&i_this->mStts);
        }
        i_this->mStts.Init(0x32, 0, a_this);
        i_this->m2C0 = 8;
        daSk_Execute(i_this);
    }
    return PVar2;
}

/* 02488F6C */
static void __sinit_d_a_sk_cpp() {
    sinit_header_statics(0x1046DEE4, 0x101D0988);
}

/* 02489000 */
static void SafeString_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}

/* 02489014 */
static void sk_class_dt(sk_class* i_this, s32 flags) {
    if (i_this != nullptr) {
        Sk_destroyArray(i_this->m348, 4, 0x12C, 0x02515AE8 /* dCcD_Sph::~dCcD_Sph */, 0);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02489094 */
static void SafeString_assureTerminationImpl(void*) {
}

/* WWHD glue (tools/verify/autoglue.py)
ext 027F7878 SkJointData* Sk_jointData(J3DNode*)
ext 02552B60 u32 JntHit_create(J3DModel*, const void*, s16)
ext 028EFFD0 void Sk_constructArray(void*, u32, u32, u32)
ext 028F0164 void Sk_destroyArray(void*, u32, u32, u32, u32)
ext 025D672C void fopAcM_SetMin(fopAc_ac_c*, f32, f32, f32)
ext 025D673C void fopAcM_SetMax(fopAc_ac_c*, f32, f32, f32)
str 1003CED0 "Sk" in 02488B88
str 1003CED8 "Sk" in 02488BB8
str 1003CEEC "Sk" in 02488D58
data body_co_sph_src 101D0948
data search_data 101D0918
data joint_offsets 1003CE9C
data sphere_radii 1003CEC0
*/
