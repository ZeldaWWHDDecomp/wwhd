/**
 * d_a_sk.cpp (WWHD)
 * Swaying vine/tentacle ("turu") with a jointed hit body.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_sk.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1003CE74 /* this TU's sead::SafeString vtable */
#define SK_VTBL 0x1003CE8C         /* sk_class vtable (HD virtual destructor) */
#define body_co_sph_src gabi::at<dCcD_SrcSph>(0x101D0948)
#define search_data 0x101D0918     /* __jnt_hit_data_c[3] */

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

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD J3DModel: matrix block at +0x2C (dirty flags +4, anm matrices +0x10), model data +0xAC, user area +0xB8 */
struct J3DMtxBlock_sk {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u16> mFlags;
    /* 0x06 */ u8 _06[0x10 - 6];
    /* 0x10 */ be<u32> mpMtx;
};
struct J3DModel_sk {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ gptr<J3DMtxBlock_sk> mpMtxBlock;
    /* 0x30 */ u8 _30[0xAC - 0x30];
    /* 0xAC */ be<u32> mModelData;
    /* 0xB0 */ u8 _B0[8];
    /* 0xB8 */ be<u32> mUserArea;
};
static inline Mtx34* getAnmMtx(J3DModel_sk* model, s32 jntNo) {
    J3DMtxBlock_sk* blk = model->mpMtxBlock;
    blk->mFlags |= 0x10; /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(blk->mpMtx + jntNo * 0x30);
}
/* 02552B60 JntHit_create(J3DModel*, __jnt_hit_data_c*, s16) */
static inline u32 JntHit_create_l(J3DModel* m, u32 data, s16 num) { return gabi::call<u32>(0x02552B60, m, data, num); }
/* 025E1A40 mDoAud_seStart; fopAcM_seStart (HD inline null checks) is in bindings.h */
static inline void __construct_array_l(void* p, u32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }
static inline void __destroy_arr_l(void* p, u32 n, u32 size, u32 dtor, u32 flags) { gabi::call(0x028F0164, p, n, size, dtor, flags); }

struct sk_class : fopEn_enemy_c {
    /* 0x3C8 */ request_of_phase_process_class mPhase;
    /* 0x3D0 */ be<u8> m2B4;
    /* 0x3D1 */ be<u8> m2B5;
    /* 0x3D2 */ u8 _3D2[2];
    /* 0x3D4 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D8 */ be<u8> m2BC;
    /* 0x3D9 */ u8 _3D9;
    /* 0x3DA */ be<s16> m2BE;
    /* 0x3DC */ be<s16> m2C0;
    /* 0x3DE */ csXyz m2C2[4]; /* HD: indexed by joint - 1 */
    /* 0x3F6 */ u8 _3F6[2];
    /* 0x3F8 */ cXyz m2DC[4];  /* HD: indexed by joint - 1 */
    /* 0x428 */ dCcD_Stts mStts;
    /* 0x464 */ dCcD_Sph m348[4]; /* HD: 4 spheres (GameCube 5) */
    /* 0x914 */ be<u32> m924;   /* JntHit_c* */
};
WWHD_OFFSET(sk_class, mpMorf, 0x3D4);
WWHD_OFFSET(sk_class, m2C2, 0x3DE);
WWHD_OFFSET(sk_class, m2DC, 0x3F8);
WWHD_OFFSET(sk_class, mStts, 0x428);
WWHD_OFFSET(sk_class, m924, 0x914);
WWHD_SIZE(sk_class, 0x918);

/* 024884FC: HD: only joints 1..4, the rotations/positions of joint j are stored at [j - 1], the
 * offsets come from a table {0, -70, 0, 0, -200} (GameCube: 180, 250, 250, 180 for joints 4..1) */
static BOOL nodeCallBack(J3DNode* node, int calcTiming) {
    WWHD_FUNC(0x024884FC, BOOL, node, calcTiming);
    if (calcTiming == 0 /* J3DNodeCBCalcTiming_In */) {
        J3DJoint* joint = J3DNode_toJoint(node);
        J3DModel_sk* model = gabi::at<J3DModel_sk>(gabi::load<u32>(0x104B462C)); /* j3dSys.getModel() */
        sk_class* i_this = gabi::at<sk_class>(model->mUserArea);
        u32 jntNo = gabi::load<u16>(gabi::ea(joint) + 4);
        if (i_this != NULL && jntNo - 1 < 4) {
            PSMTXCopy(getAnmMtx(model, jntNo), calc_mtx());
            cMtx_YrotM(calc_mtx(), i_this->m2C2[jntNo - 1].y);
            cMtx_XrotM(calc_mtx(), i_this->m2C2[jntNo - 1].x);
            cMtx_ZrotM(calc_mtx(), i_this->m2C2[jntNo - 1].z);
            gabi::Local<cXyz> offset;
            f32 zero = gabi::load<f32>(0x1003CE9C);
            offset->y = zero;
            offset->x = zero;
            offset->z = gabi::load<f32>(0x1003CE9C + jntNo * 4);
            MtxPosition(offset, &i_this->m2DC[jntNo - 1]);
            /* model->setAnmMtx(jntNo, *calc_mtx) */
            Mtx34* src = calc_mtx();
            Mtx34* dst = getAnmMtx(model, jntNo);
            mtx_copy(dst, src);
            PSMTXCopy(calc_mtx(), gabi::at<Mtx34>(0x104B4868)); /* J3DSys::mCurrentMtx */
        }
    }
    return TRUE;
}
VERIFY(0x024884FC, nodeCallBack);

