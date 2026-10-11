/**
 * d_a_obj_mkiek.cpp (WWHD)
 * Object - MkieK: a wall that vanishes when hit by light (Light Arrow / mirror light).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_obj_mkiek.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_obj_mkiek.h"
using daObjMkiek::Act_c;
const gc_tu_data gc_tu = { .safeString = 0x1002C7A0 };
const char Act_c::M_arcname[] = "MkieK";
// HD: class-static matrix is mapped as TU data by the current glue tool.
static Mtx34 M_tmp_mtx;
static dCcD_SrcSph sph_check_src;

/* 023722C8 */
// HD: daObj::PrmAbstract<Act_c::Prm_e>(actor, width, shift) (HD: out of line, per TU)
static u32 PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift) {
    /* PowerPC srw/slw: shift counts of 32..63 give 0 */
    u32 sh = (u32)shift & 63, wd = (u32)width & 63;
    u32 v = sh < 32 ? fopAcM_GetParam(a) >> sh : 0;
    u32 m = (wd < 32 ? 1u << wd : 0u) - 1;
    return v & m;
}
s32 Act_c::prm_get_sound() { return PrmAbstract(this, PRM_SOUND_W, PRM_SOUND_S); }
s32 Act_c::prm_get_swSave() { return PrmAbstract(this, PRM_SWSAVE_W, PRM_SWSAVE_S); }

/* 0237184C */
BOOL Act_c::CreateHeap() {
    J3DModelData* model_data = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MKIEK_BDL_MKIEK_e);
    JUT_ASSERT(0x96, model_data != NULL);
    mpModel = mDoExt_J3DModel__create(model_data, 0, 0x11020203);

    J3DModelData* model_data_v = (J3DModelData*)dComIfG_getObjectRes(M_arcname, dRes_INDEX_MKIEK_BDL_YLSMK00_e);
    JUT_ASSERT(0x9C, model_data_v != NULL);
    mpModelV = mDoExt_J3DModel__create(model_data_v, 0, 0x11020203);

    void* brk = dComIfG_getObjectRes(M_arcname, dRes_INDEX_MKIEK_BRK_YLSMK00_e);
    JUT_ASSERT(0xA2, brk != NULL);

    BOOL result = mBrkAnm.init( model_data_v, brk, true, J3DFrameCtrl::EMode_NONE, 1.0f, 0, -1, false, 0);
    if (result == 0)
        return FALSE;
    return mpModel != nullptr && mpModelV != nullptr;
}

/* 02371B3C */
BOOL Act_c::Create() {
    mStts.Init(0xFF, 0xFF, this);
    mSph.Set(&sph_check_src);
    mSph.SetStts(&mStts);
    fopAcM_SetMtx(this, mpModel->getBaseTRMtx());
    init_mtx();
    fopAcM_setCullSizeBox(this, -200.0f, -100.0f, -200.0f, 200.0f, 305.0f, 200.0f);
    m458 = false;
    mDieEventIdx = dComIfGp_evmng_getEventIdx("MkieK_die", 0xFF);
    mState = STATE_0;
    m460 = 0;
    return TRUE;
}

/* 02371698 */
cPhs_State Act_c::Mthd_Create() {
    fopAcM_ct(this, Act_c);
    s32 switch_index = prm_get_swSave();
    if (fopAcM_isSwitch(this, switch_index)) {
        return cPhs_STOP_e;
    }
    cPhs_State phase_state = dComIfG_resLoad(&mPhs, M_arcname);
    if (phase_state == cPhs_COMPLEATE_e) {
        phase_state = MoveBGCreate(M_arcname, dRes_INDEX_MKIEK_DZB_MKIEK_e, 0, 0x1220);
        // HD: assertion line is 0xD8 (GameCube 0xD9).
        JUT_ASSERT(0xD8, (phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e));
    }
    return phase_state;
}

/* 023717F4 */
BOOL Act_c::Mthd_Delete() {
    BOOL result = MoveBGDelete();
    if (fpcM_CreateResult(this) != cPhs_STOP_e) { /* fpcM_CreateResult */
        dComIfG_resDelete(&mPhs, M_arcname); // HD: resource deletion uses the ordinary HD binding.
    }
    return result;
}

/* 023719C8 */
void Act_c::set_mtx() {
    mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
    mDoMtx_ZXYrotM(mDoMtx_stack_c::get(), shape_angle.x, shape_angle.y, shape_angle.z);
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    PSMTXCopy(mDoMtx_stack_c::get(), &M_tmp_mtx);
    mpModelV->setBaseTRMtx(mDoMtx_stack_c::get());
}

/* 02371B00 */
void Act_c::init_mtx() {
    ((MkiekModel_view*)mpModel.get())->setBaseScale(scale);
    ((MkiekModel_view*)mpModelV.get())->setBaseScale(scale);
    set_mtx();
}

/* 02371C14 */
void Act_c::check() {
    if (dComIfGp_getDetect().chk_light(&current.pos) || mSph.ChkTgHit()) {
        m460 = m460 + 1;
        if (m460 >= 0x14) {
            fopAcM_orderOtherEventId(this, mDieEventIdx, 0xFF, 0xFFFF, 0, 1);
            mState = STATE_1;
        }
    } else {
        m460 = 0;
    }
}

