/**
 * d_a_obj_hole.cpp (WWHD)
 * Object - Grotto (a hole that starts the pitfall event and changes stage).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_hole.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_hole.h"
const gc_tu_data gc_tu = {
    .safeString = 0x1002A820,
    .objAcch = {0x1002A858, 0x1002A878, 0x1002A868},
    .objAcchDt = {0x1002A868, 0x1002A878},
};
const char daObj_Hole_c::m_arc_name[] = "Aana";
static daObj_Hole_HIO_c l_HIO;
static u32 debugDraw_guard;
static u32 debugDraw_value;
static const u32 debugDraw_src = 0xff0080;

/* 02357224 */
BOOL daObj_Hole_c::_createHeap() {
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_AANA_BDL_AANA_e);
    // HD: assertion line is 0x13F (GameCube 0x13D).
    JUT_ASSERT(0x13F, modelData != NULL);
    mpMdl = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (mpMdl == nullptr) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/* 023572C0 */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    return static_cast<daObj_Hole_c*>(i_this)->_createHeap();
}

/* 023572C4 */
void daObj_Hole_c::getArg() {
    u32 param = fopAcM_GetParam(this);
    mExitIdx = param & 0xFF;
    mHasModel = (param >> 8) & 0xFF;
    mScaleLocal = home.angle.z;
    if (mScaleLocal == 0xFFFF) {
        mScaleLocal = 0;
    }
    if (mHasModel >= 1) {
        mHasModel = 0xFF;
    }
}

/* 02357310 */
// HD: mode_tbl (0x1002A950): {init PTMF, exec PTMF, name}, 0x14 bytes each
void daObj_Hole_c::modeProc(s32 proc, s32 newMode) {
    using daObjHole_mode_t = void (daObj_Hole_c::*)();
    struct mode_struct {
        gabi::ptmf<daObjHole_mode_t> init;
        gabi::ptmf<daObjHole_mode_t> exec;
        gptr<const char> name;
    };
    static const mode_struct mode_tbl[] = {
        {&daObj_Hole_c::modeWaitInit, &daObj_Hole_c::modeWait, "WAIT"},
        {&daObj_Hole_c::modeEventInit, &daObj_Hole_c::modeEvent, "EVENT"},
    };
    if (proc == PROC_INIT_e) {
        mMode = newMode;
        (this->*mode_tbl[newMode].init)();
    } else if (proc == PROC_EXEC_e) {
        (this->*mode_tbl[mMode].exec)();
    }
}

/* 023573BC */
void daObj_Hole_c::setMtx() {
    f32 ax = current.pos.x;
    f32 ay = current.pos.y + l_HIO.m10;
    f32 az = current.pos.z;
    if (mMode != MODE_EVENT) {
        shape_angle.y = (s16)(fopCamM_GetAngleY(dComIfGp_getCamera(0)) + 0x8000);
    }
    if (mHasModel == 0xFF) {
        f32 scaleMag = l_HIO.m08 * scale.x;
        if (l_HIO.m0C != 0) {
            scaleMag += (f32)(l_HIO.m0C * 10);
        } else {
            scaleMag += (f32)(mScaleLocal * 10);
        }
        scaleMag /= l_HIO.m08;
        J3DModel* mdl = mpMdl;
        mdl->mBaseScale.x = scaleMag;
        mdl->mBaseScale.y = scaleMag;
        mdl->mBaseScale.z = scaleMag;
        mDoMtx_stack_c::transS(ax, ay, az);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, 0, shape_angle.z);
        mDoMtx_stack_c::YrotM(shape_angle.y);
        mpMdl->setBaseTRMtx(mDoMtx_stack_c::get());
    }
}

/* 023579B0 */
void daObj_Hole_c::getPosAndAngle() {
    cXyz posUp;
    posUp.set(0.0f, 10.0f, 0.0f);
    PSVECAdd(&posUp, &current.pos, &posUp);
    cXyz posDown;
    posDown.set(0.0f, -100.0f, 0.0f);
    PSVECAdd(&posDown, &current.pos, &posDown);
    csXyz unused_angle;
    csXyz_ct(&unused_angle, 0, 0, 0);
    mLinChk.Set(&posUp, &posDown, this);
    if (dComIfG_Bgsp()->LineCross(&mLinChk) != 0) {
        cM3dGPla* triPla = dComIfG_Bgsp()->GetTriPla(mLinChk.mBgIndex, mLinChk.mPolyIndex);
        if (triPla != nullptr) { /* HD: null check */
            triPla->CalcAngleXz(&shape_angle.x, &shape_angle.z);
            triPla->getCrossY(&current.pos, &current.pos.y);
        }
    }
}

/* 02357AC4 */
void daObj_Hole_c::modeWaitInit() {
    getPosAndAngle();
}

/* 02357AC8 */
void daObj_Hole_c::modeWait() {
    f32 scaleMag = l_HIO.m08 * scale.x;
    if (l_HIO.m0C != 0) {
        scaleMag += (f32)(l_HIO.m0C * 10);
    } else {
        scaleMag += (f32)(mScaleLocal * 10);
    }
    cXyz pos; /* cXyz passed by value */
    pos.x = current.pos.x;
    pos.y = current.pos.y;
    pos.z = current.pos.z;
    if (dLib_checkPlayerInCircle(&pos, scaleMag, 20.0f)) {
        modeProc(PROC_INIT_e, MODE_EVENT);
    }
}

/* 02357E08 */
void daObj_Hole_c::modeEventInit() {
}

