/**
 * d_a_obj_hole.cpp (WWHD)
 * Object - Grotto (a hole that starts the pitfall event and changes stage).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_hole.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define m_arc_name STR(0x1002A9D8) /* "Aana" */
#define SAFESTRING_VTBL 0x1002A820
#define HOLE_VTBL 0x1002A908
static const dBgS_LinChk_vt LINCHK_VT = {0x1002A8C8, 0x1002A8D8, 0x1002A8F8, 0x1002A8E8};
static const dBgS_ObjAcch_vt OBJACCH_VT = {0x1002A858, 0x1002A878, 0x1002A868};

enum { dRes_INDEX_AANA_BDL_AANA_e = 3 };

/* local bindings */
static inline BOOL dLib_checkPlayerInCircle(cXyz* pos, f32 r, f32 h) { return gabi::call<BOOL>(0x025881A4, pos, r, h); }
static inline void dLib_setNextStageBySclsNum(u8 no, s8 roomNo) { gabi::call(0x02587EFC, no, roomNo); }
static inline BOOL dComIfGp_evmng_endCheckOld(const char* name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_getPEvtManager(), name); }
static inline void fopAcM_orderOtherEvent2(fopAc_ac_c* a, const char* name, u16 flag, u16 hind) {
    gabi::call(0x025D77DC, a, name, flag, hind);
}
/* dComIfGp_setNextStage(stage, point, roomNo, layer, lastSpeed, lastMode, setPoint, wipe):
 * the play object's stage name is at play+0x5134 (dComIfGp_getStartStageName) */
static inline void dComIfGp_setNextStage(const char* stage, s16 point, s8 roomNo, s8 layer, f32 speed, u32 mode, s32 setPoint, s8 wipe) {
    gabi::call(0x0252012C, stage, point, roomNo, layer, speed, mode, setPoint, wipe);
}
static inline const char* dComIfGp_getStartStageName() { return gabi::at<const char>(dComIfGp_ea() + 0x5134); }
/* fopCamM_GetAngleY(dComIfGp_getCamera(0)): camera_class at play+0x5AF8, angle y at +0x236 */
static inline s16 camera0_angleY() { return gabi::load<s16>(gabi::load<u32>(dComIfGp_ea() + 0x5AF8) + 0x236); }

struct daObj_Hole_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ be<u8> m05;
    /* 0x02 */ u8 _02[2];
    /* 0x04 */ be<f32> m08;
    /* 0x08 */ be<s16> m0C;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ be<f32> m10;
    /* 0x10 */ be<u32> __vtbl;
};
static daObj_Hole_HIO_c& l_HIO() { return *gabi::at<daObj_Hole_HIO_c>(0x10469EDC); }

struct daObj_Hole_c : fopAc_ac_c {
    enum Mode_e { MODE_WAIT, MODE_EVENT, MODE_NULL };
    enum Proc_e { PROC_INIT_e, PROC_EXEC_e };
    void setMtx();
    void getPosAndAngle();
    void modeWaitInit();
    void modeWait();
    void modeEventInit();
    void modeEvent();
    void modeProc(s32 proc, s32 newMode);
    bool _execute();
    void debugDraw();
    bool _draw();
    void createInit();
    BOOL _createHeap();
    void getArg();
    cPhs_State _create();
    bool _delete();

    /* 0x3AC */ be<s32> mMode;
    /* 0x3B0 */ be<u8> mHasModel;
    /* 0x3B1 */ be<u8> mExitIdx;
    /* 0x3B2 */ be<u16> mScaleLocal;
    /* 0x3B4 */ request_of_phase_process_class mPhs;
    /* 0x3BC */ gptr<J3DModel> mpMdl;
    /* 0x3C0 */ u8 mLinChk[0x6C];  /* dBgS_ObjLinChk */
    /* 0x42C */ dBgS_ObjAcch mAcch;
    /* 0x5F0 */ dBgS_AcchCir mAcchCir;
};
WWHD_OFFSET(daObj_Hole_c, mAcch, 0x42C);
WWHD_SIZE(daObj_Hole_c, 0x630);

