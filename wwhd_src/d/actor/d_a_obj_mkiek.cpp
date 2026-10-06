/**
 * d_a_obj_mkiek.cpp (WWHD)
 * Object - MkieK: a wall that vanishes when hit by light (Light Arrow / mirror light).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_mkiek.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "bindings.h"

#define M_arcname STR(0x1002C880) /* "MkieK" */
#define SAFESTRING_VTBL 0x1002C7A0
#define ACT_VTBL 0x1002C888 /* HD: daObjMkiek::Act_c vtable */
#define FILE_NAME STR(0x1002C820)
#define M_tmp_mtx gabi::at<Mtx34>(0x1046A5A4)
#define sph_check_src gabi::at<dCcD_SrcSph>(0x101CAE40)

enum {
    dRes_INDEX_MKIEK_BDL_MKIEK_e = 6,
    dRes_INDEX_MKIEK_BDL_YLSMK00_e = 9,
    dRes_INDEX_MKIEK_BRK_YLSMK00_e = 0xC,
    dRes_INDEX_MKIEK_DZB_MKIEK_e = 0xF,
};

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 0252A038 dDetect_c::chk_light(cXyz*) (play+0x5A20) (as in d_a_cc) */
static inline BOOL dComIfGp_getDetect_chk_light(cXyz* pos) { return gabi::call<BOOL>(0x0252A038, dComIfGp_ea() + PLAY_DETECT, pos); }
/* 025E1988 HD: mDoAud_seStart(id) without position (as in d_a_mo2.h) */
static inline void mDoAud_seStart_1(u32 id) { gabi::call(0x025E1988, id); }
/* 025E80D0 mDoExt_brkAnm::mDoExt_brkAnm (matcher: "mDoExt_brkAnm::init") */
static inline void mDoExt_brkAnm_ct(void* p) { gabi::call(0x025E80D0, p); }
/* 025E8154 mDoExt_brkAnm::init(data, key, anmPlay, mode, rate (f1), start, end, modify, entry (stack)) */
static inline BOOL mDoExt_brkAnm_init(void* a, J3DModelData* d, void* key, s32 play, s32 mode, f32 rate, s16 start, s16 end,
                                      bool modify, s32 entry) {
    return gabi::call<BOOL>(0x025E8154, a, d, key, play, mode, rate, start, end, modify, entry);
}
/* HD J3DFrameCtrl (no vtable): rate +0, frame +4, state +0xF */
static inline bool frameCtrl_isStop(u32 fc) {
    return (gabi::load<u8>(fc + 0xF) & 1) || gabi::load<f32>(fc) == 0.0f;
}
/* fopAcM_seStartCurrent (HD inline: no null checks) */
static inline void fopAcM_seStartCurrent(fopAc_ac_c* a, u32 id, u32 param) {
    mDoAud_seStart(id, &a->current.pos, param, dComIfGp_getReverb(fopAcM_GetRoomNo(a)));
}
static inline void fopAcM_onSwitch(fopAc_ac_c* a, s32 sw) { dComIfGs_onSwitch(sw, a->home.roomNo); }

namespace daObjMkiek {
struct Act_c : dBgS_MoveBgActor {
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0, PRM_SOUND_W = 1, PRM_SOUND_S = 8 };
    enum State_e { STATE_0, STATE_1, STATE_2 };
    /* tevStr.mColorC0 (GXColorS10, actor +0x1A0) */
    s16 tevStr_C0(int i) { return gabi::load<s16>(gabi::ea(this) + 0x1A0 + 2 * i); }
    s32 prm_get_sound();
    s32 prm_get_swSave();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    void check();
    void demo_wait();
    void demo();
    BOOL Execute(Mtx34** o_mtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ gptr<J3DModel> mpModelV;
    /* 0x3F0 */ u8 mBrkAnm[0x78]; /* mDoExt_brkAnm (HD 0x78; frame ctrl at +0) */
    /* 0x468 */ dCcD_Stts mStts;
    /* 0x4A4 */ dCcD_Sph mSph;
    /* 0x5D0 */ be<u8> m458;
    /* 0x5D1 */ u8 _5D1;
    /* 0x5D2 */ be<s16> mDieEventIdx;
    /* 0x5D4 */ be<u32> mState;
    /* 0x5D8 */ be<s32> m460;
};
WWHD_OFFSET(Act_c, mStts, 0x468);
WWHD_OFFSET(Act_c, mSph, 0x4A4);
WWHD_OFFSET(Act_c, m460, 0x5D8);
WWHD_SIZE(Act_c, 0x5DC);
}  // namespace daObjMkiek
using daObjMkiek::Act_c;

