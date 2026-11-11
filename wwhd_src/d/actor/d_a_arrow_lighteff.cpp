/** Elemental arrow glow. Tww names, WWHD layouts and verified HD behavior. */
#include "d/actor/d_a_arrow_lighteff.h"
const gc_tu_data gc_tu = {.safeString = 0x10007598,
                          .headerStatics = {0x104614B4, 0x1018FF3C, 0x104614C0}};
static const char l_arcname[] = "Link";
static ArrowLightGraphics l_graphics;
static daArrow_Lighteff_c* daArrow_Lighteff_c_ct(daArrow_Lighteff_c* self);
static const s16 light_color[4][3] = {{0, 0, 0}, {600, 400, 120}, {120, 400, 600}, {600, 600, 200}};
// HD: this inline sound path has no nullable-actor guard.
static void arrowLight_seStartCurrent(fopAc_ac_c* actor, u32 id, u32 param) {
    mDoAud_seStart(id, &actor->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(actor)));
}
/* 020570BC */
void daArrow_Lighteff_c::setTopPos() {
    ArrowLightArrow* arrow = static_cast<ArrowLightArrow*>(fopAcM_SearchByID(parentActorID));
    if (arrow) {
        PSMTXCopy(&arrow->mModelMtx, mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(0.0f, 0.0f, 62.0f);
        /* mDoMtx_stack_c::multVecZero(&mTopPos) */
        Mtx34* m = mDoMtx_stack_c::get();
        mTopPos.x = m->m[0][3];
        mTopPos.y = m->m[1][3];
        mTopPos.z = m->m[2][3];
    }
}

/* 0205777C */
void daArrow_Lighteff_c::setPointLight() {
    if ((mArrowParam == 0 || mArrowParam == 1) && mArrowType != ArrowLightArrow::TYPE_NORMAL) {
        cLib_addCalc2(&mLightPower, cM_rndF(0.2f) + 1.0f, 0.5f, 0.05f);
    } else {
        mLightPower = 0.0f;
    }

    /* light_color (0x1000763C): {r, g, b} s16 per type */
    const be<s16>* color = light_color[mArrowType];
    mLight.mPos.copy(mTopPos);
    mLight.mColorR = color[0];
    mLight.mColorG = color[1];
    mLight.mColorB = color[2];
    mLight.mPower = (f32)(s16)gabi::ftoi(mLightPower * 150.0f);
    mLight.mFluctuation = 250.0f;
}

/* 02057510 */
void daArrow_Lighteff_c::delete_particle() {
    if (mParticleCb0.getEmitter()) {
        if (mArrowType == ArrowLightArrow::TYPE_LIGHT) {
            reinterpret_cast<ArrowLightEmitter*>(mParticleCb0.getEmitter())->setGlobalAlpha(0);
        }
        mParticleCb0.remove();
    }
    if (mParticleCb1.getEmitter()) {
        if (mArrowType == ArrowLightArrow::TYPE_LIGHT) {
            reinterpret_cast<ArrowLightEmitter*>(mParticleCb1.getEmitter())->setGlobalAlpha(0);
        }
        mParticleCb1.remove();
    }
}

/* 02056E24 */
static BOOL CheckCreateHeap(fopAc_ac_c* i_this) {
    return static_cast<daArrow_Lighteff_c*>(i_this)->CreateHeap();
}

/* 02056B54 */
BOOL daArrow_Lighteff_c::CreateHeap() {
    u8 type = (u8)fopAcM_GetParam(this);
    mArrowType = type;

    J3DModelData* modelData;
    if (type == ArrowLightArrow::TYPE_FIRE) {
        modelData = (J3DModelData*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_LINK_BDL_GARWFI00_e);
    } else if (type == ArrowLightArrow::TYPE_ICE) {
        modelData = (J3DModelData*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_LINK_BDL_GARWFI01_e);
    } else {
        modelData = (J3DModelData*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_LINK_BDL_GARWG00_e);
    }
    if (modelData == nullptr) /* JUT_ASSERT(188, modelData != NULL) */
        JUT_ASSERT_fail("d_a_arrow_lighteff.cpp", 0xBC, "modelData != (0)");

    mpModel = mDoExt_J3DModel__create(modelData, 0x80000, 0x3F441422);
    if (mpModel == nullptr) {
        return FALSE;
    }

    J3DAnmTextureSRTKey* btk;
    J3DAnmTevRegKey* brk;
    if (mArrowType == ArrowLightArrow::TYPE_FIRE) {
        btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_LINK_BTK_GARWFI00_e);
        brk = static_cast<J3DAnmTevRegKey*>(
            dComIfG_getObjectRes(l_arcname, dRes_INDEX_LINK_BRK_GARWFI00_e));
    } else if (mArrowType == ArrowLightArrow::TYPE_ICE) {
        btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_LINK_BTK_GARWFI01_e);
        brk = static_cast<J3DAnmTevRegKey*>(
            dComIfG_getObjectRes(l_arcname, dRes_INDEX_LINK_BRK_GARWFI01_e));
    } else {
        btk = (J3DAnmTextureSRTKey*)dComIfG_getObjectRes(l_arcname, dRes_INDEX_LINK_BTK_GARWG00_e);
        brk = static_cast<J3DAnmTevRegKey*>(
            dComIfG_getObjectRes(l_arcname, dRes_INDEX_LINK_BRK_GARWG00_e));
    }
    if (btk == nullptr) /* JUT_ASSERT(217, btk != NULL) */
        JUT_ASSERT_fail("d_a_arrow_lighteff.cpp", 0xD9, "btk != 0");
    if (brk == nullptr) /* JUT_ASSERT(218, brk != NULL) */
        JUT_ASSERT_fail("d_a_arrow_lighteff.cpp", 0xDA, "brk != 0");

    if (!mBtk.init(modelData, btk, true, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }
    if (!mBrk.init(modelData, brk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0)) {
        return FALSE;
    }

    mBrkFrame = 0.0f;
    return TRUE;
}

