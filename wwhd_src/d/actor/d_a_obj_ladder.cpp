/**
 * d_a_obj_ladder.cpp (WWHD)
 * Object - Drop-down ladder (falls when its switch is set, with an optional event).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_ladder.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_ladder.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"

const gc_tu_data gc_tu = {
    .safeString = 0x1002BD20,
    .gndChk = {0x1002BD88, 0x1002BD98, 0x1002BDB8, 0x1002BDA8},
    .gndChkDt = {0x1002BD58, 0x1002BD78, 0x1002BD38},
};
enum { ACT_VTBL = 0x1002BEB8, MoveBGProc_Trans = 0x024EE76C };
static const char M_arcname[] = "Mhsg";
static Mtx34 M_tmp_mtx;
using daObjLadder::Act_c;
using daObjLadder::AttrType;
using daObjLadder::LadderModeEntry;
static const AttrType L_attr_type[] = {
    {7,14,600.0f}, {8,15,900.0f}, {4,11,1200.0f}, {5,12,1500.0f}, {6,13,450.0f}
};
static const AttrType& attr_type(s32 type) { return L_attr_type[type]; }

/* 02369CEC */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
s32 Act_c::prm_get_type() { return PrmAbstract(this, 3, 0); }
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, 8, 8); }
u8 Act_c::prm_get_evId() { return (u8)PrmAbstract(this, 8, 0x10); }

/* 02369214 */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, attr_type(mType).modelId, gc_tu.safeString);
    if (model_data == nullptr) /* JUT_ASSERT(0x17E, model_data != 0) */
        JUT_ASSERT_fail("d_a_obj_ladder.cpp", 0x17E, "model_data != 0");
    mpModel = mDoExt_J3DModel__create(model_data, 0x80000U, 0x11000022U);
    return mpModel != nullptr;
}

/* 023693E0 */
BOOL Act_c::Create() {
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -55.0f, -1.0f, -10.0f, 55.0f, attr_type(mType).height + 41.0f, 10.0f);
    cXyz pos;
    mDoMtx_stack_push();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mDoMtx_stack_c::transM(0.0f, 10.0f, 5.0f);
    PSMTXMultVec(mDoMtx_stack_c::get(), cXyz_Zero, &pos);
    mDoMtx_stack_pop();
    mGndChk.SetPos(&pos);
    mGndChk.SetActorPid(mBsPcId);
    mGndY = dComIfG_Bgsp()->GroundCross(&mGndChk);
    unk346 = 0;
    mEventIdx = dComIfGp_evmng_getEventIdx(nullptr, prm_get_evId());
    s32 swSave = prm_get_swSave();
    if (swSave == 0xFF || fopAcM_isSwitch(this, swSave)) {
        current.pos.y = mGndY;
        set_mtx();
        mode_fell_init();
    } else {
        mode_wait_init();
    }
    return TRUE;
}

/* 02369068 */
cPhs_State Act_c::Mthd_Create() {
    /* fopAcM_ct(this, Act_c): base constructor, vtable, inline dBgS_ObjGndChk constructor */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
            dBgS_GndChk_ct(&mGndChk, gc_tu.gndChk, true);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        mType = prm_get_type();
        phase_state = MoveBGCreate(M_arcname, attr_type(mType).dzbId, MoveBGProc_Trans, 0x900);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0x1DE, ...) */
            JUT_ASSERT_fail("d_a_obj_ladder.cpp", 0x1DE, "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)");
    }
    return phase_state;
}

/* 02369C5C */
static BOOL Act_c_Delete(Act_c* i_this) {
    return TRUE;
}

/* 023691C8 */
BOOL Act_c::Mthd_Delete() {
    BOOL res = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return res;
}

/* 023695C8 */
void Act_c::demo_end_reset() {
    if (unk346 && dComIfGp_evmng_endCheck(mEventIdx)) {
        dComIfGp_event_reset();
        unk346 = 0;
    }
}

/* 023693D4 */
void Act_c::mode_wait_init() {
    mMode = Mode_WAIT_e;
}

/* 023697B0 */
void Act_c::mode_wait() {
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        mode_demoreq_init();
    }
}

/* 0236979C */
void Act_c::mode_demoreq_init() {
    mMode = Mode_DEMOREQ_e;
    unk346 = 0;
}

/* 02369830 */
void Act_c::mode_demoreq() {
    bool ret = false;
    if (dComIfGp_evmng_existence(mEventIdx)) {
        if (eventInfo_checkCommandDemoAccrpt(this)) {
            ret = true;
            unk346 = 1;
        } else {
            u8 evId = prm_get_evId();
            fopAcM_orderOtherEventId(this, mEventIdx, evId, 0xFFFF, 0, 1);
            eventInfo_onCondition(this, dEvtCnd_UNK2_e);
        }
    } else {
        ret = true;
    }
    if (ret) {
        mode_vib_init();
    }
}