/* 02357224 */
BOOL daObj_Hole_c::_createHeap() {
    WWHD_FUNC(0x02357224, BOOL, this);
    J3DModelData* modelData = (J3DModelData*)dComIfG_getObjectRes(m_arc_name, dRes_INDEX_AANA_BDL_AANA_e, SAFESTRING_VTBL);
    if (modelData == nullptr) /* JUT_ASSERT(0x13F, modelData != NULL) */
        JUT_ASSERT_fail(STR(0x1002A928), 0x13F, STR(0x1002A93C));
    mpMdl = mDoExt_J3DModel__create(modelData, 0x80000, 0x11000022);
    if (mpMdl == nullptr) {
        return FALSE;
    } else {
        return TRUE;
    }
}
VERIFY(0x02357224, &daObj_Hole_c::_createHeap);

/* 023572C0 */
static BOOL createHeap_CB(fopAc_ac_c* i_this) {
    WWHD_FUNC(0x023572C0, BOOL, i_this);
    return static_cast<daObj_Hole_c*>(i_this)->_createHeap();
}
VERIFY(0x023572C0, createHeap_CB);

/* 023572C4 */
void daObj_Hole_c::getArg() {
    WWHD_FUNC(0x023572C4, void, this);
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
VERIFY(0x023572C4, &daObj_Hole_c::getArg);

/* 02357310: mode_tbl (0x1002A950): {init PTMF, exec PTMF, name}, 0x14 bytes each */
void daObj_Hole_c::modeProc(s32 proc, s32 newMode) {
    WWHD_FUNC(0x02357310, void, this, proc, newMode);
    if (proc == PROC_INIT_e) {
        mMode = newMode;
        ptmf_call(0x1002A950 + 0x14 * newMode, this);
    } else if (proc == PROC_EXEC_e) {
        ptmf_call(0x1002A950 + 0x14 * mMode + 8, this);
    }
}
VERIFY(0x02357310, &daObj_Hole_c::modeProc);

/* 023573BC */
void daObj_Hole_c::setMtx() {
    WWHD_FUNC(0x023573BC, void, this);
    f32 ax = current.pos.x;
    f32 ay = current.pos.y + l_HIO().m10;
    f32 az = current.pos.z;
    if (mMode != MODE_EVENT) {
        shape_angle.y = (s16)(camera0_angleY() + 0x8000);
    }
    if (mHasModel == 0xFF) {
        f32 scaleMag = l_HIO().m08 * scale.x;
        if (l_HIO().m0C != 0) {
            scaleMag += (f32)(l_HIO().m0C * 10);
        } else {
            scaleMag += (f32)(mScaleLocal * 10);
        }
        scaleMag /= l_HIO().m08;
        J3DModel* mdl = mpMdl;
        gabi::store<f32>(gabi::ea(mdl) + 0xBC, scaleMag);
        gabi::store<f32>(gabi::ea(mdl) + 0xC0, scaleMag);
        gabi::store<f32>(gabi::ea(mdl) + 0xC4, scaleMag);
        mDoMtx_stack_c::transS(ax, ay, az);
        mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, 0, shape_angle.z);
        mDoMtx_stack_c::YrotM(shape_angle.y);
        J3DModel_setBaseTRMtx(mpMdl, mDoMtx_stack_c::get());
    }
}
VERIFY(0x023573BC, &daObj_Hole_c::setMtx);

/* 023579B0 */
void daObj_Hole_c::getPosAndAngle() {
    WWHD_FUNC(0x023579B0, void, this);
    gabi::Local<cXyz> posUp;
    posUp->set(0.0f, 10.0f, 0.0f);
    PSVECAdd(posUp, &current.pos, posUp);
    gabi::Local<cXyz> posDown;
    posDown->set(0.0f, -100.0f, 0.0f);
    PSVECAdd(posDown, &current.pos, posDown);
    gabi::Local<csXyz> unused_angle;
    csXyz_ct(unused_angle, 0, 0, 0);
    dBgS_LinChk_Set(mLinChk, posUp, posDown, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), mLinChk) != 0) {
        void* triPla = dBgS_GetTriPla(dComIfG_Bgsp(), gabi::at<u8>(gabi::ea(mLinChk) + 0x14));
        if (triPla != nullptr) { /* HD: null check */
            cM3dGPla_CalcAngleXz(triPla, &shape_angle.x, &shape_angle.z);
            cM3dGPla_getCrossY(triPla, &current.pos, &current.pos.y);
        }
    }
}
VERIFY(0x023579B0, &daObj_Hole_c::getPosAndAngle);

/* 02357AC4 */
void daObj_Hole_c::modeWaitInit() {
    WWHD_FUNC(0x02357AC4, void, this);
    getPosAndAngle();
}
VERIFY(0x02357AC4, &daObj_Hole_c::modeWaitInit);