/* 02357BDC */
void daObj_Hole_c::modeEvent() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        dComIfGp_evmng_getMyStaffId("Ypit00", nullptr, 0);
        if (hole_eventEndCheck("DEFAULT_PITFALL")) {
            if (mExitIdx != 0xFF) {
                dLib_setNextStageBySclsNum(mExitIdx, current.roomNo);
            } else {
                dComIfGp_setNextStage(dComIfGp_getStartStageName(), 0, current.roomNo, -1, 0.0f, 0, 1, 0);
            }
        }
    } else {
        fopAcM_orderOtherEvent(this, "DEFAULT_PITFALL", 1, 0xFFFF);
    }
}

/* 023578B8 */
bool daObj_Hole_c::_execute() {
    modeProc(PROC_EXEC_e, MODE_NULL);
    setMtx();
    return false;
}

/* 023578FC */
// HD: only the function-local static of the (debug) drawing is initialised
void daObj_Hole_c::debugDraw() {
    if (debugDraw_guard == 0) {
        debugDraw_guard = 1;
        memcpy_g(&debugDraw_value, &debugDraw_src, 4);
    }
}

/* 0235792C */
bool daObj_Hole_c::_draw() {
    if (l_HIO.m05 != 0) {
        debugDraw();
    }
    if (mHasModel == 0xFF) {
        g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
        g_env_light.setLightTevColorType(mpMdl, &tevStr);
        mDoExt_modelUpdateDL(mpMdl);
    }
    return true;
}

/* 02357660 */
void daObj_Hole_c::createInit() {
    modeProc(PROC_INIT_e, MODE_WAIT);
    fopAcM_SetMtx(this, mpMdl->getBaseTRMtx());
    cullSizeFar = 10.0f;
    mAcchCir.SetWall(100.0f, 10.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    mAcch.SetWallNone();
    mAcch.SetRoofNone();
    setMtx();
}

/* 02357700 */
cPhs_State daObj_Hole_c::_create() {
    cPhs_State result;
    fopAcM_ct(this, daObj_Hole_c);
    result = dComIfG_resLoad(&mPhs, m_arc_name);
    if (result == cPhs_COMPLEATE_e) {
        getArg();
        if (mHasModel == 0xFF) {
            u32 heapResult = fopAcM_entrySolidHeap(this, createHeap_CB, 0x1000);
            if (heapResult == 0) {
                return cPhs_ERROR_e;
            }
        }
        createInit();
    }
    return result;
}

/* 02357884 */
bool daObj_Hole_c::_delete() {
    dComIfG_resDelete(&mPhs, m_arc_name); /* dComIfG_resDeleteDemo */
    return true;
}

/* 02357880 */
static cPhs_State daObj_HoleCreate(void* i_this) {
    return static_cast<daObj_Hole_c*>(i_this)->_create();
}
/* 023578B4 */
static BOOL daObj_HoleDelete(void* i_this) {
    return static_cast<daObj_Hole_c*>(i_this)->_delete();
}
/* 023578F8 */
static BOOL daObj_HoleExecute(void* i_this) {
    return static_cast<daObj_Hole_c*>(i_this)->_execute();
}
/* 023579AC */
static BOOL daObj_HoleDraw(void* i_this) {
    return static_cast<daObj_Hole_c*>(i_this)->_draw();
}
/* 02357E00 */
static BOOL daObj_HoleIsDelete(void* i_this) {
    return TRUE;
}

/* 02357CE0 */
// HD: daObj_Hole_HIO_c::daObj_Hole_HIO_c (allocates when this == NULL)
static daObj_Hole_HIO_c* daObj_Hole_HIO_c_ct(daObj_Hole_HIO_c* h) {
    if (h == nullptr) {
        h = (daObj_Hole_HIO_c*)operator_new(0x14);
        if (h == nullptr)
            return h;
    }
    h->m05 = 0;
    h->m10 = 2.0f;
    h->m0C = 0;
    h->m08 = 65.0f;
    h->__vtbl = 0x1002A918;
    h->mNo = -1;
    return h;
}

/* 02357D4C */
static void __sinit_d_a_obj_hole_cpp() {
    sinit_header_statics(0x10469EC0, 0x101C9FF8);
    daObj_Hole_HIO_c_ct(&l_HIO);
}

/* 02357DEC */
// HD: deleting destructor of a class with a trivial destructor
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02357E0C */
// HD: daObj_Hole_c deleting destructor (inline member destructors)
static void daObj_Hole_c_dt(daObj_Hole_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->mAcchCir);
        __dt(&i_this->mAcch);
        __dt(&i_this->mLinChk);
        __dt((fopAc_ac_c*)i_this);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 02357EC0 */
// HD: an empty virtual (per-TU copy)
static void empty_virtual(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 1002A9D8 "Aana"
str 1002A9A4 "Ypit00"
str 1002A9AC "DEFAULT_PITFALL"
str 1002A928 "d_a_obj_hole.cpp"
str 1002A93C "modelData != NULL"
str 1002A978 "WAIT"
str 1002A980 "EVENT"
data l_HIO 10469EDC
data mode_tbl 1002A950
data debugDraw_guard 101FDA48
data debugDraw_value 101FEBEC
data debugDraw_src 1002A818
ext 025881A4 BOOL dLib_checkPlayerInCircle(cXyz*, f32, f32)
ext 02587EFC void dLib_setNextStageBySclsNum(u8, s8)
ext 025D77DC void fopAcM_orderOtherEvent(fopAc_ac_c*, const char*, u16, u16)
ext 0252012C void dComIfGp_setNextStage(const char*, s16, s8, s8, f32, u32, s32, s8)
ext 02018BA8 s16 cM3dGPla::CalcAngleXz(be<s16>*, be<s16>*)
ext 02018AE4 u32 cM3dGPla::getCrossY(const cXyz*, be<f32>*)
*/
