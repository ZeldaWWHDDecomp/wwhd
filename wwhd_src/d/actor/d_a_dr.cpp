/**
 * d_a_dr.cpp (WWHD)
 * NPC - Valoo (Overworld) / Dragon
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_dr.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define SAFESTRING_VTBL 0x1000E054
#define DR_VTBL 0x1000E06C     /* HD: dr_class vtable */
#define DR_HIO_VTBL 0x1000E07C /* daDr_HIO_c vtable */

enum {
    dRes_INDEX_DR_BAS_ABARE1_e = 5,
    dRes_INDEX_DR_BAS_ABARE2_e = 6,
    dRes_INDEX_DR_BAS_AKUBI1_e = 7,
    dRes_INDEX_DR_BAS_BIKU1_e = 8,
    dRes_INDEX_DR_BCK_DR_ABARE1_e = 0xB,
    dRes_INDEX_DR_BCK_DR_ABARE2_e = 0xC,
    dRes_INDEX_DR_BCK_DR_AKUBI1_e = 0xD,
    dRes_INDEX_DR_BCK_DR_BIKU1_e = 0xE,
    dRes_INDEX_DR_BCK_DR_HO1_e = 0xF,
    dRes_INDEX_DR_BCK_DR_WAIT1_e = 0x10,
    dRes_INDEX_DR_BMD_DR1_e = 0x13,
};
enum { DR1_JNT_DR_ALL_ROOT_e = 0, DR1_JNT_J_DR_SITA2_e = 0x20 };
enum { JA_SE_CM_DRG_MTOP_FIRE = 0x61AB, JA_SE_CM_DRG_MTOP_MAGMA = 0x783F };
enum {
    dPa_name_ID_AK_SN_DRPAINBIKU00 = 0x81C4,
    dPa_name_ID_AK_SN_DRPAINABARE00 = 0x81C5,
    dPa_name_ID_AK_SN_DRPAINHO00 = 0x81C6,
    dPa_name_ID_AK_SN_DRSPLASHMAGMA00 = 0x81C7,
};
enum { DSNAP_TYPE_DR = 0x99 };

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD J3D: a model's joint matrices live in a block at +0x2C (+0x4 flags, +0x10 matrices) */
static inline Mtx34* J3DModel_getAnmMtx(J3DModel* m, s32 jntNo) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, (u16)(gabi::load<u16>(blk + 4) | 0x10)); /* HD: marks the joint matrices dirty */
    return gabi::at<Mtx34>(gabi::load<u32>(blk + 0x10) + jntNo * 0x30);
}
static inline BOOL dComIfGs_isStageBossEnemy(s32 no) { return gabi::call<BOOL>(0x02520A84, no); }
static inline void JPASetRMtxTVecfromMtx(Mtx34* m, u32 r, u32 t) { gabi::call(0x028249B0, m, r, t); }
/* JPABaseEmitter::setGlobalRTMatrix (HD inline): rotation at +0x1F0, translation at +0x22C */
static inline void JPABaseEmitter_setGlobalRTMatrix(JPABaseEmitter* e, Mtx34* m) {
    JPASetRMtxTVecfromMtx(m, gabi::ea(e) + 0x1F0, gabi::ea(e) + 0x22C);
}
/* JPABaseEmitter::becomeInvalidEmitter (HD inline) */
static inline void JPABaseEmitter_becomeInvalidEmitter(JPABaseEmitter* em) {
    u32 e = gabi::ea(em);
    u32 f = gabi::load<u32>(e + 0x254);
    gabi::store<s32>(e + 0x5C, -1);
    gabi::store<u32>(e + 0x254, f | 1);
}
/* 025BEBB8 HD: dSnap_RegistFig(type, actor, const cXyz& pos, s16 angleY, f32, f32, f32) */
static inline void dSnap_RegistFig_hd(s32 type, fopAc_ac_c* a, cXyz* pos, s16 ang, f32 x, f32 y, f32 z) {
    gabi::call(0x025BEBB8, type, a, pos, ang, x, y, z);
}