/* 02357AC8 */
void daObj_Hole_c::modeWait() {
    WWHD_FUNC(0x02357AC8, void, this);
    f32 scaleMag = l_HIO().m08 * scale.x;
    if (l_HIO().m0C != 0) {
        scaleMag += (f32)(l_HIO().m0C * 10);
    } else {
        scaleMag += (f32)(mScaleLocal * 10);
    }
    gabi::Local<cXyz> pos; /* cXyz passed by value */
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    if (dLib_checkPlayerInCircle(pos, scaleMag, 20.0f)) {
        modeProc(PROC_INIT_e, MODE_EVENT);
    }
}
VERIFY(0x02357AC8, &daObj_Hole_c::modeWait);

/* 02357E08 */
void daObj_Hole_c::modeEventInit() {
    WWHD_FUNC(0x02357E08, void, this);
}
VERIFY(0x02357E08, &daObj_Hole_c::modeEventInit);

/* 02357BDC */
void daObj_Hole_c::modeEvent() {
    WWHD_FUNC(0x02357BDC, void, this);
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        dComIfGp_evmng_getMyStaffId(STR(0x1002A9A4) /* "Ypit00" */, nullptr, 0);
        if (dComIfGp_evmng_endCheckOld(STR(0x1002A9AC) /* "DEFAULT_PITFALL" */)) {
            if (mExitIdx != 0xFF) {
                dLib_setNextStageBySclsNum(mExitIdx, current.roomNo);
            } else {
                dComIfGp_setNextStage(dComIfGp_getStartStageName(), 0, current.roomNo, -1, 0.0f, 0, 1, 0);
            }
        }
    } else {
        fopAcM_orderOtherEvent2(this, STR(0x1002A9AC), 1, 0xFFFF);
    }
}
VERIFY(0x02357BDC, &daObj_Hole_c::modeEvent);

/* 023578B8 */
bool daObj_Hole_c::_execute() {
    WWHD_FUNC(0x023578B8, bool, this);
    modeProc(PROC_EXEC_e, MODE_NULL);
    setMtx();
    return false;
}
VERIFY(0x023578B8, &daObj_Hole_c::_execute);

/* 023578FC: HD: only the function-local static of the (debug) drawing is initialised */
void daObj_Hole_c::debugDraw() {
    WWHD_FUNC(0x023578FC, void, this);
    be<u32>& guard = *gabi::at<be<u32>>(0x101FDA48);
    if (guard == 0) {
        guard = 1;
        memcpy_g(gabi::at<u8>(0x101FEBEC), gabi::at<u8>(0x1002A818), 4);
    }
}
VERIFY(0x023578FC, &daObj_Hole_c::debugDraw);

/* 0235792C */
bool daObj_Hole_c::_draw() {
    WWHD_FUNC(0x0235792C, bool, this);
    if (l_HIO().m05 != 0) {
        debugDraw();
    }
    if (mHasModel == 0xFF) {
        settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
        setLightTevColorType(dKy_getEnvlight(), mpMdl, &tevStr);
        mDoExt_modelUpdateDL(mpMdl);
    }
    return true;
}
VERIFY(0x0235792C, &daObj_Hole_c::_draw);

/* 02357660 */
void daObj_Hole_c::createInit() {
    WWHD_FUNC(0x02357660, void, this);
    modeProc(PROC_INIT_e, MODE_WAIT);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpMdl)); /* fopAcM_SetMtx */
    cullSizeFar = 10.0f;
    mAcchCir.SetWall(100.0f, 10.0f);
    mAcch.Set(&current.pos, &old.pos, this, 1, &mAcchCir, &speed);
    mAcch.SetWallNone();
    mAcch.SetRoofNone();
    setMtx();
}
VERIFY(0x02357660, &daObj_Hole_c::createInit);