/* 0205715C */
void daArrow_Lighteff_c::CreateInit() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel));
    mScale.z = 1.0f;
    mScale.y = 1.0f;
    mScale.x = 1.0f;
    fopAcM_setCullSizeBox(this, -100.0f, -100.0f, -100.0f, 100.0f, 100.0f, 100.0f);
    cullSizeFar = 1.0f;
    /* HD: the scale comes from the particle manager when there is one */
    dPa_control_c* pa = dComIfGp_getParticle();
    if (pa != nullptr) {
        f32 s = reinterpret_cast<ArrowLightParticles*>(pa)->mGlobalScale;
        mScale.x = s;
        mScale.z = s;
        mScale.y = s;
    }

    set_mtx();

    mArrowParam = 0;
    mEndTimer = 0;
    mPrevArrowParam = 0;

    setTopPos();

    if (mArrowType == ArrowLightArrow::TYPE_FIRE) {
        if (mParticleCb0.getEmitter() == nullptr) {
            dComIfGp_particle_setP1(
                arrow_light_particle::ID_IT_JN_ARWF_HINOKO00, &mTopPos, &current.angle, nullptr,
                0xFF, (dPa_levelEcallBack*)&mParticleCb0, -1, nullptr, nullptr, nullptr);
        }
    } else if (mArrowType == ArrowLightArrow::TYPE_ICE) {
        if (mParticleCb0.getEmitter() == nullptr) {
            dComIfGp_particle_setP1(
                arrow_light_particle::ID_IT_JN_ARWI_KIRAKIRA00, &mTopPos, &current.angle,
                nullptr, 0xFF, (dPa_levelEcallBack*)&mParticleCb0, -1, nullptr, nullptr, nullptr);
        }
        if (mParticleCb1.getEmitter() == nullptr) {
            dComIfGp_particle_setP1(
                arrow_light_particle::ID_IT_JN_ARWI_REIKI00, &mTopPos, &current.angle, nullptr,
                0xFF, (dPa_levelEcallBack*)&mParticleCb1, -1, nullptr, nullptr, nullptr);
        }
    } else if (mArrowType == ArrowLightArrow::TYPE_LIGHT) {
        if (mParticleCb0.getEmitter() == nullptr) {
            dComIfGp_particle_setP1(
                arrow_light_particle::ID_IT_JN_ARWG_TSUBU00, &mTopPos, &current.angle, nullptr,
                0xFF, (dPa_levelEcallBack*)&mParticleCb0, -1, nullptr, nullptr, nullptr);
        }
        if (mParticleCb1.getEmitter() == nullptr) {
            dComIfGp_particle_setP1(
                arrow_light_particle::ID_IT_JN_ARWG_FLASH00, &mTopPos, &current.angle, nullptr,
                0xFF, (dPa_levelEcallBack*)&mParticleCb1, -1, nullptr, nullptr, nullptr);
        }
    }

    /* HD: the emitters take the model scale */
    f32 sx = mScale.x;
    f32 sy = mScale.y;
    f32 sz = mScale.z;
    if (mParticleCb0.getEmitter()) {
        reinterpret_cast<ArrowLightEmitter*>(mParticleCb0.getEmitter())->setGlobalScale(sx, sy, sz);
    }
    if (mParticleCb1.getEmitter()) {
        reinterpret_cast<ArrowLightEmitter*>(mParticleCb1.getEmitter())->setGlobalScale(sx, sy, sz);
    }

    mDraw = 1;
    dKy_plight_set(&mLight);
}

