/**
 * d_a_obj_ladder.cpp (WWHD)
 * Object - Drop-down ladder (falls when its switch is set, with an optional event).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_ladder.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x1002BEB0) /* "Mhsg" */
#define SAFESTRING_VTBL 0x1002BD20
#define ACT_VTBL 0x1002BEB8
#define M_tmp_mtx gabi::at<Mtx34>(0x1046A440)
static const dBgS_GndChk_vt GNDCHK_VT = {0x1002BD88, 0x1002BD98, 0x1002BDB8, 0x1002BDA8};

enum { JA_SE_OBJ_LADDER_FALL_1 = 0x6954, JA_SE_OBJ_LADDER_FALL_2 = 0x6955 };
enum { dEvtCnd_UNK2_e = 2 };

namespace daObjLadder {
/* L_attr: the HD code uses the values directly (field_0x10 3, 0x11..0x14 75/50/45/40,
 * angle steps 20000/0x3CC3, mVibDuration 15, amplitudes 2.0/1.0) */
/* L_attr_type[5] (0x101CA82C): {s16 modelId, s16 dzbId, f32 height} */
struct AttrType {
    be<s16> modelId;
    be<s16> dzbId;
    be<f32> height;
};
static AttrType& attr_type(s32 type) { return gabi::at<AttrType>(0x101CA82C)[type]; }

struct Act_c : dBgS_MoveBgActor {
    enum Mode_e { Mode_WAIT_e, Mode_DEMOREQ_e, Mode_VIB_e, Mode_DROP_e, Mode_FELL_e };
    s32 prm_get_type();
    s32 prm_get_swSave();
    u8 prm_get_evId();

    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void demo_end_reset();
    void mode_wait_init();
    void mode_wait();
    void mode_demoreq_init();
    void mode_demoreq();
    void mode_vib_init();
    void mode_vib();
    void mode_drop_init();
    void mode_drop();
    void mode_fell_init();
    void mode_fell();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34** ppMtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ be<s32> mType;
    /* 0x3F0 */ be<s32> mMode;
    /* 0x3F4 */ be<s16> mVibTimer;
    /* 0x3F6 */ be<s16> unk2DE;
    /* 0x3F8 */ be<f32> mGndY;
    /* 0x3FC */ u8 mGndChk[0x54];  /* dBgS_ObjGndChk */
    /* 0x450 */ be<s16> unk338;
    /* 0x452 */ be<s16> unk33A;
    /* 0x454 */ be<f32> mVibXOffset;
    /* 0x458 */ be<f32> mVibYOffset;
    /* 0x45C */ be<s16> mEventIdx;
    /* 0x45E */ be<u8> unk346;
    /* 0x45F */ u8 _45F;
};
WWHD_OFFSET(Act_c, mGndChk, 0x3FC);
WWHD_OFFSET(Act_c, unk346, 0x45E);
WWHD_SIZE(Act_c, 0x460);
}  // namespace daObjLadder
using daObjLadder::Act_c;
using daObjLadder::attr_type;

/* 02369CEC: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x02369CEC, u32, a, width, shift);
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x02369CEC, PrmAbstract);
s32 Act_c::prm_get_type() { return PrmAbstract(this, 3, 0); }
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, 8, 8); }
u8 Act_c::prm_get_evId() { return (u8)PrmAbstract(this, 8, 0x10); }

/* 02369214 */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x02369214, BOOL, this);
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, attr_type(mType).modelId, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(0x17E, model_data != NULL) */
        JUT_ASSERT_fail(STR(0x1002BE34), 0x17E, STR(0x1002BE24));
    mpModel = mDoExt_J3DModel__create(model_data, 0x80000U, 0x11000022U);
    return mpModel != nullptr;
}
VERIFY(0x02369214, &Act_c::CreateHeap);

/* 023693E0 */
BOOL Act_c::Create() {
    WWHD_FUNC(0x023693E0, BOOL, this);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -55.0f, -1.0f, -10.0f, 55.0f, attr_type(mType).height + 41.0f, 10.0f);
    gabi::Local<cXyz> pos;
    mDoMtx_stack_push();
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_stack_c::YrotM(shape_angle.y);
    mDoMtx_stack_c::transM(0.0f, 10.0f, 5.0f);
    PSMTXMultVec(mDoMtx_stack_c::get(), cXyz_Zero, pos);
    mDoMtx_stack_pop();
    dBgS_GndChk_SetPos(mGndChk, pos);
    dBgS_GndChk_SetActorPid(mGndChk, gabi::load<u32>(gabi::ea(this) + 4) /* base.base.mBsPcId */);
    mGndY = cBgS_GroundCross(dComIfG_Bgsp(), mGndChk);
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
VERIFY(0x023693E0, &Act_c::Create);