/* 02357700 */
cPhs_State daObj_Hole_c::_create() {
    WWHD_FUNC(0x02357700, cPhs_State, this);
    cPhs_State result;
    /* fopAcM_ct(this, daObj_Hole_c): inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            fopAc_ac_c_ct(this);
            __vtbl = HOLE_VTBL;
            dBgS_LinChk_ct(mLinChk, LINCHK_VT, true);
            dBgS_ObjAcch_ct(&mAcch, OBJACCH_VT);
            dBgS_AcchCir_ct(&mAcchCir);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    result = dComIfG_resLoad(&mPhs, m_arc_name);
    if (result == cPhs_COMPLEATE_e) {
        getArg();
        if (mHasModel == 0xFF) {
            u32 heapResult = fopAcM_entrySolidHeap(this, 0x023572C0 /* createHeap_CB */, 0x1000);
            if (heapResult == 0) {
                return cPhs_ERROR_e;
            }
        }
        createInit();
    }
    return result;
}
VERIFY(0x02357700, &daObj_Hole_c::_create);

/* 02357884 */
bool daObj_Hole_c::_delete() {
    WWHD_FUNC(0x02357884, bool, this);
    dComIfG_resDelete(&mPhs, m_arc_name); /* dComIfG_resDeleteDemo */
    return true;
}
VERIFY(0x02357884, &daObj_Hole_c::_delete);

static cPhs_State daObj_HoleCreate(void* i_this) {
    WWHD_FUNC(0x02357880, cPhs_State, i_this);
    return static_cast<daObj_Hole_c*>(i_this)->_create();
}
VERIFY(0x02357880, daObj_HoleCreate);
static BOOL daObj_HoleDelete(void* i_this) {
    WWHD_FUNC(0x023578B4, BOOL, i_this);
    return static_cast<daObj_Hole_c*>(i_this)->_delete();
}
VERIFY(0x023578B4, daObj_HoleDelete);
static BOOL daObj_HoleExecute(void* i_this) {
    WWHD_FUNC(0x023578F8, BOOL, i_this);
    return static_cast<daObj_Hole_c*>(i_this)->_execute();
}
VERIFY(0x023578F8, daObj_HoleExecute);
static BOOL daObj_HoleDraw(void* i_this) {
    WWHD_FUNC(0x023579AC, BOOL, i_this);
    return static_cast<daObj_Hole_c*>(i_this)->_draw();
}
VERIFY(0x023579AC, daObj_HoleDraw);
static BOOL daObj_HoleIsDelete(void* i_this) {
    WWHD_FUNC(0x02357E00, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02357E00, daObj_HoleIsDelete);

/* 02357CE0: daObj_Hole_HIO_c::daObj_Hole_HIO_c (allocates when this == NULL) */
static daObj_Hole_HIO_c* daObj_Hole_HIO_c_ct(daObj_Hole_HIO_c* h) {
    WWHD_FUNC(0x02357CE0, daObj_Hole_HIO_c*, h);
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
VERIFY(0x02357CE0, daObj_Hole_HIO_c_ct);

/* 02357D4C */
static void __sinit_d_a_obj_hole_cpp() {
    WWHD_FUNC(0x02357D4C, void, (u32)0);
    sinit_header_statics(0x10469EC0, 0x101C9FF8);
    daObj_Hole_HIO_c_ct(&l_HIO());
}
VERIFY(0x02357D4C, __sinit_d_a_obj_hole_cpp);

/* 02357DEC: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02357DEC, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02357DEC, trivial_dt);

/* 02357E0C: daObj_Hole_c deleting destructor (inline member destructors) */
static void daObj_Hole_c_dt(daObj_Hole_c* i_this, s32 flags) {
    WWHD_FUNC(0x02357E0C, void, i_this, flags);
    if (i_this != nullptr) {
        u32 b = gabi::ea(i_this);
        gabi::call(0x02018034, gabi::at<u8>(b + 0x604), 2); /* mAcchCir's cM3dGCir */
        gabi::store<u32>(b + 0x44C, OBJACCH_VT.v20);
        gabi::store<u32>(b + 0x440, OBJACCH_VT.v14);
        gabi::call(0x024EFD9C, &i_this->mAcch, 0);         /* dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(b + 0x418, 0x1002A8B8);
        gabi::store<u32>(b + 0x424, 0x1002A848);
        gabi::store<u32>(b + 0x3E0, 0x1002A838);
        gabi::call(0x02008B4C, i_this->mLinChk, 0);         /* cBgS_LinChk::~cBgS_LinChk */
        gabi::call(0x025D50BC, i_this, 0);                  /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x02357E0C, daObj_Hole_c_dt);

/* 02357EC0: an empty virtual (per-TU copy) */
static void empty_virtual(void* p) {
    WWHD_FUNC(0x02357EC0, void, p);
}
VERIFY(0x02357EC0, empty_virtual);