/* 02056EB8 */
void daArrow_Lighteff_c::set_mtx() {
    J3DModel_setBaseScale(mpModel, &mScale);

    ArrowLightArrow* arrow = static_cast<ArrowLightArrow*>(fopAcM_SearchByID(parentActorID));
    if (arrow) {
        PSMTXCopy(&arrow->mModelMtx, mDoMtx_stack_c::get());
        mDoMtx_stack_c::transM(0.0f, 0.0f, 62.0f);
        J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    } else {
        mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), current.angle.x, current.angle.y, 0);
        mDoMtx_stack_c::transM(0.0f, 0.0f, 62.0f);
        J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    }
}

/* 02057478 */
cPhs_State daArrow_Lighteff_c::_create() {
    /* fopAcM_ct(this, daArrow_Lighteff_c) */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (gabi::ea(this) != 0) {
            daArrow_Lighteff_c_ct(this);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }

    mUseArrowEffect = 0;
    if (!fopAcM_entrySolidHeap(this, gabi::guest_fn((void*)CheckCreateHeap), 0x2660)) {
        return cPhs_ERROR_e;
    }

    CreateInit();
    return cPhs_COMPLEATE_e;
}

/* 020575A4 */
bool daArrow_Lighteff_c::_delete() {
    delete_particle();

    if (mUseArrowEffect != 0) { /* HD: any nonzero flag, unlike GC's ==1. */
        ArrowLightPlayer* link = static_cast<ArrowLightPlayer*>(dComIfGp_getPlayer(0));
        ArrowLightArrow* arrow = static_cast<ArrowLightArrow*>(fopAcM_SearchByID(parentActorID));
        if (arrow) {
            if (!arrow->isSetByZelda()) {
                link->offUseArrowEffect();
            }
        } else {
            link->offUseArrowEffect();
        }
    }

    dKy_plight_cut(&mLight);
    return true;
}

// HD: uses the demo draw structure, gated by mDraw.
/* 0205763C */
bool daArrow_Lighteff_c::_draw() {
    if (mDraw != 0) {
        J3DModelData* modelData = J3DModel_getModelData(mpModel);
        bool monotone = l_graphics.mMonotone != 0;
        if (monotone)
            dComIfGd_setListP1();
        else
            dComIfGd_setListMaskOff();

        mBtk.entry(modelData, mBtk.getFrame());
        mBrk.entry(modelData, mBrk.getFrame());
        mDoExt_modelUpdateDL(mpModel);
        /* mBrk.remove(modelData); mBtk.remove(modelData) */
        reinterpret_cast<ArrowLightModelData*>(modelData)->mBrk = nullptr;
        reinterpret_cast<ArrowLightModelData*>(modelData)->mBtk = nullptr;

        dComIfGd_setList();
    }
    return true;
}

/* inlined into daArrow_Lighteff_c::_execute */
void daArrow_Lighteff_c::brk_play() {
    mBtk.play();

    s32 cam = dComIfGp_getPlayerCameraID(0);
    if (!dComIfGp_checkCameraAttentionStatus(cam, dCamAttnStts_00000020_e)) {
        if (mBrkFrame < mBrk.getEndFrame()) {
            mBrkFrame = mBrkFrame + 1.0f;
            if (mBrkFrame > mBrk.getEndFrame()) {
                mBrkFrame = mBrk.getEndFrame();
            }
        }
    } else {
        if (mBrkFrame > 5.0f) {
            mBrkFrame = mBrkFrame - 1.0f;
            if (mBrkFrame < 5.0f) {
                mBrkFrame = 5.0f;
            }
        } else if (mBrkFrame < 5.0f) {
            mBrkFrame = mBrkFrame + 1.0f;
            if (mBrkFrame > 5.0f) {
                mBrkFrame = 5.0f;
            }
        }
    }

    mBrk.setFrame(mBrkFrame);
}