/* 02369068 */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x02369068, cPhs_State, this);
    /* fopAcM_ct(this, Act_c): base constructor, vtable, inline dBgS_ObjGndChk constructor */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
            dBgS_GndChk_ct(mGndChk, GNDCHK_VT, true);
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        mType = prm_get_type();
        phase_state = MoveBGCreate(M_arcname, attr_type(mType).dzbId, 0x024EE76C /* dBgS_MoveBGProc_Trans */, 0x900);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0x1DE, ...) */
            JUT_ASSERT_fail(STR(0x1002BDCC), 0x1DE, STR(0x1002BDE0));
    }
    return phase_state;
}
VERIFY(0x02369068, &Act_c::Mthd_Create);

/* 02369C5C */
static BOOL Act_c_Delete(Act_c* i_this) {
    WWHD_FUNC(0x02369C5C, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02369C5C, Act_c_Delete);

/* 023691C8 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x023691C8, BOOL, this);
    BOOL res = MoveBGDelete();
    dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    return res;
}
VERIFY(0x023691C8, &Act_c::Mthd_Delete);

/* 023695C8 */
void Act_c::demo_end_reset() {
    WWHD_FUNC(0x023695C8, void, this);
    if (unk346 && dComIfGp_evmng_endCheck(mEventIdx)) {
        dComIfGp_event_reset();
        unk346 = 0;
    }
}
VERIFY(0x023695C8, &Act_c::demo_end_reset);

/* 023693D4 */
void Act_c::mode_wait_init() {
    WWHD_FUNC(0x023693D4, void, this);
    mMode = Mode_WAIT_e;
}
VERIFY(0x023693D4, &Act_c::mode_wait_init);

/* 023697B0 */
void Act_c::mode_wait() {
    WWHD_FUNC(0x023697B0, void, this);
    if (fopAcM_isSwitch(this, prm_get_swSave())) {
        mode_demoreq_init();
    }
}
VERIFY(0x023697B0, &Act_c::mode_wait);

/* 0236979C */
void Act_c::mode_demoreq_init() {
    WWHD_FUNC(0x0236979C, void, this);
    mMode = Mode_DEMOREQ_e;
    unk346 = 0;
}
VERIFY(0x0236979C, &Act_c::mode_demoreq_init);

/* 02369830 */
void Act_c::mode_demoreq() {
    WWHD_FUNC(0x02369830, void, this);
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
VERIFY(0x02369830, &Act_c::mode_demoreq);

/* 02369810 */
void Act_c::mode_vib_init() {
    WWHD_FUNC(0x02369810, void, this);
    mVibTimer = 15; /* attr().mVibDuration */
    unk338 = 0;
    unk33A = 0;
    mMode = Mode_VIB_e;
}
VERIFY(0x02369810, &Act_c::mode_vib_init);

/* 02369934 */
void Act_c::mode_vib() {
    WWHD_FUNC(0x02369934, void, this);
    unk338 += 20000;  /* attr().field_0x16 */
    unk33A += 0x3CC3; /* attr().field_0x18 */
    mVibXOffset = cM_scos(unk338) * 2.0f; /* attr().field_0x1C */
    mVibYOffset = cM_scos(unk33A);        /* * attr().field_0x20 (1.0) */
    if (--mVibTimer <= 0) {
        mode_drop_init();
    }
}
VERIFY(0x02369934, &Act_c::mode_vib);

/* 023698FC */
void Act_c::mode_drop_init() {
    WWHD_FUNC(0x023698FC, void, this);
    gravity = -5.0f;
    speed.set(cXyz_Zero->x, cXyz_Zero->y, cXyz_Zero->z);
    mMode = Mode_DROP_e;
    unk2DE = 3; /* attr().field_0x10 */
}
VERIFY(0x023698FC, &Act_c::mode_drop_init);

/* 023699A0 */
void Act_c::mode_drop() {
    WWHD_FUNC(0x023699A0, void, this);
    daObj_posMoveF_stream(this, nullptr, cXyz_Zero, 0.005f, 0.0005f);
    if (current.pos.y < mGndY) {
        if (unk2DE == 3) {
            fopAcM_seStart(this, JA_SE_OBJ_LADDER_FALL_1, dBgS_GetMtrlSndId(dComIfG_Bgsp(), dBgS_GndChk_PolyInfo(mGndChk)));
            gabi::Local<cXyz> dir;
            dir->set(0.0f, 1.0f, 0.0f);
            dComIfGp_getVibration_StartShock(4, -0x21, dir);
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
VERIFY(0x023699A0, &Act_c::mode_drop);

/* 023693C8 */
void Act_c::mode_fell_init() {
    WWHD_FUNC(0x023693C8, void, this);
    mMode = Mode_FELL_e;
}
VERIFY(0x023693C8, &Act_c::mode_fell_init);

/* 02369C64 */
void Act_c::mode_fell() {
    WWHD_FUNC(0x02369C64, void, this);
}
VERIFY(0x02369C64, &Act_c::mode_fell);

/* 023692C0 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x023692C0, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
    mDoMtx_stack_c::transM(mVibXOffset, mVibYOffset, 0.0f);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x023692C0, &Act_c::set_mtx);

/* 023693A8 */
void Act_c::init_mtx() {
    WWHD_FUNC(0x023693A8, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    set_mtx();
}
VERIFY(0x023693A8, &Act_c::init_mtx);

/* 02369638 */
BOOL Act_c::Execute(Mtx34** ppMtx) {
    WWHD_FUNC(0x02369638, BOOL, this, ppMtx);
    /* static const ModeFunc mode_proc[] (0x1002BE68): wait, demoreq, vib, drop, fell */
    demo_end_reset();
    ptmf_call(0x1002BE68 + 8 * mMode, this);
    eyePos.y = current.pos.y;
    set_mtx();
    gabi::store<u32>(gabi::ea(ppMtx), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x02369638, &Act_c::Execute);

/* 02369704 */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x02369704, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
    dComIfGd_setListBG();
    mDoExt_modelUpdateDL(mpModel);
    dComIfGd_setList();
    return TRUE;
}
VERIFY(0x02369704, &Act_c::Draw);

/* method table entries (tail branches) */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x02369B7C, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x02369B7C, Mthd_Create);
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x02369B80, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x02369B80, Mthd_Delete);
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x02369B84, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x02369B84, Mthd_Execute);
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x02369B88, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x02369B88, Mthd_Draw);
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x02369B98, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x02369B98, Mthd_IsDelete);

/* 02369BA8 */
static void __sinit_d_a_obj_ladder_cpp() {
    WWHD_FUNC(0x02369BA8, void, (u32)0);
    sinit_header_statics(0x1046A424, 0x101CA7D8);
}
VERIFY(0x02369BA8, __sinit_d_a_obj_ladder_cpp);

/* 02369C3C: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02369C3C, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02369C3C, trivial_dt);

/* ---- leftover functions of the translation unit ---- */

/* 02369C50 dBgS_MoveBgActor::IsDelete (out-of-line copy; daObjLadder::Act_c vtable slot 1002BEF4) */
static BOOL ladder_MoveBgActor_IsDelete(void* p) {
    WWHD_FUNC(0x02369C50, BOOL, p);
    return TRUE;
}
VERIFY(0x02369C50, ladder_MoveBgActor_IsDelete);

/* 02369C58 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 1002BD34, after the destructor 02369C3C */
static void ladder_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x02369C58, void, p);
}
VERIFY(0x02369C58, ladder_SafeString_assureTermination);

/* 02369C68 daObjLadder::Act_c::~Act_c (deleting; vtable slot 1002BEC4): the embedded line check (vtables, cBgS_Chk base), then fopAc_ac_c */
static void ladder_Act_c_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02369C68, void, p, flags);
    if (p != nullptr) {
        u32 t = gabi::ea(p);
        gabi::store<u32>(t + 0x41C, 0x1002BD58);
        gabi::store<u32>(t + 0x43C, 0x1002BD78);
        gabi::store<u32>(t + 0x448, 0x1002BD38);
        gabi::call(0x02008DAC, t + 0x3FC, 0); /* cBgS_Chk::~cBgS_Chk */
        gabi::call(0x025D50BC, p, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1) operator_delete(p);
    }
}
VERIFY(0x02369C68, ladder_Act_c_dt);
