/** NPC - Valoo (Overworld) / Dragon.
 * Ported from tww to the WWHD layout; verified against cking.rpx. */
#include "d/actor/d_a_dr.h"
const gc_tu_data gc_tu = {
    .safeString = 0x1000E054,
    .headerStatics = {0x10463BF0, 0x101B4724, 0x10463C24},
};
static daDr_HIO_c l_HIO;
enum { STAGE_DRC = 3 };

/* inlined into daDr_Execute */
static void Dr_setGlobalRTMatrix(DrEmitterHD *emitter, Mtx34 *mtx) {
    JPASetRMtxTVecfromMtx(mtx, reinterpret_cast<Mtx34*>(emitter->mGlobalRotation), &emitter->mGlobalTranslation);
}
/* inlined into daDr_Execute */
static void Dr_invalidateEmitter(DrEmitterHD *emitter) {
    u32 flags = emitter->mFlags;
    emitter->mMaxFrame = -1;
    emitter->mFlags = flags | JPAEmtrStts_StopEmit;
}

/* 0212A92C */
// HD: the GHS constructor allocates when passed a null this pointer.
static daDr_HIO_c *daDr_HIO_c_ct(daDr_HIO_c *i_this) {
    if (i_this == nullptr) {
        i_this = (daDr_HIO_c *)operator_new(0x28);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x1000E07C;
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

/* 02129D28 */
static BOOL daDr_Draw(dr_class *i_this) {
    J3DModel *model = i_this->mpMorf->getModel();
    g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &i_this->current.pos, &i_this->tevStr);
    g_env_light.setLightTevColorType(model, &i_this->tevStr);
    i_this->mpMorf->entryDL();
    dSnap_RegistFig(DSNAP_TYPE_DR, i_this, &i_this->eyePos, i_this->shape_angle.y, 1.0f, 1.0f, 1.0f);
    return TRUE;
}

/* 02129DB4 */
static void anm_init(dr_class *i_this, int bckFileIdx, f32 morf, u8 loopMode, f32 speed, int soundFileIdx) {
    if (i_this->mCurrBckIdx == bckFileIdx) {
        morf = 0.0f;
    }
    if (soundFileIdx >= 0) {
        J3DAnmTransform *bck = (J3DAnmTransform *)dComIfG_getObjectRes("Dr" /* "Dr" */, bckFileIdx);
        void *bas = dComIfG_getObjectRes("Dr", soundFileIdx);
        i_this->mpMorf->setAnm(bck, loopMode, morf, speed, 0.0f, -1.0f, bas);
    } else {
        J3DAnmTransform *bck = (J3DAnmTransform *)dComIfG_getObjectRes("Dr", bckFileIdx);
        i_this->mpMorf->setAnm(bck, loopMode, morf, speed, 0.0f, -1.0f, nullptr);
    }
    i_this->mCurrBckIdx = bckFileIdx;
}

static inline void breath_invalidate(dr_class *i_this) {
    if (i_this->mpBreathEmitter) {
        Dr_invalidateEmitter(static_cast<DrEmitterHD *>((JPABaseEmitter *)i_this->mpBreathEmitter));
        i_this->mpBreathEmitter = nullptr;
    }
}

/* inlined into daDr_Execute */
// HD: move has no standalone entry point.
static inline void move(dr_class *i_this) {
    bool isIdle = false;
    switch (i_this->mMode) {
    case 0:
        isIdle = true;
        anm_init(i_this, dRes_INDEX_DR_BCK_DR_WAIT1_e, l_HIO.mWait1Morf, J3DFrameCtrl::EMode_LOOP, 1.0f, -1);
        i_this->mMode = i_this->mMode + 1;
        i_this->mCountDownTimers[0] = (s16)gabi::ftoi(cM_rndF(200.0f) + 200.0f);
        break;
    case 1:
        isIdle = true;
        if (i_this->mCountDownTimers[0] == 0) {
            anm_init(i_this, dRes_INDEX_DR_BCK_DR_AKUBI1_e, l_HIO.mAkubi1Morf, J3DFrameCtrl::EMode_NONE, 1.0f,
                     dRes_INDEX_DR_BAS_AKUBI1_e);
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
        anm_init(i_this, dRes_INDEX_DR_BCK_DR_BIKU1_e, l_HIO.mBiku1Morf, J3DFrameCtrl::EMode_NONE, 1.0f,
                 dRes_INDEX_DR_BAS_BIKU1_e);
        i_this->mMode = i_this->mMode + 1;
        i_this->mCountDownTimers[0] = l_HIO.m0E;
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
                    anm_init(i_this, dRes_INDEX_DR_BCK_DR_ABARE1_e, l_HIO.mAbare1Morf,
                             J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_DR_BAS_ABARE1_e);
                    i_this->mpBreathEmitter =
                        dComIfGp_particle_set(dPa_name_ID_AK_SN_DRPAINABARE00, &i_this->current.pos);
                    i_this->mCountDownTimers[1] = 500;
                } else {
                    anm_init(i_this, dRes_INDEX_DR_BCK_DR_ABARE2_e, l_HIO.mAbare2Morf,
                             J3DFrameCtrl::EMode_NONE, 1.0f, dRes_INDEX_DR_BAS_ABARE2_e);

                    cXyz rootPos;
                    cXyz offset(0.0f, 0.0f, 0.0f);
                    Mtx34 *rootJntMtx = i_this->mpMorf->getModel()->getAnmMtx(DR1_JNT_DR_ALL_ROOT_e);
                    PSMTXCopy(rootJntMtx, calc_mtx());
                    MtxPosition(&offset, &rootPos);
                    dComIfGp_particle_set(dPa_name_ID_AK_SN_DRSPLASHMAGMA00, &rootPos);

                    fopAcM_seStart(i_this, JA_SE_CM_DRG_MTOP_MAGMA, 0);
                    i_this->mCountDownTimers[1] = 0;
                }
            } else {
                anm_init(i_this, dRes_INDEX_DR_BCK_DR_HO1_e, l_HIO.mHo1Morf, J3DFrameCtrl::EMode_NONE, 1.0f,
                         -1);
                i_this->mpBreathEmitter =
                    dComIfGp_particle_set(dPa_name_ID_AK_SN_DRPAINHO00, &i_this->current.pos);
                i_this->mMode = i_this->mMode + 1;
            }
        }

        if (i_this->mpBreathEmitter) {
            JPABaseEmitter *em = i_this->mpBreathEmitter;
            Mtx34 *tongueJntMtx = i_this->mpMorf->getModel()->getAnmMtx(DR1_JNT_J_DR_SITA2_e);
            Dr_setGlobalRTMatrix(static_cast<DrEmitterHD *>(em), tongueJntMtx);
        }
        break;
    case 12:
        if (i_this->mpBreathEmitter) {
            JPABaseEmitter *em = i_this->mpBreathEmitter;
            Mtx34 *tongueJntMtx = i_this->mpMorf->getModel()->getAnmMtx(DR1_JNT_J_DR_SITA2_e);
            Dr_setGlobalRTMatrix(static_cast<DrEmitterHD *>(em), tongueJntMtx);
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

    if (!dComIfGs_isStageBossEnemy(STAGE_DRC)) {
        if ((isIdle && (l_HIO.m0C || dComIfGp_getVibration_CheckQuake())) || i_this->m2C8 != 0) {
            l_HIO.m0C = false;
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
static void daDr_setMtx(dr_class *i_this) {
    J3DModel *model = i_this->mpMorf->getModel();
    model->setBaseScale(i_this->scale);
    mDoMtx_stack_c::transS(i_this->current.pos.x, i_this->current.pos.y, i_this->current.pos.z);
    mDoMtx_stack_c::YrotM(i_this->current.angle.y);
    mDoMtx_stack_c::XrotM(i_this->current.angle.x);
    mDoMtx_stack_c::ZrotM(i_this->current.angle.z);
    model->setBaseTRMtx(mDoMtx_stack_c::get());

    i_this->mpMorf->calc();
}

/* 0212A00C */
static BOOL daDr_Execute(dr_class *i_this) {
    for (int i = 0; i < 3; i++) {
        if (i_this->mCountDownTimers[i] != 0) {
            i_this->mCountDownTimers[i] = (s16)(i_this->mCountDownTimers[i] - 1);
        }
    }

    move(i_this);

    f32 s = l_HIO.mScale;
    i_this->scale.z = s;
    i_this->scale.y = s;
    i_this->scale.x = s;

    daDr_setMtx(i_this);

    Mtx34 *tongueJntMtx = i_this->mpMorf->getModel()->getAnmMtx(DR1_JNT_J_DR_SITA2_e);
    PSMTXCopy(tongueJntMtx, calc_mtx());
    cXyz offset(0.0f, 0.0f, 0.0f);
    MtxPosition(&offset, &i_this->eyePos);

    return TRUE;
}

/* 0212A6DC */
static BOOL daDr_IsDelete(dr_class *) {
    return TRUE;
}

/* 0212A6E4 */
static BOOL daDr_Delete(dr_class *i_this) {
    dComIfG_resDelete(&i_this->mPhs, "Dr" /* "Dr" */);
    if (l_HIO.mNo >= 0) {
        mDoHIO_deleteChild(l_HIO.mNo);
    }
    return TRUE;
}

/* 0212A728 */
static BOOL createHeap(fopAc_ac_c *i_actor) {
    dr_class *i_this = (dr_class *)i_actor;
    J3DModelData *data = (J3DModelData *)dComIfG_getObjectRes("Dr" /* "Dr" */, dRes_INDEX_DR_BMD_DR1_e);
    J3DAnmTransform *anm = (J3DAnmTransform *)dComIfG_getObjectRes("Dr", dRes_INDEX_DR_BCK_DR_BIKU1_e);
    void *bas = dComIfG_getObjectRes("Dr", dRes_INDEX_DR_BAS_BIKU1_e);
    i_this->mpMorf = mDoExt_McaMorf::create(nullptr, data, nullptr, nullptr, anm, J3DFrameCtrl::EMode_NONE,
                                            1.0f, 0, -1, 1, bas, 0x00000000, 0x11020203);
    mDoExt_McaMorf *morf = i_this->mpMorf;
    if (!morf || !morf->getModel()) {
        return FALSE;
    }
    return TRUE;
}

/* 0212A830 */
static cPhs_State daDr_Create(fopAc_ac_c *i_this) {
    dr_class *a_this = (dr_class *)i_this;
    fopAcM_ct(a_this, dr_class);

    cPhs_State phase_state = dComIfG_resLoad(&a_this->mPhs, "Dr" /* "Dr" */);
    if (phase_state == cPhs_COMPLEATE_e) {
        if (!fopAcM_entrySolidHeap(a_this, createHeap, 0xF000)) {
            return cPhs_ERROR_e;
        }

        daDr_setMtx(a_this);

        if (l_HIO.mNo < 0) {
            l_HIO.mNo = mDoHIO_createChild("ドラゴン" /* "ドラゴン" */, &l_HIO);
        }
    }

    return phase_state;
}

// HD: compiler-generated header objects and deleting destructors.
/* 0212A9C0 */
static void __sinit_d_a_dr_cpp() {
    __sinit_header_statics();
    daDr_HIO_c_ct(&l_HIO); /* static daDr_HIO_c l_HIO */
}

/* 0212AA60 */
static void trivial_dt(void *p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0212AA74 */
static void dr_class_dt(dr_class *p, s32 flags) {
    if (p != nullptr) {
        __dt(p);
        if (flags & 1)
            operator_delete(p);
    }
}

/* 0212AAC8 */
static void empty_virtual(void *p) {}

/* WWHD glue
data l_HIO 10463BFC
str 1000E090 in 02129DB4 # "Dr"
str 1000E09C in 0212A6E4 # "Dr"
str 1000E09F in 0212A728 # "Dr"
str 1000E0A2 in 0212A830 # "Dr"
str 1000E0A8 "\u30c9\u30e9\u30b4\u30f3" renamed
ext 025BEBB8 void dSnap_RegistFig(s32, fopAc_ac_c*, cXyz*, s16, f32, f32, f32)
*/