/* 023722C8: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU) */
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    WWHD_FUNC(0x023722C8, u32, a, width, shift);
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
VERIFY(0x023722C8, PrmAbstract);
s32 Act_c::prm_get_sound() { return PrmAbstract(this, PRM_SOUND_W, PRM_SOUND_S); }
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 0237184C */
BOOL Act_c::CreateHeap() {
    WWHD_FUNC(0x0237184C, BOOL, this);
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MKIEK_BDL_MKIEK_e, SAFESTRING_VTBL);
    if (model_data == nullptr) /* JUT_ASSERT(0x96, model_data != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x96, STR(0x1002C810));
    mpModel = mDoExt_J3DModel__create(model_data, 0, 0x11020203);

    J3DModelData* model_data_v = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MKIEK_BDL_YLSMK00_e, SAFESTRING_VTBL);
    if (model_data_v == nullptr) /* JUT_ASSERT(0x9C, model_data_v != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0x9C, STR(0x1002C834));
    mpModelV = mDoExt_J3DModel__create(model_data_v, 0, 0x11020203);

    void* brk = dComIfG_getObjectRes(M_arcname, dRes_INDEX_MKIEK_BRK_YLSMK00_e, SAFESTRING_VTBL);
    if (brk == nullptr) /* JUT_ASSERT(0xA2, brk != NULL) */
        JUT_ASSERT_fail(FILE_NAME, 0xA2, STR(0x1002C848));

    BOOL result = mDoExt_brkAnm_init(mBrkAnm, model_data_v, brk, true, 0 /* EMode_NONE */, 1.0f, 0, -1, false, 0);
    if (result == 0)
        return FALSE;
    return mpModel != nullptr && mpModelV != nullptr;
}
VERIFY(0x0237184C, &Act_c::CreateHeap);

/* 02371B3C */
BOOL Act_c::Create() {
    WWHD_FUNC(0x02371B3C, BOOL, this);
    mStts.Init(0xFF, 0xFF, this);
    mSph.Set(sph_check_src);
    mSph.SetStts(&mStts);
    cullMtx = gabi::ea(J3DModel_getBaseTRMtx(mpModel)); /* fopAcM_SetMtx */
    init_mtx();
    fopAcM_setCullSizeBox(this, -200.0f, -100.0f, -200.0f, 200.0f, 305.0f, 200.0f);
    m458 = false;
    mDieEventIdx = dComIfGp_evmng_getEventIdx(STR(0x1002C864) /* "MkieK_die" */, 0xFF);
    mState = STATE_0;
    m460 = 0;
    return TRUE;
}
VERIFY(0x02371B3C, &Act_c::Create);

/* 02371698 */
cPhs_State Act_c::Mthd_Create() {
    WWHD_FUNC(0x02371698, cPhs_State, this);
    /* fopAcM_ct(this, Act_c): inline member constructors */
    if (!fopAcM_CheckCondition(this, fopAcCnd_INIT_e)) {
        if (this != nullptr) {
            dBgS_MoveBgActor::ct(this);
            __vtbl = ACT_VTBL;
            mDoExt_brkAnm_ct(mBrkAnm);
            dCcD_Stts_ct(&mStts);
            gabi::call(0x025166F0, &mSph); /* dCcD_Sph::dCcD_Sph */
        }
        fopAcM_OnCondition(this, fopAcCnd_INIT_e);
    }
    s32 switch_index = prm_get_swSave();
    if (fopAcM_isSwitch(this, switch_index)) {
        return 3 /* cPhs_STOP_e */;
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_MKIEK_DZB_MKIEK_e, 0, 0x1220);
        if (!((phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e))) /* JUT_ASSERT(0xD9, ...) HD line 0xD8 */
            JUT_ASSERT_fail(STR(0x1002C7B8), 0xD8, STR(0x1002C7CC));
    }
    return phase_state;
}
VERIFY(0x02371698, &Act_c::Mthd_Create);

/* 023717F4 */
BOOL Act_c::Mthd_Delete() {
    WWHD_FUNC(0x023717F4, BOOL, this);
    BOOL result = MoveBGDelete();
    if (gabi::load<u8>(gabi::ea(this) + 0xD) != 3 /* cPhs_STOP_e */) { /* fpcM_CreateResult */
        dComIfG_resDelete(&mPhs, M_arcname); /* dComIfG_resDeleteDemo */
    }
    return result;
}
VERIFY(0x023717F4, &Act_c::Mthd_Delete);