/* 02488668 */
static BOOL daSk_Draw(sk_class* i_this) {
    WWHD_FUNC(0x02488668, BOOL, i_this);
    J3DModel* model = i_this->mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0_PLIGHT, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    i_this->mpMorf->entryDL();
    return TRUE;
}
VERIFY(0x02488668, daSk_Draw);

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
            void* hitObj = i_this->m348[i].GetTgHitObj();
            if (hitObj == NULL) {
                break;
            }
            i_this->m2C0 = 8;

            switch (gabi::load<u32>(gabi::ea(hitObj) + 0x10) /* GetAtType() */) {
            case 0x2:        /* AT_TYPE_SWORD */
            case 0x400:      /* AT_TYPE_MACHETE */
            case 0x800:      /* AT_TYPE_UNK800 */
            case 0x4000000:  /* AT_TYPE_DARKNUT_SWORD */
            case 0x10000000: /* AT_TYPE_MOBLIN_SPEAR */
            case 0x20000000: /* AT_TYPE_PGANON_SWORD */
                fopAcM_seStart(i_this, JA_SE_LK_SW_HIT_S, 0x44);
                break;

            case 0x40:      /* AT_TYPE_BOOMERANG */
            case 0x80:      /* AT_TYPE_BOKO_STICK */
            case 0x2000:    /* AT_TYPE_UNK2000 */
            case 0x1000000: /* AT_TYPE_STALFOS_MACE */
                fopAcM_seStart(i_this, JA_SE_LK_W_WEP_HIT, 0x44);
                break;

            case 0x10000: /* AT_TYPE_SKULL_HAMMER */
                fopAcM_seStart(i_this, JA_SE_LK_HAMMER_HIT, 0x44);
                break;

            case 0x4000:    /* arrows, grappling hook */
            case 0x40000:
            case 0x80000:
            case 0x100000:
            case 0x8000000:
                fopAcM_seStart(i_this, JA_SE_LK_MS_WEP_HIT, 0x44);
                break;
            }
        }
    }
}

/* 024886D0 */
static BOOL daSk_Execute(sk_class* i_this) {
    WWHD_FUNC(0x024886D0, BOOL, i_this);
    dousa_move(i_this);

    mDoExt_McaMorf* morf = i_this->mpMorf;
    f32 sy = i_this->scale.y;
    f32 sx = i_this->scale.x;
    i_this->m2BE += 1;
    J3DModel* model = morf->getModel();
    f32 sz = i_this->scale.z;
    gabi::store<f32>(gabi::ea(model) + 0xBC, sx);
    gabi::store<f32>(gabi::ea(model) + 0xC0, sy);
    gabi::store<f32>(gabi::ea(model) + 0xC4, sz);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->current.angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->current.angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());
    i_this->mpMorf->calc();

    for (s32 i = 0; i < 4; i++) {
        i_this->m348[i].SetC(&i_this->m2DC[i]);
        i_this->m348[i].SetR(gabi::load<f32>(0x1003CEC0 + i * 4)); /* {220, 180, 120, 70} */
        dComIfG_Ccsp_Set(&i_this->m348[i]);
    }
    body_atari_check(i_this);
    return TRUE;
}
VERIFY(0x024886D0, daSk_Execute);