/* 02371CBC */
void Act_c::demo_wait() {
    if (eventInfo.checkCommandDemoAccrpt()) {
        if (prm_get_sound() == 0) {
            mDoAud_seStart(JA_SE_READ_RIDDLE_1);
        }
        if (m460 < 0x14) {
            m460 = m460 + 1;
        } else {
            if (mpBgW->ChkUsed()) {
                dBgS* bgs = dComIfG_Bgsp();
                bgs->Release(mpBgW);
            }
            GXColor color;
            color.r = (u8)tevStr.mColorC0.r;
            color.g = (u8)tevStr.mColorC0.g;
            color.b = (u8)tevStr.mColorC0.b;
            color.a = (u8)tevStr.mColorC0.a;
            s8 roomNo = current.roomNo;
            dPa_control_c* pa = dComIfGp_getParticle();
            pa->set(4, dPa_name::ID_AK_SN_VANISHWALL00, &current.pos, &current.angle, nullptr, 0xFF,
                            nullptr, roomNo, &tevStr.mColorK0, &color, nullptr);
            fopAcM_onSwitch(this, prm_get_swSave());
            fopAcM_seStartCurrent(this, JA_SE_OBJ_L_OBJ_BRK_TAME, 0);
            m458 = true;
            mState = STATE_2;
        }
    } else {
        fopAcM_orderOtherEventId(this, mDieEventIdx, 0xFF, 0xFFFF, 0, 1);
    }
}

/* 02371E6C */
void Act_c::demo() {
    mBrkAnm.play();
    if (!mBrkAnm.isStop()) {
        return;
    }
    dComIfGp_event_reset();
    fopAcM_seStartCurrent(this, JA_SE_OBJ_L_WALL_BREAK, 0);
    cXyz v;
    v.x = 0.0f;
    v.y = 1.0f;
    v.z = 0.0f;
    dComIfGp_getVibration()->StartShock(4, -0x21, &v);
    fopAcM_delete(this);
}

/* 02371F38 */
BOOL Act_c::Execute(Mtx34** o_mtx) {
    cXyz sph_pos_offset;
    sph_pos_offset.z = 0.0f;
    sph_pos_offset.y = 150.0f;
    sph_pos_offset.x = 0.0f;
    cXyz c;
    c = current.pos + sph_pos_offset;
    mSph.SetC(&c);
    cCcS_Set(dComIfG_Ccsp(), &mSph);
    switch ((u32)mState) {
    case STATE_0:
        check();
        break;
    case STATE_1:
        demo_wait();
        break;
    case STATE_2:
        demo();
        break;
    }
    set_mtx();
    *reinterpret_cast<gptr<Mtx34>*>(o_mtx) = &M_tmp_mtx; // HD: guest matrix pointer.
    return TRUE;
}

/* 0237205C */
// HD: no simple shadow
BOOL Act_c::Draw() {
    g_env_light.settingTevStruct(TEV_TYPE_BG0, &current.pos, &tevStr);
    if (m458) {
        g_env_light.setLightTevColorType(mpModelV, &tevStr);
        J3DModel* mdl = mpModelV;
        f32 frame = mBrkAnm.getFrame();
        mBrkAnm.entry(mdl->getModelData(), frame);
        dComIfGd_setListBG();
        mDoExt_modelUpdateDL(mpModelV);
        dComIfGd_setList();
    } else {
        g_env_light.setLightTevColorType(mpModel, &tevStr);
        dComIfGd_setListBG();
        mDoExt_modelUpdateDL(mpModel);
        dComIfGd_setList();
    }
    return TRUE;
}

/* method table entries (HD: tail branches) */
/* 02372174 */
static cPhs_State Mthd_Create(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Create();
}
/* 02372178 */
static BOOL Mthd_Delete(void* i_this) {
    return ((Act_c*)i_this)->Mthd_Delete();
}
/* 0237217C */
static BOOL Mthd_Execute(void* i_this) {
    return ((Act_c*)i_this)->MoveBGExecute();
}
/* 02372180 */
// HD: virtual Draw
static BOOL Mthd_Draw(void* i_this) {
    return ((Act_c*)i_this)->Draw_v();
}
/* 02372190 */
// HD: virtual IsDelete
static BOOL Mthd_IsDelete(void* i_this) {
    return ((Act_c*)i_this)->IsDelete_v();
}

/* 023721A0 */
static void __sinit_d_a_obj_mkiek_cpp() {
    sinit_header_statics(0x1046A588, 0x101CAE80);
}

/* 02372234 */
// HD: deleting destructor of a class with a trivial destructor
static void trivial_dt(void* p, s32 flags) {
    if (p != nullptr && (flags & 1))
        operator_delete(p);
}

/* 02372248 */
// HD: dBgS_MoveBgActor::IsDelete (per-TU copy)
static BOOL MoveBgActor_IsDelete(Act_c* i_this) {
    return TRUE;
}

/* 02372254 */
static BOOL Act_c_Delete(Act_c* i_this) {
    return TRUE;
}

/* 0237225C */
// HD: daObjMkiek::Act_c deleting destructor (inline member destructors)
static void Act_c_dt(Act_c* i_this, s32 flags) {
    if (i_this != nullptr) {
        __dt(&i_this->mSph);
        dCcD_Stts_dt(&i_this->mStts, 2);
        __dt((fopAc_ac_c*)i_this);
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* ---- leftover functions of the translation unit ---- */

/* 02372250 */
// HD: sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 1002C7B4, after the destructor 02372234
static void mkiek_SafeString_assureTermination(void* p) {
}

/* WWHD glue (tools/verify/autoglue.py)
str 1002C880 "MkieK"
str 1002C864 "MkieK_die"
str 1002C820 "d_a_obj_mkiek.cpp" in 0237184C
str 1002C7B8 "d_a_obj_mkiek.cpp" in 02371698
str 1002C810 "model_data != NULL" renamed
str 1002C834 "model_data_v != NULL" renamed
str 1002C848 "brk != NULL" renamed
str 1002C7CC "(phase_state == cPhs_COMPLEATE_e) || (phase_state == cPhs_ERROR_e)"
data M_tmp_mtx 1046A5A4
data sph_check_src 101CAE40
ext 025E1988 void mDoAud_seStart(u32)
*/