/* 023719C8 */
void Act_c::set_mtx() {
    WWHD_FUNC(0x023719C8, void, this);
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), M_tmp_mtx);
    J3DModel_setBaseTRMtx(mpModelV, mDoMtx_stack_c::get());
}
VERIFY(0x023719C8, &Act_c::set_mtx);

/* 02371B00 */
void Act_c::init_mtx() {
    WWHD_FUNC(0x02371B00, void, this);
    J3DModel_setBaseScale(mpModel, &scale);
    J3DModel_setBaseScale(mpModelV, &scale);
    set_mtx();
}
VERIFY(0x02371B00, &Act_c::init_mtx);

/* 02371C14 */
void Act_c::check() {
    WWHD_FUNC(0x02371C14, void, this);
    if (dComIfGp_getDetect_chk_light(&current.pos) || mSph.ChkTgHit()) {
        m460 = m460 + 1;
        if (m460 >= 0x14) {
            fopAcM_orderOtherEventId(this, mDieEventIdx, 0xFF, 0xFFFF, 0, 1);
            mState = STATE_1;
        }
    } else {
        m460 = 0;
    }
}
VERIFY(0x02371C14, &Act_c::check);

/* 02371CBC */
void Act_c::demo_wait() {
    WWHD_FUNC(0x02371CBC, void, this);
    if (eventInfo_checkCommandDemoAccrpt(this)) {
        if (prm_get_sound() == 0) {
            mDoAud_seStart_1(0x806); /* JA_SE_READ_RIDDLE_1 */
        }
        if (m460 < 0x14) {
            m460 = m460 + 1;
        } else {
            if (dBgW_ChkUsed(mpBgW)) {
                dBgS* bgs = dComIfG_Bgsp();
                cBgS_Release(bgs, mpBgW);
            }
            gabi::Local<GXColor> color;
            color->r = (u8)tevStr_C0(0);
            color->g = (u8)tevStr_C0(1);
            color->b = (u8)tevStr_C0(2);
            color->a = (u8)tevStr_C0(3);
            s8 roomNo = current.roomNo;
            dPa_control_c* pa = dComIfGp_getParticle();
            dPa_control_set(pa, 4, 0x819F /* dPa_name::ID_AK_SN_VANISHWALL00 */, &current.pos, &current.angle, nullptr, 0xFF,
                            nullptr, roomNo, gabi::at<GXColor>(gabi::ea(this) + 0x1A8) /* tevStr.mColorK0 */, color, nullptr);
            fopAcM_onSwitch(this, prm_get_swSave());
            fopAcM_seStartCurrent(this, 0x69C2 /* JA_SE_OBJ_L_OBJ_BRK_TAME */, 0);
            m458 = true;
            mState = STATE_2;
        }
    } else {
        fopAcM_orderOtherEventId(this, mDieEventIdx, 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x02371CBC, &Act_c::demo_wait);

/* 02371E6C */
void Act_c::demo() {
    WWHD_FUNC(0x02371E6C, void, this);
    mDoExt_baseAnm_play(mBrkAnm);
    if (!frameCtrl_isStop(gabi::ea(mBrkAnm))) {
        return;
    }
    dComIfGp_event_reset();
    fopAcM_seStartCurrent(this, 0x6964 /* JA_SE_OBJ_L_WALL_BREAK */, 0);
    gabi::Local<cXyz> v;
    v->x = 0.0f;
    v->y = 1.0f;
    v->z = 0.0f;
    dComIfGp_getVibration_StartShock(4, -0x21, v);
    fopAcM_delete(this);
}
VERIFY(0x02371E6C, &Act_c::demo);

/* 02371F38 */
BOOL Act_c::Execute(Mtx34** o_mtx) {
    WWHD_FUNC(0x02371F38, BOOL, this, o_mtx);
    gabi::Local<cXyz> sph_pos_offset;
    sph_pos_offset->z = 0.0f;
    sph_pos_offset->y = 150.0f;
    sph_pos_offset->x = 0.0f;
    gabi::Local<cXyz> c;
    cXyz_pl(&current.pos, c, sph_pos_offset);
    mSph.SetC(c);
    dComIfG_Ccsp_Set(&mSph);
    switch ((u32)mState) {
    case STATE_0: check(); break;
    case STATE_1: demo_wait(); break;
    case STATE_2: demo(); break;
    }
    set_mtx();
    gabi::store<u32>(gabi::ea(o_mtx), gabi::ea(M_tmp_mtx));
    return TRUE;
}
VERIFY(0x02371F38, &Act_c::Execute);

/* 0237205C: HD: no simple shadow */
BOOL Act_c::Draw() {
    WWHD_FUNC(0x0237205C, BOOL, this);
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_BG0, &current.pos, &tevStr);
    if (m458) {
        setLightTevColorType(dKy_getEnvlight(), mpModelV, &tevStr);
        J3DModel* mdl = mpModelV;
        f32 frame = gabi::load<f32>(gabi::ea(mBrkAnm) + 4);
        mDoExt_brkAnm_entry((mDoExt_brkAnm*)mBrkAnm, J3DModel_getModelData(mdl), frame);
        dComIfGd_setListBG();
        mDoExt_modelUpdateDL(mpModelV);
        dComIfGd_setList();
    } else {
        setLightTevColorType(dKy_getEnvlight(), mpModel, &tevStr);
        dComIfGd_setListBG();
        mDoExt_modelUpdateDL(mpModel);
        dComIfGd_setList();
    }
    return TRUE;
}
VERIFY(0x0237205C, &Act_c::Draw);

/* method table entries (HD: tail branches) */
/* 02372174 */
static cPhs_State Mthd_Create(void* i_this) {
    WWHD_FUNC(0x02372174, cPhs_State, i_this);
    return ((Act_c*)i_this)->Mthd_Create();
}
VERIFY(0x02372174, Mthd_Create);
/* 02372178 */
static BOOL Mthd_Delete(void* i_this) {
    WWHD_FUNC(0x02372178, BOOL, i_this);
    return ((Act_c*)i_this)->Mthd_Delete();
}
VERIFY(0x02372178, Mthd_Delete);
/* 0237217C */
static BOOL Mthd_Execute(void* i_this) {
    WWHD_FUNC(0x0237217C, BOOL, i_this);
    return ((Act_c*)i_this)->MoveBGExecute();
}
VERIFY(0x0237217C, Mthd_Execute);
/* 02372180: virtual Draw */
static BOOL Mthd_Draw(void* i_this) {
    WWHD_FUNC(0x02372180, BOOL, i_this);
    return ((Act_c*)i_this)->Draw_v();
}
VERIFY(0x02372180, Mthd_Draw);
/* 02372190: virtual IsDelete */
static BOOL Mthd_IsDelete(void* i_this) {
    WWHD_FUNC(0x02372190, BOOL, i_this);
    return ((Act_c*)i_this)->IsDelete_v();
}
VERIFY(0x02372190, Mthd_IsDelete);

/* 023721A0 */
static void __sinit_d_a_obj_mkiek_cpp() {
    WWHD_FUNC(0x023721A0, void, (u32)0);
    sinit_header_statics(0x1046A588, 0x101CAE80);
}
VERIFY(0x023721A0, __sinit_d_a_obj_mkiek_cpp);

/* 02372234: deleting destructor of a class with a trivial destructor */
static void trivial_dt(void* p, s32 flags) {
    WWHD_FUNC(0x02372234, void, p, flags);
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}
VERIFY(0x02372234, trivial_dt);

/* 02372248: dBgS_MoveBgActor::IsDelete (per-TU copy) */
static BOOL MoveBgActor_IsDelete(Act_c* i_this) {
    WWHD_FUNC(0x02372248, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02372248, MoveBgActor_IsDelete);

/* 02372254 */
static BOOL Act_c_Delete(Act_c* i_this) {
    WWHD_FUNC(0x02372254, BOOL, i_this);
    return TRUE;
}
VERIFY(0x02372254, Act_c_Delete);

/* 0237225C: daObjMkiek::Act_c deleting destructor (inline member destructors) */
static void Act_c_dt(Act_c* i_this, s32 flags) {
    WWHD_FUNC(0x0237225C, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515AE8, &i_this->mSph, 2);  /* dCcD_Sph::~dCcD_Sph */
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x025D50BC, i_this, 0);         /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x0237225C, Act_c_dt);

/* ---- leftover functions of the translation unit ---- */

/* 02372250 sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 1002C7B4, after the destructor 02372234 */
static void mkiek_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x02372250, void, p);
}
VERIFY(0x02372250, mkiek_SafeString_assureTermination);