/* 02369810 */
void Act_c::mode_vib_init() {
    mVibTimer = 15; /* attr().mVibDuration */
    unk338 = 0;
    unk33A = 0;
    mMode = Mode_VIB_e;
}

/* 02369934 */
void Act_c::mode_vib() {
    unk338 += 20000;  /* attr().field_0x16 */
    unk33A += 0x3CC3; /* attr().field_0x18 */
    mVibXOffset = cM_scos(unk338) * 2.0f; /* attr().field_0x1C */
    mVibYOffset = cM_scos(unk33A);        /* * attr().field_0x20 (1.0) */
    if (--mVibTimer <= 0) {
        mode_drop_init();
    }
}

/* 023698FC */
void Act_c::mode_drop_init() {
    gravity = -5.0f;
    speed.set(cXyz_Zero->x, cXyz_Zero->y, cXyz_Zero->z);
    mMode = Mode_DROP_e;
    unk2DE = 3; /* attr().field_0x10 */
}

/* 023699A0 */
void Act_c::mode_drop() {
    daObj_posMoveF_stream(this, nullptr, cXyz_Zero, 0.005f, 0.0005f);
    if (current.pos.y < mGndY) {
        if (unk2DE == 3) {
            fopAcM_seStart(this, JA_SE_OBJ_LADDER_FALL_1, dBgS_GetMtrlSndId(dComIfG_Bgsp(), static_cast<cBgS_PolyInfo*>(&mGndChk)));
            cXyz dir;
            dir.set(0.0f, 1.0f, 0.0f);
            dComIfGp_getVibration_StartShock(4, -0x21, &dir);
        } else {
            s32 tmp = 3 - unk2DE;
            s32 param_2;
            if (tmp == 1) {
                param_2 = 75;
            } else if (tmp == 2) {
                param_2 = 50;
            } else if (tmp == 3) {
                param_2 = 45;
            } else {
                param_2 = 40;
            }
            fopAcM_seStart(this, JA_SE_OBJ_LADDER_FALL_2, param_2);
        }
        if (unk2DE >= 0) {
            unk2DE--;
            f32 temp = current.pos.y - mGndY;
            current.pos.y = gabi::fnmsubs(temp, 0.5f, mGndY); /* mGndY - temp * 0.5f */
            speed.y *= -0.5f;
        } else {
            current.pos.y = mGndY;
            mode_fell_init();
        }
    }
}

/* 023693C8 */
void Act_c::mode_fell_init() {
    mMode = Mode_FELL_e;
}

/* 02369C64 */
void Act_c::mode_fell() {
}

/* 023692C0 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    PSMTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
    mDoMtx_stack_c::transM(mVibXOffset, mVibYOffset, 0.0f);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}

/* 023693A8 */
void Act_c::init_mtx() {
    J3DModel_setBaseScale(mpModel, &scale);
    set_mtx();
}

/* 02369638 */
BOOL Act_c::Execute(gptr<Mtx34>* ppMtx) {
    // HD: GHS stores each member-function descriptor in eight bytes.
    static LadderModeEntry mode_proc[] = {
        {&Act_c::mode_wait}, {&Act_c::mode_demoreq}, {&Act_c::mode_vib},
        {&Act_c::mode_drop}, {&Act_c::mode_fell}
    };
    demo_end_reset();
    (this->*mode_proc[mMode].proc)();
    eyePos.y = current.pos.y;
    set_mtx();
    // HD: the output slot contains a four-byte guest pointer.
    *ppMtx = &M_tmp_mtx;
    return TRUE;
}

/* 02369704 */
BOOL Act_c::Draw() {
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}

/* method table entries (tail branches) */
/* 02369B7C */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 02369B80 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 02369B84 */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
/* 02369B88 */
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
/* 02369B98 */
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* 02369BA8 */
static void __sinit_d_a_obj_ladder_cpp() {
    sinit_header_statics(0x1046A424, 0x101CA7D8);
}

/* 02369C3C */
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

// HD: compiler-generated and template code of this translation unit.

/* 02369C50 */
static BOOL ladder_MoveBgActor_IsDelete(void* p) {
    return TRUE;
}

/* 02369C58 */
static void ladder_SafeString_assureTermination(void* p) {
}

/* 02369C68 */
static void ladder_Act_c_dt(void* p, s32 flags) {
    if (p != nullptr) {
        Act_c* actor = static_cast<Act_c*>(p);
        gabi_destroy(static_cast<dBgS_GndChk&>(actor->mGndChk));
        __dt(static_cast<fopAc_ac_c*>(actor));
        if (flags & 1) operator_delete(p);
    }
}

/* WWHD glue
str 1002BEB0
str 1002BE34 in 02369214
str 1002BDCC in 02369068
str 1002BE24 "model_data != 0"
str 1002BDE0 "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
data M_tmp_mtx 1046A440
data L_attr_type 101CA82C
data mode_proc 1002BE68
*/