/* 02057930 */
bool daArrow_Lighteff_c::_execute() {
    mPrevArrowParam = (u32)mArrowParam;

    brk_play();

    ArrowLightPlayer* link = static_cast<ArrowLightPlayer*>(dComIfGp_getPlayer(0));
    ArrowLightArrow* arrow = static_cast<ArrowLightArrow*>(fopAcM_SearchByID(parentActorID));
    if (arrow) {
        u32 param = fopAcM_GetParam(arrow);
        mArrowParam = param;
        if (arrow->mFlewFar != 0) { /* HD: any nonzero flag, unlike GC's ==1. */
            fopAcM_delete(this);
        } else if (param != 0) {
            if (param == 1) {
                if (mPrevArrowParam != 1) {
                    if (!arrow->isSetByZelda()) {
                        link->onUseArrowEffect();
                    }
                    mScale.z = 1.0f;
                    mUseArrowEffect = 1;
                } else {
                    f32 z = (f32)((f64)mScale.z + 1.0);
                    // HD: the unordered result clamps, as the original blt/bgt branches do.
                    if (!(z < 20.0f)) {
                        z = 20.0f;
                    }
                    f32 x =
                        mScale.x - 0.025f; /* HD: single-precision subtraction, unlike GC. */
                    mScale.z = z;
                    if (!(x > 0.5f)) {
                        x = 0.5f;
                    }
                    mScale.x = x;
                    mScale.y = x;
                }
            } else if (param == 3) {
                fopAcM_delete(this);
            } else {
                mDraw = 0;
                delete_particle();
                mEndTimer = mEndTimer + 1;
                if (0x3C <= mEndTimer) {
                    fopAcM_delete(this);
                }
            }
        }

        current.pos.copy(arrow->current.pos);
        csXyz* ang = &arrow->mHitAngle;
        current.angle.x = ang->x;
        current.angle.y = ang->y;
        current.angle.z = ang->z;
        setTopPos();
        setPointLight();
    } else {
        fopAcM_delete(this);
    }

    s32 cam = dComIfGp_getPlayerCameraID(0);
    if (mArrowType == ArrowLightArrow::TYPE_FIRE) {
        arrowLight_seStartCurrent(this, JA_SE_OBJ_FIRE_ARROW_AMB, 0);
        if (!dComIfGp_checkCameraAttentionStatus(cam, dCamAttnStts_00000020_e)) {
            dComIfGp_particle_setSimple(arrow_light_particle::ID_AK_JP_O_KAGEROU00, &mTopPos);
        }
    } else if (mArrowType == ArrowLightArrow::TYPE_ICE) {
        arrowLight_seStartCurrent(this, JA_SE_OBJ_ICE_ARROW_AMB, 0);
        if (dComIfGp_checkCameraAttentionStatus(cam, dCamAttnStts_00000020_e)) {
            if (mParticleCb0.getEmitter()) {
                reinterpret_cast<ArrowLightEmitter*>(mParticleCb0.getEmitter())
                    ->setGlobalAlpha(0x64);
            }
            if (mParticleCb1.getEmitter()) {
                reinterpret_cast<ArrowLightEmitter*>(mParticleCb1.getEmitter())
                    ->setGlobalAlpha(0x64);
            }
        } else {
            if (mParticleCb0.getEmitter()) {
                reinterpret_cast<ArrowLightEmitter*>(mParticleCb0.getEmitter())
                    ->setGlobalAlpha(0xFF);
            }
            if (mParticleCb1.getEmitter()) {
                reinterpret_cast<ArrowLightEmitter*>(mParticleCb1.getEmitter())
                    ->setGlobalAlpha(0xFF);
            }
        }
    } else {
        arrowLight_seStartCurrent(this, JA_SE_OBJ_LIGHT_ARROW_AMB, 0);
    }

    set_mtx();
    return true;
}

/* 02057EF0 */
static BOOL daArrow_Lighteff_IsDelete(void*) { return TRUE; }

// HD: compiler-generated construction, initialization and deleting destructors.
/* 02056E28 */
static daArrow_Lighteff_c* daArrow_Lighteff_c_ct(daArrow_Lighteff_c* self) {
    if (self == nullptr) {
        self = (daArrow_Lighteff_c*)operator_new(0x520);
        if (self == nullptr)
            return self;
    }
    fopAc_ac_c_ct(self);
    self->__vtbl = 0x100075B0;
    mDoExt_brkAnm::ct(&self->mBrk);
    mDoExt_btkAnm::ct(&self->mBtk);
    dPa_followEcallBack_ct(&self->mParticleCb0, 0, 0);
    dPa_followEcallBack_ct(&self->mParticleCb1, 0, 0);
    self->mLight.mHD20 = 1.0f; /* LIGHT_INFLUENCE (HD) */
    return self;
}

/* 02057E48 */
static void __sinit_d_a_arrow_lighteff_cpp() { __sinit_header_statics(); }

/* 02057EDC */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02057EF8 */
static void daArrow_Lighteff_c_dt(daArrow_Lighteff_c* self, s32 flags) {
    if (self != nullptr) {
        arrowLight_actor_dt(self, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(self);
    }
}

/* 02057F4C */
static void empty_virtual(void* p) {}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py): per-TU data and HD base destructor
str 100075C8 "Link"
str 100075D0 "d_a_arrow_lighteff.cpp"
str 100075E8 "modelData != (0)"
str 100075FC "btk != 0"
str 10007608 "brk != 0"
data light_color 1000763C
data l_graphics 101F4829
ext 025D50BC void arrowLight_actor_dt(fopAc_ac_c* self,s32 flags)
*/
// clang-format on