/* daDr_HIO_c (HD: vtable after the members) */
struct daDr_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> mScale;
    /* 0x08 */ be<u8> m0C;
    /* 0x09 */ u8 _09;
    /* 0x0A */ be<s16> m0E;
    /* 0x0C */ be<f32> mWait1Morf;
    /* 0x10 */ be<f32> mAkubi1Morf;
    /* 0x14 */ be<f32> mBiku1Morf;
    /* 0x18 */ be<f32> mAbare1Morf;
    /* 0x1C */ be<f32> mAbare2Morf;
    /* 0x20 */ be<f32> mHo1Morf;
    /* 0x24 */ be<u32> __vtbl;
};
WWHD_SIZE(daDr_HIO_c, 0x28);
static daDr_HIO_c& l_HIO() { return *gabi::at<daDr_HIO_c>(0x10463BFC); }

struct dr_class : fopAc_ac_c {
    /* 0x3AC */ u8 m290[0x3C8 - 0x3AC];
    /* 0x3C8 */ request_of_phase_process_class mPhs;
    /* 0x3D0 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3D4 */ be<u8> mMode;
    /* 0x3D5 */ u8 m2B9;
    /* 0x3D6 */ be<s16> mCountDownTimers[3];
    /* 0x3DC */ be<s32> mCurrBckIdx;
    /* 0x3E0 */ gptr<JPABaseEmitter> mpBreathEmitter;
    /* 0x3E4 */ be<s8> m2C8;
    /* 0x3E5 */ be<s8> m2C9;
    /* 0x3E6 */ u8 m2CA[2];
};
WWHD_OFFSET(dr_class, mPhs, 0x3C8);
WWHD_OFFSET(dr_class, mpBreathEmitter, 0x3E0);
WWHD_SIZE(dr_class, 0x3E8);

/* 0212A92C daDr_HIO_c::daDr_HIO_c (HD: allocates when this == NULL) */
static daDr_HIO_c* daDr_HIO_c_ct(daDr_HIO_c* i_this) {
    WWHD_FUNC(0x0212A92C, daDr_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daDr_HIO_c*)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = DR_HIO_VTBL;
    i_this->mNo = -1;
    i_this->mScale = 1.0f;
    i_this->m0C = false;
    i_this->m0E = 10 * 30;
    i_this->mWait1Morf = 10.0f;
    i_this->mAkubi1Morf = 10.0f;
    i_this->mBiku1Morf = 2.0f;
    i_this->mAbare1Morf = 5.0f;
    i_this->mAbare2Morf = 5.0f;
    i_this->mHo1Morf = 10.0f;
    return i_this;
}
VERIFY(0x0212A92C, daDr_HIO_c_ct);

/* 02129D28 */
static BOOL daDr_Draw(dr_class* i_this) {
    WWHD_FUNC(0x02129D28, BOOL, i_this);
    J3DModel* model = i_this->mpMorf->getModel();
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    setLightTevColorType(dKy_getEnvlight(), model, &i_this->tevStr);
    i_this->mpMorf->entryDL();
    dSnap_RegistFig_hd(DSNAP_TYPE_DR, i_this, &i_this->eyePos, i_this->shape_angle.y, 1.0f, 1.0f, 1.0f);
    return TRUE;
}
VERIFY(0x02129D28, daDr_Draw);

/* 02129DB4 */
static void anm_init(dr_class* i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx) {
    WWHD_FUNC(0x02129DB4, void, i_this, bckFileIdx, morf, loopMode, speed, soundFileIdx);
    if (i_this->mCurrBckIdx == bckFileIdx) {
        morf = 0.0f;
    }
    if (soundFileIdx >= 0) {
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1000E090) /* "Dr" */, bckFileIdx, SAFESTRING_VTBL);
        void* bas = dComIfG_getObjectRes(STR(0x1000E090), soundFileIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(bck, loopMode, morf, speed, 0.0f, -1.0f, bas);
    } else {
        J3DAnmTransform* bck = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1000E090), bckFileIdx, SAFESTRING_VTBL);
        i_this->mpMorf->setAnm(bck, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
    i_this->mCurrBckIdx = bckFileIdx;
}
VERIFY(0x02129DB4, anm_init);