/* 02488B80 */
static BOOL daSk_IsDelete(sk_class*) {
    WWHD_FUNC(0x02488B80, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x02488B80, daSk_IsDelete);

/* 02488B88: HD: resDelete (GameCube resDeleteDemo) */
static BOOL daSk_Delete(sk_class* i_this) {
    WWHD_FUNC(0x02488B88, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhase, STR(0x1003CED0) /* "Sk" */);
    return TRUE;
}
VERIFY(0x02488B88, daSk_Delete);

/* 02488BB8: HD: the callback is set on joints 1.. only */
static BOOL useHeapInit(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02488BB8, BOOL, a_this);
    sk_class* i_this = (sk_class*)a_this;
    J3DModelData* pModelData = (J3DModelData*)dComIfG_getObjectRes(STR(0x1003CED8) /* "Sk" */, dRes_INDEX_SK_BDL_TURU_00_e, SAFESTRING_VTBL);

    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, pModelData, NULL, NULL, NULL, -1, 1.0f, 0, -1, 1, NULL, 0x80000, 0x11000022);
    if (i_this->mpMorf == NULL || i_this->mpMorf->getModel() == NULL) {
        return FALSE;
    }

    gabi::store<u32>(gabi::ea(i_this->mpMorf->getModel()) + 0xB8, gabi::ea(i_this)); /* setUserArea */

    for (u16 i = 0; i < gabi::load<u16>(gabi::call<u32>(0x027F3F94, J3DModel_getModelData(i_this->mpMorf->getModel())) + 8); i++) {
        if (i != 0) {
            u32 data = gabi::ea(J3DModel_getModelData(i_this->mpMorf->getModel()));
            u32 n = gabi::load<u32>(data + 4);
            u32 p = gabi::load<u32>(data + 8);
            if (i < n) p += i * 0x1C;
            gabi::store<u32>(p + 8, 0x024884FC /* nodeCallBack */);
        }
    }

    i_this->m924 = JntHit_create_l(i_this->mpMorf->getModel(), search_data, 3);
    if (i_this->m924 != 0) {
        i_this->jntHit = i_this->m924;
    } else {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x02488BB8, useHeapInit);

/* 02488D58: HD: no initial dousa/matrix setup; the spheres are set up, then Execute runs once */
static cPhs_State daSk_Create(fopAc_ac_c* a_this) {
    WWHD_FUNC(0x02488D58, cPhs_State, a_this);
    sk_class* i_this = (sk_class*)a_this;
    /* fopAcM_ct(a_this, sk_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = SK_VTBL;
            dCcD_Stts_ct(&i_this->mStts);
            __construct_array_l(i_this->m348, 4, 0x12C, 0x025166F0 /* dCcD_Sph::dCcD_Sph */);
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }
    cPhs_State PVar2 = dComIfG_resLoad(&i_this->mPhase, STR(0x1003CEEC) /* "Sk" */);
    if (PVar2 == cPhs_COMPLEATE_e) {
        i_this->m2B4 = fopAcM_GetParam(i_this);
        if (i_this->m2B4 == 0xff) {
            i_this->m2B4 = 0;
        }

        i_this->m2B5 = fopAcM_GetParam(i_this) >> 0x18;
        if (i_this->m2B5 != 0xff && fopAcM_isSwitch(a_this, i_this->m2B5)) {
            return cPhs_ERROR_e;
        }

        if (!fopAcM_entrySolidHeap(a_this, 0x02488BB8 /* useHeapInit */, 0xD00)) {
            return cPhs_ERROR_e;
        }

        /* fopAcM_SetMtx(a_this, model->getBaseTRMtx()) */
        a_this->cullMtx = gabi::ea(J3DModel_getBaseTRMtx(i_this->mpMorf->getModel()));
        gabi::call(0x025D672C, a_this, -800.0f, -200.0f, -400.0f); /* fopAcM_SetMin */
        gabi::call(0x025D673C, a_this, 500.0f, 1000.0f, 1000.0f);  /* fopAcM_SetMax */
        gabi::store<u32>(gabi::ea(a_this) + 0x39C, 0);              /* attention_info.flags */
        i_this->m2BE = (s16)((u8)fopAcM_GetID(a_this) << 0xC);

        for (s32 i = 0; i < 4; i++) {
            i_this->m348[i].Set(body_co_sph_src);
            i_this->m348[i].SetStts(&i_this->mStts);
        }
        i_this->mStts.Init(0x32, 0, a_this);
        i_this->m2C0 = 8;
        daSk_Execute(i_this);
    }
    return PVar2;
}
VERIFY(0x02488D58, daSk_Create);

/* 02488F6C: static initialisation of the translation unit (header statics only) */
static void __sinit_d_a_sk_cpp() {
    WWHD_FUNC(0x02488F6C, void, (u32)0);
    sinit_header_statics(0x1046DEE4, 0x101D0988);
}
VERIFY(0x02488F6C, __sinit_d_a_sk_cpp);

/* 02489000: sead::SafeString deleting destructor (this TU's vtable 0x1003CE74) */
static void SafeString_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02489000, void, p, flags);
    if (p != nullptr && (flags & 1)) {
        operator_delete(p);
    }
}
VERIFY(0x02489000, SafeString_dt);

/* 02489014: sk_class deleting destructor (compiler-generated, HD virtual destructor) */
static void sk_class_dt(sk_class* i_this, s32 flags) {
    WWHD_FUNC(0x02489014, void, i_this, flags);
    if (i_this != nullptr) {
        __destroy_arr_l(i_this->m348, 4, 0x12C, 0x02515AE8 /* dCcD_Sph::~dCcD_Sph */, 0);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02489014, sk_class_dt);

/* 02489094: sead::SafeString::assureTerminationImpl_ (this TU's copy, empty) */
static void SafeString_assureTerminationImpl(void*) {
    WWHD_FUNC(0x02489094, void, (u32)0);
}
VERIFY(0x02489094, SafeString_assureTerminationImpl);
