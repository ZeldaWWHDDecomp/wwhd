/**
 * d_a_ikari.cpp (WWHD)
 * Object - Forsaken Fortress - Anchors
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_ikari.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_ikari.h"

const gc_tu_data gc_tu = {
    .safeString = 0x10011C7C,
    .headerStatics = {0x104647D0, 0x101B79A4, 0x104647DC},
};

static daObjIkariHIO_c l_HIO;
const char daIkari_c::M_arcname[6] = "Ikari";

/* 0217D7F0 */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    return ((daIkari_c*)i_this)->_createHeap();
}

/* 0217D860 */
void daIkari_c::setMtx() {
    mpModel->setBaseScale(scale);

    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mDoMtx_stack_c::XrotM(current.angle.x);
    mDoMtx_stack_c::YrotM(-shape_angle.y);
    mDoMtx_stack_c::YrotM(current.angle.y);

    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 0217D744 */
BOOL daIkari_c::_createHeap() {
    // Autoglue checks the literal entries: three S_IKARI2 models, then S_IKARI3 and S_IKARI4.
    static s32 ikari_bdl[] = {3, 3, 3, 4, 5};
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(M_arcname, ikari_bdl[mModelType]);
    JUT_ASSERT(0x7e, modelData != NULL);
    mpModel = mDoExt_J3DModel__create(modelData, 0x00080000, 0x11000022);
    if (!mpModel)
        return false;
    return true;
}

/* 0217D7F4 */
void daIkari_c::getArg() {
    u32 param = fopAcM_GetParam(this);

    mModelType = (param >> 8) & 0xFF;
    mEnvType = param;

    if (mModelType == 0xff) {
        mModelType = 0;
    }

    if (mEnvType == 0xff) {
        scale.x = 1.0f;
        scale.y = 1.0f;
        scale.z = 1.0f;
    } else if (mEnvType == 0x01) {
        scale.x = 1.27f;
        scale.y = 1.27f;
        scale.z = 1.27f;
    }
}

/* 0217DAF8 */
bool daIkari_c::_execute() {
    mTimer = mTimer + 1;

    cXyz* windVec = dKyw_get_wind_vec();
    s16 windAngle = cM_atan2s(windVec->x, windVec->z);
    be<f32>* windPow = dKyw_get_wind_power();

    shape_angle.y = windAngle;

    f32 rotX = *windPow * 10000.0f * l_HIO.mWindPowerScale;

    // HD: keep the fused single-precision operation and saturating conversion.
    current.angle.x = (s16)ftoi(fmadds(cM_ssin(mTimer * (REG0_S(5) + 500)), rotX, rotX));

    setMtx();

    return true;
}

/* 0217DBC0 */
bool daIkari_c::_draw() {
    if (mEnvType == 1) {
        // HD: cache this accessor before settingTevStruct; only RGB is copied.
        daIkari_env_light_view* light = (daIkari_env_light_view*)dKy_getEnvlight();
        g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
        tevStr.mColorC0.r = light->mActorC0.r;
        tevStr.mColorC0.g = light->mActorC0.g;
        tevStr.mColorC0.b = light->mActorC0.b;
        tevStr.mColorK0.r = light->mActorK0.r;
        tevStr.mColorK0.g = light->mActorK0.g;
        tevStr.mColorK0.b = light->mActorK0.b;
    } else {
        g_env_light.settingTevStruct(TEV_TYPE_ACTOR, &current.pos, &tevStr);
    }
    g_env_light.setLightTevColorType(mpModel, &tevStr);
    mDoExt_modelUpdateDL(mpModel);
    return true;
}

/* 0217D964 */
cPhs_State daIkari_c::_create() {
    cPhs_State phase = dComIfG_resLoad(&mPhs, M_arcname);

    fopAcM_ct(this, daIkari_c);

    if (phase == cPhs_COMPLEATE_e) {
        getArg();

        if (!fopAcM_entrySolidHeap(this, createHeap_CB, 0xC20)) {
            return cPhs_ERROR_e;
        } else {
            setMtx();

            J3DModel* model = mpModel;
            f32 scaleX = scale.x;
            fopAcM_SetMtx(this, model ? model->getBaseTRMtx() : nullptr);
            fopAcM_setCullSizeBox(this, -160.0f * scaleX, -2500.0f * scaleX, -600.0f * scaleX, 160.0f * scaleX,
                                  100.0f * scaleX, 600.0f * scaleX);
            fopAcM_setCullSizeFar(this, 10.0f);

            mTimer = (s16)ftoi(cM_rndF(32768.0f));
        }
    }

    return phase;
}

/* 0217DAC4 */
bool daIkari_c::_delete() {
    dComIfG_resDelete(&mPhs, M_arcname);
    return true;
}

/* 0217DAC0 */
static cPhs_State daIkariCreate(void* i_this) {
    return ((daIkari_c*)i_this)->_create();
}

/* 0217DAF4 */
static BOOL daIkariDelete(void* i_this) {
    return ((daIkari_c*)i_this)->_delete();
}

/* 0217DBBC */
static BOOL daIkariExecute(void* i_this) {
    return ((daIkari_c*)i_this)->_execute();
}

/* 0217DCA0 */
static BOOL daIkariDraw(void* i_this) {
    return ((daIkari_c*)i_this)->_draw();
}

/* 0217DD7C */
static BOOL daIkariIsDelete(void* i_this) {
    return TRUE;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0217DCA4 */
// HD: header statics, then l_HIO's constructor
static void __sinit_d_a_ikari_cpp() {
    __sinit_header_statics();
    l_HIO.unk[0] = 0;
    l_HIO.mWindPowerScale = 0.1f;
    l_HIO.__vtbl = daObjIkariHIO_c::__vt;
    l_HIO.unk[1] = 0;
}

/* 0217DD68 */
// HD: sead::SafeString deleting destructor (per-TU copy)
static void safestring_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 0217DD84 */
// HD: daIkari_c deleting destructor
static void daIkari_c_dt(daIkari_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(static_cast<fopAc_ac_c*>(i_this));
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 0217DDD8 */
// HD: an empty virtual of the SafeString vtable (per-TU copy)
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and existing local binding
str 10011D18
str 10011CB4
str 10011CC4 "modelData != NULL"
data ikari_bdl 101B7990
data l_HIO 104647EC
ext 0257DB04 be<f32>* dKyw_get_wind_power()
*/