static inline void breath_invalidate(dr_class* i_this) {
    if (i_this->mpBreathEmitter) {
        JPABaseEmitter_becomeInvalidEmitter(i_this->mpBreathEmitter);
        i_this->mpBreathEmitter = nullptr;
    }
}

/* move (inlined into daDr_Execute in HD) */
static inline void move(dr_class* i_this) {
    bool isIdle = false;
    switch (i_this->mMode) {
    case 0:
        isIdle = true;
        anm_init(i_this, dRes_INDEX_DR_BCK_DR_WAIT1_e, l_HIO().mWait1Morf, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        i_this->mMode = i_this->mMode + 1;
        i_this->mCountDownTimers[0] = (s16)gabi::ftoi(cM_rndF(200.0f) + 200.0f);
        break;
    case 1:
        isIdle = true;
        if (i_this->mCountDownTimers[0] == 0) {
            anm_init(i_this, dRes_INDEX_DR_BCK_DR_AKUBI1_e, l_HIO().mAkubi1Morf, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_DR_BAS_AKUBI1_e);
            i_this->mMode = i_this->mMode + 1;
        }
        break;
    case 2:
        isIdle = true;
        if (i_this->mpMorf->isStop()) {
            i_this->mMode = 0;
        }
        break;
    case 10:
        anm_init(i_this, dRes_INDEX_DR_BCK_DR_BIKU1_e, l_HIO().mBiku1Morf, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_DR_BAS_BIKU1_e);
        i_this->mMode = i_this->mMode + 1;
        i_this->mCountDownTimers[0] = l_HIO().m0E;
        i_this->mpBreathEmitter = dComIfGp_particle_set(dPa_name_ID_AK_SN_DRPAINBIKU00, &i_this->current.pos);
        i_this->m2C9 = 0;
        // Fall-through
    case 11:
        if (i_this->m2C9 == 0 && gabi::ftoi(i_this->mpMorf->getFrame()) == 15) {
            i_this->mCountDownTimers[1] = 5;
        }

        if (i_this->mpMorf->isStop()) {
            i_this->m2C9 = 1;
            breath_invalidate(i_this);

            if (i_this->mCountDownTimers[0] != 0) {
                if (cM_rndF(1.0f) < 0.5f) {
                    anm_init(i_this, dRes_INDEX_DR_BCK_DR_ABARE1_e, l_HIO().mAbare1Morf, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_DR_BAS_ABARE1_e);
                    i_this->mpBreathEmitter = dComIfGp_particle_set(dPa_name_ID_AK_SN_DRPAINABARE00, &i_this->current.pos);
                    i_this->mCountDownTimers[1] = 500;
                } else {
                    anm_init(i_this, dRes_INDEX_DR_BCK_DR_ABARE2_e, l_HIO().mAbare2Morf, J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_DR_BAS_ABARE2_e);

                    gabi::Local<cXyz> rootPos;
                    gabi::Local<cXyz> offset;
                    offset->x = 0.0f;
                    offset->y = 0.0f;
                    offset->z = 0.0f;
                    Mtx34* rootJntMtx = J3DModel_getAnmMtx(i_this->mpMorf->getModel(), DR1_JNT_DR_ALL_ROOT_e);
                    PSMTXCopy(rootJntMtx, calc_mtx());
                    MtxPosition(offset.get(), rootPos.get());
                    dComIfGp_particle_set(dPa_name_ID_AK_SN_DRSPLASHMAGMA00, rootPos.get());

                    fopAcM_seStart(i_this, JA_SE_CM_DRG_MTOP_MAGMA, 0);
                    i_this->mCountDownTimers[1] = 0;
                }
            } else {
                anm_init(i_this, dRes_INDEX_DR_BCK_DR_HO1_e, l_HIO().mHo1Morf, J3DFrameCtrl::EMode_NONE, 1.0f, -1);
                i_this->mpBreathEmitter = dComIfGp_particle_set(dPa_name_ID_AK_SN_DRPAINHO00, &i_this->current.pos);
                i_this->mMode = i_this->mMode + 1;
            }
        }

        if (i_this->mpBreathEmitter) {
            JPABaseEmitter* em = i_this->mpBreathEmitter;
            Mtx34* tongueJntMtx = J3DModel_getAnmMtx(i_this->mpMorf->getModel(), DR1_JNT_J_DR_SITA2_e);
            JPABaseEmitter_setGlobalRTMatrix(em, tongueJntMtx);
        }
        break;
    case 12:
        if (i_this->mpBreathEmitter) {
            JPABaseEmitter* em = i_this->mpBreathEmitter;
            Mtx34* tongueJntMtx = J3DModel_getAnmMtx(i_this->mpMorf->getModel(), DR1_JNT_J_DR_SITA2_e);
            JPABaseEmitter_setGlobalRTMatrix(em, tongueJntMtx);
        }

        if (gabi::ftoi(i_this->mpMorf->getFrame()) == 34) {
            i_this->mCountDownTimers[1] = 5;
        }

        if (i_this->mpMorf->isStop()) {
            i_this->mMode = 0;
            breath_invalidate(i_this);
        }
        break;
    }

    if (!dComIfGs_isStageBossEnemy(3 /* dSv_save_c::STAGE_DRC */)) {
        if ((isIdle && (l_HIO().m0C || dComIfGp_getVibration_CheckQuake())) || i_this->m2C8 != 0) {
            l_HIO().m0C = false;
            i_this->m2C8 = 0;
            i_this->mMode = 10;
        }
    }

    i_this->mpMorf->play(&i_this->current.pos, 0, 0);

    if (i_this->mCountDownTimers[1] != 0) {
        fopAcM_seStart(i_this, JA_SE_CM_DRG_MTOP_FIRE, 0);
    }
}

/* 02129F0C */
static void daDr_setMtx(dr_class* i_this) {
    WWHD_FUNC(0x02129F0C, void, i_this);
    J3DModel* model = i_this->mpMorf->getModel();
    J3DModel_setBaseScale(model, &i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);
    mDoMtx_XrotM(mDoMtx_stack_c::get(), i_this->current.angle.x);
    mDoMtx_ZrotM(mDoMtx_stack_c::get(), i_this->current.angle.z);
    J3DModel_setBaseTRMtx(model, mDoMtx_stack_c::get());

    i_this->mpMorf->calc();
}
VERIFY(0x02129F0C, daDr_setMtx);

/* 0212A00C */
static BOOL daDr_Execute(dr_class* i_this) {
    WWHD_FUNC(0x0212A00C, BOOL, i_this);
    for (int i = 0; i < 3; i++) {
        if (i_this->mCountDownTimers[i] != 0) {
            i_this->mCountDownTimers[i] = (s16)(i_this->mCountDownTimers[i] - 1);
        }
    }

    move(i_this);

    f32 s = l_HIO().mScale;
    i_this->scale.z = s;
    i_this->scale.y = s;
    i_this->scale.x = s;

    daDr_setMtx(i_this);

    Mtx34* tongueJntMtx = J3DModel_getAnmMtx(i_this->mpMorf->getModel(), DR1_JNT_J_DR_SITA2_e);
    PSMTXCopy(tongueJntMtx, calc_mtx());
    gabi::Local<cXyz> offset;
    offset->x = 0.0f;
    offset->y = 0.0f;
    offset->z = 0.0f;
    MtxPosition(offset.get(), &i_this->eyePos);

    return TRUE;
}
VERIFY(0x0212A00C, daDr_Execute);

/* 0212A6DC */
static BOOL daDr_IsDelete(dr_class*) {
    WWHD_FUNC(0x0212A6DC, BOOL, (u32)0);
    return TRUE;
}
VERIFY(0x0212A6DC, daDr_IsDelete);

/* 0212A6E4 */
static BOOL daDr_Delete(dr_class* i_this) {
    WWHD_FUNC(0x0212A6E4, BOOL, i_this);
    dComIfG_resDelete(&i_this->mPhs, STR(0x1000E09C) /* "Dr" */);
    if (l_HIO().mNo >= 0) {
        mDoHIO_deleteChild(l_HIO().mNo);
    }
    return TRUE;
}
VERIFY(0x0212A6E4, daDr_Delete);

/* 0212A728 */
static BOOL createHeap(fopAc_ac_c* i_actor) {
    WWHD_FUNC(0x0212A728, BOOL, i_actor);
    dr_class* i_this = (dr_class*)i_actor;
    J3DModelData* data = (J3DModelData*)dComIfG_getObjectRes(STR(0x1000E09F) /* "Dr" */, dRes_INDEX_DR_BMD_DR1_e, SAFESTRING_VTBL);
    J3DAnmTransform* anm = (J3DAnmTransform*)dComIfG_getObjectRes(STR(0x1000E09F), dRes_INDEX_DR_BCK_DR_BIKU1_e, SAFESTRING_VTBL);
    void* bas = dComIfG_getObjectRes(STR(0x1000E09F), dRes_INDEX_DR_BAS_BIKU1_e, SAFESTRING_VTBL);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, 1, bas,
                                            0x00000000, 0x11020203);
    mDoExt_McaMorf* morf = i_this->mpMorf;
    if (!morf || !morf->getModel()) {
        return FALSE;
    }
    return TRUE;
}
VERIFY(0x0212A728, createHeap);

/* 0212A830 */
static cPhs_State daDr_Create(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x0212A830, cPhs_State, i_this);
    dr_class* a_this = (dr_class*)i_this;
    /* fopAcM_ct(a_this, dr_class) */
    if (!fopAcM_CheckCondition(a_this, fopAcCnd_INIT_e)) {
        if (a_this != nullptr) {
            fopAc_ac_c_ct(a_this);
            a_this->__vtbl = DR_VTBL;
        }
        fopAcM_OnCondition(a_this, fopAcCnd_INIT_e);
    }

    cPhs_State phase_state = dComIfG_resLoad(&a_this->mPhs, STR(0x1000E0A2) /* "Dr" */);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(a_this, 0x0212A728 /* createHeap */, 0xF000)) {
            return cPhs_ERROR_e;
        }

        daDr_setMtx(a_this);

        if (l_HIO().mNo < 0) {
            l_HIO().mNo = mDoHIO_createChild(STR(0x1000E0A8) /* "ドラゴン" */, &l_HIO());
        }
    }

    return phase_state;
}
VERIFY(0x0212A830, daDr_Create);

/* 0212A9C0 */
static void __sinit_d_a_dr_cpp() {
    WWHD_FUNC(0x0212A9C0, void, (u32)0);
    sinit_header_statics_z(0x10463BF0, 0x101B4724, 0x10463C24);
    daDr_HIO_c_ct(&l_HIO()); /* static daDr_HIO_c l_HIO */
}
VERIFY(0x0212A9C0, __sinit_d_a_dr_cpp);

/* 0212AA60: deleting destructor of a class with a trivial destructor (daDr_HIO_c) */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x0212AA60, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x0212AA60, trivial_dt);

/* 0212AA74: dr_class deleting destructor (HD) */
static void dr_class_dt(dr_class* p, s32 flags) {
    WWHD_FUNC(0x0212AA74, void, p, flags);
    if (p != nullptr) {
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(p);
    }
}
VERIFY(0x0212AA74, dr_class_dt);

/* 0212AAC8: empty virtual */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x0212AAC8, void, p);
}
VERIFY(0x0212AAC8, empty_virtual);
